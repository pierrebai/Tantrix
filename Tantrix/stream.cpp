#include "tantrix.h"
#include "tantrix_stream.h"


namespace dak::tantrix
{
   std::ostream& operator<<(std::ostream& a_stream, const direction_t& a_direction)
   {
      switch (a_direction.as_int())
      {
         case 0:  return a_stream << "left";
         case 1:  return a_stream << "top-left";
         case 2:  return a_stream << "top-right";
         case 3:  return a_stream << "right";
         case 4:  return a_stream << "bottom-right";
         case 5:  return a_stream << "bottom-left";
         default: return a_stream;
      }
   }

   std::ostream& operator<<(std::ostream& a_stream, const position_t& a_position)
   {
      a_stream << a_position.x() << " / " << a_position.y();
      return a_stream;
   }

   std::ostream& operator<<(std::ostream& a_stream, const color_t& a_color)
   {
      switch (a_color.as_int())
      {
         case 0:  return a_stream << 'R';
         case 1:  return a_stream << 'G';
         case 2:  return a_stream << 'B';
         case 3:  return a_stream << 'Y';
         default: return a_stream;
      }
   }

   std::ostream& operator<<(std::ostream& a_stream, const tile_t& a_tile)
   {
      a_stream << a_tile.number();
      for (const direction_t& dir : directions)
         a_stream << ' ' << a_tile.color(dir);
      return a_stream;
   }

   std::ostream& operator<<(std::ostream& a_stream, const solution_t& a_solution)
   {
      a_stream << (a_solution.is_valid()   ? "valid:"   : "invalid:") << std::endl;
      const placed_tile_t* tiles = a_solution.tiles();
      for (size_t i = 0; i < a_solution.tiles_count(); ++i)
      {
         a_stream << tiles[i].pos << " : " << tiles[i].tile << std::endl;
      }
      return a_stream;
   }

   std::ostream& operator<<(std::ostream& a_stream, const puzzle_t& a_puzzle)
   {
      for (const auto& [color, tiles] : a_puzzle.tiles())
         for (const auto& tile : tiles)
            a_stream << tile.number() << ' ';
      a_stream << "lines of colors";
      for (const auto& color : a_puzzle.line_colors())
         a_stream << ' ' << color;
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const direction_t& a_direction)
   {
      switch (a_direction.as_int())
      {
         case 0:  return a_stream << "left";
         case 1:  return a_stream << "top-left";
         case 2:  return a_stream << "top-right";
         case 3:  return a_stream << "right";
         case 4:  return a_stream << "bottom-right";
         case 5:  return a_stream << "bottom-left";
         default: return a_stream;
      }
   }

   std::wostream& operator<<(std::wostream& a_stream, const position_t& a_position)
   {
      a_stream << a_position.x() << " / " << a_position.y();
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const color_t& a_color)
   {
      switch (a_color.as_int())
      {
         case 0:  return a_stream << 'R';
         case 1:  return a_stream << 'G';
         case 2:  return a_stream << 'B';
         case 3:  return a_stream << 'Y';
         default: return a_stream;
      }
   }

   std::wostream& operator<<(std::wostream& a_stream, const tile_t& a_tile)
   {
      a_stream << a_tile.number();
      for (const direction_t& dir : directions)
         a_stream << ' ' << a_tile.color(dir);
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const solution_t& a_solution)
   {
      a_stream << (a_solution.is_valid() ? "valid:" : "invalid:") << std::endl;
      const placed_tile_t* tiles = a_solution.tiles();
      for (size_t i = 0; i < a_solution.tiles_count(); ++i)
      {
         a_stream << tiles[i].pos << " : " << tiles[i].tile << std::endl;
      }
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const puzzle_t& a_puzzle)
   {
      for (const auto& [color, tiles] : a_puzzle.tiles())
         for (const auto& tile : tiles)
            a_stream << tile.number() << ' ';
      a_stream << "lines of colors";
      for (const auto& color : a_puzzle.line_colors())
         a_stream << ' ' << color;
      return a_stream;
   }

}