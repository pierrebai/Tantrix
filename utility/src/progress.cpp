#include <dak/utility/progress.h>


namespace dak::utility
{
   void progress_t::progress(size_t a_done_count)
   {
      const size_t pre_count = my_total_count_so_far;
      my_total_count_so_far += a_done_count;
      const size_t post_count = pre_count + a_done_count;
      static constexpr size_t once_every = 1000 * 1000;
      if ((pre_count / once_every) != (post_count / once_every))
         update_progress(post_count);
   }
}

