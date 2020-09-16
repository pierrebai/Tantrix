#include "dak/tantrix/triangle_puzzle.h"
#include "dak/tantrix/solution.h"

namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Solution validation.

   bool triangle_puzzle_t::is_solution_valid(const solution_t& a_solution) const
   {
      if (!puzzle_t::is_solution_valid(a_solution))
         return false;
      return is_solution_correctly_shaped(a_solution);
   }

   bool triangle_puzzle_t::is_solution_correctly_shaped(const solution_t& a_solution) const
   {
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
   // Solver interaction.
   //
   // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.

   std::vector<position_t> triangle_puzzle_t::get_sub_puzzle_positions(const sub_puzzle_t& a_current_sub_puzzle, const solution_t& partial_solution) const
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
