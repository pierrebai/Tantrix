#pragma once

#ifndef DAK_TANTRIX_SOLUTION_H
#define DAK_TANTRIX_SOLUTION_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "position.h"
#include "tile.h"

#include <map>
#include <vector>

namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Solution that places all the given tiles.

   struct solution_t
   {
      using tiles_by_pos_t = tile_t[32][32];

      // Create a new solution.
      solution_t() = default;
      solution_t(const tile_t& a_tile, const position_t& a_pos)
      {
         add_tile(a_tile, a_pos);
      }

      // Access the solution tiles.
      const tile_t& tile_at(int x, int y) const { return my_tiles[x + 16][y + 16]; }
      const tile_t& tile_at(const position_t& a_pos) const { return tile_at(a_pos.x(), a_pos.y()); }

      tile_t& tile_at(int x, int y) { return my_tiles[x + 16][y + 16]; }
      tile_t& tile_at(const position_t& a_pos) { return tile_at(a_pos.x(), a_pos.y()); }

      // Add a tile to the solution.
      void add_tile(const tile_t& a_tile, const position_t& a_pos) { tile_at(a_pos) = a_tile; }

      // Get the positions outside the solution where they touch a color.
      std::vector<position_t> get_borders(const color_t& a_color) const;

      // Create a copy of this solution rotated of a given amount (increment of sixth of a turn).
      solution_t rotate(int rotation) const;

      // Rotate this solution in place of a given amount (increment of sixth of a turn).
      void rotate_in_place(int rotation, const position_t& new_center);

      // Normalize the solution so that any other identical solutions will
      // have the same set of positions and orientation.
      void normalize();

      // Check if a position is already occupied.
      bool is_occupied(const position_t& a_pos) const { return tile_at(a_pos).is_valid(); }

      // Check if a tile at a given position would be compatible with the solution.
      bool is_compatible(const tile_t& a_tile, const position_t a_pos) const;

      // Check if the solution has no invalid holes or borders.
      // (Hole with more than 3 sides or having more than two of the same color.)
      bool is_valid() const;

      // Check if the solution has a contiuous line of the given color.
      // (Including a loop.)
      bool has_line(const color_t& a_color) const;

      // Compare solutions.
      auto operator<=>(const solution_t& another_solution) const = default;

   private:
      tiles_by_pos_t my_tiles;
   };
}

#endif /* DAK_TANTRIX_SOLUTION_H */
