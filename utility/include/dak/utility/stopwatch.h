#pragma once

#ifndef DAK_UTILITY_STOPWATCH_H
#define DAK_UTILITY_STOPWATCH_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <chrono>
#include <string>


namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Measure elapsed time.

   struct stopwatch_t
   {
      // Create a stopwatch to measure elapsed time.
      // Optionally will fill a buffer when elapsed time is measured.
      stopwatch_t(
         double* a_seconds_buffer,
         std::string* a_formatted_time_buffer,
         std::wstring* a_wide_formatted_time_buffer);

      stopwatch_t()                             : stopwatch_t(nullptr, nullptr, nullptr) {}
      stopwatch_t(double& a_seconds_buffer)     : stopwatch_t(&a_seconds_buffer, nullptr, nullptr) {}
      stopwatch_t(std::string& a_time_buffer)   : stopwatch_t(nullptr, &a_time_buffer, nullptr) {}
      stopwatch_t(std::wstring& a_time_buffer)  : stopwatch_t(nullptr, nullptr, &a_time_buffer) {}

      // Destroy the stopwatch. Do a final measures if not stopped.
      // Updates all linked buffers that were given in constructors if not previously stopped.
      ~stopwatch_t();

      // Start a new measurement of time.
      void start();

      // Stop all measures and return the elapsed time.
      // Updates all linked buffers that were given in constructors if not previously stopped.
      double stop();

      // Return the elapsed time so far but continue to measure time.
      // Updates all linked buffers that were given in constructors if not previously stopped.
      double elapsed();

   private:
      using clock_t = std::chrono::steady_clock;

      clock_t::time_point  my_begin_time;
      double               my_seconds = 0.;
      double*              my_seconds_buffer = nullptr;
      std::wstring*        my_wide_formatted_time = nullptr;
      std::string*         my_formatted_time = nullptr;
   };
}

#endif /* DAK_UTILITY_STOPWATCH_H */
