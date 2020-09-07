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

   void solution_t::add_tile(const tile_t& a_tile, const position_t& a_pos)
   {
      my_tiles[a_pos] = a_tile;
      my_last_pos     = a_pos;
   }

   bool solution_t::is_occupied(const position_t& a_pos) const
   {
      return my_tiles.find(a_pos) != my_tiles.end();
   }

   bool solution_t::is_compatible(const tile_t& a_tile, const position_t a_pos) const
   {
      const auto my_tiles_end = my_tiles.end();
      if (my_tiles.find(a_pos) != my_tiles_end)
         return false;

      for (const auto dir : directions)
      {
         const auto neighbour = a_pos.move(dir);
         const auto iter = my_tiles.find(neighbour);
         if (iter == my_tiles_end)
            continue;
         if (iter->second.color(dir.rotate(3)) != a_tile.color(dir))
         {
            return false;
         }
      }
      return true;
   }

   bool solution_t::is_same_without_rotation(const solution_t& another_solution) const
   {
      if (my_tiles.size() == 0)
         return another_solution.my_tiles.size() == 0;

      const auto my_first_pos    = my_tiles.begin()->first;
      const auto other_first_pos = another_solution.my_tiles.begin()->first;

      const auto delta_x = other_first_pos.x() - my_first_pos.x();
      const auto delta_y = other_first_pos.y() - my_first_pos.y();

      for (const auto& [pos, tile] : my_tiles)
      {
         const auto other_pos = pos.move(delta_x, delta_y);
         const auto other_tile = another_solution.my_tiles.find(other_pos);
         if (other_tile == another_solution.my_tiles.end())
            return false;

         if (other_tile->second != tile)
            return false;
      }

      return true;
   }

   bool solution_t::is_same(const solution_t& another_solution) const
   {
      for (int rotation = 0; rotation < 6; ++rotation)
         if (is_same_without_rotation(another_solution.rotate(rotation)))
            return true;
      return false;
   }

   solution_t solution_t::rotate(int rotation) const
   {
      rotation = rotation % 6;
      if (!rotation)
         return *this;

      solution_t rotated;

      for (const auto& [pos, tile] : my_tiles)
         rotated.my_tiles[pos.rotate(rotation)] = tile.rotate(rotation);

      return rotated;
   }

   void solution_t::normalize()
   {
      my_last_pos = position_t();
      if (my_tiles.size() <= 0)
         return;

      auto smallest_tile   = &my_tiles.begin()->second;
      auto smallest_number = smallest_tile->number();
      auto smallest_pos    = my_tiles.begin()->first;
      for (auto& [pos, tile] : my_tiles)
      {
         auto tile_number = tile.number();
         if (tile_number < smallest_number)
         {
            smallest_tile     = &tile;
            smallest_number   = tile_number;
            smallest_pos      = pos;
         }
      }

      int rotation = 0;
      tile_t target_tile(smallest_number);
      while (*smallest_tile != target_tile)
      {
         rotation += 5;
         target_tile.rotate_in_place(1);
      }

      tiles_by_pos_t new_tiles;

      for (auto& [pos, tile] : my_tiles)
      {
         const position_t new_pos(pos.x() - smallest_pos.x(), pos.y() - smallest_pos.y());
         const position_t rot_pos = new_pos.rotate(rotation);
         new_tiles[rot_pos] = tile.rotate(rotation);
      }

      my_tiles.swap(new_tiles);
   }

   std::vector<position_t> solution_t::get_borders(const color_t& a_color) const
   {
      std::vector<position_t> positions;

      for (const auto& [pos, tile] : my_tiles)
      {
         for (const auto& dir : directions)
         {
            if (tile.color(dir) != a_color)
               continue;

            const auto new_pos = pos.move(dir);
            if (is_occupied(new_pos))
               continue;

            positions.push_back(new_pos);
         }
      }

      std::sort(positions.begin(), positions.end());
      positions.erase(std::unique(positions.begin(), positions.end()), positions.end());

      return positions;
   }

   bool solution_t::has_line(const color_t& a_color) const
   {
      position_t start_pos;
      direction_t left_start_dir;
      direction_t right_start_dir;
      size_t expected_count = 0;
      for (const auto& [pos, tile] : my_tiles)
      {
         if (!tile.has_color(a_color))
            continue;

         if (!expected_count)
         {
            start_pos = pos;
            left_start_dir = tile.find_color(a_color, 0);
            right_start_dir = tile.find_color(a_color, left_start_dir.rotate(1));
         }

         expected_count += 1;
      }

      const auto my_tiles_end = my_tiles.end();
      size_t found_count = 1;

      position_t pos = start_pos;
      direction_t dir = left_start_dir;
      while (true)
      {
         pos = pos.move(dir);

         // Check for loops.
         if (pos == start_pos)
            return found_count == expected_count;

         // Check for line end.
         const auto iter = my_tiles.find(pos);
         if (iter == my_tiles_end)
            break;

         // Line is longer.
         found_count += 1;
         const tile_t& tile = iter->second;
         dir = tile.find_color(a_color, dir.rotate(4));
      }

      pos = start_pos;
      dir = right_start_dir;
      while (true)
      {
         pos = pos.move(dir);

         // Check for loops.
         if (pos == start_pos)
            return found_count == expected_count;

         // Check for line end.
         const auto iter = my_tiles.find(pos);
         if (iter == my_tiles_end)
            return found_count == expected_count;

         // Line is longer.
         found_count += 1;
         const tile_t& tile = iter->second;
         dir = tile.find_color(a_color, dir.rotate(4));
      }

   }

   std::size_t solution_hash_t::operator()(const solution_t& a_solution) const
   {
      std::size_t hash = 0;

      for (const auto& [pos, tile] : a_solution.tiles())
      {
         hash += (tile.number() * 2) * (pos.x() + 371) * 1483;
         hash += pos.y() * 78653;
      }

      return hash;
   }

   bool solution_is_same_t::operator()(const solution_t& lhs, const solution_t& rhs) const
   {
      return lhs.is_same(rhs);
   }
}
