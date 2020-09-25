#pragma once

#ifndef DAK_UTILITY_THREAD_POOL_H
#define DAK_UTILITY_THREAD_POOL_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <memory>
#include <thread>
#include <vector>


namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // The provider of work for the pool.

   struct work_provider_t
   {
      // Request that the threads stop.
      virtual void stop() = 0;

      // Check if stop was requested.
      virtual bool is_stopped() const = 0;

      // The wait-or-execute implementation, called in a loop
      // by the threads in the thread pool.
      virtual void wait_or_execute() = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // A pool of threads of execution.

   struct thread_pool_t
   {
      // Create a thread pool with a given number of threads
      // that will take its work from the given work provider.
      thread_pool_t(work_provider_t& a_work_provider, size_t a_thread_count = 0);
      ~thread_pool_t();

   private:
      // The internal function that execute queued functions in a loop.
      static void execution_loop(thread_pool_t* self);

      std::vector<std::shared_ptr<std::thread>> my_threads;
      work_provider_t&                          my_work_provider;
   };
}

#endif /* DAK_UTILITY_THREAD_POOL_H */
