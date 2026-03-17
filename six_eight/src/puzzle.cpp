#include "dak/six_eight/puzzle.h"
#include "dak/six_eight/solution.h"
#include "dak/six_eight/stream.h"

#include <set>
#include <iostream>


namespace dak::six_eight
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Puzzle, provide tiles and next position to try.

   puzzle_t::puzzle_t()
   {
   }

   puzzle_t::puzzle_t(const std::vector<tile_t>& some_tiles)
   : my_initial_tiles(some_tiles)
   {
   }

   // Create the initial list of sub-puzzles to solve.
   std::vector<solver::sub_problem_t::ptr_t> puzzle_t::create_initial_sub_problems() const
   {
      sub_puzzle_t sub_puzzle;
      sub_puzzle.tile_to_place = my_initial_tiles[0];
      sub_puzzle.other_tiles = my_initial_tiles;
      sub_puzzle.position_to_fill = position_t(0, 0);
      return create_sub_problems(
         std::make_shared<sub_puzzle_t>(sub_puzzle),
         std::make_shared<solution_t>());
   }

   std::vector<solver::sub_problem_t::ptr_t> puzzle_t::create_sub_problems(
      const solver::sub_problem_t::ptr_t& a_current_sub_problem,
      const solver::solution_t::ptr_t& a_partial_solution) const
   {
      auto current_sub_puzzle = std::dynamic_pointer_cast<sub_puzzle_t>(a_current_sub_problem);

      if (current_sub_puzzle->other_tiles.size() <= 0) {
         // std::cout << "No more tiles for partial solution: " << a_partial_solution << std::endl;
         return {};
      }

      auto partial_solution = std::dynamic_pointer_cast<six_eight::solution_t>(a_partial_solution);
      if (!partial_solution)
         return {};

      position_t position_to_fill = partial_solution->get_next_position_to_fill();
      if (partial_solution->is_occupied(position_to_fill)) {
         // std::cout << "Cannot find unoccupied position for partial solution: " << a_partial_solution << std::endl;
         return {};
      }

      std::vector<solver::sub_problem_t::ptr_t> sub_puzzles;

      std::vector<tile_t> other_tiles(current_sub_puzzle->other_tiles);
      for (size_t i = 0; i < other_tiles.size(); ++i) {
         const tile_t& tile_to_place = other_tiles[0];
         const int possible_rotations = tile_to_place.get_description().possible_rotations;
         for (int rotation = 0; rotation < possible_rotations; ++rotation) {
            sub_puzzle_t sub_puzzle;
            sub_puzzle.tile_to_place = other_tiles[0].rotate(rotation);
            sub_puzzle.other_tiles = { other_tiles.begin() + 1, other_tiles.end() };
            sub_puzzle.position_to_fill = position_to_fill;
            sub_puzzles.emplace_back(std::make_shared<sub_puzzle_t>(sub_puzzle));
         }
         std::rotate(other_tiles.begin(), other_tiles.begin() + 1, other_tiles.end());
      }

      // std::cout << "Created " << sub_puzzles.size() << " sub puzzles with position " << position_to_fill << std::endl;
      // if (sub_puzzles.size() == 1) {
      //    std::cout << a_partial_solution << std::endl;
      // }

      return sub_puzzles;
   }

   // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.
   std::vector<solver::solution_part_t::ptr_t> puzzle_t::get_sub_problem_potential_parts(
      const solver::sub_problem_t::ptr_t& a_current_sub_problem,
      const solver::solution_t::ptr_t& a_partial_solution) const
   {
      auto current_sub_puzzle = std::dynamic_pointer_cast<sub_puzzle_t>(a_current_sub_problem);
      if (!current_sub_puzzle)
         return {};
      // std::cout << "Sub-puzzle position to fill " << current_sub_puzzle->position_to_fill << std::endl;
      return { placed_tile_t::make(current_sub_puzzle->tile_to_place, current_sub_puzzle->position_to_fill) };
   }

   ////////////////////////////////////////////////////////////////////////////
   //
   // Verify if the solution satisfies the initial puzzle.

   bool puzzle_t::is_solution_valid(const solver::solution_t::ptr_t& a_solution) const
   {
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   //
   // This is how the puzzle control the solver.
   //
   // Create the initial list of sub-puzzles to solve.
   // Create sub-puzzles from a given sub-puzzle that has its tile placed.
   // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.
   // Verify if there are more sub-puzzles to be created from the given sub-puzzle.

   bool puzzle_t::has_more_sub_problems(const solver::sub_problem_t::ptr_t& a_current_sub_problem) const
   {
      auto sub_puzzle = std::dynamic_pointer_cast<sub_puzzle_t>(a_current_sub_problem);
      return sub_puzzle->other_tiles.size() > 0;
   }

}
