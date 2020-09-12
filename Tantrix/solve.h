#pragma once

#ifndef DAK_TANTRIX_SOLVE_H
#define DAK_TANTRIX_SOLVE_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "solution.h"

#include <set>
#include <vector>
#include <atomic>

namespace dak::tantrix
{
   struct puzzle_t;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Report progress on the solution.

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
      std::atomic<size_t> my_total_count_so_far = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Solve the placement of all given tiles.

   using all_solutions_t = std::set<solution_t>;

   all_solutions_t solve(const puzzle_t& a_puzzle, progress_t& a_progress);
}

#endif /* DAK_TANTRIX_SOLVE_H */
