#pragma once

#ifndef DAK_TANTRIX_STREAM_PROGRESS_H
#define DAK_TANTRIX_STREAM_PROGRESS_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <dak/utility/progress.h>

#include <ostream>

namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Report progress of work to a std::ostream.

   struct stream_progress : progress_t
   {
      stream_progress() = default;
      stream_progress(std::ostream& a_stream) : my_stream(&a_stream) {}

   protected:
      void update_progress(size_t a_total_count_so_far) override
      {
         if (!my_stream)
            return;

         *my_stream << a_total_count_so_far << " / " << estimated_total_count() << "\r";
      }

   private:
      std::ostream* my_stream = nullptr;
   };
}

#endif /* DAK_TANTRIX_STREAM_PROGRESS_H */
