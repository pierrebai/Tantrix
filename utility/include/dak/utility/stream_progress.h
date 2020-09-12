#pragma once

#ifndef DAK_UTILITY_STREAM_PROGRESS_H
#define DAK_UTILITY_STREAM_PROGRESS_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <dak/utility/progress.h>

#include <ostream>

namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Report progress of work to a std::ostream.
   //
   // Not thread safe. Wrap in a multi_thread_progress_t if needed.

   struct stream_progress_t : progress_t
   {
      // Create a progress reporter that writes to a stream.
      stream_progress_t() = default;
      stream_progress_t(std::ostream& a_stream, size_t a_report_every = my_default_report_every)
         : progress_t(a_report_every), my_stream(&a_stream) {}

      // Report the final progress tally when destroyed.
      ~stream_progress_t();

   protected:
      void update_progress(size_t a_total_count_so_far) override;

   private:
      std::ostream* my_stream = nullptr;
   };
}

#endif /* DAK_UTILITY_STREAM_PROGRESS_H */
