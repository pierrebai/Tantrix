#include <dak/utility/multi_thread_progress.h>


namespace dak::utility
{
   void per_thread_progress_t::update_progress(size_t a_total_count_so_far)
   {
      if (!my_mt_progress)
         return;

      my_mt_progress->update_progress_from_thread(a_total_count_so_far);
      clear_progress();
   }

   per_thread_progress_t::~per_thread_progress_t()
   {
      try
      {
         if (!my_mt_progress)
         return;

         my_mt_progress->update_progress_from_thread(total_progress());
         clear_progress();
      }
      catch (std::exception&)
      {
         // Dont'let exceptions out of the destructor.
      }
   }

   multi_thread_progress_t::~multi_thread_progress_t()
   {
      try
      {
         if (!my_non_thread_safe_progress)
         return;

         report_to_non_thread_safe_progress(my_total_count_so_far);
      }
      catch (std::exception&)
      {
         // Dont'let exceptions out of the destructor.
      }
   }

   void multi_thread_progress_t::update_progress_from_thread(size_t a_count_from_thread)
   {
      if (!my_non_thread_safe_progress)
         return;

      const size_t pre_count = my_total_count_so_far.fetch_add(a_count_from_thread);
      const size_t post_count = pre_count + a_count_from_thread;

      if ((pre_count / my_report_every) != (post_count / my_report_every))
      {
         report_to_non_thread_safe_progress(post_count);
      }
   }

   void multi_thread_progress_t::report_to_non_thread_safe_progress(size_t a_count)
   {
      std::lock_guard lock(my_mutex);
      my_non_thread_safe_progress->update_progress(a_count);
   }
}

