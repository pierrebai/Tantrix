#pragma once

#ifndef DAK_UTILITY_PROGRESS_H
#define DAK_UTILITY_PROGRESS_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

namespace dak::utility
{
   struct multi_thread_progress_t;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Report progress of work.
   //
   // Not thread safe. Wrap in a multi_thread_progress_t if needed.

   struct progress_t
   {
      static constexpr size_t my_default_report_every = 1000 * 1000;

      // Create a progress reporter.
      progress_t() = default;
      progress_t(size_t a_report_every) : my_report_every(a_report_every) {}

      // Force to report the progress tally.
      void flush_progress();

      // Clear the progress.
      void clear_progress() { my_total_count_so_far = 0; }

      // Update the progress with an additional count.
      void progress(size_t a_done_count);

      // Get the total progress so far.
      size_t total_progress() const { return my_total_count_so_far; }

   protected:
      // Update the total progress so far to the actual implementation.
      virtual void update_progress(size_t a_total_count_so_far) = 0;

   private:
      size_t my_report_every = my_default_report_every;
      size_t my_total_count_so_far = 0;

      friend struct multi_thread_progress_t;
   };
}

#endif /* DAK_UTILITY_PROGRESS_H */
