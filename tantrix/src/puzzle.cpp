#include "dak/tantrix/puzzle.h"
#include "dak/tantrix/solution.h"

#include <set>


namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Puzzle, provide tiles and next position to try.
   //
   // Restrict the order in which tiles are selected.

   puzzle_t::puzzle_t()
   {
   }

   puzzle_t::puzzle_t(const std::vector<tile_t>& some_tiles,
                      const std::vector<color_t>& some_line_colors,
                      bool must_be_loops, shape_t a_shape)
      : my_line_colors(some_line_colors), my_must_be_loops(must_be_loops), my_shape(a_shape)
   {
      if (!some_line_colors.size())
         throw std::exception("invalid puzzle: no required line colors provided");

      std::set<tile_t> done_tiles;
      for (const auto color : some_line_colors)
      {
         for (const auto& tile : some_tiles)
         {
            if (done_tiles.contains(tile))
               continue;

            if (tile.has_color(color))
            {
               my_initial_tiles.emplace_back(tile);
               done_tiles.insert(tile);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   //
   // SOlution validation.

   bool puzzle_t::is_solution_valid(const solution_t& a_solution) const
   {
      for (const auto& color : my_line_colors)
         if (!a_solution.has_line(color, my_must_be_loops))
            return false;
      return is_solution_correctly_shaped(a_solution);
   }

   bool puzzle_t::is_solution_correctly_shaped(const solution_t& a_solution) const
   {
      if (my_shape == shape_t::any)
         return true;

      const size_t tiles_count = a_solution.tiles_count();

      position_t low_pos;
      for (int i = 0; i < tiles_count; ++i)
      {
         const auto& pos = a_solution.tiles()[i].pos;
         if (pos <= low_pos)
            low_pos = pos;
      }

      int pyramid_width = 0;
      if (tiles_count <= 3)
         pyramid_width = 2;
      else if (tiles_count <= 6)
         pyramid_width = 3;
      else if (tiles_count <= 10)
         pyramid_width = 4;
      else if (tiles_count <= 15)
         pyramid_width = 5;
      else if (tiles_count <= 21)
         pyramid_width = 6;
      else if (tiles_count <= 28)
         pyramid_width = 7;
      else if (tiles_count <= 36)
         pyramid_width = 8;
      else if (tiles_count <= 45)
         pyramid_width = 9;
      else
         pyramid_width = 10;


      bool up_pyramid = true;
      bool down_pyramid = true;
      for (int delta_y = 0; delta_y < pyramid_width; ++delta_y)
      {
         for (int delta_x = 0; delta_x < pyramid_width - delta_y; ++delta_x)
         {
            if (!a_solution.is_occupied(low_pos.move(delta_x, delta_y)))
               down_pyramid = false;
            if (!a_solution.is_occupied(low_pos.move(delta_x + delta_y, -delta_y)))
               up_pyramid = false;
         }
      }

      return up_pyramid | down_pyramid;
   }

   ////////////////////////////////////////////////////////////////////////////
   //
   // This is how the puzzle control the solver.
   // TODO: better document what the puzzle solver control do.

   bool puzzle_t::has_more_sub_puzzles(const sub_puzzle_t& a_current_sub_puzzle) const
   {
      return a_current_sub_puzzle.other_tiles.size() > 0;
   }

   std::vector<sub_puzzle_t> puzzle_t::create_initial_sub_puzzles() const
   {
      if (my_line_colors.size() <= 0)
         return {};

      if (my_initial_tiles.size() <= 0)
         return {};

      std::vector<sub_puzzle_t> sub_puzzles;
      for (size_t i = 0; i < my_initial_tiles.size(); ++i)
      {
         sub_puzzle_t sub_puzzle
         {
            my_initial_tiles.front(),
            { my_initial_tiles.begin() + 1, my_initial_tiles.end() },
            int(i), 0
         };
         sub_puzzles.emplace_back(std::move(sub_puzzle));
      }

      return sub_puzzles;
   }

   std::vector<sub_puzzle_t> puzzle_t::create_sub_puzzles(const sub_puzzle_t& a_current_sub_puzzle) const
   {
      std::vector<sub_puzzle_t> subs;

      for (const auto color : my_line_colors)
      {
         if (!a_current_sub_puzzle.other_tiles[0].has_color(color))
            continue;


         for (size_t i = 0; i < a_current_sub_puzzle.other_tiles.size(); ++i)
         {
            if (!a_current_sub_puzzle.other_tiles[0].has_color(color))
               break;

            sub_puzzle_t sub_puzzle(a_current_sub_puzzle);
            sub_puzzle.tile_to_place = a_current_sub_puzzle.other_tiles[i];
            sub_puzzle.other_tiles.erase(sub_puzzle.other_tiles.begin() + i);
            sub_puzzle.right_sub_puzzles_count -= 1;
            sub_puzzle.depth += 1;
            subs.emplace_back(sub_puzzle);
         }
         break;
      }

      return subs;
   }

   std::vector<position_t> puzzle_t::get_next_positions(const sub_puzzle_t& a_current_sub_puzzle, const solution_t& partial_solution) const
   {
      std::vector<position_t> next_positions;

      // The idea is that the first line is built linearly.
      //
      // The other lines must try all connections points afterward since that may
      // reconnect the separate line segment created while building the first line.
      const auto first_color = my_line_colors[0];
      if (a_current_sub_puzzle.tile_to_place.has_color(first_color))
      {
         const bool is_zero = (a_current_sub_puzzle.right_sub_puzzles_count == 0);
         const size_t tile_index = is_zero ? 0 : (partial_solution.tiles_count() - 1);

         const placed_tile_t& last_tile = partial_solution.tiles()[tile_index];
         for (int i = 0; i < 6; ++i)
         {
            // When the starting initial puzzle wants to have zero right tiles
            // we need to search direction in the reverse direction.
            //
            // (When we go to zero after placing tiles, it doesn't matter that
            // we go in reverse direction too since there will be only one connection
            // withthe desired color anyway, so we don't have to detect that case.)
            const auto dir = direction_t(is_zero ? 5 - i : i);
            const auto new_pos = last_tile.pos.move(dir);
            if (last_tile.tile.color(dir) != first_color)
               continue;
            if (partial_solution.is_occupied(new_pos))
               continue;

            next_positions.emplace_back(new_pos);

            break;
         }
      }
      else
      {
         for (size_t i = 1; i < my_line_colors.size(); ++i)
         {
            const auto color = my_line_colors[i];
            if (a_current_sub_puzzle.tile_to_place.has_color(color))
            {
               auto border_positions = partial_solution.get_borders(color);

               // If there are more ends to connect around the border that the
               // number of tiles can plug plus the number of ends we must have
               // at the end, then there is no point in going further: we will
               // never be able to connect them all.
               //
               // TODO: code below is only correct for puzzle with two lines.
               //       For more lines, we'd have to count the number of tiles of the color.
               const size_t desired_ends_count = my_must_be_loops ? 0 : 2;
               if (border_positions.size() > (a_current_sub_puzzle.other_tiles.size() + 1) * 2 + desired_ends_count)
               {
                  return {};
               }

               return border_positions;
            }
         }
      }

      return next_positions;
   }
}
