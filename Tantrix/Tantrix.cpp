// Tantrix.cpp : Defines the functions for the static library.
//

#include "tantrix.h"

#include <algorithm>
#include <thread>
#include <future>

////////////////////////////////////////////////////////////////////////////
//
// Direction of movement from tile to tile on the hexagoal grid.
//
// We can:
//    - Rotate by a multiple of sixth of a turn.

const direction_t directions[6] =
{
   direction_t::dir_0(),
   direction_t::dir_1(),
   direction_t::dir_2(),
   direction_t::dir_3(),
   direction_t::dir_4(),
   direction_t::dir_5(),
};


////////////////////////////////////////////////////////////////////////////
//
// Position of the tile in the hexagonal grid.

position_t position_t::rotate(int rotation) const
{
   int factor_x_from_x;
   int factor_x_from_y;
   int factor_y_from_x;
   int factor_y_from_y;

   switch (rotation % 6)
   {
      default:
      case 0:
         return *this;
      case 1:
         factor_x_from_x = 0;
         factor_x_from_y = -1;
         factor_y_from_x = 1;
         factor_y_from_y = 1;
         break;
      case 2:
         factor_x_from_x = -1;
         factor_x_from_y = -1;
         factor_y_from_x = 1;
         factor_y_from_y = 0;
         break;
      case 3:
         factor_x_from_x = -1;
         factor_x_from_y = 0;
         factor_y_from_x = 0;
         factor_y_from_y = -1;
         break;
      case 4:
         factor_x_from_x = 1;
         factor_x_from_y = 1;
         factor_y_from_x = -1;
         factor_y_from_y = 0;
         break;
      case 5:
         factor_x_from_x = 1;
         factor_x_from_y = 1;
         factor_y_from_x = -1;
         factor_y_from_y = 0;
         break;
   }

   return position_t(
      my_x * factor_x_from_x + my_x * factor_x_from_y,
      my_x * factor_y_from_x + my_x * factor_y_from_y);
}


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

void solution_t::add_tile(const tile_t& a_tile, const position_t& a_pos)
{
   my_tiles[a_pos] = a_tile;
   my_last_pos = a_pos;

}

bool solution_t::is_occupied(const position_t& a_pos) const
{
   return my_tiles.find(a_pos) != my_tiles.end();
}

bool solution_t::is_compatible(const tile_t& a_tile, const position_t a_pos) const
{
   if (my_tiles.find(a_pos) != my_tiles.end())
      return false;

   for (const auto& [pos, tile] : my_tiles)
   {
      const auto dir = pos.relative(a_pos);
      if (dir.has_value())
      {
         if (tile.color(dir.value()) != a_tile.color(dir.value().rotate(3)))
         {
            return false;
         }
      }
   }
   return true;
}

solution_t solution_t::rotate(int rotation) const
{
   solution_t rotated;

   for (const auto& [pos, tile] : my_tiles)
      rotated.my_tiles[pos.rotate(rotation)] = tile.rotate(rotation);

   return rotated;
}

bool solution_t::has_line(const color_t& a_color) const
{
   position_t start_pos;
   size_t expected_count = 0;
   for (const auto& [pos, tile] : my_tiles)
   {
      if (!tile.has_color(a_color))
         continue;
      expected_count += 1;
      start_pos = pos;
   }

   std::set<position_t> seen;
   std::set<position_t> todo;
   todo.insert(start_pos);
   while (todo.size() > 0)
   {
      const auto pos = *todo.begin();
      todo.erase(todo.begin());
      seen.insert(pos);
      for (const auto& dir : directions)
      {
         const auto new_pos = pos.move(dir);
         if (seen.count(new_pos) > 0)
            continue;
         if (todo.count(new_pos) > 0)
            continue;
         if (my_tiles.find(pos)->second.color(dir) != a_color)
            continue;
         if (!is_occupied(new_pos))
            continue;
         if (!my_tiles.find(new_pos)->second.has_color(a_color))
            continue;
         todo.insert(new_pos);
      }
   }

   return seen.size() == expected_count;
}


////////////////////////////////////////////////////////////////////////////
//
// Keeper of all solutions.

void add_solution(std::vector<solution_t>& all_solutions, solution_t&& a_solution)
{
   all_solutions.emplace_back(a_solution);
}

void add_solutions(std::vector<solution_t>& all_solutions, all_solutions_t&& other_solutions)
{
   all_solutions.insert(all_solutions.end(), other_solutions.begin(), other_solutions.end());
}

void add_solution(std::set<solution_t>& all_solutions, solution_t&& a_solution)
{
   all_solutions.insert(a_solution);
}

void add_solutions(std::set<solution_t>& all_solutions, all_solutions_t&& other_solutions)
{
   all_solutions.insert(other_solutions.begin(), other_solutions.end());
}


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
      // TODO: find all red positions.
   }

   return next_positions;
}

////////////////////////////////////////////////////////////////////////////
//
// Solve the placement of all given tiles.

static void solve_partial(all_solutions_t& solutions, const solution_t& partial_solution, const puzzle_t& a_sub_puzzle);

static void solve_recursion(
   all_solutions_t& solutions,
   const solution_t& partial_solution,
   const puzzle_t& a_sub_puzzle,
   const tile_t& new_tile, position_t new_pos)
{
   solution_t new_partial = partial_solution;
   new_partial.add_tile(new_tile, new_pos);

   if (a_sub_puzzle.has_more_sub_puzzles())
   {
      solve_partial(solutions, new_partial, a_sub_puzzle);
   }
   else if (a_sub_puzzle.is_solution_valid(new_partial))
   {
      add_solution(solutions, std::move(new_partial));
   }
}

static void solve_sub_puzzle_with_tile(all_solutions_t& solutions, const solution_t& partial_solution, const puzzle_t& a_sub_puzzle, const tile_t& a_tile)
{
   const auto next_positions = a_sub_puzzle.get_next_positions(partial_solution, a_tile);
   for (const auto new_pos : next_positions)
   {
      for (int selected_orientation = 0; selected_orientation < 6; ++selected_orientation)
      {
         const tile_t& tile = a_tile.rotate(selected_orientation);
         if (partial_solution.is_compatible(tile, new_pos))
         {
            solve_recursion(solutions, partial_solution, a_sub_puzzle, tile, new_pos);
         }
      }
   }
}

static void solve_partial(all_solutions_t& solutions, const solution_t& partial_solution, const puzzle_t& a_sub_puzzle)
{
   const auto sub_sub_puzzles = a_sub_puzzle.sub_puzzles();
   for (const auto&[tile, sub_sub_puzzle] : sub_sub_puzzles)
   {
      solve_sub_puzzle_with_tile(solutions, partial_solution, sub_sub_puzzle, tile);
   } 
}

static all_solutions_t solve_sub_puzzle(const puzzle_t& a_sub_puzzle, const tile_t& a_tile)
{
   all_solutions_t solutions;

   solution_t partial_solution(a_tile, position_t(0, 0));
   solve_partial(solutions, partial_solution, a_sub_puzzle);

   return solutions;
}

all_solutions_t solve(const puzzle_t& a_puzzle)
{
   // The first tile can be chosen arbitrarily and placed.
   // This will force the orientation of the solution, so
   // we won't have to compare with rotations or translations.
   const auto sub_puzzles = a_puzzle.sub_puzzles();

   std::vector<std::future<all_solutions_t>> solutions_async;

   for (const auto&[tile, puzzle] : sub_puzzles)
   {
      auto new_async = std::async(std::launch::async, solve_sub_puzzle, puzzle, tile);
      solutions_async.emplace_back(std::move(new_async));
   }

   all_solutions_t all_solutions;

   for (auto& new_solutions_async : solutions_async)
   {
      auto partial_solutions = new_solutions_async.get();
      add_solutions(all_solutions, std::move(partial_solutions));
   }

   return all_solutions;
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
      //tile_t( 9, tiles_colors[9]),
      //tile_t(28, tiles_colors[10]),
      //tile_t(37, tiles_colors[11]),
   };

   return solve(puzzle_t(tiles));
}
