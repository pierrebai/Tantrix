#pragma once

#ifndef DAK_TANTRIX_TILE_H
#define DAK_TANTRIX_TILE_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "dak/tantrix/direction.h"
#include "dak/tantrix/color.h"

#include <algorithm>


namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // A tile with its colored lines. (Colored side connections, really.)

   struct tile_t
   {
      // Create a tile.
      tile_t() = default;
      tile_t(int a_number) : my_number(std::uint8_t(a_number)), my_rotation(0) {}

      // Get the color of the tile in the given direction.
      color_t color(const direction_t a_dir) const
      {
         return tiles_colors[my_number][(a_dir.as_int() + my_rotation) % 6u];
      }

      // Find in which direction a color is found.
      // Assumes that the tile has that color, otherwise the result is invalid.
      direction_t find_color(const color_t& a_color, direction_t a_from_dir) const
      {
         for (int i = 0; i < 6; ++i)
         {
            if (color(a_from_dir) == a_color)
            {
               return a_from_dir;
            }
            a_from_dir = a_from_dir.rotate(1);
         }
         return direction_t(0);
      }

      // Return the tile number.
      int number() const { return my_number; }

      // Check if the tile has the given color.
      bool has_color(const color_t& a_color) const
      {
         for (int i = 0; i < 6; ++i)
            if (a_color == tiles_colors[my_number][i])
               return true;
         return false;
      }

      // Check if the tile has the given colors.
      bool has_colors(const color_t some_colors[6], int a_count = 6) const
      {
         for (int i = 0; i < 6 && i < a_count; ++i)
            if (some_colors[i] != color(i))
               return false;
         return true;
      }

      // Rotate the tile in place by a multiple of sixth of a turn.
      tile_t& rotate_in_place(int an_amount)
      {
         my_rotation = (my_rotation + (6000 - an_amount)) % 6;
         return *this;
      }

      // Rotate the tile by a multiple of sixth of a turn and return a new tile.
      tile_t rotate(int an_amount) const
      {
         return tile_t(*this).rotate_in_place(an_amount);
      }

      // Check if the tile are the same tile, ignoring orientation.
      bool is_same(const tile_t& an_other) const
      {
         return my_number == an_other.my_number;
      }

      // Check if the tile is valid.
      bool is_valid() const { return my_number != 0; }

      // Tile comparison. The same tile with different orientations will be different.
      auto operator<=>(const tile_t& an_other) const = default;

   private:
      static color_t tiles_colors[57][6];

      std::uint8_t  my_number = 0;
      std::uint8_t  my_rotation = 0;
   };
}

#endif /* DAK_TANTRIX_TILE_H */
