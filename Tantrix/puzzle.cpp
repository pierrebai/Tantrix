#include "puzzle.h"


////////////////////////////////////////////////////////////////////////////
//
// Puzzle, provide tiles and next position to try.
//
// Restrict the order in which tiles are selected.

puzzle_t::puzzle_t()
{
}

puzzle_t::puzzle_t(const std::vector<tile_t>& some_tiles)
{
   for (const auto& tile : some_tiles)
   {
      if (tile.has_color(color_t::yellow()))
         my_yellow_tiles.emplace_back(tile);
      else
         my_red_tiles.emplace_back(tile);
   }
}

bool puzzle_t::has_more_sub_puzzles() const
{
   return my_yellow_tiles.size() > 0 || my_red_tiles.size() > 0;
}

bool puzzle_t::is_solution_valid(const solution_t& a_solution) const
{
   return a_solution.has_line(color_t::yellow()) && a_solution.has_line(color_t::red());
}

std::vector<puzzle_t::sub_puzzle> puzzle_t::sub_puzzles() const
{
   std::vector<sub_puzzle> subs;
   if (my_yellow_tiles.size())
   {
      for (size_t i = 0; i < my_yellow_tiles.size(); ++i)
      {
         puzzle_t sub_puzzle(*this);
         sub_puzzle.my_yellow_tiles.erase(sub_puzzle.my_yellow_tiles.begin() + i);
         subs.emplace_back(my_yellow_tiles[i], sub_puzzle);
      }
   }
   else
   {
      for (size_t i = 0; i < my_red_tiles.size(); ++i)
      {
         puzzle_t sub_puzzle(*this);
         sub_puzzle.my_red_tiles.erase(sub_puzzle.my_red_tiles.begin() + i);
         subs.emplace_back(my_red_tiles[i], sub_puzzle);
      }
   }
   return subs;
}

std::vector<position_t> puzzle_t::get_next_positions(const solution_t& partial_solution, const tile_t& a_tile) const
{
   std::vector<position_t> next_positions;

   if (a_tile.has_color(color_t::yellow()))
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
      return partial_solution.get_borders(color_t::red());
   }

   return next_positions;
}


all_solutions_t solve_genius_puzzle()
{
   const color_t R = color_t::red();
   const color_t G = color_t::green();
   const color_t B = color_t::blue();
   const color_t Y = color_t::yellow();

   const color_t tiles_colors[][6] =
   {
      { R, Y, B, Y, R, B },   //  4
      { R, R, Y, B, B, Y },   // 13
      { B, B, G, Y, Y, G },   // 49
      { R, R, Y, G, G, Y },   // 16
      { Y, Y, B, R, B, R },   //  1
      { R, B, Y, B, R, Y },   //  6
      { Y, Y, R, B, R, B },   // 10
      { Y, Y, B, G, G, B },   // 48
      { B, B, Y, R, Y, R },   //  7
      { B, Y, R, Y, B, R },   //  9
      { R, R, G, G, B, B },   // 28
      { B, B, G, R, G, R },   // 37
   };

   const std::vector<tile_t> tiles =
   {
      tile_t( 4, tiles_colors[0]),
      tile_t(13, tiles_colors[1]),
      tile_t(49, tiles_colors[2]),
      tile_t(16, tiles_colors[3]),
      tile_t( 1, tiles_colors[4]),
      tile_t( 6, tiles_colors[5]),
      tile_t(10, tiles_colors[6]),
      tile_t(48, tiles_colors[7]),
      tile_t( 7, tiles_colors[8]),
      tile_t( 9, tiles_colors[9]),
      tile_t(28, tiles_colors[10]),
      tile_t(37, tiles_colors[11]),
   };

   return solve(puzzle_t(tiles));
}
