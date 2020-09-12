#pragma once

#ifndef DAK_TANTRIX_PUZZLE_H
#define DAK_TANTRIX_PUZZLE_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "position.h"
#include "tile.h"

#include <vector>
#include <map>


namespace dak::tantrix
{
   struct solution_t;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Puzzle, provide tiles and next position to try.
   //
   // Restrict the order in which tiles are selected.

   struct puzzle_t
   {
      using sub_puzzle = std::pair<tile_t, puzzle_t>;
      using tiles_by_color_t = std::map<color_t, std::vector<tile_t>>;
      using line_colors_t = std::vector<color_t>;

      // Create a puzzle.
      puzzle_t();
      puzzle_t(const std::vector<tile_t>& some_tiles, const std::vector<color_t>& some_line_colors, bool must_be_loops);

      // This is how the puzzle control the solver.
      // TODO: better document what the puzzle solver control do.
      std::vector<sub_puzzle> sub_puzzles() const;
      std::vector<position_t> get_next_positions(const solution_t& partial_solution, const position_t& a_last_add_pos, const tile_t& a_tile) const;
      bool has_more_sub_puzzles() const;
      bool is_solution_valid(const solution_t& a_solution) const;

      // The description of the puzzle.
      const tiles_by_color_t& tiles() const { return my_tiles; }
      const line_colors_t& line_colors() const { return my_line_colors; }
      size_t depth() const { return my_depth; }
      bool must_be_loops() const { return my_must_be_loops; }
      size_t estimated_total_count() const;

   private:
      line_colors_t     my_line_colors;
      tiles_by_color_t  my_tiles;
      size_t            my_depth = 0;
      bool              my_must_be_loops = false;
   };

}

#endif /* DAK_TANTRIX_PUZZLE_H */
