#pragma once

#ifndef DAK_TANTRIX_PUZZLE_H
#define DAK_TANTRIX_PUZZLE_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "position.h"
#include "tile.h"
#include "solution.h"

#include <set>
#include <unordered_set>
#include <vector>


namespace dak::tantrix
{
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
      puzzle_t(const std::vector<tile_t>& some_tiles, const std::vector<color_t>& some_line_colors);

      // This is how the puzzle control the solver.
      // TODO: better document what the puzzle solver control do.
      std::vector<sub_puzzle> sub_puzzles() const;
      std::vector<position_t> get_next_positions(const solution_t& partial_solution, const tile_t& a_tile) const;
      bool has_more_sub_puzzles() const;
      bool is_solution_valid(const solution_t& a_solution) const;

      // The description of the puzzle.
      const tiles_by_color_t& tiles() const { return my_tiles; }
      const line_colors_t& line_colors() const { return my_line_colors; }

   private:
      line_colors_t     my_line_colors;
      tiles_by_color_t  my_tiles;
   };


   ////////////////////////////////////////////////////////////////////////////
   //
   // Solve the placement of all given tiles.

   //using all_solutions_t = std::vector<solution_t>;
   //using all_solutions_t = std::unordered_set<solution_t, solution_hash_t, solution_is_same_t>;
   using all_solutions_t = std::set<solution_t>;

   all_solutions_t solve(const puzzle_t& a_puzzle);
   all_solutions_t solve_genius_puzzle();
}

#endif /* DAK_TANTRIX_PUZZLE_H */
