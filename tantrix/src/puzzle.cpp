#include "dak/tantrix/puzzle.h"
#include "dak/tantrix/solution.h"

#include <set>


namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Puzzle, provide tiles and next position to try.

   puzzle_t::puzzle_t()
   {
   }

   puzzle_t::puzzle_t(const std::vector<tile_t>& some_tiles,
                      const std::vector<color_t>& some_line_colors,
                      bool must_be_loops, const maybe_size_t& a_holes_count)
      : my_line_colors(some_line_colors)
      , my_must_be_loops(must_be_loops)
      , my_holes_count(a_holes_count)
   {
      if (!some_line_colors.size())
         throw std::exception("invalid puzzle: no required line colors provided");

      std::set<tile_t> done_tiles;
      for (const auto color : some_line_colors)
      {
         for (const auto& tile : some_tiles)
         {
            if (done_tiles.contains(tile))
               continue;

            if (tile.has_color(color))
            {
               my_initial_tiles.emplace_back(tile);
               done_tiles.insert(tile);
            }
         }
      }

      for (const auto& tile : some_tiles)
      {
         if (done_tiles.contains(tile))
            continue;

         my_initial_tiles.emplace_back(tile);
         done_tiles.insert(tile);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   //
   // Verify if the solution satisfies the initial puzzle.

   bool puzzle_t::is_solution_valid(const solution_t& a_solution) const
   {
      for (const auto& color : my_line_colors)
         if (!a_solution.has_line(color, my_must_be_loops))
            return false;

      if (my_holes_count.has_value() && a_solution.count_holes() != my_holes_count.value())
         return false;

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   //
   // This is how the puzzle control the solver.
   //
   // Create the initial list of sub-puzzles to solve.
   // Create sub-puzzles from a given sub-puzzle that has its tile placed.
   // Get the list of potential position for the tile-to-be-placed of the given sub-puzzle.
   // Verify if there are more sub-puzzles to be created from the given sub-puzzle.

   bool puzzle_t::has_more_sub_puzzles(const sub_puzzle_t& a_current_sub_puzzle) const
   {
      return a_current_sub_puzzle.other_tiles.size() > 0;
   }

}
