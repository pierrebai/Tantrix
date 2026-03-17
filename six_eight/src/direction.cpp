#include "dak/six_eight/direction.h"


namespace dak::six_eight
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // Direction of movement from tile to tile on the rectangular grid.
   //
   // We can:
   //    - Rotate by a multiple of quarter of a turn.

   const direction_t directions[4] =
   {
      direction_t(0),
      direction_t(1),
      direction_t(2),
      direction_t(3),
   };
}
