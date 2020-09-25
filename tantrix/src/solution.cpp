#include "dak/tantrix/solution.h"

#include <algorithm>
#include <set>
#include <map>


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

   void solution_t::add_tile(const tile_t& a_tile, const position_t& a_pos)
   {
      my_tiles[my_tiles_count].pos = a_pos;
      my_tiles[my_tiles_count].tile = a_tile;
      my_tiles_count += 1;
   }

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
      // The first algorithm can reject having no line or too many lines,
      // but it cannot detect extra loops. The second algorithm is slower
      // but fixes that. This avoids using the more accurate but slower
      // algorithm for the common case of rejecting the solution.
      return internal_fast_has_line(a_color, must_be_loop)
          && internal_slow_has_line(a_color, must_be_loop);
   }

   bool solution_t::internal_fast_has_line(const color_t& a_color, bool must_be_loop) const
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

      return line_end_count == (must_be_loop ? 0 : 2);
   }

   bool solution_t::internal_slow_has_line(const color_t& a_color, bool must_be_loop) const
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
   
   size_t solution_t::count_neighbours(const position_t& a_pos) const
   {
      size_t count = 0;

      for (direction_t dir : directions)
         if (is_occupied(a_pos.move(dir)))
            count += 1;

      return count;
   }

   // Counts how many fully-surrounded holes the solution has.
   size_t solution_t::count_holes() const
   {
      std::vector<position_t> todo;
      todo.reserve(32);

      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         const placed_tile_t& placed_tile = my_tiles[i];
         for (const auto& dir : directions)
         {
            const position_t new_pos = placed_tile.pos.move(dir);
            if (is_occupied(new_pos))
               continue;

            todo.emplace_back(new_pos);
         }
      }

      std::sort(todo.begin(), todo.end());
      todo.erase(std::unique(todo.begin(), todo.end()), todo.end());

      size_t holes_count = 0;

      while (todo.size() > 0)
      {
         holes_count += 1;

         std::vector<position_t> hole;
         hole.emplace_back(*todo.begin());
         todo.erase(todo.begin());

         while (true)
         {
            bool hole_grew = false;
            const auto end = todo.end();
            std::vector<position_t> new_todo;
            new_todo.reserve(todo.size());
            for (auto todo_pos : todo)
            {
               bool in_hole = false;
               for (auto pos : hole)
               {
                  if (pos.relative(todo_pos).has_value())
                  {
                     in_hole = true;
                     break;
                  }
               }

               if (in_hole)
               {
                  hole.emplace_back(todo_pos);
                  hole_grew = true;
               }
               else
               {
                  new_todo.emplace_back(todo_pos);
               }
            }
            todo.swap(new_todo);
            if (!hole_grew)
               break;
         }
      }

      // The outside of the solution is counted as a hole, so remove it.
      return holes_count - 1;
   }
}
