#include "position.h"


namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Position of the tile in the hexagonal grid.

   position_t position_t::rotate(std::int16_t rotation) const
   {
      rotation %= 6u;

      std::int16_t factor_x_from_x;
      std::int16_t factor_x_from_y;
      std::int16_t factor_y_from_x;
      std::int16_t factor_y_from_y;

      switch (rotation)
      {
         default:
         case 0:
            return *this;
         case 1:
            factor_x_from_x =  0;
            factor_x_from_y = -1;
            factor_y_from_x =  1;
            factor_y_from_y =  1;
            break;
         case 2:
            factor_x_from_x = -1;
            factor_x_from_y = -1;
            factor_y_from_x =  1;
            factor_y_from_y =  0;
            break;
         case 3:
            factor_x_from_x = -1;
            factor_x_from_y =  0;
            factor_y_from_x =  0;
            factor_y_from_y = -1;
            break;
         case 4:
            factor_x_from_x =  1;
            factor_x_from_y =  1;
            factor_y_from_x = -1;
            factor_y_from_y =  0;
            break;
         case 5:
            factor_x_from_x =  1;
            factor_x_from_y =  1;
            factor_y_from_x = -1;
            factor_y_from_y =  0;
            break;
      }

      return position_t(
         my_x * factor_x_from_x + my_y * factor_x_from_y,
         my_x * factor_y_from_x + my_y * factor_y_from_y);
   }
}
