#pragma once

#ifndef DAK_UTILITY_THREADED_WORK_H
#define DAK_UTILITY_THREADED_WORK_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <dak/utility/thread_provider.h>


namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // A coordinator of work items, workers and threads.

   template <class WORK_ITEM, class RESULT>
   struct threaded_work_t
   {
      using function_t = std::function<void()>;

      // Create a thread provider with a given number of threads.
      threaded_work_t(std::shared_ptr<);
      ~threaded_work_t();

      // Queue the the given function to be executed in a thread.
      void execute_in_thread(const function_t& a_function);

      // Stop all threads.
      void stop();

   private:
      // The internal function that execute queued functions in a loop.
      static void execution_loop(threaded_work_t& self);

      std::mutex my_mutex;
      std::condition_variable my_cond;

      std::vector<std::shared_ptr<std::thread>> my_threads;
      std::vector<function_t> my_functions;

      std::atomic<bool> my_stop = false;
   };
}

#endif /* DAK_UTILITY_THREADED_WORK_H */
