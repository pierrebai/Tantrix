#include "dak/tantrix/tile.h"


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

   color_t tile_t::tiles_colors[57][6] =
   {
      { R, R, R, R, R, R },   //  0, invalid
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
      { R, R, Y, G, G, Y },   // 16
      { Y, Y, G, R, G, R },   // 17
      { Y, Y, R, G, R, G },   // 18
      { R, R, G, Y, G, Y },   // 19
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

}
