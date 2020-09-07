#include "puzzle.h"

#include <thread>
#include <future>

namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Keeper of all solutions.

   // Add a solution, *even* if it is a duplicate.
   static void add_solution(std::vector<solution_t>& all_solutions, solution_t&& a_solution)
   {
      all_solutions.emplace_back(a_solution);
   }

   // Add many solutions, *even* if they are duplicates.
   static void add_solutions(std::vector<solution_t>& all_solutions, all_solutions_t&& other_solutions)
   {
      all_solutions.insert(all_solutions.end(), other_solutions.begin(), other_solutions.end());
   }

   // Normalize and add a solution if it is not already known.
   static void add_solution(std::set<solution_t>& all_solutions, solution_t&& a_solution)
   {
      a_solution.normalize();
      all_solutions.insert(a_solution);
   }

   // Add many solutions, we assume they have been already normalized.
   static void add_solutions(std::set<solution_t>& all_solutions, all_solutions_t&& other_solutions)
   {
      all_solutions.merge(other_solutions);
   }


   ////////////////////////////////////////////////////////////////////////////
   //
   // Solve the placement of all given tiles.

   static void solve_partial(all_solutions_t& solutions, const solution_t& partial_solution, const puzzle_t& a_sub_puzzle);

   static void solve_recursion(
      all_solutions_t& solutions,
      const solution_t& partial_solution,
      const puzzle_t& a_sub_puzzle,
      const tile_t& new_tile, position_t new_pos)
   {
      solution_t new_partial = partial_solution;
      new_partial.add_tile(new_tile, new_pos);

      if (a_sub_puzzle.has_more_sub_puzzles())
      {
         solve_partial(solutions, new_partial, a_sub_puzzle);
      }
      else if (a_sub_puzzle.is_solution_valid(new_partial))
      {
         add_solution(solutions, std::move(new_partial));
      }
   }

   static all_solutions_t solve_sub_puzzle_with_tile(const solution_t& partial_solution, const puzzle_t& a_sub_puzzle, const tile_t& a_tile)
   {
      all_solutions_t solutions;

      const auto next_positions = a_sub_puzzle.get_next_positions(partial_solution, a_tile);
      for (const auto new_pos : next_positions)
      {
         for (int selected_orientation = 0; selected_orientation < 6; ++selected_orientation)
         {
            const tile_t& tile = a_tile.rotate(selected_orientation);
            if (partial_solution.is_compatible(tile, new_pos))
            {
               solve_recursion(solutions, partial_solution, a_sub_puzzle, tile, new_pos);
            }
         }
      }

      return solutions;
   }

   static void solve_partial(all_solutions_t& solutions, const solution_t& partial_solution, const puzzle_t& a_sub_puzzle)
   {
      std::vector<std::future<all_solutions_t>> solutions_async;

      const auto sub_sub_puzzles = a_sub_puzzle.sub_puzzles();
      for (const auto&[tile, sub_sub_puzzle] : sub_sub_puzzles)
      {
         if (a_sub_puzzle.depth() < 2)
         {
            auto new_async = std::async(std::launch::async, solve_sub_puzzle_with_tile, partial_solution, sub_sub_puzzle, tile);
            solutions_async.emplace_back(std::move(new_async));
         }
         else
         {
            auto partial_solutions = solve_sub_puzzle_with_tile(partial_solution, sub_sub_puzzle, tile);
            add_solutions(solutions, std::move(partial_solutions));
         }
      }

      for (auto& new_solutions_async : solutions_async)
      {
         auto partial_solutions = new_solutions_async.get();
         add_solutions(solutions, std::move(partial_solutions));
      }
   }

   static all_solutions_t solve_sub_puzzle(const puzzle_t& a_sub_puzzle, const tile_t& a_tile)
   {
      all_solutions_t solutions;

      solution_t partial_solution(a_tile, position_t(0, 0));
      solve_partial(solutions, partial_solution, a_sub_puzzle);

      return solutions;
   }

   all_solutions_t solve(const puzzle_t& a_puzzle)
   {
      // The first tile can be chosen arbitrarily and placed.
      // This will force the orientation of the solution, so
      // we won't have to compare with rotations or translations.
      const auto sub_puzzles = a_puzzle.sub_puzzles();

      std::vector<std::future<all_solutions_t>> solutions_async;

      all_solutions_t all_solutions;

      //for (const auto& [tile, puzzle] : sub_puzzles)
      //{
      //   auto partial_solutions = solve_sub_puzzle(puzzle, tile);
      //   add_solutions(all_solutions, std::move(partial_solutions));
      //}

      for (const auto& [tile, puzzle] : sub_puzzles)
      {
         auto new_async = std::async(std::launch::async, solve_sub_puzzle, puzzle, tile);
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