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
      using function_t = typename std::function<result_t(work_item_t)>;
      using token_t = size_t;

      // Create a threaded work using the given thread pool.
      threaded_work_t() : my_thread_pool(*this, std::thread::hardware_concurrency()) {}

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
         return internal_wait_or_execute(lock);
      }

      // Queue the the given function and work item to be executed in a thread.
      token_t add_work(work_item_t a_work_item, function_t a_function)
      {
         std::unique_lock lock(my_mutex);

         token_t token = my_next_token.fetch_add(1);

         queued_item_t item;
         item.item = std::move(a_work_item);
         item.func = std::move(a_function);
         item.token = token;

         my_work_items.emplace_back(std::move(item));
         my_cond.notify_one();

         return token;
      }

      // Wait for a particular result, execute work while waiting.
      result_t wait_for(token_t a_token)
      {
         while (!is_stopped())
         {
            std::unique_lock lock(my_mutex);
            const auto iter = my_results.find(a_token);
            if (iter != my_results.end())
            {
               result_t result = std::move(iter->second);
               my_results.erase(iter);
               return std::move(result);
            }

            internal_wait_or_execute(lock);
         }

         return {};
      }

   private:
      // Wait for something to execute or execute something already in queue.
      void internal_wait_or_execute(std::unique_lock<std::mutex>& lock)
      {
         if (my_stop)
            return;

         if (my_work_items.size() <= 0)
         {
            my_cond.wait(lock);
            return;
         }

         queued_item_t item = std::move(my_work_items.back());
         my_work_items.pop_back();

         lock.unlock();
         result_t result = item.func(item.item);

         std::unique_lock relock(my_mutex);
         my_results[item.token] = std::move(result);
         my_cond.notify_all();
      }

      struct queued_item_t
      {
         work_item_t    item;
         function_t     func;
         token_t        token;
      };

      std::mutex                    my_mutex;
      std::condition_variable       my_cond;
      std::atomic<bool>             my_stop = false;
      std::atomic<token_t>          my_next_token = 0;
      std::vector<queued_item_t>    my_work_items;
      std::map<token_t, result_t>   my_results;
      thread_pool_t                 my_thread_pool;
   };
}

#endif /* DAK_UTILITY_THREADED_WORK_H */
