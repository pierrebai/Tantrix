#include "dak/tantrix/direction.h"


namespace dak::tantrix
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Direction of movement from tile to tile on the hexagoal grid.
   //
   // We can:
   //    - Rotate by a multiple of sixth of a turn.

   const direction_t directions[6] =
   {
      direction_t(0),
      direction_t(1),
      direction_t(2),
      direction_t(3),
      direction_t(4),
      direction_t(5),
   };
}
