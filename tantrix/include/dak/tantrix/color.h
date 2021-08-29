#pragma once

#ifndef DAK_TANTRIX_COLOR_H
#define DAK_TANTRIX_COLOR_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <compare>
#include <cstdint>

namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Colors of lines on the tiles.

   struct color_t
   {
      // Well-known colors.
      static constexpr color_t red()      { return color_t(0); }
      static constexpr color_t green()    { return color_t(1); }
      static constexpr color_t blue()     { return color_t(2); }
      static constexpr color_t yellow()   { return color_t(3); }

      // Color creation. This creates a red.
      color_t() = default;

      // Gets the color internal integer value, useful for switches or indexing into arrays.
      int as_int() const { return int(my_color); }

      // Color comparison.
      auto operator<=>(const color_t& an_other) const = default;

   private:
      constexpr color_t(std::int8_t a_color) : my_color(a_color % 4) {}

      std::int8_t my_color = 0;
   };
}


#endif /* DAK_TANTRIX_COLOR_H */
