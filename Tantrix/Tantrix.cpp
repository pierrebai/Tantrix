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

bool is_occupied(const solution_t& a_solution, const position_t& a_pos)
{
   return a_solution.find(a_pos) != a_solution.end();
}

bool is_compatible(const solution_t& a_solution, const tile_t& a_tile, const position_t a_pos)
{
   if (a_solution.find(a_pos) != a_solution.end())
      return false;

   for (const auto& [pos, tile] : a_solution)
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


solution_t rotate(const solution_t& a_solution, int rotation)
{
   solution_t rotated;

   for (const auto& [pos, tile] : a_solution)
      rotated[pos.rotate(rotation)] = tile.rotate(rotation);

   return rotated;
}

bool is_exactly_same(const solution_t& a_solution, const solution_t& another_solution)
{
   for (const auto& [pos, tile] : a_solution)
   {
      const auto another_tile_pos = another_solution.find(pos);
      if (another_tile_pos == another_solution.end())
         return false;

      if (another_tile_pos->second != tile)
         return false;
   }

   return true;
}

bool has_same_solution(const std::vector<solution_t>& some_solutions, const solution_t& a_solution)
{
   for (const auto& solution : some_solutions)
      if (is_exactly_same(solution, a_solution))
         return true;
   return false;
}

void add_unique_solution(std::vector<solution_t>& some_solutions, const solution_t& a_solution)
{
   //if (!has_same_solution(some_solutions, a_solution))
      some_solutions.emplace_back(a_solution);
}

////////////////////////////////////////////////////////////////////////////
//
// Solve the placement of all given tiles.

void solve_partial(std::vector<solution_t>& solutions, const solution_t& partial_solution, const avail_tiles_t& some_tiles);

void solve_recursion(
   std::vector<solution_t>& solutions,
   const solution_t& partial_solution,
   const avail_tiles_t& some_tiles,
   const tile_t& new_tile, position_t new_pos)
{
   solution_t new_partial = partial_solution;
   new_partial[new_pos] = new_tile;

   if (some_tiles.size())
   {
      solve_partial(solutions, new_partial, some_tiles);
   }
   else
   {
      add_unique_solution(solutions, new_partial);
   }
}

void solve_partial_with_tile(std::vector<solution_t>& solutions, const solution_t& partial_solution, const avail_tiles_t& some_tiles, const tile_t& a_tile)
{
   for (const direction_t& dir : directions)
   {
      for (const auto& [end_pos, end_tile] : partial_solution)
      {
         const auto new_pos = end_pos.move(dir);
         if (is_occupied(partial_solution, new_pos))
            continue;

         for (int selected_orientation = 0; selected_orientation < 6; ++selected_orientation)
         {
            const tile_t& tile = a_tile.rotate(selected_orientation);
            if (is_compatible(partial_solution, tile, new_pos))
            {
               solve_recursion(solutions, partial_solution, some_tiles, tile, new_pos);
            }
         }
      }
   }
}

void solve_partial(std::vector<solution_t>& solutions, const solution_t& partial_solution, const avail_tiles_t& some_tiles)
{
   for (size_t selected_tile = 0; selected_tile < some_tiles.size(); ++selected_tile)
   {
      avail_tiles_t new_avail = some_tiles;
      new_avail.erase(new_avail.begin() + selected_tile);
      solve_partial_with_tile(solutions, partial_solution, new_avail, some_tiles[selected_tile]);
   } 
}

std::vector<solution_t> solve_base_solution(const solution_t& base_solution, const avail_tiles_t& some_tiles, size_t selected_tile)
{
   std::vector<solution_t> solutions;

    avail_tiles_t new_avail = some_tiles;
   new_avail.erase(new_avail.begin() + selected_tile);

   solve_partial_with_tile(solutions, base_solution, new_avail, some_tiles[selected_tile]);

   return solutions;
}

std::vector<solution_t> solve(avail_tiles_t some_tiles)
{
   // The first tile can be chosen arbitrarily and placed.
   // This will force the orientation of the solution, so
   // we won't have to compare with rotations or translations.
   const solution_t base_solution({ std::make_pair(position_t(0, 0), some_tiles.back()) });
   some_tiles.pop_back();

   std::vector<std::future<std::vector<solution_t>>> solutions_async;

   for (size_t selected_tile = 0; selected_tile < some_tiles.size(); ++selected_tile)
   {
      auto new_async = std::async(std::launch::async, solve_base_solution, base_solution, (const avail_tiles_t &)some_tiles, selected_tile);
      solutions_async.emplace_back(std::move(new_async));
   }

   std::vector<solution_t> all_solutions;

   for (auto& new_solutions_async : solutions_async)
   {
      const auto& partial_solutions = new_solutions_async.get();
      all_solutions.insert(all_solutions.end(), partial_solutions.begin(), partial_solutions.end());
      //for (const auto& solution : partial_solutions)
      //   add_unique_solution(all_solutions, solution);
   }

   return all_solutions;
}

std::vector<solution_t> solve_genius_puzzle()
{
   const color_t R = color_t::red();
   const color_t G = color_t::green();
   const color_t B = color_t::blue();
   const color_t Y = color_t::yellow();

   const color_t tiles_colors[][6] =
   {
      { R, Y, B, Y, R, B },
      { R, R, Y, B, B, Y },
      { B, B, G, Y, Y, G },
      { R, R, Y, G, G, Y },
      { B, B, G, R, G, R },
      { Y, Y, B, R, B, R },
      { R, B, Y, B, R, Y },
      { Y, Y, R, B, R, B },
      { Y, Y, B, G, G, B },
      { B, B, Y, R, Y, R },
      { B, Y, R, Y, B, R },
      { R, R, G, G, B, B },
   };

   const avail_tiles_t tiles =
   {
      tile_t(tiles_colors[0]),
      tile_t(tiles_colors[1]),
      tile_t(tiles_colors[2]),
      tile_t(tiles_colors[3]),
      tile_t(tiles_colors[4]),
      //tile_t(tiles_colors[5]),
      //tile_t(tiles_colors[6]),
      //tile_t(tiles_colors[7]),
      //tile_t(tiles_colors[8]),
      //tile_t(tiles_colors[9]),
      //tile_t(tiles_colors[10]),
      //tile_t(tiles_colors[11]),
   };

   return solve(tiles);
}
