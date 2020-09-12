#include "dak/tantrix/solution.h"

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
      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         const placed_tile_t& placed_tile = my_tiles[i];
         switch (a_pos.x() - placed_tile.pos.x())
         {
            case -1:
               switch (a_pos.y() - placed_tile.pos.y())
               {
                  case -1:
                     // Not a neighbour.
                     break;
                  case 0:
                     if (placed_tile.tile.color(0) != a_tile.color(direction_t(3)))
                        return false;
                     break;
                  case 1:
                     if (placed_tile.tile.color(5) != a_tile.color(direction_t(2)))
                        return false;
                     break;
               }
               break;
            case 0:
               switch (a_pos.y() - placed_tile.pos.y())
               {
                  case -1:
                     if (placed_tile.tile.color(1) != a_tile.color(direction_t(4)))
                        return false;
                     break;
                  case 0:
                     // Same location, always incompatible!
                     return false;
                  case 1:
                     if (placed_tile.tile.color(4) != a_tile.color(direction_t(1)))
                        return false;
                     break;
               }
               break;
            case 1:
               switch (a_pos.y() - placed_tile.pos.y())
               {
                  case -1:
                     if (placed_tile.tile.color(2) != a_tile.color(direction_t(5)))
                        return false;
                     break;
                  case 0:
                     if (placed_tile.tile.color(3) != a_tile.color(direction_t(0)))
                        return false;
                     break;
                  case 1:
                     // Not a neighbour.
                     break;
               }
               break;
         }
      }

      return true;
   }

   solution_t& solution_t::rotate_in_place(int rotation, const position_t& new_center)
   {
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

      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         placed_tile_t& placed_tile = rotated.my_tiles[i];
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
      std::sort(my_tiles, my_tiles + my_tiles_count);
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

   bool solution_t::has_line(const color_t& a_color, bool must_be_loop) const
   {
      // The idea for the algorithm is to expand the grid
      // and to record the junctions between tiles are grid point
      // instead of the tiles. Then we record the position of the
      // junctions with the tile position + direction of the junction.
      //
      // For a simple one-dimensional example, for the tile at
      // X coordinate 2 and 3, the coordinates become 4 and 6,
      // and the right of 4 is 5 and the left of 6 is 5. So 5
      // is recorded twice. When we have a pair, we know the line
      // is continuous there.
      //
      // We then simply have to count the number of discontinuities.
      // If more than 2, we have more than two ends, so more than 1 line.
      position_t ends[32];
      size_t ends_count = 0;

      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         const placed_tile_t& placed_tile = my_tiles[i];

         for (auto dir : directions)
         {
            if (placed_tile.tile.color(dir) != a_color)
               continue;

            ends[ends_count++] = position_t(
               placed_tile.pos.x() * 2 + dir.delta_x(),
               placed_tile.pos.y() * 2 + dir.delta_y());
         }
      }

      // No color at all.
      if (ends_count <= 0)
         return false;

      // This is the unfortunate expensive bit. 33% of the running time is spent here!
      std::sort(ends, ends + ends_count);

      size_t line_end_count = ends_count;
      for (size_t i = 1; i < ends_count; ++i)
      {
         if (ends[i - 1] == ends[i])
         {
            line_end_count -= 2;
         }
      }

      return line_end_count <= (must_be_loop ? 0 : 2);
   }

   bool solution_t::is_valid() const
   {
      using counts_by_color_t = std::map<color_t, int>;
      using colors_by_pos_t = std::map<position_t, counts_by_color_t>;

      colors_by_pos_t colors_by_pos;

      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         const placed_tile_t& placed_tile = my_tiles[i];
         for (direction_t dir : directions)
         {
            const auto new_pos = placed_tile.pos.move(dir);
            if (is_occupied(new_pos))
               continue;
            const auto color = placed_tile.tile.color(dir);
            colors_by_pos[new_pos][color] += 1;

            if (colors_by_pos[new_pos][color] > 2)
               return false;
         }
      }

      for (const auto& [pos, colors] : colors_by_pos)
      {
         int neighbour_count = 0;
         for (const auto& [color, count] : colors)
         {
            neighbour_count += count;
            if (neighbour_count > 3)
               return false;
         }
      }

      return true;
   }
}