#pragma once

#ifndef DAK_TANTRIX_ANY_SHAPE_PUZZLE_H
#define DAK_TANTRIX_ANY_SHAPE_PUZZLE_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "dak/tantrix/puzzle.h"


namespace dak::tantrix
{
   struct solution_t;
   ////////////////////////////////////////////////////////////////////////////
   //
   // Puzzle of any shape.

   struct any_shape_puzzle_t : puzzle_t
   {
      // Create a puzzle.
      any_shape_puzzle_t() = default;
      any_shape_puzzle_t(const std::vector<tile_t>& some_tiles,
         const std::vector<color_t>& some_line_colors,
         bool must_be_loops,
         const maybe_size_t& a_holes_count = {})
         : puzzle_t(some_tiles, some_line_colors, must_be_loops, a_holes_count) {}

      // Solver interaction.

      // Create the initial list of sub-puzzles to solve.
      std::vector<sub_puzzle_t> create_initial_sub_puzzles() const override;

      // Create sub-puzzles from a given sub-puzzle that has its tile placed.
      std::vector<sub_puzzle_t> create_sub_puzzles(const sub_puzzle_t& a_current_sub_puzzle, const solution_t& a_partial_solution) const override;

      // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.
      std::vector<position_t> get_sub_puzzle_positions(const sub_puzzle_t& a_current_sub_puzzle, const solution_t& a_partial_solution) const override;
   };

}

#endif /* DAK_TANTRIX_ANY_SHAPE_PUZZLE_H */
