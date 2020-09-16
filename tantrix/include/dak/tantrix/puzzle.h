#pragma once

#ifndef DAK_TANTRIX_PUZZLE_H
#define DAK_TANTRIX_PUZZLE_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "dak/tantrix/position.h"
#include "dak/tantrix/tile.h"

#include <vector>


namespace dak::tantrix
{
   struct solution_t;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Sub puzzle

   struct sub_puzzle_t
   {
      tile_t               tile_to_place;
      std::vector<tile_t>  other_tiles;
      int                  right_sub_puzzles_count = 0;
      size_t               depth = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Puzzle, provide tiles and next position to try to the solver.

   struct puzzle_t
   {
      using tiles_t = std::vector<tile_t>;
      using line_colors_t = std::vector<color_t>;

      // Create a puzzle.
      puzzle_t();
      puzzle_t(const std::vector<tile_t>& some_tiles,
               const std::vector<color_t>& some_line_colors,
               bool must_be_loops);

      // Solver interaction.

      // Create the initial list of sub-puzzles to solve.
      std::vector<sub_puzzle_t> create_initial_sub_puzzles() const;

      // Create sub-puzzles from a given sub-puzzle that has its tile placed.
      std::vector<sub_puzzle_t> create_sub_puzzles(const sub_puzzle_t& a_current_sub_puzzle) const;

      // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.
      virtual std::vector<position_t> get_sub_puzzle_positions(const sub_puzzle_t& a_current_sub_puzzle, const solution_t& partial_solution) const = 0;

      // Verify if there are more sub-puzzles to be created from the given sub-puzzle.
      bool has_more_sub_puzzles(const sub_puzzle_t& a_current_sub_puzzle) const;

      // Verify if the solution satisfies the initial puzzle.
      virtual bool is_solution_valid(const solution_t& a_solution) const;

      // The description of the puzzle.

      // The list of tiles to place.
      const tiles_t& initial_tiles() const { return my_initial_tiles; }
      size_t initial_tiles_count() { return my_initial_tiles.size(); }

      // The list of lines or loops to form.
      const line_colors_t& line_colors() const { return my_line_colors; }

      // Check if loops must be formed, returns false for simple lines.
      bool must_be_loops() const { return my_must_be_loops; }

   protected:
      tiles_t           my_initial_tiles;
      line_colors_t     my_line_colors;
      bool              my_must_be_loops = false;
   };

}

#endif /* DAK_TANTRIX_PUZZLE_H */
