#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "position.h"
#include "tile.h"
#include "solution.h"

#include <set>
#include <vector>


////////////////////////////////////////////////////////////////////////////
//
// Puzzle, provide tiles and next position to try.
//
// Restrict the order in which tiles are selected.

struct puzzle_t
{
   using sub_puzzle = std::pair<tile_t, puzzle_t>;

   puzzle_t();
   puzzle_t(const std::vector<tile_t>& some_tiles);

   std::vector<sub_puzzle> sub_puzzles() const;
   std::vector<position_t> get_next_positions(const solution_t& partial_solution, const tile_t& a_tile) const;
   bool has_more_sub_puzzles() const;
   bool is_solution_valid(const solution_t& a_solution) const;

private:
   std::vector<tile_t> my_yellow_tiles;
   std::vector<tile_t> my_red_tiles;
};


////////////////////////////////////////////////////////////////////////////
//
// Solve the placement of all given tiles.

//using all_solutions_t = std::vector<solution_t>;
using all_solutions_t = std::set<solution_t>;

all_solutions_t solve(const puzzle_t& a_puzzle);
all_solutions_t solve_genius_puzzle();
