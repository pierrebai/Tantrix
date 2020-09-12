#include <dak/utility/multi_thread_progress.h>


namespace dak::utility
{
   void multi_thread_progress_t::progress(size_t a_done_count)
   {
      const size_t pre_count = my_total_count_so_far.fetch_add(a_done_count);
      const size_t post_count = pre_count + a_done_count;
      static constexpr size_t once_every = 1000 * 1000;
      if ((pre_count / once_every) != (post_count / once_every))
         update_progress(post_count);
   }

   void multi_thread_progress_t::update_progress(size_t a_done_count)
   {
      if (!my_mono_thread_progress)
         return;

      std::lock_guard lock(my_mutex);
      my_mono_thread_progress->update_progress(a_done_count);
   }
}

