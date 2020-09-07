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
   //
   // We can:
   //    - Rotate by a multiple of sixth of a turn.
   //    - Ask the corresponding position delta.

   struct direction_t
   {
      direction_t() = default;
      constexpr direction_t(int a_dir) : my_dir(a_dir % 6) {}

      constexpr direction_t rotate(int an_amount) const
      {
         return direction_t((my_dir + std::int8_t(an_amount)) % 6u);
      }

      int as_int() const { return int(my_dir); }

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

      auto operator<=>(const direction_t& an_other) const = default;

   private:
      std::int8_t my_dir      =  0;
   };

   extern const direction_t directions[6];
}

#endif /* DAK_TANTRIX_DIRECTION_H */
