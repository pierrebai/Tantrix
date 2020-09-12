#pragma once

#ifndef DAK_TANTRIX_MULTI_THREAD_PROGRESS_H
#define DAK_TANTRIX_MULTI_THREAD_PROGRESS_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <dak/utility/progress.h>

#include <atomic>
#include <mutex>

namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Report progress of work.

   struct multi_thread_progress_t
   {
      multi_thread_progress_t() = default;
      multi_thread_progress_t(progress_t& a_mono_thread_progress) : my_mono_thread_progress(&a_mono_thread_progress) {}

      void set_estimated_total_count(size_t a_count) { my_estimated_total_count = a_count; }
      void progress(size_t a_done_count);

      size_t estimated_total_count() const { return my_estimated_total_count; }
      size_t total_count_so_far() const { return my_total_count_so_far; }

   protected:
      void update_progress(size_t a_total_count_so_far);

   private:
      progress_t*          my_mono_thread_progress = nullptr;
      size_t               my_estimated_total_count = 0;
      std::atomic<size_t>  my_total_count_so_far = 0;
      std::mutex           my_mutex;
   };
}

#endif /* DAK_TANTRIX_MULTI_THREAD_PROGRESS_H */
