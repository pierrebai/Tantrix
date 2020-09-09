#pragma once

#ifndef DAK_TANTRIX_POSITION_H
#define DAK_TANTRIX_POSITION_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <compare>
#include <optional>

#include "direction.h"


namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Position of the tile in the hexagonal grid.

   struct position_t
   {
      // Create a position.
      position_t() = default;
      position_t(std::int8_t a_x, std::int8_t a_y) : my_x(a_x), my_y(a_y) {}

      // Move by an integer amount in the hexagonal grid.
      position_t move(std::int8_t delta_x, std::int8_t delta_y) const
      {
         return position_t(my_x + delta_x, my_y + delta_y);
      }

      // Move by an unit in the given direction.
      position_t move(direction_t a_dir) const
      {
         return move(a_dir.delta_x(), a_dir.delta_y());
      }

      // Add another position to this one.
      position_t& operator+=(const position_t& an_other)
      {
         my_x += an_other.my_x;
         my_y += an_other.my_y;
         return *this;
      }

      // Subtract another position from this one.
      position_t& operator-=(const position_t& an_other)
      {
         my_x -= an_other.my_x;
         my_y -= an_other.my_y;
         return *this;
      }

      // Rotate by a multiple of sixth of a turn around the origin position.
      position_t rotate(std::int8_t rotation) const;

      // Get the direction, if any, where a position is adjacent relative to this position.
      std::optional<direction_t> relative(const position_t& a_pos) const
      {
         for (const direction_t dir : directions)
            if (move(dir.delta_x(), dir.delta_y()) == a_pos)
               return std::optional<direction_t>(dir);
         return std::optional<direction_t>();
      }

      // The position coordinates.
      std::int8_t x() const { return my_x; }
      std::int8_t y() const { return my_y; }

      // Checik if valid, between -15 and 15.
      bool is_valid() const { return -15 <= my_x && my_x <= 15 && -15 <= my_y && my_y <= 15; }

      // Position comparison.
      auto operator<=>(const position_t& an_other) const = default;

   private:
      std::int8_t my_x = 0;
      std::int8_t my_y = 0;
   };

}

#endif /* DAK_TANTRIX_POSITION_H */
