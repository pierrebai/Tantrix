#pragma once

#ifndef DAK_TANTRIX_PROGRESS_H
#define DAK_TANTRIX_PROGRESS_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

namespace dak::utility
{
   struct multi_thread_progress_t;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Report progress of work.

   struct progress_t
   {
      void set_estimated_total_count(size_t a_count) { my_estimated_total_count = a_count; }
      void progress(size_t a_done_count);

      size_t estimated_total_count() const { return my_estimated_total_count; }
      size_t total_count_so_far() const { return my_total_count_so_far; }

   protected:
      virtual void update_progress(size_t a_total_count_so_far) = 0;

   private:
      size_t my_estimated_total_count = 0;
      size_t my_total_count_so_far = 0;

      friend struct multi_thread_progress_t;
   };
}

#endif /* DAK_TANTRIX_PROGRESS_H */
