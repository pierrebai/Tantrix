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
      if (my_tiles.find(a_pos) != my_tiles.end())
         return false;

      for (const auto& [pos, tile] : my_tiles)
      {
         const auto dir = pos.relative(a_pos);
         if (dir.has_value())
         {
            if (tile.color(dir.value()) != a_tile.color(dir.value().rotate(3)))
            {
               return false;
            }
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
      size_t expected_count = 0;
      for (const auto& [pos, tile] : my_tiles)
      {
         if (!tile.has_color(a_color))
            continue;
         expected_count += 1;
         start_pos = pos;
      }

      std::set<position_t> seen;
      std::set<position_t> todo;
      todo.insert(start_pos);
      while (todo.size() > 0)
      {
         const auto pos = *todo.begin();
         todo.erase(todo.begin());
         seen.insert(pos);
         for (const auto& dir : directions)
         {
            const auto new_pos = pos.move(dir);
            if (seen.count(new_pos) > 0)
               continue;
            if (todo.count(new_pos) > 0)
               continue;
            if (my_tiles.find(pos)->second.color(dir) != a_color)
               continue;
            if (!is_occupied(new_pos))
               continue;
            if (!my_tiles.find(new_pos)->second.has_color(a_color))
               continue;
            todo.insert(new_pos);
         }
      }

      return seen.size() == expected_count;
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
