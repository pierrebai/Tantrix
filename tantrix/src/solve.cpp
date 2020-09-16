#include "dak/tantrix/puzzle.h"
#include "dak/tantrix/solve.h"
#include "dak/utility/progress.h"
#include "dak/utility/multi_thread_progress.h"

#include <thread>
#include <future>

namespace dak::tantrix
{
   using multi_thread_progress_t = dak::utility::multi_thread_progress_t;
   using per_thread_progress_t = dak::utility::per_thread_progress_t;


   ////////////////////////////////////////////////////////////////////////////
   //
   // Keeper of all solutions.

   // Normalize and add a solution if it is not already known.
   static void add_solution(all_solutions_t& all_solutions, solution_t&& a_solution)
   {
      a_solution.normalize();
      all_solutions[a_solution] += 1;
   }

   // Add many solutions, we assume they have been already normalized.
   static void add_solutions(all_solutions_t& all_solutions, all_solutions_t&& other_solutions)
   {
      for (auto& [solution, count] : other_solutions)
         all_solutions[solution] += count;
   }


   ////////////////////////////////////////////////////////////////////////////
   //
   // Solve the placement of all given tiles.

   static void solve_partial(const puzzle_t& a_puzzle, const sub_puzzle_t& a_sub_puzzle, all_solutions_t& solutions, const solution_t& partial_solution, per_thread_progress_t* a_progress);

   static void solve_recursion(
      const puzzle_t& a_puzzle,
      const sub_puzzle_t& a_sub_puzzle,
      all_solutions_t& solutions,
      const solution_t& partial_solution,
      const tile_t& new_tile, position_t new_pos,
      per_thread_progress_t* a_progress)
   {
      solution_t new_partial = partial_solution;
      new_partial.add_tile(new_tile, new_pos);

      if (a_puzzle.has_more_sub_puzzles(a_sub_puzzle))
      {
         solve_partial(a_puzzle, a_sub_puzzle, solutions, new_partial, a_progress);
      }
      else if (a_puzzle.is_solution_valid(new_partial))
      {
         add_solution(solutions, std::move(new_partial));
      }
   }

   static void solve_sub_puzzle_with_tile(const puzzle_t& a_puzzle, const sub_puzzle_t& a_sub_puzzle, all_solutions_t& solutions, const solution_t& partial_solution, per_thread_progress_t* a_progress)
   {
      a_progress->progress(1);

      const auto next_positions = a_puzzle.get_next_positions(a_sub_puzzle, partial_solution);
      for (const auto new_pos : next_positions)
      {
         for (int selected_orientation = 0; selected_orientation < 6; ++selected_orientation)
         {
            const tile_t& tile = a_sub_puzzle.tile_to_place.rotate(selected_orientation);
            if (partial_solution.is_compatible(tile, new_pos))
            {
               solve_recursion(a_puzzle, a_sub_puzzle, solutions, partial_solution, tile, new_pos, a_progress);
            }
         }
      }
   }

   static all_solutions_t solve_sub_puzzle_with_tile_async(const puzzle_t& a_puzzle, const sub_puzzle_t& a_sub_puzzle, const solution_t& partial_solution, per_thread_progress_t a_progress)
   {
      all_solutions_t solutions;
      try
      {
         solve_sub_puzzle_with_tile(a_puzzle, a_sub_puzzle, solutions, partial_solution, &a_progress);
      }
      catch (const std::exception&)
      {
      }
      return solutions;
   }

   static void solve_partial(const puzzle_t& a_puzzle, const sub_puzzle_t& a_sub_puzzle, all_solutions_t& solutions, const solution_t& partial_solution, per_thread_progress_t* a_progress)
   {
      std::vector<std::future<all_solutions_t>> solutions_async;

      const auto sub_sub_puzzles = a_puzzle.create_sub_puzzles(a_sub_puzzle);
      for (const sub_puzzle_t& sub_sub_puzzle : sub_sub_puzzles)
      {
         if (sub_sub_puzzle.depth < 2)
         {
            auto new_async = std::async(std::launch::async, solve_sub_puzzle_with_tile_async, a_puzzle, sub_sub_puzzle, partial_solution, *a_progress);
            solutions_async.emplace_back(std::move(new_async));
         }
         else
         {
            solve_sub_puzzle_with_tile(a_puzzle, sub_sub_puzzle, solutions, partial_solution, a_progress);
         }
      }

      for (auto& new_solutions_async : solutions_async)
      {
         auto partial_solutions = new_solutions_async.get();
         add_solutions(solutions, std::move(partial_solutions));
      }
   }

   static all_solutions_t solve_sub_puzzle_async(const puzzle_t& a_puzzle, const sub_puzzle_t& a_sub_puzzle, const solution_t& partial_solution, per_thread_progress_t a_progress)
   {
      all_solutions_t solutions;
      try
      {
         solve_partial(a_puzzle, a_sub_puzzle, solutions, partial_solution, &a_progress);
      }
      catch (const std::exception&)
      {
      }
      return solutions;
   }

   all_solutions_t solve(const puzzle_t& a_puzzle, progress_t& a_progress)
   {
      all_solutions_t all_solutions;

      // Multi-thread the solution.
      std::vector<std::future<all_solutions_t>> solutions_async;

      // Protect the normal non-thread-safe progress against multi-threading.
      multi_thread_progress_t mt_progress(a_progress);

      // The first tile can be chosen arbitrarily and placed.
      // This will force the orientation of the solution, so
      // we won't have to compare with rotations or translations.

      for (const auto& sub_puzzle : a_puzzle.create_initial_sub_puzzles())
      {
         solution_t initial_partial_solution(sub_puzzle.tile_to_place, position_t(0, 0));
         auto new_async = std::async(std::launch::async, solve_sub_puzzle_async, a_puzzle, sub_puzzle, initial_partial_solution, per_thread_progress_t(mt_progress));
         solutions_async.emplace_back(std::move(new_async));

         // For loops, the line will use all tiles and starting from any tile is equivalent,
         // so we don't need to try all different first tiles!
         if (a_puzzle.must_be_loops())
            break;
      }

      for (auto& new_solutions_async : solutions_async)
      {
         auto partial_solutions = new_solutions_async.get();
         add_solutions(all_solutions, std::move(partial_solutions));
      }

      return all_solutions;
   }

}
