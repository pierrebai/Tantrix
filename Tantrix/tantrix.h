#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <vector>
#include <map>
#include <set>
#include <optional>
#include <compare>


////////////////////////////////////////////////////////////////////////////
//
// Direction of movement from tile to tile on the hexagoal grid.
//
// We can:
//    - Rotate by a multiple of sixth of a turn.
//    - Ask the corresponding position delta.

struct direction_t
{
   static constexpr direction_t dir_0() { return direction_t(0, -1,  0); }
   static constexpr direction_t dir_1() { return direction_t(1,  0, -1); }
   static constexpr direction_t dir_2() { return direction_t(2,  1, -1); }
   static constexpr direction_t dir_3() { return direction_t(3,  1,  0); }
   static constexpr direction_t dir_4() { return direction_t(4,  0,  1); }
   static constexpr direction_t dir_5() { return direction_t(5, -1,  1); }

   direction_t() = default;

   constexpr direction_t rotate(int an_amount) const
   {
      const unsigned int new_dir = (my_dir + unsigned int(an_amount)) % 6u;
      switch (new_dir)
      {
         default:
         case 0: return dir_0();
         case 1: return dir_1();
         case 2: return dir_2();
         case 3: return dir_3();
         case 4: return dir_4();
         case 5: return dir_5();
      }
   }

   int as_int() const { return my_dir; }

   int delta_x() const { return my_delta_x; }
   int delta_y() const { return my_delta_y; }

   auto operator<=>(const direction_t& an_other) const = default;

private:
   constexpr direction_t(int a_dir, int a_dx, int a_dy) : my_dir(a_dir % 6), my_delta_x(a_dx), my_delta_y(a_dy) {}

   unsigned int my_dir = 0;
   int my_delta_x = -1;
   int my_delta_y = 0;
};

extern const direction_t directions[6];


////////////////////////////////////////////////////////////////////////////
//
// Colors of lines on the tiles.

struct color_t
{
   static constexpr color_t red() { return color_t(0); }
   static constexpr color_t green() { return color_t(1); }
   static constexpr color_t blue() { return color_t(2); }
   static constexpr color_t yellow() { return color_t(3); }

   color_t() = default;

   int as_int() const { return my_color; }

   auto operator<=>(const color_t& an_other) const = default;

private:
   constexpr color_t(int a_dir) : my_color(a_dir % 6) {}

   int my_color = 0;
};


////////////////////////////////////////////////////////////////////////////
//
// Position of the tile in the hexagonal grid.
//
// We can:
//    - Move by an integer position in the hexagonal grid.
//    - Rotate by a multiple of sixth of a turn around the origin position.
//    - Get the direction, if any, where a position is adjacent relative to this position.

struct position_t
{
   position_t() = default;
   position_t(int a_x, int a_y) : my_x(a_x), my_y(a_y) {}

   position_t move(int delta_x, int delta_y) const
   {
      return position_t(my_x + delta_x, my_y + delta_y);
   }

   position_t move(direction_t a_dir) const
   {
      return move(a_dir.delta_x(), a_dir.delta_y());
   }

   position_t rotate(int rotation) const;

   std::optional<direction_t> relative(const position_t& a_pos) const
   {
      for (const direction_t dir : directions)
         if (move(dir.delta_x(), dir.delta_y()) == a_pos)
            return std::optional<direction_t>(dir);
      return std::optional<direction_t>();
   }

   int x() const { return my_x; }
   int y() const { return my_y; }

   auto operator<=>(const position_t& an_other) const = default;

private:
   int my_x = 0;
   int my_y = 0;
};


////////////////////////////////////////////////////////////////////////////
//
// A tile with its colored lines. (Colored side connections, really.)
//
// Can be rotated. Two tiles are the same if they match under some rotation.

struct tile_t
{
   tile_t() = default;

   tile_t(int a_number, const color_t some_sides[6])
   : my_number(a_number)
   {
      for (int i = 0; i < 6; ++i)
         my_sides[i] = some_sides[i];
   }

   color_t color(const direction_t a_dir) const
   {
      return my_sides[a_dir.as_int()];
   }

   int number() const { return my_number; }

   bool has_color(const color_t& color) const
   {
      for (int i = 0; i < 6; ++i)
         if (color == my_sides[i])
            return true;
      return false;
   }

   tile_t rotate(int an_amount) const
   {
      color_t new_sides[6];
      for (int i = 0; i < 6; ++i)
      {
         new_sides[i] = my_sides[(i + an_amount) % 6];
      }
      return tile_t(my_number, new_sides);
   }

   bool is_same(const tile_t& an_other) const
   {
      return my_number == an_other.my_number;
   }

   auto operator<=>(const tile_t& an_other) const = default;

private:
   int my_number = 0;
   color_t  my_sides[6] = { color_t::red(), color_t::red(), color_t::red(), color_t::red(), color_t::red(), color_t::red() };
};


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

struct solution_t
{
   solution_t() = default;
   solution_t(const tile_t& a_tile, const position_t& a_pos)
   {
      add_tile(a_tile, a_pos);
   }

   const std::map<position_t, tile_t>& tiles() const { return my_tiles; }
   void add_tile(const tile_t& a_tile, const position_t& a_pos);

   position_t last_add_pos() const { return my_last_pos; }

   solution_t rotate(int rotation) const;

   bool is_occupied(const position_t& a_pos) const;
   bool is_compatible(const tile_t& a_tile, const position_t a_pos) const;

   bool has_line(const color_t& a_color) const;

   auto operator<=>(const solution_t& another_solution) const
   {
      if (my_tiles == another_solution.my_tiles)
         return std::strong_ordering::equal;
      if (my_tiles < another_solution.my_tiles)
         return std::strong_ordering::less;
      return std::strong_ordering::greater;
   }

private:
   std::map<position_t, tile_t> my_tiles;
   position_t my_last_pos;
};


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
