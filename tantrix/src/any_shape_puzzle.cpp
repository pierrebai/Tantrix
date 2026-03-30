#include "dak/tantrix/any_shape_puzzle.h"
#include "dak/tantrix/solution.h"


namespace dak::tantrix
{

   ////////////////////////////////////////////////////////////////////////////
   //
   // Solver interaction.
   //
   // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.

   std::vector<puzzle_t::sub_problem_t> any_shape_puzzle_t::create_initial_sub_problems() const
   {
      if (my_line_colors.size() <= 0)
         return {};

      if (my_initial_tiles.size() <= 0)
         return {};

      std::vector<sub_problem_t> sub_puzzles;
      for (size_t i = 0; i < my_initial_tiles.size(); ++i)
      {
         sub_problem_t sub_puzzle;
         sub_puzzle.tile_to_place = my_initial_tiles.front();
         sub_puzzle.other_tiles = { my_initial_tiles.begin() + 1, my_initial_tiles.end() };
         sub_puzzle.right_sub_puzzles_count = int(i);
         sub_puzzles.emplace_back(sub_puzzle);

         // For loops, the line will use all tiles and starting from any tile is equivalent,
         // so we don't need to try all different first tiles!
         if (must_be_loops())
            break;
      }

      return sub_puzzles;
   }

   std::vector<puzzle_t::sub_problem_t> any_shape_puzzle_t::create_sub_problems(
         const sub_problem_t& a_current_sub_problem,
         const solution_t& /*a_partial_solution*/) const
   {
      std::vector<sub_problem_t> subs;

      bool no_Line_found = true;

      for (const auto color : my_line_colors)
      {
         if (!a_current_sub_problem.other_tiles[0].has_color(color))
            continue;

         no_Line_found = false;

         for (size_t i = 0; i < a_current_sub_problem.other_tiles.size(); ++i)
         {
            if (!a_current_sub_problem.other_tiles[i].has_color(color))
               break;

            sub_problem_t sub_puzzle(a_current_sub_problem);
            sub_puzzle.tile_to_place = sub_puzzle.other_tiles[i];
            sub_puzzle.other_tiles.erase(sub_puzzle.other_tiles.begin() + i);
            sub_puzzle.right_sub_puzzles_count -= 1;
            subs.emplace_back(sub_puzzle);
         }
         break;
      }

      if (no_Line_found)
      {
         for (size_t i = 0; i < a_current_sub_problem.other_tiles.size(); ++i)
         {
            sub_problem_t sub_puzzle(a_current_sub_problem);
            sub_puzzle.tile_to_place = sub_puzzle.other_tiles[i];
            sub_puzzle.other_tiles.erase(sub_puzzle.other_tiles.begin() + i);
            sub_puzzle.right_sub_puzzles_count -= 1;
            subs.emplace_back(sub_puzzle);
         }
      }

      return subs;
   }

   std::vector<solution_t::part_t> any_shape_puzzle_t::get_sub_problem_potential_parts(
         const sub_problem_t& a_current_sub_problem,
         const solution_t& a_partial_solution) const
   {
      std::vector<solution_t::part_t> next_positions;

      // The idea is that the first line is built linearly.
      //
      // The other lines must try all connections points afterward since that may
      // reconnect the separate line segment created while building the first line.
      const auto first_color = my_line_colors[0];
      if (a_current_sub_problem.tile_to_place.has_color(first_color))
      {
         if (a_partial_solution.tiles_count() == 0) {
            next_positions.emplace_back(
                  a_current_sub_problem.tile_to_place, position_t(0, 0));
            return next_positions;
         }
         const bool is_zero = (a_current_sub_problem.right_sub_puzzles_count == 0);
         const size_t tile_index = is_zero ? 0 : (a_partial_solution.tiles_count() - 1);

         const solution_t::part_t& last_tile = a_partial_solution.tiles()[tile_index];
         for (int i = 0; i < 6; ++i)
         {
            // When the starting initial puzzle wants to have zero right tiles
            // we need to search direction in the reverse direction.
            //
            // (When we go to zero after placing tiles, it doesn't matter that
            // we go in reverse direction too since there will be only one connection
            // with the desired color anyway, so we don't have to detect that case.)
            const auto dir = direction_t(is_zero ? 5 - i : i);
            const auto new_pos = last_tile.pos.move(dir);
            if (last_tile.tile.color(dir) != first_color)
               continue;
            if (a_partial_solution.is_occupied(new_pos))
               continue;

            for (int rotation = 0; rotation < 6; ++rotation) {
               next_positions.emplace_back(
                     a_current_sub_problem.tile_to_place.rotate(rotation), new_pos);
            }

            break;
         }

         return next_positions;
      }
      else
      {
         for (size_t i = 1; i < my_line_colors.size(); ++i)
         {
            const auto color = my_line_colors[i];
            if (a_current_sub_problem.tile_to_place.has_color(color))
            {
               auto border_positions = a_partial_solution.get_borders(color);

               // If there are more ends to connect around the border that the
               // number of tiles can plug plus the number of ends we must have
               // at the end, then there is no point in going further: we will
               // never be able to connect them all.
               const size_t desired_ends_count = my_must_be_loops ? 0 : 2;
               const size_t tiles_count = a_current_sub_problem.right_sub_puzzles_count;
               if (border_positions.size() > (tiles_count + 1) * 2 + desired_ends_count)
               {
                  return {};
               }

               for (const position_t& new_pos : border_positions) {
                  for (int rotation = 0; rotation < 6; ++rotation) {
                     next_positions.emplace_back(
                           a_current_sub_problem.tile_to_place.rotate(rotation), new_pos);
                  }
               }
               return next_positions;
            }
         }

         // We allow extra tiles that are not part of loops nor lines but are used to fill holes.
         // Just return all holes they could fill.
         std::vector<solution_t::hole_t> holes = a_partial_solution.get_holes();
         for (auto& hole : holes) {
            for (const position_t& new_pos : hole) {
               for (int rotation = 0; rotation < 6; ++rotation) {
                  next_positions.emplace_back(
                        a_current_sub_problem.tile_to_place.rotate(rotation), new_pos);
               }
            }
         }
      }

      return next_positions;
   }
}
