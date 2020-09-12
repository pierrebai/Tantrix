#pragma once

#ifndef DAK_UTILITY_THREAD_PROVIDER_H
#define DAK_UTILITY_THREAD_PROVIDER_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <thread>
#include <vector>


namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // A provider of threads of execution.

   struct thread_provider_t
   {
      using function_t = std::function<void()>;

      // Create a thread provider with a given number of threads.
      thread_provider_t(size_t a_thread_count = 0);
      ~thread_provider_t();

      // Queue the the given function to be executed in a thread.
      void execute_in_thread(const function_t& a_function);

      // Stop all threads.
      void stop();

   private:
      // The internal function that execute queued functions in a loop.
      static void execution_loop(thread_provider_t* self);

      std::mutex my_mutex;
      std::condition_variable my_cond;

      std::vector<std::shared_ptr<std::thread>> my_threads;
      std::vector<function_t> my_functions;

      std::atomic<bool> my_stop = false;
   };
}

#endif /* DAK_UTILITY_THREAD_PROVIDER_H */
