#include "dak/tantrix/any_shape_puzzle.h"
#include "dak/tantrix/solution.h"


namespace dak::tantrix
{

   ////////////////////////////////////////////////////////////////////////////
   //
   // Solver interaction.
   //
   // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.

   std::vector<sub_puzzle_t> any_shape_puzzle_t::create_initial_sub_puzzles() const
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
            int(i)
         };
         sub_puzzles.emplace_back(std::move(sub_puzzle));

         // For loops, the line will use all tiles and starting from any tile is equivalent,
         // so we don't need to try all different first tiles!
         if (must_be_loops())
            break;
      }

      return sub_puzzles;
   }

   std::vector<sub_puzzle_t> any_shape_puzzle_t::create_sub_puzzles(const sub_puzzle_t& a_current_sub_puzzle, const solution_t&) const
   {
      std::vector<sub_puzzle_t> subs;

      bool no_Line_found = true;

      for (const auto color : my_line_colors)
      {
         if (!a_current_sub_puzzle.other_tiles[0].has_color(color))
            continue;

         no_Line_found = false;

         for (size_t i = 0; i < a_current_sub_puzzle.other_tiles.size(); ++i)
         {
            if (!a_current_sub_puzzle.other_tiles[i].has_color(color))
               break;

            sub_puzzle_t sub_puzzle(a_current_sub_puzzle);
            sub_puzzle.tile_to_place = a_current_sub_puzzle.other_tiles[i];
            sub_puzzle.other_tiles.erase(sub_puzzle.other_tiles.begin() + i);
            sub_puzzle.right_sub_puzzles_count -= 1;
            subs.emplace_back(sub_puzzle);
         }
         break;
      }

      if (no_Line_found)
      {
         for (size_t i = 0; i < a_current_sub_puzzle.other_tiles.size(); ++i)
         {
            sub_puzzle_t sub_puzzle(a_current_sub_puzzle);
            sub_puzzle.tile_to_place = a_current_sub_puzzle.other_tiles[i];
            sub_puzzle.other_tiles.erase(sub_puzzle.other_tiles.begin() + i);
            sub_puzzle.right_sub_puzzles_count -= 1;
            subs.emplace_back(sub_puzzle);
         }
      }

      return subs;
   }

   std::vector<position_t> any_shape_puzzle_t::get_sub_puzzle_positions(const sub_puzzle_t& a_current_sub_puzzle, const solution_t& a_partial_solution) const
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
         const size_t tile_index = is_zero ? 0 : (a_partial_solution.tiles_count() - 1);

         const placed_tile_t& last_tile = a_partial_solution.tiles()[tile_index];
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
            if (a_partial_solution.is_occupied(new_pos))
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
               auto border_positions = a_partial_solution.get_borders(color);

               // If there are more ends to connect around the border that the
               // number of tiles can plug plus the number of ends we must have
               // at the end, then there is no point in going further: we will
               // never be able to connect them all.
               const size_t desired_ends_count = my_must_be_loops ? 0 : 2;
               const size_t tiles_count = a_current_sub_puzzle.count_tiles_of_color(color);
               if (border_positions.size() > (tiles_count + 1) * 2 + desired_ends_count)
               {
                  return {};
               }

               return border_positions;
            }
         }

         // We allow extra tiles that are not part of loops nor lines but are used to fill holes.
         // Just return all holes they could fill.
         std::vector<solution_t::hole_t> holes = a_partial_solution.get_holes();
         for (auto& hole : holes)
            next_positions.insert(next_positions.end(), hole.begin(), hole.end());
      }

      return next_positions;
   }
}
