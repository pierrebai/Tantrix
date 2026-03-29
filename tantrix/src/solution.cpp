#include "dak/tantrix/solution.h"
#include "dak/tantrix/puzzle.h"

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

   solution_t::ptr_t solution_t::clone() const
   {
      return std::make_shared<solution_t>(*this);
   }

   // Compare solutions.
   std::strong_ordering solution_t::operator<=>(const solver::solution_t& another_solution) const
   {
      auto other = dynamic_cast<const tantrix::solution_t *>(&another_solution);
      if (!other)
         return std::strong_ordering::less;

      return *this <=> *other;
   }

   std::strong_ordering solution_t::operator<=>(const tantrix::solution_t& another_solution) const
   {
      auto order = (my_tiles_count <=> another_solution.my_tiles_count);
      if (order != std::strong_ordering::equal)
         return order;

      for (size_t i = 0; i < my_tiles_count; ++i) {
         order = (my_tiles[i].pos <=> another_solution.my_tiles[i].pos);
         if (order != std::strong_ordering::equal)
            return order;
      }

      for (size_t i = 0; i < my_tiles_count; ++i) {
         order = (my_tiles[i].tile <=> another_solution.my_tiles[i].tile);
         if (order != std::strong_ordering::equal)
            return order;
      }

      return std::strong_ordering::equal;
     
   }

   bool solution_t::operator==(const tantrix::solution_t& another_solution) const
   {
      return (*this <=> another_solution) == std::strong_ordering::equal;
   }

   void solution_t::add_similar_solution(const solver::solution_t::ptr_t& another_solution)
   {
      // TODO
   }
   
   void solution_t::add_tile(const tile_t& a_tile, const position_t& a_pos)
   {
      my_tiles[my_tiles_count].pos = a_pos;
      my_tiles[my_tiles_count].tile = a_tile;
      my_tiles_count += 1;
   }

   void solution_t::add_part(const solver::solution_part_t::ptr_t& a_part)
   {
      auto part = std::dynamic_pointer_cast<placed_tile_t>(a_part);
      if (!part)
         return;

      add_tile(part->tile, part->pos);
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

   bool solution_t::is_compatible(const solver::solution_part_t::ptr_t& a_part) const
   {
      auto part = std::dynamic_pointer_cast<placed_tile_t>(a_part);
      if (!part)
         return false;

      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         const placed_tile_t& placed_tile = my_tiles[i];
         switch (part->pos.x() - placed_tile.pos.x())
         {
            case -1:
               switch (part->pos.y() - placed_tile.pos.y())
               {
                  case -1:
                     // Not a neighbour.
                     break;
                  case 0:
                     if (placed_tile.tile.color(0) != part->tile.color(direction_t(3)))
                        return false;
                     break;
                  case 1:
                     if (placed_tile.tile.color(5) != part->tile.color(direction_t(2)))
                        return false;
                     break;
               }
               break;
            case 0:
               switch (part->pos.y() - placed_tile.pos.y())
               {
                  case -1:
                     if (placed_tile.tile.color(1) != part->tile.color(direction_t(4)))
                        return false;
                     break;
                  case 0:
                     // Same location, always incompatible!
                     return false;
                  case 1:
                     if (placed_tile.tile.color(4) != part->tile.color(direction_t(1)))
                        return false;
                     break;
               }
               break;
            case 1:
               switch (part->pos.y() - placed_tile.pos.y())
               {
                  case -1:
                     if (placed_tile.tile.color(2) != part->tile.color(direction_t(5)))
                        return false;
                     break;
                  case 0:
                     if (placed_tile.tile.color(3) != part->tile.color(direction_t(0)))
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

   static int calculate_placed_tiles_value(const solution_t::tiles_by_pos_t& some_placed_tiles, size_t a_count)
   {
      int value = 0;
      for (size_t i = 0; i < a_count; ++i) {
         const placed_tile_t& placed_tile = some_placed_tiles[i];
         value += placed_tile.pos.x() * 1000;
         value += placed_tile.pos.y();
      }
      return value;
   }

   void solution_t::normalize()
   {
      {
         const solution_t original_solution = *this;
         int lowest_rotation_value = 1000000;
         for (int rotation = 0; rotation < 6; ++rotation) {
            solution_t rot_solution = original_solution.rotate(rotation);

            std::sort(rot_solution.my_tiles, rot_solution.my_tiles + rot_solution.my_tiles_count);
            const position_t new_origin = rot_solution.my_tiles[0].pos;
            rot_solution.rotate_in_place(0, new_origin);

            int rot_value = calculate_placed_tiles_value(rot_solution.my_tiles, rot_solution.my_tiles_count);
            if (rot_value < lowest_rotation_value) {
               lowest_rotation_value = rot_value;
               *this = rot_solution;
            }
         }
      }

      std::sort(my_tiles, my_tiles + my_tiles_count);

      {
         const position_t new_origin = my_tiles[0].pos;
         rotate_in_place(0, new_origin);
      }
   }

   std::vector<position_t> solution_t::get_borders(const std::optional<color_t>& a_color) const
   {
      std::vector<position_t> positions;
      positions.reserve(32);

      for (size_t i = 0; i < my_tiles_count; ++i)
      {
         const placed_tile_t& placed_tile = my_tiles[i];
         for (const auto& dir : directions)
         {
            if (a_color.has_value() && placed_tile.tile.color(dir) != a_color.value())
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

   bool solution_t::is_almost_done(const solver::problem_t::ptr_t& a_problem) const
   {
      auto puzzle = std::dynamic_pointer_cast<puzzle_t>(a_problem);
      if (!puzzle)
         return true;
      return puzzle->initial_tiles().size() - my_tiles_count < 3;
   }
   
   size_t solution_t::count_neighbours(const position_t& a_pos) const
   {
      size_t count = 0;

      for (direction_t dir : directions)
         if (is_occupied(a_pos.move(dir)))
            count += 1;

      return count;
   }

   size_t solution_t::count_holes() const
   {
      return get_holes().size();
   }

   // Counts how many fully-surrounded holes the solution has.
   std::vector<solution_t::hole_t> solution_t::get_holes() const
   {
      std::vector<hole_t> holes;

      std::vector<position_t> borders = get_borders();

      size_t holes_count = 0;

      while (borders.size() > 0)
      {
         holes_count += 1;

         hole_t hole;
         hole.emplace_back(*borders.begin());
         borders.erase(borders.begin());

         // Expand the new hole to all touching borders.
         // Create a new vector of borders untouched by this hole.
         while (true)
         {
            bool hole_grew = false;
            const auto end = borders.end();
            std::vector<position_t> untouched_borders;
            untouched_borders.reserve(borders.size());
            for (auto todo_pos : borders)
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
                  untouched_borders.emplace_back(todo_pos);
               }
            }
            borders.swap(untouched_borders);
            if (!hole_grew)
               break;
         }

         holes.emplace_back(std::move(hole));
      }

      // The outside of the solution is counted as a hole, so remove it.
      // We can identify it because it is the largest hole.
      if (holes.size() > 0)
      {
         std::sort(holes.begin(), holes.end(), [](const hole_t& a, const hole_t& b) { return a.size() < b.size(); });
         holes.pop_back();
      }

      return holes;
   }
}
