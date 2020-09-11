#include "tantrix.h"
#include "tantrix_stream.h"

#include <sstream>
#include <string>

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

   std::wostream& operator<<(std::wostream& a_stream, const direction_t& a_direction)
   {
      switch (a_direction.as_int())
      {
         case 0:  return a_stream << L"left";
         case 1:  return a_stream << L"top-left";
         case 2:  return a_stream << L"top-right";
         case 3:  return a_stream << L"right";
         case 4:  return a_stream << L"bottom-right";
         case 5:  return a_stream << L"bottom-left";
         default: return a_stream;
      }
   }

   std::ostream& operator<<(std::ostream& a_stream, const position_t& a_position)
   {
      a_stream << a_position.x() << " / " << a_position.y();
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const position_t& a_position)
   {
      a_stream << a_position.x() << L" / " << a_position.y();
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

   std::wostream& operator<<(std::wostream& a_stream, const color_t& a_color)
   {
      switch (a_color.as_int())
      {
         case 0:  return a_stream << L'R';
         case 1:  return a_stream << L'G';
         case 2:  return a_stream << L'B';
         case 3:  return a_stream << L'Y';
         default: return a_stream;
      }
   }

   std::istream& operator>>(std::istream& a_stream, color_t& a_color)
   {
      char color = ' ';
      if (!(a_stream >> color))
         return a_stream;

      switch (color)
      {
         case 'R': a_color = color_t::red(); break;
         case 'G': a_color = color_t::green(); break;
         case 'B': a_color = color_t::blue(); break;
         case 'Y': a_color = color_t::yellow(); break;
      }

      return a_stream;
   }

   std::wistream& operator>>(std::wistream& a_stream, color_t& a_color)
   {
      wchar_t color = ' ';
      if (!(a_stream >> color))
         return a_stream;

      switch (color)
      {
         case L'R': a_color = color_t::red(); break;
         case L'G': a_color = color_t::green(); break;
         case L'B': a_color = color_t::blue(); break;
         case L'Y': a_color = color_t::yellow(); break;
      }

      return a_stream;
   }

   std::ostream& operator<<(std::ostream& a_stream, const tile_t& a_tile)
   {
      a_stream << a_tile.number();
      for (const direction_t& dir : directions)
         a_stream << ' ' << a_tile.color(dir);
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const tile_t& a_tile)
   {
      a_stream << a_tile.number();
      for (const direction_t& dir : directions)
         a_stream << L' ' << a_tile.color(dir);
      return a_stream;
   }

   std::istream& operator>>(std::istream& a_stream, tile_t& a_tile)
   {
      int tile_number = 0;
      if (a_stream >> tile_number)
         a_tile = tile_t(tile_number);

      return a_stream;
   }

   std::wistream& operator>>(std::wistream& a_stream, tile_t& a_tile)
   {
      int tile_number = 0;
      if (a_stream >> tile_number)
         a_tile = tile_t(tile_number);

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
      a_stream << "tiles:";
      for (const auto& [color, tiles] : a_puzzle.tiles())
         for (const auto& tile : tiles)
            a_stream << ' ' << tile.number();
      a_stream << std::endl;
      a_stream << "lines:";
      for (const auto& color : a_puzzle.line_colors())
         a_stream << ' ' << color;
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const puzzle_t& a_puzzle)
   {
      a_stream << L"tiles:";
      for (const auto& [color, tiles] : a_puzzle.tiles())
         for (const auto& tile : tiles)
            a_stream << ' ' << tile.number();
      a_stream << std::endl;
      a_stream << L"lines:";
      for (const auto& color : a_puzzle.line_colors())
         a_stream << ' ' << color;
      return a_stream;
   }

   std::istream& operator>>(std::istream& a_stream, puzzle_t& a_puzzle)
   {
      enum { none, reading_tiles, reading_lines } state = none;

      std::vector<tile_t> tiles;
      std::vector<color_t> lines;

      std::string word;
      while (a_stream >> word)
      {
         if (word == "tiles:")
            state = reading_tiles;
         else if (word == "lines:")
            state = reading_lines;
         else if (state == reading_tiles)
         {
            tile_t tile;
            if (std::istringstream(word) >> tile)
               tiles.push_back(tile);
         }
         else if (state == reading_lines)
         {
            color_t color;
            if (std::istringstream(word) >> color)
               lines.push_back(color);
         }
      }

      a_puzzle = puzzle_t(tiles, lines);

      return a_stream;
   }

   std::wistream& operator>>(std::wistream& a_stream, puzzle_t& a_puzzle)
   {
      enum { none, reading_tiles, reading_lines } state = none;

      std::vector<tile_t> tiles;
      std::vector<color_t> lines;

      std::wstring word;
      while (a_stream >> word)
      {
         if (word == L"tiles:")
            state = reading_tiles;
         else if (word == L"lines:")
            state = reading_lines;
         else if (state == reading_tiles)
         {
            tile_t tile;
            if (std::wistringstream(word) >> tile)
               tiles.push_back(tile);
         }
         else if (state == reading_lines)
         {
            color_t color;
            if (std::wistringstream(word) >> color)
               lines.push_back(color);
         }
      }

      a_puzzle = puzzle_t(tiles, lines);

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

}