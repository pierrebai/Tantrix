#pragma once

#ifndef DAK_TANTRIX_STREAM_H
#define DAK_TANTRIX_STREAM_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <dak/tantrix/solve.h>

#include <iostream>
#include <memory>

namespace dak::tantrix
{
   struct direction_t;
   struct color_t;
   struct tile_t;
   struct solution_t;
   struct puzzle_t;

   ////////////////////////////////////////////////////////////////////////////
   //
   // Write or read various classes to an output stream or input stream.

   std::ostream&  operator<<(std::ostream&  a_stream, const color_t& a_color);
   std::wostream& operator<<(std::wostream& a_stream, const color_t& a_color);
   std::istream&  operator>>(std::istream&  a_stream,       color_t& a_color);
   std::wistream& operator>>(std::wistream& a_stream,       color_t& a_color);

   std::ostream&  operator<<(std::ostream&  a_stream, const direction_t& a_direction);
   std::wostream& operator<<(std::wostream& a_stream, const direction_t& a_direction);

   std::ostream&  operator<<(std::ostream&  a_stream, const position_t& a_position);
   std::wostream& operator<<(std::wostream& a_stream, const position_t& a_position);
   std::istream&  operator>>(std::istream&  a_stream,       position_t& a_position);
   std::wistream& operator>>(std::wistream& a_stream,       position_t& a_position);

   std::ostream&  operator<<(std::ostream&  a_stream, const tile_t& a_tile);
   std::wostream& operator<<(std::wostream& a_stream, const tile_t& a_tile);
   std::istream&  operator>>(std::istream&  a_stream,       tile_t& a_tile);
   std::wistream& operator>>(std::wistream& a_stream,       tile_t& a_tile);

   std::ostream&  operator<<(std::ostream&  a_stream, const solution_t& a_solution);
   std::wostream& operator<<(std::wostream& a_stream, const solution_t& a_solution);
   std::istream&  operator>>(std::istream& a_stream,        solution_t& a_solution);
   std::wistream& operator>>(std::wistream& a_stream,       solution_t& a_solution);

   std::ostream&  operator<<(std::ostream&  a_stream, const all_solutions_t& some_solutions);
   std::wostream& operator<<(std::wostream& a_stream, const all_solutions_t& some_solutions);
   std::istream&  operator>>(std::istream&  a_stream,       all_solutions_t& some_solutions);
   std::wistream& operator>>(std::wistream& a_stream,       all_solutions_t& some_solutions);

   std::ostream&  operator<<(std::ostream&  a_stream, const std::shared_ptr<puzzle_t>& a_puzzle);
   std::wostream& operator<<(std::wostream& a_stream, const std::shared_ptr<puzzle_t>& a_puzzle);
   std::istream&  operator>>(std::istream&  a_stream,       std::shared_ptr<puzzle_t>& a_puzzle);
   std::wistream& operator>>(std::wistream& a_stream,       std::shared_ptr<puzzle_t>& a_puzzle);

}

#endif /* DAK_TANTRIX_STREAM_H */
