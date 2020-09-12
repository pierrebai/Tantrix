#include "puzzle.h"
#include "solution.h"


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

   puzzle_t::puzzle_t(const std::vector<tile_t>& some_tiles, const std::vector<color_t>& some_line_colors, bool must_be_loops)
      : my_line_colors(some_line_colors), my_must_be_loops(must_be_loops)
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

   size_t puzzle_t::estimated_total_count() const
   {
      size_t count = 1;
      for (const auto& [color, tiles] : my_tiles)
      {
         const size_t tiles_count = tiles.size();
         if (1 == count)
         {
            for (size_t i = 1; i < tiles_count; ++i)
               count *= i * 2;
         }
         else
         {
            for (size_t i = 1; i < tiles_count; ++i)
               count *= i * 6;

         }
      }

      return count;
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
         if (!a_solution.has_line(color, my_must_be_loops))
            return false;
      return true;
   }

   std::vector<puzzle_t::sub_puzzle> puzzle_t::sub_puzzles() const
   {
      std::vector<sub_puzzle> subs;

      for (const auto color : my_line_colors)
      {
         const auto& tiles = const_cast<tiles_by_color_t&>(my_tiles)[color];
         if (tiles.size())
         {
            for (size_t i = 0; i < tiles.size(); ++i)
            {
               puzzle_t sub_puzzle(*this);
               sub_puzzle.my_tiles[color].erase(sub_puzzle.my_tiles[color].begin() + i);
               sub_puzzle.my_depth = my_depth + 1;
               subs.emplace_back(tiles[i], sub_puzzle);
            }
            break;
         }
      }

      return subs;
   }

   std::vector<position_t> puzzle_t::get_next_positions(const solution_t& partial_solution, const position_t& a_last_add_pos, const tile_t& a_tile) const
   {
      std::vector<position_t> next_positions;

      const auto first_color = my_line_colors[0];
      if (a_tile.has_color(first_color))
      {
         const tile_t& last_tile = partial_solution.tile_at(a_last_add_pos);
         for (const direction_t& dir : directions)
         {
            const auto new_pos = a_last_add_pos.move(dir);
            if (last_tile.color(dir) != first_color)
               continue;
            if (partial_solution.is_occupied(new_pos))
               continue;
            next_positions.emplace_back(new_pos);
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
}