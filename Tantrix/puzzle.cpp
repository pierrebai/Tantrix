#include "puzzle.h"


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

   puzzle_t::puzzle_t(const std::vector<tile_t>& some_tiles, const std::vector<color_t>& some_line_colors)
      : my_line_colors(some_line_colors)
   {
      if (!some_line_colors.size())
         throw std::exception("invalid puzzle: no required line colors provided");

      for (const auto& tile : some_tiles)
      {
         bool inserted = false;
         for (const auto color : some_line_colors)
         {
            if (tile.has_color(color))
            {
               my_tiles[color].emplace_back(tile);
               inserted = true;
               break;
            }
         }

         if (!inserted)
            throw std::exception("invalid puzzle: a tile has no color of any required lines");
      }
   }

   bool puzzle_t::has_more_sub_puzzles() const
   {
      for (const auto& [color, tiles] : my_tiles)
         if (tiles.size() > 0)
            return true;
      return false;
   }

   bool puzzle_t::is_solution_valid(const solution_t& a_solution) const
   {
      for (const auto& color : my_line_colors)
         if (!a_solution.has_line(color))
            return false;
      return true;
   }

   std::vector<puzzle_t::sub_puzzle> puzzle_t::sub_puzzles() const
   {
      std::vector<sub_puzzle> subs;

      for (const auto color : my_line_colors)
      {
         auto& tiles = const_cast<tiles_by_color_t&>(my_tiles)[color];
         if (tiles.size())
         {
            for (size_t i = 0; i < tiles.size(); ++i)
            {
               puzzle_t sub_puzzle(*this);
               sub_puzzle.my_tiles[color].erase(sub_puzzle.my_tiles[color].begin() + i);
               subs.emplace_back(tiles[i], sub_puzzle);
            }
            break;
         }
      }

      return subs;
   }

   std::vector<position_t> puzzle_t::get_next_positions(const solution_t& partial_solution, const tile_t& a_tile) const
   {
      std::vector<position_t> next_positions;

      const auto first_color = my_line_colors[0];
      if (a_tile.has_color(first_color))
      {
         const auto last_pos = partial_solution.last_add_pos();
         for (const direction_t& dir : directions)
         {
            const auto new_pos = last_pos.move(dir);
            if (partial_solution.is_occupied(new_pos))
               continue;
            if (partial_solution.tiles().find(last_pos)->second.color(dir) != color_t::yellow())
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
            if (a_tile.has_color(color))
               return partial_solution.get_borders(color);
         }
      }

      return next_positions;
   }


   all_solutions_t solve_genius_puzzle()
   {
      const std::vector<tile_t> puzzle_tiles =
      {
         tiles[ 1],
         tiles[ 4],
         tiles[ 6],
         tiles[ 7],
         tiles[ 9],
         tiles[10],
         tiles[13],
         tiles[16],
         tiles[28],
         tiles[37],
         tiles[48],
         tiles[49],
      };

      const std::vector<color_t> puzzle_line_colors =
      {
         color_t::yellow(),
         color_t::red(),
      };

      return solve(puzzle_t(puzzle_tiles, puzzle_line_colors));
   }
}