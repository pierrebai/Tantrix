#include "dak/six_eight/position.h"


namespace dak::six_eight
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Position of the tile in the rectangular grid.

   position_t& position_t::rotate_in_place(int rotation)
   {
      rotation %= 4;
      if (!rotation)
         return *this;

      int factor_x_from_x;
      int factor_x_from_y;
      int factor_y_from_x;
      int factor_y_from_y;

      switch (rotation)
      {
         default:
         case 0:
            return *this;
         case 1:
            factor_x_from_x =  0;
            factor_x_from_y = -1;
            factor_y_from_x =  1;
            factor_y_from_y =  0;
            break;
         case 2:
            factor_x_from_x = -1;
            factor_x_from_y =  0;
            factor_y_from_x =  0;
            factor_y_from_y = -1;
            break;
         case 3:
            factor_x_from_x =  0;
            factor_x_from_y =  1;
            factor_y_from_x = -1;
            factor_y_from_y =  0;
            break;
      }

      const auto old_x = my_x;
      const auto old_y = my_y;
      my_x = old_x * factor_x_from_x + old_y * factor_x_from_y;
      my_y = old_x * factor_y_from_x + old_y * factor_y_from_y;

      return *this;
   }

}
