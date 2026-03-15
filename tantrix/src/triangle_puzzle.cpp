#include "dak/tantrix/triangle_puzzle.h"
#include "dak/tantrix/solution.h"

#include <algorithm>


namespace dak::tantrix
{
   triangle_puzzle_t::triangle_puzzle_t(const std::vector<tile_t>& some_tiles,
                                        const std::vector<color_t>& some_line_colors,
                                        bool must_be_loops,
                                        const maybe_size_t& a_holes_count)
      : puzzle_t(some_tiles, some_line_colors, must_be_loops, a_holes_count)
   {
      my_up_pyramid_positions.resize(some_tiles.size());
      my_down_pyramid_positions.resize(some_tiles.size());

      int line_index = 0;
      size_t pos_index = some_tiles.size() - 1;
      while (pos_index < some_tiles.size())
      {
         position_t up_pos(0 - line_index, line_index);
         position_t down_pos(0, -line_index);
         const int tiles_per_line = line_index + 1;
         for (int i = 0; i < tiles_per_line && pos_index < some_tiles.size(); ++i)
         {
            my_up_pyramid_positions[pos_index] = up_pos;
            up_pos = up_pos.move(1, 0);
            my_down_pyramid_positions[pos_index] = down_pos;
            down_pos = down_pos.move(1, 0);
            pos_index -= 1;
         }

         if (line_index % 2)
         {
            const size_t line_begin = pos_index + 1;
            const size_t line_end   = line_begin + tiles_per_line;
            std::reverse(my_up_pyramid_positions.begin() + line_begin, my_up_pyramid_positions.begin() + line_end);
            std::reverse(my_down_pyramid_positions.begin() + line_begin, my_down_pyramid_positions.begin() + line_end);
         }

         line_index += 1;
      }

      // Place first tile at 0/0
      const position_t up_delta = my_up_pyramid_positions[0];
      const position_t down_delta = my_down_pyramid_positions[0];
      for (size_t i = 0; i < my_up_pyramid_positions.size(); ++i)
      {
         my_up_pyramid_positions[i] = my_up_pyramid_positions[i].move(-up_delta.x(), -up_delta.y());
         my_down_pyramid_positions[i] = my_down_pyramid_positions[i].move(-down_delta.x(), -down_delta.y());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   //
   // Solver interaction.
   //
   // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.

   std::vector<solver::sub_problem_t::ptr_t> triangle_puzzle_t::create_initial_sub_problems() const
   {
      if (my_line_colors.size() <= 0)
         return {};

      if (my_initial_tiles.size() <= 0)
         return {};

      std::vector<solver::sub_problem_t::ptr_t> sub_puzzles;
      for (size_t i = 0; i < initial_tiles_count(); ++i)
      {
         for (int selected_orientation = 0; selected_orientation < 6; ++selected_orientation)
         {
            tile_t tile = my_initial_tiles[i].rotate(selected_orientation);
            sub_puzzle_t sub_puzzle;
            sub_puzzle.tile_to_place = tile;
            sub_puzzle.other_tiles = my_initial_tiles;
            sub_puzzle.right_sub_puzzles_count = 1;
            sub_puzzle.other_tiles.erase(sub_puzzle.other_tiles.begin() + i);
            sub_puzzles.emplace_back(std::make_shared<sub_puzzle_t>(sub_puzzle));

            sub_puzzle.right_sub_puzzles_count = -1;
            sub_puzzles.emplace_back(std::make_shared<sub_puzzle_t>(sub_puzzle));
         }
      }

      return sub_puzzles;
   }

   std::vector<solver::sub_problem_t::ptr_t> triangle_puzzle_t::create_sub_problems(
            const solver::sub_problem_t::ptr_t& a_current_sub_problem,
            const solver::solution_t::ptr_t& a_partial_solution) const
   {
      auto current_sub_puzzle = std::dynamic_pointer_cast<sub_puzzle_t>(a_current_sub_problem);
      if (!current_sub_puzzle)
         return {};

      auto partial_solution = std::dynamic_pointer_cast<tantrix::solution_t>(a_partial_solution);
      if (!partial_solution)
         return {};

      std::vector<solver::sub_problem_t::ptr_t> subs;

      const auto& last_placed_tile = partial_solution->tiles()[partial_solution->tiles_count() - 1];
      const auto last_pos = last_placed_tile.pos;
      const auto next_pos = next_pyramid_position(*current_sub_puzzle, *partial_solution);
      const auto maybe_dir = last_pos.relative(next_pos);
      if (!maybe_dir.has_value())
         throw std::exception("bug in triangle solver");
      const auto dir = maybe_dir.value();
      const auto color = last_placed_tile.tile.color(dir);

      for (size_t i = 0; i < current_sub_puzzle->other_tiles.size(); ++i)
      {
         const tile_t& tile = current_sub_puzzle->other_tiles[i];
         if (!tile.has_color(color))
            continue;

         auto sub_puzzle = std::make_shared<sub_puzzle_t>(*current_sub_puzzle);
         sub_puzzle->tile_to_place = tile;
         sub_puzzle->other_tiles.erase(sub_puzzle->other_tiles.begin() + i);
         subs.emplace_back(sub_puzzle);
      }

      return subs;
   }

   position_t triangle_puzzle_t::next_pyramid_position(const sub_puzzle_t& a_current_sub_puzzle, const solution_t& a_partial_solution) const
   {
      const size_t done_count = a_partial_solution.tiles_count();
      if (a_current_sub_puzzle.right_sub_puzzles_count > 0)
         return my_up_pyramid_positions[done_count];
      else
         return my_down_pyramid_positions[done_count];
   }

   std::vector<solver::solution_part_t::ptr_t> triangle_puzzle_t::get_sub_problem_potential_parts(
         const solver::sub_problem_t::ptr_t& a_current_sub_problem,
         const solver::solution_t::ptr_t& a_partial_solution) const
   {
      auto current_sub_puzzle = std::dynamic_pointer_cast<sub_puzzle_t>(a_current_sub_problem);
      if (!current_sub_puzzle)
         return {};

      auto partial_solution = std::dynamic_pointer_cast<tantrix::solution_t>(a_partial_solution);
      if (!partial_solution)
         return {};

      std::vector<solver::solution_part_t::ptr_t> next_positions;
      auto placed_tile = std::make_shared<placed_tile_t>();
      placed_tile->pos = next_pyramid_position(*current_sub_puzzle, *partial_solution);
      placed_tile->tile = current_sub_puzzle->tile_to_place;
      next_positions.emplace_back(placed_tile);
      return next_positions;
   }
}
