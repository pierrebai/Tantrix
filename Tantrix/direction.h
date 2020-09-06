#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <compare>
#include <cstdint>


////////////////////////////////////////////////////////////////////////////
//
// Direction of movement from tile to tile on the hexagoal grid.
//
// We can:
//    - Rotate by a multiple of sixth of a turn.
//    - Ask the corresponding position delta.

struct direction_t
{
   static constexpr direction_t dir_0() { return direction_t(0, -1,  0); }
   static constexpr direction_t dir_1() { return direction_t(1,  0, -1); }
   static constexpr direction_t dir_2() { return direction_t(2,  1, -1); }
   static constexpr direction_t dir_3() { return direction_t(3,  1,  0); }
   static constexpr direction_t dir_4() { return direction_t(4,  0,  1); }
   static constexpr direction_t dir_5() { return direction_t(5, -1,  1); }

   direction_t() = default;

   constexpr direction_t rotate(int an_amount) const
   {
      const auto new_dir = (my_dir + std::int8_t(an_amount)) % 6u;
      switch (new_dir)
      {
         default:
         case 0: return dir_0();
         case 1: return dir_1();
         case 2: return dir_2();
         case 3: return dir_3();
         case 4: return dir_4();
         case 5: return dir_5();
      }
   }

   int as_int() const { return int(my_dir); }

   std::int8_t delta_x() const { return my_delta_x; }
   std::int8_t delta_y() const { return my_delta_y; }

   auto operator<=>(const direction_t& an_other) const = default;

private:
   constexpr direction_t(std::int8_t a_dir, std::int8_t a_dx, std::int8_t a_dy) : my_dir(a_dir % 6), my_delta_x(a_dx), my_delta_y(a_dy) {}

   std::int8_t my_dir      =  0;
   std::int8_t my_delta_x  = -1;
   std::int8_t my_delta_y  =  0;
};

extern const direction_t directions[6];


