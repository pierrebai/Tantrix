#include "solution.h"

#include <algorithm>
#include <set>


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
   my_last_pos = a_pos;

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

solution_t solution_t::rotate(int rotation) const
{
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
