#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "dak/six_eight/solution.h"
#include "dak/six_eight/position.h"
#include "dak/six_eight/tile.h"
#include <dak/solver/problem.h>

#include <algorithm>
#include <string>
#include <vector>


namespace dak::six_eight
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Puzzle, provide tiles and next position to try to the solver.

   struct puzzle_t : solver::problem_t
   {
      // Sub puzzle

      struct sub_problem_t
      {
         tile_t               tile_to_place;
         std::vector<tile_t>  other_tiles;
         position_t           position_to_fill;
      };

      using tiles_t = std::vector<tile_t>;

      // Create a puzzle.
      puzzle_t();
      puzzle_t(const std::vector<tile_t>& some_tiles);

      // Verify if the problem is valid.
      bool is_valid() const;

      // Solver interaction.

      // Create the initial list of sub-puzzles to solve.
      std::vector<sub_problem_t> create_initial_sub_problems() const;

      // Create sub-puzzles from a given sub-puzzle that has its tile placed.
      std::vector<sub_problem_t> create_sub_problems(
         const sub_problem_t& a_current_sub_problem,
         const solution_t& a_partial_solution) const;

      // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.
      virtual std::vector<solution_t::part_t> get_sub_problem_potential_parts(
         const sub_problem_t& a_current_sub_problem,
         const solution_t& a_partial_solution) const;

      // Verify if there are more sub-puzzles to be created from the given sub-puzzle.
      bool has_more_sub_problems(const sub_problem_t& a_current_sub_problem) const;

      // Verify if the solution satisfies the initial puzzle.
      bool is_solution_valid(const solution_t& a_solution) const;

      // The description of the puzzle.

      // The list of tiles to place.
      const tiles_t& initial_tiles() const { return my_initial_tiles; }
      const tile_t* get_tile(const std::string) const;

   protected:
      tiles_t my_initial_tiles;
   };

}

