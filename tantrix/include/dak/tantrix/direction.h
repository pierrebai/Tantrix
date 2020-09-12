#pragma once

#ifndef DAK_TANTRIX_DIRECTION_H
#define DAK_TANTRIX_DIRECTION_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <compare>
#include <cstdint>


namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Direction of movement from tile to tile on the hexagoal grid.

   struct direction_t
   {
      // Create a direction.
      direction_t() = default;
      constexpr direction_t(int a_dir) : my_dir(a_dir % 6) {}

      // Rotate by a multiple of sixth of a turn, returning a new direction.
      constexpr direction_t rotate(int8_t an_amount) const
      {
         return direction_t((my_dir + an_amount) % 6u);
      }

      // Convert to integer for array indexing.
      int as_int() const { return int(my_dir); }

      // The corresponding X-coordinate delta of the direction.
      int delta_x() const
      {
         switch (my_dir)
         {
            default:
            case 0: return -1;
            case 1: return  0;
            case 2: return  1;
            case 3: return  1;
            case 4: return  0;
            case 5: return -1;
         }
      }

      // The corresponding Y-coordinate delta of the direction.
      int delta_y() const
      {
         switch (my_dir)
         {
            default:
            case 0: return  0;
            case 1: return -1;
            case 2: return -1;
            case 3: return  0;
            case 4: return  1;
            case 5: return  1;
         }
      }

      // Direction comparison.
      auto operator<=>(const direction_t& an_other) const = default;

   private:
      std::uint8_t my_dir = 0;
   };

   // All directions for easy looping.
   extern const direction_t directions[6];
}

#endif /* DAK_TANTRIX_DIRECTION_H */
