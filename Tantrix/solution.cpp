#include "solution.h"

#include <algorithm>
#include <set>


namespace dak::tantrix
{
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

   tile_t* solution_t::internal_tile_at(const position_t& a_pos) const
   {
      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         const placed_tile_t& placed_tile = my_tiles[i];
         if (placed_tile.pos == a_pos)
            return const_cast<tile_t*>(&placed_tile.tile);
      }

      return nullptr;
   }

   bool solution_t::is_occupied(const position_t& a_pos) const
   {
      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         const placed_tile_t& placed_tile = my_tiles[i];
         if (placed_tile.pos == a_pos)
            return true;
      }
      return false;
   }

   bool solution_t::is_compatible(const tile_t& a_tile, const position_t a_pos) const
   {
      if (is_occupied(a_pos))
         return false;

      for (const auto dir : directions)
      {
         const auto neighbour_pos = a_pos.move(dir);
         const tile_t* neighbour_tile = internal_tile_at(neighbour_pos);
         if (!neighbour_tile)
            continue;
         if (neighbour_tile->color(dir.rotate(3)) != a_tile.color(dir))
         {
            return false;
         }
      }
      return true;
   }

   solution_t& solution_t::rotate_in_place(int rotation, const position_t& new_center)
   {
      rotation = rotation % 6;
      if (!rotation)
         return *this;

      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         placed_tile_t& placed_tile = my_tiles[i];
         placed_tile.pos -= new_center;
         placed_tile.pos.rotate_in_place(rotation);
         placed_tile.tile.rotate_in_place(rotation);
      }

      return *this;
   }

   solution_t solution_t::rotate(int rotation) const
   {
      rotation = rotation % 6;
      if (!rotation)
         return *this;

      solution_t rotated(*this);

      for (auto& placed_tile : rotated.my_tiles)
      {
         placed_tile.pos.rotate_in_place(rotation);
         placed_tile.tile.rotate_in_place(rotation);
      }

      return rotated;
   }

   void solution_t::normalize()
   {
      position_t smallest_pos;
      tile_t* smallest_tile = nullptr;
      std::uint8_t smallest_number = 0;
      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         placed_tile_t& placed_tile = my_tiles[i];
         auto tile_number = placed_tile.tile.number();
         if (smallest_number == 0 || tile_number < smallest_number)
         {
            smallest_tile     = &placed_tile.tile;
            smallest_number   = tile_number;
            smallest_pos      = placed_tile.pos;
         }
      }

      int rotation = 0;
      tile_t target_tile(smallest_number);
      while (*smallest_tile != target_tile)
      {
         rotation += 5;
         target_tile.rotate_in_place(1);
      }

      rotate_in_place(rotation, smallest_pos);
   }

   std::vector<position_t> solution_t::get_borders(const color_t& a_color) const
   {
      std::vector<position_t> positions;
      positions.reserve(32);

      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         const placed_tile_t& placed_tile = my_tiles[i];
         for (const auto& dir : directions)
         {
            if (placed_tile.tile.color(dir) != a_color)
               continue;

            const position_t new_pos = placed_tile.pos.move(dir);
            if (is_occupied(new_pos))
               continue;

            positions.emplace_back(new_pos);
         }
      }

      std::sort(positions.begin(), positions.end());
      positions.erase(std::unique(positions.begin(), positions.end()), positions.end());

      return positions;
   }

   bool solution_t::has_line(const color_t& a_color) const
   {
      // Find a starting tile containg the color and the two directions where
      // the color are found on the tile.
      //
      // Also count the number of tiles with the color to be compared later.

      position_t start_pos;
      direction_t left_start_dir;
      direction_t right_start_dir;
      size_t expected_count = 0;
      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         const placed_tile_t& placed_tile = my_tiles[i];

         if (!placed_tile.tile.has_color(a_color))
            continue;

         if (!expected_count)
         {
            start_pos = placed_tile.pos;
            left_start_dir = placed_tile.tile.find_color(a_color, 0);
            right_start_dir = placed_tile.tile.find_color(a_color, left_start_dir.rotate(1));
         }

         expected_count += 1;
      }

      // Now find how many tiles form a continuous line.
      size_t found_count = 1;

      // Follow the line toward the left.
      position_t pos = start_pos;
      direction_t dir = left_start_dir;
      while (true)
      {
         pos = pos.move(dir);

         // Check for loops.
         if (pos == start_pos)
            return found_count == expected_count;

         // Check for line end.
         const tile_t* tile = internal_tile_at(pos);
         if (!tile)
            break;

         // Line is longer.
         found_count += 1;

         // Find second location of the color on the tile.
         // The connection point is at dir + 3, so search from dir + 4.
         dir = tile->find_color(a_color, dir.rotate(4));
      }

      // Follow the line toward the right.
      pos = start_pos;
      dir = right_start_dir;
      while (true)
      {
         pos = pos.move(dir);

         // Check for loops.
         if (pos == start_pos)
            return found_count == expected_count;

         // Check for line end.
         const tile_t* tile = internal_tile_at(pos);
         if (!tile)
            return found_count == expected_count;

         // Line is longer.
         found_count += 1;

         // Find second location of the color on the tile.
         // The connection point is at dir + 3, so search from dir + 4.
         dir = tile->find_color(a_color, dir.rotate(4));
      }

   }

   bool solution_t::is_valid() const
   {
      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         const placed_tile_t& placed_tile = my_tiles[i];

         int count_by_colors[4] = { 0, 0, 0, 0 };
         int neighbour_count = 0;
         for (direction_t dir : directions)
         {
            const auto new_pos = placed_tile.pos.move(dir);
            const tile_t* neighbour_tile = internal_tile_at(new_pos);
            if (!neighbour_tile)
               continue;

            const auto color = neighbour_tile->color(dir.rotate(3));
            count_by_colors[color.as_int()] += 1;

            if (count_by_colors[color.as_int()] > 2)
            return false;

            neighbour_count += 1;
            if (neighbour_count > 3)
               return false;
         }
      }

      return true;
   }
}
