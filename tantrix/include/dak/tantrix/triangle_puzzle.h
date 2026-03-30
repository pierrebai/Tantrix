#pragma once

#ifndef DAK_TANTRIX_TRIANGLE_PUZZLE_H
#define DAK_TANTRIX_TRIANGLE_PUZZLE_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "dak/tantrix/puzzle.h"
#include "dak/tantrix/solution.h"


namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Puzzle with tiles placed in a triangle.

   struct triangle_puzzle_t : puzzle_t
   {
      // Create a puzzle.
      triangle_puzzle_t() = default;
      triangle_puzzle_t(const std::vector<tile_t>& some_tiles,
                        const std::vector<color_t>& some_line_colors,
                        bool must_be_loops,
                        const maybe_size_t& a_holes_count = {});

      // Solver interaction.

      // Create the initial list of sub-puzzles to solve.
      std::vector<sub_problem_t> create_initial_sub_problems() const;

      // Create sub-puzzles from a given sub-puzzle that has its tile placed.
      std::vector<sub_problem_t> create_sub_problems(
         const sub_problem_t& a_current_sub_problem,
         const solution_t& a_partial_solution) const;

      // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.
      std::vector<solution_t::part_t> get_sub_problem_potential_parts(
         const sub_problem_t& a_current_sub_problem,
         const solution_t& a_partial_solution) const;

   private:
      position_t next_pyramid_position(
         const sub_problem_t& a_current_sub_puzzle,
         const solution_t& a_partial_solution) const;

      std::vector<position_t> my_pyramid_positions;
   };

}

#endif /* DAK_TANTRIX_TRIANGLE_PUZZLE_H */
