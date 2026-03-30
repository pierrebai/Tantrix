#include "dak/six_eight/solution.h"

#include <algorithm>
#include <set>
#include <map>
#include <exception>


namespace dak::six_eight
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Solution that places all the given tiles.
   //
   // We can:
   //    - Rotate a solution of a given amount (increment of quarter of a turn).
   //    - Check if a position is already occupied.
   //    - Check if a tile at a given position would be compatible with the solution.
   //    - Check if two solutions are the exactly the same.
   //    - Check if vector of solutions already contains a solution.
   //    - Add a solution if it is not already known.
   solution_t::solution_t()
   {
      memset(my_tiles_at_pos, 0, sizeof(my_tiles_at_pos));
   }

   void solution_t::add_tile(const tile_t& a_tile, const position_t& a_pos)
   {
      if (!is_compatible(a_tile, a_pos))
         return;
      my_tiles[my_tiles_count].pos = a_pos;
      my_tiles[my_tiles_count].tile = a_tile;
      my_tiles_count += 1;
      const auto& block_positions = a_tile.get_description().block_positions;
      for (const position_t& tile_block_pos : block_positions) {
         const position_t placed_block_pos = a_pos.move(tile_block_pos);
         my_tiles_at_pos[placed_block_pos.x()][placed_block_pos.y()] = a_tile.id();
      }
   }

   void solution_t::add_part(const part_t& a_part)
   {
      add_tile(a_part.tile, a_part.pos);
   }

   tile_t& solution_t::internal_tile_at(const position_t& a_pos) const
   {
      static tile_t invalid(0);

      if (a_pos.x() < 0 || a_pos.x() >= 6)
         return invalid;

      if (a_pos.y() < 0 || a_pos.y() >= 8)
         return invalid;
      
      const tile_t::id_t id = my_tiles_at_pos[a_pos.x()][a_pos.y()];
      if (id == 0)
         return invalid;

      for (size_t i = 0; i < my_tiles_count; ++i) {
         const part_t& placed_tile = my_tiles[i];
         if (placed_tile.tile.id() == id)
            return const_cast<tile_t &>(placed_tile.tile);
      }
      
      return invalid;
   }

   bool solution_t::is_occupied(const position_t& a_pos) const
   {
      if (a_pos.x() < 0 || a_pos.x() >= 6)
         return true;

      if (a_pos.y() < 0 || a_pos.y() >= 8)
         return true;
      
      return internal_tile_at(a_pos).id() != 0;
   }

   position_t solution_t::get_next_position_to_fill() const
   {
      for (int y = 0; y < 8; ++y) {
         for (int x = 0; x < 6; ++x) {
            position_t position_to_fill = position_t(x, y);
            if (!is_occupied(position_to_fill)) {
               return position_to_fill;
            }
         }
      }

      return position_t(-1000, -1000);
   }

   bool solution_t::is_compatible(const tile_t& a_tile, const position_t& a_pos) const
   {
      const auto& block_positions =  a_tile.get_description().block_positions;
      for (const position_t& tile_block_pos : block_positions) {
         const position_t placed_block_pos = a_pos.move(tile_block_pos);
         if (is_occupied(placed_block_pos))
            return false;
      }
      return true;
   }

   bool solution_t::is_compatible(const part_t& a_part) const
   {
      return is_compatible(a_part.tile, a_part.pos);
   }

   void solution_t::normalize()
   {
      std::sort(my_tiles, my_tiles + my_tiles_count);
      if (my_tiles_at_pos[0][0] > my_tiles_at_pos[5][7]) {
         tile_t::id_t new_tiles_at_pos[6][8];
         for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 6; ++x) {
               new_tiles_at_pos[x][y] = my_tiles_at_pos[5-x][7-y];
            }
         }
         std::memcpy(my_tiles_at_pos, new_tiles_at_pos, sizeof(my_tiles_at_pos));
      }
   }

   std::strong_ordering solution_t::operator<=>(const six_eight::solution_t& another_solution) const
   {
      int result = std::memcmp(my_tiles_at_pos, another_solution.my_tiles_at_pos, sizeof(my_tiles_at_pos));
      return result == 0 ? std::strong_ordering::equal
           : result > 0 ? std::strong_ordering::greater
           : std::strong_ordering::less;
   }

   void solution_t::add_similar_solution(const solution_t& /*another_solution*/)
   {
      // Similar solutions are actually identical for this puzzle type.
   }
   
   bool solution_t::is_valid() const
   {
      return my_tiles_count == 8;
   }
  
}
