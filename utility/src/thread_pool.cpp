#include <dak/utility/thread_pool.h>


namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // A pool of threads of execution.

   thread_pool_t::thread_pool_t(work_provider_t& a_work_provider, size_t a_thread_count)
      : my_work_provider(a_work_provider)
   {
      my_threads.reserve(a_thread_count);

      for (size_t i = 0; i < a_thread_count; ++i)
      {
         my_threads.emplace_back(std::make_shared<std::thread>(std::thread(execution_loop, this)));
      }
   }

   thread_pool_t::~thread_pool_t()
   {
      my_work_provider.stop();

      for (auto& thread : my_threads)
      {
         thread->join();
      }
   }

   /*static*/
   void thread_pool_t::execution_loop(thread_pool_t* self)
   {
      if (!self)
         return;

      while (!self->my_work_provider.is_stopped())
      {
         try
         {
            self->my_work_provider.wait_or_execute();
         }
         catch (std::exception&)
         {
            // TODO: report thread provider function execution errors.
         }
      }
   }
}
