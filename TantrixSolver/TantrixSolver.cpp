#include "tantrix.h"
#include "tantrix_stream.h"

#include <iostream>

using namespace std;
using namespace dak::tantrix;

int main()
{
   const std::vector<tile_t> genius_puzzle_tiles =
   {
      tile_t(1),
      tile_t(4),
      tile_t(6),
      tile_t(7),
      tile_t(9),
      tile_t(10),
      tile_t(13),
      tile_t(16),
      tile_t(48),
      tile_t(49),
      tile_t(28),
      tile_t(37),
   };

   const std::vector<color_t> blue_red_line_colors =
   {
      color_t::blue(),
      color_t::red(),
   };

   const std::vector<color_t> red_yellow_line_colors =
   {
      color_t::red(),
      color_t::yellow(),
   };

   //const auto solutions = solve(puzzle_t(genius_puzzle_tiles, blue_red_line_colors));
   const auto solutions = solve(puzzle_t(genius_puzzle_tiles, red_yellow_line_colors));

   cout << solutions.size() << endl;

   int max_print = 100;

   for (const auto& sol : solutions)
   {
      cout << "----------------------------" << endl;
      cout << sol << endl;

      if (--max_print == 0)
         break;
   }
}

