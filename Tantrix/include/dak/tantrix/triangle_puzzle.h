#pragma once

#ifndef DAK_TANTRIX_TRIANGLE_PUZZLE_H
#define DAK_TANTRIX_TRIANGLE_PUZZLE_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "dak/tantrix/puzzle.h"


namespace dak::tantrix
{
   struct solution_t;
   ////////////////////////////////////////////////////////////////////////////
   //
   // Puzzle of any shape.

   struct triangle_puzzle_t : puzzle_t
   {
      // Create a puzzle.
      triangle_puzzle_t() = default;
      triangle_puzzle_t(const std::vector<tile_t>& some_tiles,
         const std::vector<color_t>& some_line_colors,
         bool must_be_loops)
         : puzzle_t(some_tiles, some_line_colors, must_be_loops) {}

      // Solver interaction.

      // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.
      std::vector<position_t> get_sub_puzzle_positions(const sub_puzzle_t& a_current_sub_puzzle, const solution_t& partial_solution) const override;

      // Verify if the solution satisfies the initial puzzle.
      bool is_solution_valid(const solution_t& a_solution) const override;

   private:
      bool is_solution_correctly_shaped(const solution_t& a_solution) const;
   };

}

#endif /* DAK_TANTRIX_TRIANGLE_PUZZLE_H */
