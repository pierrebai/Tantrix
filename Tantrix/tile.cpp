#include "tile.h"


namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // A tile with its colored lines. (Colored side connections, really.)
   //
   // Can be rotated. Two tiles are the same if they match under some rotation.

   static constexpr color_t R = color_t::red();
   static constexpr color_t G = color_t::green();
   static constexpr color_t B = color_t::blue();
   static constexpr color_t Y = color_t::yellow();

   static constexpr color_t tiles_colors[57][6] =
   {
      { Y, Y, Y, Y, Y, Y },   //  0, invalid
      { Y, Y, B, R, B, R },   //  1
      { Y, Y, B, R, R, B },   //  2
      { Y, Y, R, R, B, B },   //  3
      { R, Y, B, Y, R, B },   //  4
      { Y, Y, R, B, B, R },   //  5
      { R, B, Y, B, R, Y },   //  6
      { B, B, Y, R, Y, R },   //  7
      { B, B, R, Y, R, Y },   //  8
      { B, Y, R, Y, B, R },   //  9
      { Y, Y, R, B, R, B },   // 10
      { R, R, B, Y, B, Y },   // 11
      { R, R, Y, B, Y, B },   // 12
      { R, R, Y, B, B, Y },   // 13
      { Y, Y, B, B, R, R },   // 14
      { Y, Y, R, G, G, R },   // 15
      { Y, Y, R, G, R, G },   // 18
      { R, R, G, Y, G, Y },   // 19
      { R, R, Y, G, G, Y },   // 16
      { Y, Y, G, R, G, R },   // 17
      { R, R, Y, G, Y, G },   // 20
      { Y, Y, G, G, R, R },   // 21
      { Y, Y, G, R, R, G },   // 22
      { Y, Y, R, R, G, G },   // 23
      { R, R, B, G, G, B },   // 24
      { R, R, B, B, G, G },   // 25
      { R, R, G, B, B, G },   // 26
      { R, R, B, G, B, G },   // 27
      { R, R, G, G, B, B },   // 28
      { R, R, G, B, G, B },   // 29
      { B, B, R, G, G, R },   // 30
      { G, G, R, Y, R, Y },   // 31
      { Y, G, Y, R, G, R },   // 32
      { G, G, Y, R, Y, R },   // 33
      { Y, G, R, G, Y, R },   // 34
      { Y, G, R, Y, R, G },   // 35
      { G, G, R, B, R, B },   // 36
      { B, B, G, R, G, R },   // 37
      { B, B, R, G, R, G },   // 38
      { R, B, G, B, R, G },   // 39
      { R, G, B, G, R, B },   // 40
      { G, G, B, R, B, R },   // 41
      { B, R, B, G, R, G },   // 42
      { Y, Y, G, G, B, B },   // 43
      { Y, G, B, Y, B, G },   // 44
      { Y, Y, B, B, G, G },   // 45
      { G, G, B, Y, B, Y },   // 46
      { G, G, Y, B, B, Y },   // 47
      { Y, Y, B, G, G, B },   // 48
      { B, B, G, Y, Y, G },   // 49
      { Y, B, Y, G, B, G },   // 50
      { Y, G, Y, B, G, B },   // 51
      { G, G, Y, B, Y, B },   // 52
      { Y, Y, G, B, G, B },   // 53
      { Y, Y, B, G, B, G },   // 54
      { B, B, G, Y, G, Y },   // 55
      { B, B, Y, G, Y, G },   // 56
   };

   const tile_t tiles[57] =
   {
      tile_t( 0, tiles_colors[ 0]),
      tile_t( 1, tiles_colors[ 1]),
      tile_t( 2, tiles_colors[ 2]),
      tile_t( 3, tiles_colors[ 3]),
      tile_t( 4, tiles_colors[ 4]),
      tile_t( 5, tiles_colors[ 5]),
      tile_t( 6, tiles_colors[ 6]),
      tile_t( 7, tiles_colors[ 7]),
      tile_t( 8, tiles_colors[ 8]),
      tile_t( 9, tiles_colors[ 9]),
      tile_t(10, tiles_colors[10]),
      tile_t(11, tiles_colors[11]),
      tile_t(12, tiles_colors[12]),
      tile_t(13, tiles_colors[13]),
      tile_t(14, tiles_colors[14]),
      tile_t(15, tiles_colors[15]),
      tile_t(16, tiles_colors[16]),
      tile_t(17, tiles_colors[17]),
      tile_t(18, tiles_colors[18]),
      tile_t(19, tiles_colors[19]),
      tile_t(20, tiles_colors[20]),
      tile_t(21, tiles_colors[21]),
      tile_t(22, tiles_colors[22]),
      tile_t(23, tiles_colors[23]),
      tile_t(24, tiles_colors[24]),
      tile_t(25, tiles_colors[25]),
      tile_t(26, tiles_colors[26]),
      tile_t(27, tiles_colors[27]),
      tile_t(28, tiles_colors[28]),
      tile_t(29, tiles_colors[29]),
      tile_t(30, tiles_colors[30]),
      tile_t(31, tiles_colors[31]),
      tile_t(32, tiles_colors[32]),
      tile_t(33, tiles_colors[33]),
      tile_t(34, tiles_colors[34]),
      tile_t(35, tiles_colors[35]),
      tile_t(36, tiles_colors[36]),
      tile_t(37, tiles_colors[37]),
      tile_t(38, tiles_colors[38]),
      tile_t(39, tiles_colors[39]),
      tile_t(40, tiles_colors[40]),
      tile_t(41, tiles_colors[41]),
      tile_t(42, tiles_colors[42]),
      tile_t(43, tiles_colors[43]),
      tile_t(44, tiles_colors[44]),
      tile_t(45, tiles_colors[45]),
      tile_t(46, tiles_colors[46]),
      tile_t(47, tiles_colors[47]),
      tile_t(48, tiles_colors[48]),
      tile_t(49, tiles_colors[49]),
      tile_t(50, tiles_colors[50]),
      tile_t(51, tiles_colors[51]),
      tile_t(52, tiles_colors[52]),
      tile_t(53, tiles_colors[53]),
      tile_t(54, tiles_colors[54]),
      tile_t(55, tiles_colors[55]),
      tile_t(56, tiles_colors[56]),
   };
}