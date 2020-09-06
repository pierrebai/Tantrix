#include "direction.h"


////////////////////////////////////////////////////////////////////////////
//
// Direction of movement from tile to tile on the hexagoal grid.
//
// We can:
//    - Rotate by a multiple of sixth of a turn.

const direction_t directions[6] =
{
   direction_t::dir_0(),
   direction_t::dir_1(),
   direction_t::dir_2(),
   direction_t::dir_3(),
   direction_t::dir_4(),
   direction_t::dir_5(),
};
