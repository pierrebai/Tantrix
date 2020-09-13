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
      all_solutions.insert(a_solution);
   }

   // Add many solutions, we assume they have been already normalized.
   static void add_solutions(all_solutions_t& all_solutions, all_solutions_t&& other_solutions)
   {
      all_solutions.merge(other_solutions);
   }


   ////////////////////////////////////////////////////////////////////////////
   //
   // Solve the placement of all given tiles.

   static void solve_partial(all_solutions_t& solutions, const solution_t& partial_solution, const puzzle_t& a_sub_puzzle, const position_t& a_last_add_pos, per_thread_progress_t* a_progress);

   static void solve_recursion(
      all_solutions_t& solutions,
      const solution_t& partial_solution,
      const puzzle_t& a_sub_puzzle,
      const tile_t& new_tile, position_t new_pos,
      per_thread_progress_t* a_progress)
   {
      solution_t new_partial = partial_solution;
      new_partial.add_tile(new_tile, new_pos);

      if (a_sub_puzzle.has_more_sub_puzzles())
      {
         solve_partial(solutions, new_partial, a_sub_puzzle, new_pos, a_progress);
      }
      else if (a_sub_puzzle.is_solution_valid(new_partial))
      {
         add_solution(solutions, std::move(new_partial));
      }
   }

   static void solve_sub_puzzle_with_tile(all_solutions_t& solutions, const solution_t& partial_solution, const puzzle_t& a_sub_puzzle, const position_t& a_last_add_pos, const tile_t& a_tile, per_thread_progress_t* a_progress)
   {
      a_progress->progress(1);

      const auto next_positions = a_sub_puzzle.get_next_positions(partial_solution, a_last_add_pos, a_tile);
      for (const auto new_pos : next_positions)
      {
         for (int selected_orientation = 0; selected_orientation < 6; ++selected_orientation)
         {
            const tile_t& tile = a_tile.rotate(selected_orientation);
            if (partial_solution.is_compatible(tile, new_pos))
            {
               solve_recursion(solutions, partial_solution, a_sub_puzzle, tile, new_pos, a_progress);
            }
         }
      }
   }

   static all_solutions_t solve_sub_puzzle_with_tile_async(const solution_t& partial_solution, const puzzle_t& a_sub_puzzle, const position_t& a_last_add_pos, const tile_t& a_tile, per_thread_progress_t a_progress)
   {
      all_solutions_t solutions;
      try
      {
         solve_sub_puzzle_with_tile(solutions, partial_solution, a_sub_puzzle, a_last_add_pos, a_tile, &a_progress);
      }
      catch (const std::exception&)
      {
      }
      return solutions;
   }

   static void solve_partial(all_solutions_t& solutions, const solution_t& partial_solution, const puzzle_t& a_sub_puzzle, const position_t& a_last_add_pos, per_thread_progress_t* a_progress)
   {
      std::vector<std::future<all_solutions_t>> solutions_async;

      const auto sub_sub_puzzles = a_sub_puzzle.sub_puzzles();
      for (const auto&[tile, sub_sub_puzzle] : sub_sub_puzzles)
      {
         if (a_sub_puzzle.depth() < 2)
         {
            auto new_async = std::async(std::launch::async, solve_sub_puzzle_with_tile_async, partial_solution, sub_sub_puzzle, a_last_add_pos, tile, *a_progress);
            solutions_async.emplace_back(std::move(new_async));
         }
         else
         {
            solve_sub_puzzle_with_tile(solutions, partial_solution, sub_sub_puzzle, a_last_add_pos, tile, a_progress);
         }
      }

      for (auto& new_solutions_async : solutions_async)
      {
         auto partial_solutions = new_solutions_async.get();
         add_solutions(solutions, std::move(partial_solutions));
      }
   }

   static all_solutions_t solve_sub_puzzle_async(const solution_t& partial_solution, const puzzle_t& a_sub_puzzle, const position_t& a_last_add_pos, per_thread_progress_t a_progress)
   {
      all_solutions_t solutions;
      try
      {
         solve_partial(solutions, partial_solution, a_sub_puzzle, a_last_add_pos, &a_progress);
      }
      catch (const std::exception&)
      {
      }
      return solutions;
   }

   all_solutions_t solve(const puzzle_t& a_puzzle, progress_t& a_progress)
   {
      a_progress.set_estimated_total_count(a_puzzle.estimated_total_count());

      // The first tile can be chosen arbitrarily and placed.
      // This will force the orientation of the solution, so
      // we won't have to compare with rotations or translations.
      const auto sub_puzzles = a_puzzle.sub_puzzles();

      std::vector<std::future<all_solutions_t>> solutions_async;

      all_solutions_t all_solutions;

      multi_thread_progress_t mt_progress(a_progress);

      //for (const auto& [tile, puzzle] : sub_puzzles)
      //{
      //   auto partial_solutions = solve_sub_puzzle(puzzle, position_t(0, 0), tile, a_progress);
      //   add_solutions(all_solutions, std::move(partial_solutions));
      //}

      for (const auto& [tile, puzzle] : sub_puzzles)
      {
         solution_t partial_solution(tile, position_t(0, 0));
         auto new_async = std::async(std::launch::async, solve_sub_puzzle_async, partial_solution, puzzle, position_t(0, 0), per_thread_progress_t(mt_progress));
         solutions_async.emplace_back(std::move(new_async));
      }

      for (auto& new_solutions_async : solutions_async)
      {
         auto partial_solutions = new_solutions_async.get();
         add_solutions(all_solutions, std::move(partial_solutions));
      }

      return all_solutions;
   }

}
