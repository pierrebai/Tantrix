#include <dak/utility/progress.h>


namespace dak::utility
{
   void progress_t::progress(size_t a_done_count)
   {
      const size_t pre_count = my_total_count_so_far;
      my_total_count_so_far += a_done_count;
      const size_t post_count = pre_count + a_done_count;

      if ((pre_count / my_report_every) != (post_count / my_report_every))
         update_progress(post_count);
   }

   void progress_t::flush_progress()
   {
      update_progress(my_total_count_so_far);
   }
}

