#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "position.h"
#include "tile.h"

#include <map>
#include <vector>

////////////////////////////////////////////////////////////////////////////
//
// Solution that places all the given tiles.
//
// We can:
//    - Rotate a solution of a given amount (increment of sixth of a turn).
//    - Check if a position is already occupied.
//    - Check if a tile at a given position would be compatible with the solution.
//    - Check if two solutions are the exactly the same.
//    - Check if vector of solutions already contains a solution.
//    - Add a solution if it is not already known.

struct solution_t
{
   solution_t() = default;
   solution_t(const tile_t& a_tile, const position_t& a_pos)
   {
      add_tile(a_tile, a_pos);
   }

   const std::map<position_t, tile_t>& tiles() const { return my_tiles; }
   void add_tile(const tile_t& a_tile, const position_t& a_pos);

   position_t last_add_pos() const { return my_last_pos; }

   std::vector<position_t> get_borders(const color_t& a_color) const;

   solution_t rotate(int rotation) const;

   bool is_occupied(const position_t& a_pos) const;
   bool is_compatible(const tile_t& a_tile, const position_t a_pos) const;

   bool has_line(const color_t& a_color) const;

   auto operator<=>(const solution_t& another_solution) const
   {
      if (my_tiles == another_solution.my_tiles)
         return std::strong_ordering::equal;
      if (my_tiles < another_solution.my_tiles)
         return std::strong_ordering::less;
      return std::strong_ordering::greater;
   }

private:
   std::map<position_t, tile_t> my_tiles;
   position_t my_last_pos;
};

