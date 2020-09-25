#pragma once

#ifndef DAK_UTILITY_THREADED_WORK_H
#define DAK_UTILITY_THREADED_WORK_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <dak/utility/thread_pool.h>

#include <atomic>
#include <future>
#include <mutex>
#include <condition_variable>
#include <map>
#include <algorithm>


namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // A dispatcher of work items to a thread pool.

   template <class WORK_ITEM, class RESULT>
   struct threaded_work_t : work_provider_t
   {
      using result_t = typename RESULT;
      using work_item_t = typename WORK_ITEM;
      using function_t = typename std::function<result_t(work_item_t, size_t)>;
      using task_t = std::packaged_task<result_t(work_item_t, size_t)>;

      // How the function, work item and recursion depth is kept internally.
      struct work_t
      {
         task_t      task;
         work_item_t item;
      };

      // Create a threaded work using the given thread pool.
      threaded_work_t(size_t a_max_recursion = 3)
         : my_max_recursion(a_max_recursion), my_thread_pool(*this, std::thread::hardware_concurrency()) {}

      ~threaded_work_t() { stop(); }

      // Stop all waiters.
      void stop() override
      {
         my_stop = true;
         my_cond.notify_all();
      }

      // Check if it is stopped.
      bool is_stopped() const override { return my_stop; }

      // Wait for something to execute or execute something already in queue.
      void wait_or_execute() override
      {
         std::unique_lock lock(my_mutex);
         return internal_wait_or_execute(lock, 0);
      }

      // Queue the the given function and work item to be executed in a thread.
      std::future<result_t> add_work(work_item_t a_work_item, size_t a_recursion_depth, function_t a_function)
      {
         if (my_stop)
            return {};

         // Only queue the work item if we've recursed into the threaded work only a few times.
         // Otherwise, we can end-up with too-deep stack recursion and crash.
         if (a_recursion_depth < my_max_recursion)
         {
            // Shallow: queue the function to be called by any thread.
            work_t work;
            work.task = std::move(task_t(a_function));
            work.item = std::move(a_work_item);

            auto result = work.task.get_future();

            {
               std::unique_lock lock(my_mutex);
               my_work_items.emplace_back(std::move(work));
            }

            my_cond.notify_all();

            return result;
         }
         else
         {
            // Too deep: call the function directly instead.
            std::promise<result_t> result;
            result.set_value(a_function(a_work_item, a_recursion_depth + 1));
            return result.get_future();
         }
      }

      // Wait for a particular result, execute work while waiting.
      result_t wait_for(std::future<result_t>& a_token, size_t a_recursion_depth)
      {
         while (!is_stopped())
         {
            std::unique_lock lock(my_mutex);

            if (a_token.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
               return a_token.get();

            internal_wait_or_execute(lock, a_recursion_depth);
         }

         return {};
      }

   private:
      // Wait for something to execute or execute something already in queue.
      void internal_wait_or_execute(std::unique_lock<std::mutex>& a_lock, size_t a_recursion_depth)
      {
         if (my_stop)
            return;

         if (my_work_items.size() <= 0)
         {
            my_cond.wait(a_lock);
            return;
         }

         work_t work = std::move(my_work_items.back());
         my_work_items.pop_back();
         a_lock.unlock();

         work.task(work.item, a_recursion_depth + 1);

         my_cond.notify_all();
      }

      std::mutex                    my_mutex;
      std::condition_variable       my_cond;
      std::atomic<bool>             my_stop = false;
      std::vector<work_t>           my_work_items;
      const size_t                  my_max_recursion;

      // Note: the thread pool must be the last variable so that it gets
      //       destroyed first while the mutex, etc are still valid.  
      thread_pool_t                 my_thread_pool;
   };
}

#endif /* DAK_UTILITY_THREADED_WORK_H */
