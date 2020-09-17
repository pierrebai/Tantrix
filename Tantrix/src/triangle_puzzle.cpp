#include "dak/tantrix/triangle_puzzle.h"
#include "dak/tantrix/solution.h"

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

   std::vector<sub_puzzle_t> triangle_puzzle_t::create_initial_sub_puzzles() const
   {
      if (my_line_colors.size() <= 0)
         return {};

      if (my_initial_tiles.size() <= 0)
         return {};

      const color_t first_line_color = my_line_colors[0];
      const size_t  first_line_count = std::count_if(my_initial_tiles.begin(), my_initial_tiles.end(), [color = first_line_color](const tile_t& tile) -> bool
      {
         return tile.has_color(color);
      });

      std::vector<sub_puzzle_t> sub_puzzles;
      for (size_t i = 0; i < first_line_count; ++i)
      {
         for (int selected_orientation = 0; selected_orientation < 6; ++selected_orientation)
         {
            const tile_t& tile = my_initial_tiles[i].rotate(selected_orientation);
            sub_puzzle_t sub_puzzle
            {
               tile,
               my_initial_tiles,
               1, 0
            };
            sub_puzzle.other_tiles.erase(sub_puzzle.other_tiles.begin() + i);
            sub_puzzles.emplace_back(sub_puzzle);

            sub_puzzle.right_sub_puzzles_count = -1;
            sub_puzzles.emplace_back(sub_puzzle);
         }
      }

      return sub_puzzles;
   }

   std::vector<position_t> triangle_puzzle_t::get_sub_puzzle_positions(const sub_puzzle_t& a_current_sub_puzzle, const solution_t& partial_solution) const
   {
      std::vector<position_t> next_positions;

      const size_t done_count = partial_solution.tiles_count();
      if (a_current_sub_puzzle.right_sub_puzzles_count > 0)
      {
         next_positions.push_back(my_up_pyramid_positions[done_count]);
      }
      else
      {
         next_positions.push_back(my_down_pyramid_positions[done_count]);
      }

      return next_positions;
   }
}
