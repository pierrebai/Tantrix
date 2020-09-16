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

   tile_description_t tile_t::tiles[57] =
   {
      { R, R, R, R, R, R, R },   //  0, invalid
      { Y, Y, B, R, B, R, G },   //  1
      { Y, Y, B, R, R, B, G },   //  2
      { Y, Y, R, R, B, B, G },   //  3
      { R, Y, B, Y, R, B, G },   //  4
      { Y, Y, R, B, B, R, G },   //  5
      { R, B, Y, B, R, Y, G },   //  6
      { B, B, Y, R, Y, R, G },   //  7
      { B, B, R, Y, R, Y, G },   //  8
      { B, Y, R, Y, B, R, G },   //  9
      { Y, Y, R, B, R, B, G },   // 10
      { R, R, B, Y, B, Y, G },   // 11
      { R, R, Y, B, Y, B, G },   // 12
      { R, R, Y, B, B, Y, G },   // 13
      { Y, Y, B, B, R, R, G },   // 14
      { Y, Y, R, G, G, R, B },   // 15
      { R, R, Y, G, G, Y, B },   // 16
      { Y, Y, G, R, G, R, B },   // 17
      { Y, Y, R, G, R, G, B },   // 18
      { R, R, G, Y, G, Y, B },   // 19
      { R, R, Y, G, Y, G, B },   // 20
      { Y, Y, G, G, R, R, B },   // 21
      { Y, Y, G, R, R, G, B },   // 22
      { Y, Y, R, R, G, G, B },   // 23
      { R, R, B, G, G, B, Y },   // 24
      { R, R, B, B, G, G, Y },   // 25
      { R, R, G, B, B, G, Y },   // 26
      { R, R, B, G, B, G, Y },   // 27
      { R, R, G, G, B, B, Y },   // 28
      { R, R, G, B, G, B, Y },   // 29
      { B, B, R, G, G, R, Y },   // 30
      { G, G, R, Y, R, Y, B },   // 31
      { Y, G, Y, R, G, R, B },   // 32
      { G, G, Y, R, Y, R, B },   // 33
      { Y, G, R, G, Y, R, B },   // 34
      { Y, G, R, Y, R, G, B },   // 35
      { G, G, R, B, R, B, Y },   // 36
      { B, B, G, R, G, R, Y },   // 37
      { B, B, R, G, R, G, Y },   // 38
      { R, B, G, B, R, G, Y },   // 39
      { R, G, B, G, R, B, Y },   // 40
      { G, G, B, R, B, R, Y },   // 41
      { B, R, B, G, R, G, Y },   // 42
      { Y, Y, G, G, B, B, R },   // 43
      { Y, G, B, Y, B, G, R },   // 44
      { Y, Y, B, B, G, G, R },   // 45
      { G, G, B, Y, B, Y, R },   // 46
      { G, G, Y, B, B, Y, R },   // 47
      { Y, Y, B, G, G, B, R },   // 48
      { B, B, G, Y, Y, G, R },   // 49
      { Y, B, Y, G, B, G, R },   // 50
      { Y, G, Y, B, G, B, R },   // 51
      { G, G, Y, B, Y, B, R },   // 52
      { Y, Y, G, B, G, B, R },   // 53
      { Y, Y, B, G, B, G, R },   // 54
      { B, B, G, Y, G, Y, R },   // 55
      { B, B, Y, G, Y, G, R },   // 56
   };

}
