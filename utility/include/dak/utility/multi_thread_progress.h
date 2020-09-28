#pragma once

#ifndef DAK_UTILITY_MULTI_THREAD_PROGRESS_H
#define DAK_UTILITY_MULTI_THREAD_PROGRESS_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <dak/utility/progress.h>

#include <atomic>
#include <mutex>

namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Wrap a non-thread-safe progress in a multi-thread-safe progress.
   //
   // The progress can only be reported by a per-thread-progress referencing
   // this multi-thread progress.

   struct multi_thread_progress_t
   {
      // Wrap a non-threas safe progress.
      multi_thread_progress_t() = default;
      multi_thread_progress_t(progress_t& a_non_thread_safe_progress)
         : my_non_thread_safe_progress(&a_non_thread_safe_progress), my_report_every(a_non_thread_safe_progress.my_report_every) {}

      // Report the final progress tally when destroyed.
      ~multi_thread_progress_t();

      // Force to report the progress tally.
      void flush_progress() { report_to_non_thread_safe_progress(my_total_count_so_far); }

      // Clear the progress.
      void clear_progress() { my_total_count_so_far = 0; }

   protected:
      // Receive progress from a per-thread progress. (see below)
      void update_progress_from_thread(size_t a_count_from_thread);

      // Propagate the progress to the non-thread-safe progress.
      void report_to_non_thread_safe_progress(size_t a_count);

   private:
      progress_t*          my_non_thread_safe_progress = nullptr;
      size_t               my_report_every = 100 * 1000;
      std::atomic<size_t>  my_total_count_so_far = 0;
      std::mutex           my_mutex;

      friend struct per_thread_progress_t;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Report the progress of work from one thread to a multi-thread progress.
   //
   // Create one instance in each thread. It caches the thread progress and
   // only report from time to time to the multi-thread progress to avoid
   // accessing the shared atomic variable too often.

   struct per_thread_progress_t : progress_t
   {
      // Create a per-thread progress that report to the given multi-thread progress.
      per_thread_progress_t() = default;
      per_thread_progress_t(multi_thread_progress_t& a_mt_progress)
         : progress_t(a_mt_progress.my_report_every / 10), my_mt_progress(&a_mt_progress) {}

      per_thread_progress_t(const per_thread_progress_t& an_other)
         : progress_t(an_other), my_mt_progress(an_other.my_mt_progress) { clear_progress(); }

      per_thread_progress_t& operator=(const per_thread_progress_t& an_other)
      {
         progress_t::operator=(an_other);
         // Avoid copying the per-thread progress accumulated.
         clear_progress();
         return *this;
      }

      // Report the final progress tally when destroyed.
      ~per_thread_progress_t();

   protected:
      // Propagate the progress to the multi-thread progress.
      void update_progress(size_t a_total_count_so_far) override;

   private:
      multi_thread_progress_t*   my_mt_progress = nullptr;
   };

}

#endif /* DAK_UTILITY_MULTI_THREAD_PROGRESS_H */
