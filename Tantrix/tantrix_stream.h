#pragma once

#ifndef DAK_TANTRIX_STREAM_H
#define DAK_TANTRIX_STREAM_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <iostream>

namespace dak::tantrix
{
   struct direction_t;
   struct color_t;
   struct tile_t;
   struct solution_t;
   struct puzzle_t;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Write various classes to an output stream.

   std::ostream& operator<<(std::ostream& a_stream, const direction_t& a_direction);
   std::ostream& operator<<(std::ostream& a_stream, const color_t& a_color);
   std::ostream& operator<<(std::ostream& a_stream, const tile_t& a_tile);
   std::ostream& operator<<(std::ostream& a_stream, const solution_t& a_solution);
   std::ostream& operator<<(std::ostream& a_stream, const puzzle_t& a_puzzle);
}

#endif /* DAK_TANTRIX_STREAM_H */
