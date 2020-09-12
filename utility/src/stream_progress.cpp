#include <dak/utility/stream_progress.h>

namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Report progress of work to a std::ostream.

   stream_progress_t::~stream_progress_t()
   {
      flush_progress();
   }

   void stream_progress_t::update_progress(size_t a_total_count_so_far)
   {
      if (!my_stream)
         return;

      *my_stream << a_total_count_so_far << " / " << estimated_total_count() << "\r";
   }
}

