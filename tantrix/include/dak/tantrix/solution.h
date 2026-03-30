#pragma once

#ifndef DAK_TANTRIX_SOLUTION_H
#define DAK_TANTRIX_SOLUTION_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "dak/tantrix/puzzle.h"
#include "dak/tantrix/position.h"
#include "dak/tantrix/tile.h"
#include "dak/solver/solution.h"

#include <set>
#include <vector>
#include <optional>

namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Solution that places all the given tiles.

   struct solution_t : solver::solution_t
   {
      // Part of a solution
      struct part_t
      {
         position_t  pos;
         tile_t      tile;

         part_t() = default;
         part_t(tile_t tile, position_t pos) : pos(std::move(pos)), tile(std::move(tile)) {}
      
         // Compare placed tiles.
         auto operator<=>(const part_t&) const = default;
      };


      using tiles_by_pos_t = part_t[32];
      using hole_t = std::vector<position_t>;

      // Create a new solution.
      solution_t(const puzzle_t& a_puzzle);
      solution_t(const puzzle_t& a_puzzle, const tile_t& a_tile, const position_t& a_pos);

      // Access the solution tiles.
      const part_t* tiles() const { return my_tiles; }
      size_t tiles_count() const { return my_tiles_count; }

      const tile_t& tile_at(int x, int y) const { return tile_at(position_t(x, y)); }
      const tile_t& tile_at(const position_t& a_pos) const { return *internal_tile_at(a_pos); }

      tile_t& tile_at(int x, int y) { return tile_at(position_t(x, y)); }
      tile_t& tile_at(const position_t& a_pos) { return *internal_tile_at(a_pos); }

      // Add a tile to the solution.
      void add_tile(const tile_t& a_tile, const position_t& a_pos);

      // Add a part of the solution to this solution.
      void add_part(const part_t& a_part);

      // Get the positions outside the solution where they touch a color.
      std::vector<position_t> get_borders(const std::optional<color_t>& a_color = std::optional<color_t>()) const;

      // Create a copy of this solution rotated of a given amount (increment of sixth of a turn).
      solution_t rotate(int rotation) const;

      // Rotate this solution in place of a given amount (increment of sixth of a turn).
      solution_t& rotate_in_place(int rotation, const position_t& new_center);

      // Normalize the solution so that any other identical solutions will
      // have the same set of positions and orientation.
      void normalize();

      // Check if a position is already occupied.
      bool is_occupied(const position_t& a_pos) const;

      // Check if a tile at a given position would be compatible with the solution.
      bool is_compatible(const part_t& a_part) const;

      // Check if the solution has no invalid holes or borders.
      // (Hole with more than 3 sides or having more than two of the same color.)
      bool is_valid() const;

      // Nearing completion, stop spawning threads.
      bool is_almost_done(const puzzle_t& a_problem) const;

      // Check if the solution has a continuous line of the given color.
      bool has_line(const color_t& a_color, bool must_be_loop) const;

      // Count how many occupied positions surround a position.
      size_t count_neighbours(const position_t& a_pos) const;

      // Count how many similar solutions have been added to this solution.
      size_t count_similar_solutions() const { return my_similar_solutions_count; }

      // Counts how many fully-surrounded holes the solution has.
      std::vector<hole_t> get_holes() const;
      size_t count_holes() const;

      // Compare solutions.
      //
      // The comparison is based on the shape of the desired lines of the puzzle,
      // ignoring the other colors of the tiles. This is used to avoid having
      // multiple solutions that are the same but with irrelevant differences.
      std::strong_ordering operator<=>(const solution_t& another_solution) const;
      bool operator==(const solution_t& another_solution) const;
   
      // Check if this solution is exactly the same as another solution,
      // meaning that they have the same tiles at the same positions and orientations.
      std::strong_ordering is_identical(const solution_t& another_solution) const;

      // Add a similar solution to this solution.
      void add_similar_solution(const solution_t& another_solution);

   private:
      tile_t* internal_tile_at(const position_t& a_pos) const;

      size_t gather_line_positions(const color_t& a_color, position_t some_ends[32]) const;
      bool internal_fast_has_line(const color_t& a_color, bool must_be_loop) const;
      bool internal_slow_has_line(const color_t& a_color, bool must_be_loop) const;

      puzzle_t                   my_puzzle;
      size_t                     my_similar_solutions_count = 0;
      size_t                     my_tiles_count = 0;
      tiles_by_pos_t             my_tiles;
   };

   using all_solutions_t = std::set<solution_t>;
}

#endif /* DAK_TANTRIX_SOLUTION_H */
