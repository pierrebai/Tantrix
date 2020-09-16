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
   // Puzzle, provide tiles and next position to try.
   //
   // Restrict the order in which tiles are selected.

   struct puzzle_t
   {
      using tiles_t = std::vector<tile_t>;
      using line_colors_t = std::vector<color_t>;

      // The shape the solution of the puzzle must have.
      enum class shape_t
      {
         any,
         triangle
      };

      // Create a puzzle.
      puzzle_t();
      puzzle_t(const std::vector<tile_t>& some_tiles,
               const std::vector<color_t>& some_line_colors,
               bool must_be_loops, shape_t a_shape = shape_t::any);

      // This is how the puzzle control the solver.
      // TODO: better document what the puzzle solver control do.
      std::vector<sub_puzzle_t> create_initial_sub_puzzles() const;
      std::vector<sub_puzzle_t> create_sub_puzzles(const sub_puzzle_t& a_current_sub_puzzle) const;
      std::vector<position_t> get_next_positions(const sub_puzzle_t& a_current_sub_puzzle, const solution_t& partial_solution) const;
      bool has_more_sub_puzzles(const sub_puzzle_t& a_current_sub_puzzle) const;
      bool is_solution_valid(const solution_t& a_solution) const;

      // The description of the puzzle.
      const tiles_t& initial_tiles() const { return my_initial_tiles; }
      size_t initial_tiles_count() { return my_initial_tiles.size(); }
      const line_colors_t& line_colors() const { return my_line_colors; }
      bool must_be_loops() const { return my_must_be_loops; }
      shape_t shape() const { return my_shape; }

   private:
      bool is_solution_correctly_shaped(const solution_t& a_solution) const;

      tiles_t           my_initial_tiles;
      line_colors_t     my_line_colors;
      bool              my_must_be_loops = false;
      int               my_initial_right_sub_puzzles_count = 0;
      shape_t           my_shape = shape_t::any;
   };

}

#endif /* DAK_TANTRIX_PUZZLE_H */
