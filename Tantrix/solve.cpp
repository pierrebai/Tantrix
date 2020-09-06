#include "puzzle.h"

#include <thread>
#include <future>


////////////////////////////////////////////////////////////////////////////
//
// Keeper of all solutions.

static void add_solution(std::vector<solution_t>& all_solutions, solution_t&& a_solution)
{
   all_solutions.emplace_back(a_solution);
}

static void add_solutions(std::vector<solution_t>& all_solutions, all_solutions_t&& other_solutions)
{
   all_solutions.insert(all_solutions.end(), other_solutions.begin(), other_solutions.end());
}

static void add_solution(std::set<solution_t>& all_solutions, solution_t&& a_solution)
{
   all_solutions.insert(a_solution);
}

static void add_solutions(std::set<solution_t>& all_solutions, all_solutions_t&& other_solutions)
{
   all_solutions.insert(other_solutions.begin(), other_solutions.end());
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

static void solve_sub_puzzle_with_tile(all_solutions_t& solutions, const solution_t& partial_solution, const puzzle_t& a_sub_puzzle, const tile_t& a_tile)
{
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
}

static void solve_partial(all_solutions_t& solutions, const solution_t& partial_solution, const puzzle_t& a_sub_puzzle)
{
   const auto sub_sub_puzzles = a_sub_puzzle.sub_puzzles();
   for (const auto&[tile, sub_sub_puzzle] : sub_sub_puzzles)
   {
      solve_sub_puzzle_with_tile(solutions, partial_solution, sub_sub_puzzle, tile);
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

   for (const auto&[tile, puzzle] : sub_puzzles)
   {
      auto new_async = std::async(std::launch::async, solve_sub_puzzle, puzzle, tile);
      solutions_async.emplace_back(std::move(new_async));
   }

   all_solutions_t all_solutions;

   for (auto& new_solutions_async : solutions_async)
   {
      auto partial_solutions = new_solutions_async.get();
      add_solutions(all_solutions, std::move(partial_solutions));
   }

   return all_solutions;
}

