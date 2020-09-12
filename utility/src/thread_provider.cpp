#include <dak/utility/thread_provider.h>


namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // A thread of execution.

   thread_provider_t::thread_provider_t(size_t a_thread_count)
   {
      my_threads.reserve(a_thread_count);
      my_functions.reserve(a_thread_count);

      for (size_t i = 0; i < a_thread_count; ++i)
      {
         my_threads.emplace_back(std::make_shared<std::thread>(std::thread(execution_loop, this)));
      }
   }

   thread_provider_t::~thread_provider_t()
   {
      std::lock_guard lock(my_mutex);
      my_stop = true;
      my_cond.notify_all();
   }

   void thread_provider_t::execute_in_thread(const std::function<void()>& a_function)
   {
      std::unique_lock lock(my_mutex);
      my_functions.emplace_back(a_function);
      my_cond.notify_one();
   }

   /*static*/
   void thread_provider_t::execution_loop(thread_provider_t* self)
   {
      if (!self)
         return;

      while (!self->my_stop)
      {
         try
         {
            function_t func;
            {
               std::unique_lock lock(self->my_mutex);
               if (self->my_functions.size() > 0)
               {
                  func = self->my_functions.back();
                  self->my_functions.pop_back();
               }
               else if (!self->my_stop)
               {
                  self->my_cond.wait(lock);
               }
            }

            if (func)
               func();
         }
         catch (std::exception&)
         {
         }
      }
   }
}
