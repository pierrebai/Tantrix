#pragma once

#ifndef DAK_TANTRIX_TILE_H
#define DAK_TANTRIX_TILE_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "direction.h"
#include "color.h"

#include <algorithm>


namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // A tile with its colored lines. (Colored side connections, really.)
   //
   // Can be rotated. Two tiles are the same if they match under some rotation.

   struct tile_t
   {
      tile_t() = default;

      tile_t(std::uint16_t a_number);

      color_t color(const direction_t a_dir) const
      {
         return my_sides[a_dir.as_int()];
      }

      direction_t find_color(const color_t& a_color, direction_t a_from_dir) const
      {
         for (int i = 0; i < 6; ++i)
         {
            if (my_sides[a_from_dir.as_int()] == a_color)
            {
               return a_from_dir;
            }
            a_from_dir = a_from_dir.rotate(1);
         }
         return direction_t(0);
      }

      std::uint16_t number() const { return my_number; }

      bool has_color(const color_t& a_color) const
      {
         for (int i = 0; i < 6; ++i)
            if (a_color == my_sides[i])
               return true;
         return false;
      }

      void rotate_in_place(int an_amount)
      {
         an_amount %= 6;
         std::rotate(my_sides, my_sides + an_amount, my_sides + 6);
      }

      tile_t rotate(int an_amount) const
      {
         tile_t new_tile(*this);
         new_tile.rotate_in_place(an_amount);
         return new_tile;
      }

      bool is_same(const tile_t& an_other) const
      {
         return my_number == an_other.my_number;
      }

      auto operator<=>(const tile_t& an_other) const = default;

   private:
      std::uint16_t  my_number = 0;
      color_t        my_sides[6];
   };
}

#endif /* DAK_TANTRIX_TILE_H */
