#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "direction.h"
#include "color.h"


////////////////////////////////////////////////////////////////////////////
//
// A tile with its colored lines. (Colored side connections, really.)
//
// Can be rotated. Two tiles are the same if they match under some rotation.

struct tile_t
{
   tile_t() = default;

   tile_t(std::uint16_t a_number, const color_t some_sides[6])
   : my_number(a_number)
   {
      for (int i = 0; i < 6; ++i)
         my_sides[i] = some_sides[i];
   }

   color_t color(const direction_t a_dir) const
   {
      return my_sides[a_dir.as_int()];
   }

   std::uint16_t number() const { return my_number; }

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
   std::uint16_t  my_number   = 0;
   color_t        my_sides[6] = { color_t::red(), color_t::red(), color_t::red(), color_t::red(), color_t::red(), color_t::red() };
};

