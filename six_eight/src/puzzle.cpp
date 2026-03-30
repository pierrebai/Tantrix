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

   bool puzzle_t::is_valid() const
   {
      return my_initial_tiles.size() > 0;
   }

   // Create the initial list of sub-puzzles to solve.
   std::vector<puzzle_t::sub_problem_t> puzzle_t::create_initial_sub_problems() const
   {
      sub_problem_t sub_puzzle;
      sub_puzzle.tile_to_place = my_initial_tiles[0];
      sub_puzzle.other_tiles = my_initial_tiles;
      sub_puzzle.position_to_fill = position_t(0, 0);
      return create_sub_problems(sub_puzzle, solution_t());
   }

   std::vector<puzzle_t::sub_problem_t> puzzle_t::create_sub_problems(
      const sub_problem_t& a_current_sub_problem,
      const solution_t& a_partial_solution) const
   {
      if (a_current_sub_problem.other_tiles.size() <= 0) {
         // std::cout << "No more tiles for partial solution: " << a_partial_solution << std::endl;
         return {};
      }

      position_t position_to_fill = a_partial_solution.get_next_position_to_fill();
      if (a_partial_solution.is_occupied(position_to_fill)) {
         // std::cout << "Cannot find unoccupied position for partial solution: " << a_partial_solution << std::endl;
         return {};
      }

      std::vector<sub_problem_t> sub_puzzles;

      std::vector<tile_t> other_tiles(a_current_sub_problem.other_tiles);
      for (size_t i = 0; i < other_tiles.size(); ++i) {
         const int possible_rotations = other_tiles[0].get_description().possible_rotations;
         for (int rotation = 0; rotation < possible_rotations; ++rotation) {
            sub_problem_t sub_puzzle;
            sub_puzzle.tile_to_place = other_tiles[0].rotate(rotation);
            sub_puzzle.other_tiles = { other_tiles.begin() + 1, other_tiles.end() };
            sub_puzzle.position_to_fill = position_to_fill;
            sub_puzzles.emplace_back(sub_puzzle);
         }
         std::rotate(other_tiles.begin(), other_tiles.begin() + 1, other_tiles.end());
      }

      // std::cout << "Created " << sub_puzzles.size() << " sub puzzles with position " << position_to_fill << std::endl;
      // if (sub_puzzles.size() == 7)
      // {
      //    std::cout << *partial_solution << std::endl;
      // }

      return sub_puzzles;
   }

   // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.
   std::vector<solution_t::part_t> puzzle_t::get_sub_problem_potential_parts(
      const sub_problem_t& a_current_sub_problem,
      const solution_t& a_partial_solution) const
   {
      // std::cout << "Created part for " << current_sub_puzzle->tile_to_place.id() << " in\n" << *std::dynamic_pointer_cast<six_eight::solution_t>(a_partial_solution) << std::endl;

      return { solution_t::part_t(
         a_current_sub_problem.tile_to_place,
         a_current_sub_problem.position_to_fill) };

   }

   ////////////////////////////////////////////////////////////////////////////
   //
   // Verify if the solution satisfies the initial puzzle.

   bool puzzle_t::is_solution_valid(const solution_t& a_solution) const
   {
      return a_solution.tiles_count() == 8;
   }

   ////////////////////////////////////////////////////////////////////////////
   //
   // This is how the puzzle control the solver.
   //
   // Create the initial list of sub-puzzles to solve.
   // Create sub-puzzles from a given sub-puzzle that has its tile placed.
   // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.
   // Verify if there are more sub-puzzles to be created from the given sub-puzzle.

   bool puzzle_t::has_more_sub_problems(const sub_problem_t& a_current_sub_problem) const
   {
      return a_current_sub_problem.other_tiles.size() > 0;
   }

}
