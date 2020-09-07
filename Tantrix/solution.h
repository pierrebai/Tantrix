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
      using tiles_by_pos_t = std::map<position_t, tile_t>;

      // Create a new solution.
      solution_t() = default;
      solution_t(const tile_t& a_tile, const position_t& a_pos)
      {
         add_tile(a_tile, a_pos);
      }

      // Access the solution tiles.
      const tiles_by_pos_t& tiles() const { return my_tiles; }

      // Add a tile to the solution.
      void add_tile(const tile_t& a_tile, const position_t& a_pos);

      // Get the position where the last tile was added.
      position_t last_add_pos() const { return my_last_pos; }

      // Get the positions outside the solution where they touch a color.
      std::vector<position_t> get_borders(const color_t& a_color) const;

      // Rotate a solution of a given amount (increment of sixth of a turn).
      solution_t rotate(int rotation) const;

      // Normalize the solution so that any other identical solutions will
      // have the same set of positions and orientation.
      void normalize();

      // Check if a position is already occupied.
      bool is_occupied(const position_t& a_pos) const;

      // Check if a tile at a given position would be compatible with the solution.
      bool is_compatible(const tile_t& a_tile, const position_t a_pos) const;

      // Check if two solutions are the the same, ignoring only translation.
      // (Not efficient. You're better off normalizing the solution and using ==)
      bool is_same_without_rotation(const solution_t& another_solution) const;

      // Check if two solutions are the the same, ignoring translation + rotation.
      // (Not efficient. You're better off normalizing the solution and using ==)
      bool is_same(const solution_t& another_solution) const;

      // Check if the solution has no invalid holes or borders.
      // (Hole with more than 3 sides or having more than two of the same color.)
      bool is_valid() const;

      // Check if it got rotated by normalization.
      bool is_rotated() const { return my_rotated; }

      // Check if the solution has a contiuous line of the given color.
      // (Including a loop.)
      bool has_line(const color_t& a_color) const;

      // Compare solutions.
      std::strong_ordering operator<=>(const solution_t& another_solution) const
      {
         if (my_tiles == another_solution.my_tiles)
            return std::strong_ordering::equal;
         if (my_tiles < another_solution.my_tiles)
            return std::strong_ordering::less;
         return std::strong_ordering::greater;
      }

      // Compare solutions.
      bool operator==(const solution_t& another_solution) const
      {
         return my_tiles == another_solution.my_tiles;
      }

   private:
      tiles_by_pos_t my_tiles;
      position_t my_last_pos;
      bool my_rotated = false;
   };

   struct solution_hash_t
   {
      std::size_t operator()(const solution_t& a_solution) const;
   };

   struct solution_is_same_t
   {
      bool operator()(const solution_t& lhs, const solution_t& rhs) const;
   };

}

#endif /* DAK_TANTRIX_SOLUTION_H */
