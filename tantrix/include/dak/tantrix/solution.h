#pragma once

#ifndef DAK_TANTRIX_SOLUTION_H
#define DAK_TANTRIX_SOLUTION_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "dak/tantrix/position.h"
#include "dak/tantrix/tile.h"

#include <map>
#include <vector>

namespace dak::tantrix
{
   struct placed_tile_t
   {
      position_t  pos;
      tile_t      tile;
   
      // Compare placed tiles.
      auto operator<=>(const placed_tile_t&) const = default;
   };

   ////////////////////////////////////////////////////////////////////////////
   //
   // Solution that places all the given tiles.

   struct solution_t
   {
      using tiles_by_pos_t = placed_tile_t[16];

      // Create a new solution.
      solution_t() = default;
      solution_t(const tile_t& a_tile, const position_t& a_pos)
      {
         add_tile(a_tile, a_pos);
      }

      // Access the solution tiles.
      const placed_tile_t* tiles() const { return my_tiles; }
      size_t tiles_count() const { return my_tiles_count; }

      const tile_t& tile_at(int x, int y) const { return tile_at(position_t(x, y)); }
      const tile_t& tile_at(const position_t& a_pos) const { return *internal_tile_at(a_pos); }

      tile_t& tile_at(int x, int y) { return tile_at(position_t(x, y)); }
      tile_t& tile_at(const position_t& a_pos) { return *internal_tile_at(a_pos); }

      // Add a tile to the solution.
      void add_tile(const tile_t& a_tile, const position_t& a_pos) { my_tiles[my_tiles_count].pos = a_pos; my_tiles[my_tiles_count].tile = a_tile; my_tiles_count += 1; }

      // Get the positions outside the solution where they touch a color.
      std::vector<position_t> get_borders(const color_t& a_color) const;

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
      bool is_compatible(const tile_t& a_tile, const position_t a_pos) const;

      // Check if the solution has no invalid holes or borders.
      // (Hole with more than 3 sides or having more than two of the same color.)
      bool is_valid() const;

      // Check if the solution has a contiuous line of the given color.
      // (Including a loop.)
      bool has_line(const color_t& a_color, bool must_be_loop) const;

      // Compare solutions.
      auto operator<=>(const solution_t& another_solution) const = default;

   private:
      tile_t* internal_tile_at(const position_t& a_pos) const;

      bool internal_fast_has_line(const color_t& a_color, bool must_be_loop) const;
      bool internal_slow_has_line(const color_t& a_color, bool must_be_loop) const;

      size_t         my_tiles_count = 0;
      tiles_by_pos_t my_tiles;
   };
}

#endif /* DAK_TANTRIX_SOLUTION_H */
