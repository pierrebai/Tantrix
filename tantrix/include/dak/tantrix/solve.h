#pragma once

#ifndef DAK_TANTRIX_SOLVE_H
#define DAK_TANTRIX_SOLVE_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "dak/tantrix/solution.h"

#include <set>
#include <vector>

namespace dak::utility
{
   struct progress_t;
}

namespace dak::tantrix
{
   struct puzzle_t;
   using progress_t = dak::utility::progress_t;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Solve the placement of all given tiles.

   using all_solutions_t = std::set<solution_t>;

   all_solutions_t solve(const puzzle_t& a_puzzle, progress_t& a_progress);
}

#endif /* DAK_TANTRIX_SOLVE_H */
