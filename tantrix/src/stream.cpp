#include "dak/tantrix/tantrix.h"
#include "dak/tantrix/stream.h"

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
      a_stream << a_position.x() << "/" << a_position.y();
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const position_t& a_position)
   {
      a_stream << a_position.x() << "/" << a_position.y();
      return a_stream;
   }

   std::istream& operator>>(std::istream& a_stream, position_t& a_position)
   {
      int x = 0, y = 0;
      char slash = 0;
      a_stream >> x >> slash >> y;
      a_position = position_t(x, y);
      return a_stream;
   }

   std::wistream& operator>>(std::wistream& a_stream, position_t& a_position)
   {
      int x = 0, y = 0;
      wchar_t slash = 0;
      a_stream >> x >> slash >> y;
      a_position = position_t(x, y);
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
         case 0:  return a_stream << 'R';
         case 1:  return a_stream << 'G';
         case 2:  return a_stream << 'B';
         case 3:  return a_stream << 'Y';
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
         case 'R': a_color = color_t::red(); break;
         case 'G': a_color = color_t::green(); break;
         case 'B': a_color = color_t::blue(); break;
         case 'Y': a_color = color_t::yellow(); break;
      }

      return a_stream;
   }

   std::ostream& operator<<(std::ostream& a_stream, const tile_t& a_tile)
   {
      a_stream << a_tile.number() << ' ';
      for (const direction_t& dir : directions)
         a_stream << a_tile.color(dir);
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const tile_t& a_tile)
   {
      a_stream << a_tile.number() << ' ';
      for (const direction_t& dir : directions)
         a_stream << a_tile.color(dir);
      return a_stream;
   }

   std::istream& operator>>(std::istream& a_stream, tile_t& a_tile)
   {
      int tile_number = 0;
      if (a_stream >> tile_number)
         a_tile = tile_t(tile_number);

      std::string colors_buffer;
      a_stream >> colors_buffer;

      std::istringstream colors_stream(colors_buffer);
      color_t colors[6];
      for (int i = 0; i < 6; ++i)
         colors_stream >> colors[i];

      for (int i = 0; i < 6; ++i)
      {
         if (a_tile.has_colors(colors))
            break;

         a_tile.rotate_in_place(1);
      }

      return a_stream;
   }

   std::wistream& operator>>(std::wistream& a_stream, tile_t& a_tile)
   {
      int tile_number = 0;
      if (a_stream >> tile_number)
         a_tile = tile_t(tile_number);

      std::wstring colors_buffer;
      a_stream >> colors_buffer;

      std::wistringstream colors_stream(colors_buffer);
      color_t colors[6];
      for (int i = 0; i < 6; ++i)
         colors_stream >> colors[i];


      for (int i = 0; i < 6; ++i)
      {
         if (a_tile.has_colors(colors))
            break;

         a_tile.rotate_in_place(1);
      }

      return a_stream;
   }

   std::ostream& operator<<(std::ostream& a_stream, const solution_t& a_solution)
   {
      a_stream << (a_solution.is_valid()   ? "valid\n"   : "invalid\n");
      const placed_tile_t* tiles = a_solution.tiles();
      for (size_t i = 0; i < a_solution.tiles_count(); ++i)
      {
         a_stream << tiles[i].pos << ": " << tiles[i].tile << "\n";
      }
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const solution_t& a_solution)
   {
      a_stream << (a_solution.is_valid() ? "valid\n" : "invalid\n");
      const placed_tile_t* tiles = a_solution.tiles();
      for (size_t i = 0; i < a_solution.tiles_count(); ++i)
      {
         a_stream << tiles[i].pos << ": " << tiles[i].tile << "\n";
      }
      return a_stream;
   }

   std::istream& operator>>(std::istream& a_stream, solution_t& a_solution)
   {
      std::string validity;
      a_stream >> validity;
      a_solution = solution_t();
      while (a_stream)
      {
         position_t pos;
         a_stream >> pos;
         std::string column;
         a_stream >> column;
         tile_t tile;
         a_stream >> tile;
         if (tile.is_valid())
            a_solution.add_tile(tile, pos);
      }

      return a_stream;
   }

   std::wistream& operator>>(std::wistream& a_stream, solution_t& a_solution)
   {
      std::wstring validity;
      a_stream >> validity;
      a_solution = solution_t();
      while (a_stream)
      {
         position_t pos;
         a_stream >> pos;
         std::wstring column;
         a_stream >> column;
         tile_t tile;
         a_stream >> tile;
         if (tile.is_valid())
            a_solution.add_tile(tile, pos);
      }

      return a_stream;
   }

   std::ostream& operator<<(std::ostream& a_stream, const all_solutions_t& some_solutions)
   {
      a_stream << "solutions: " << some_solutions.size() << "\n";
      for (const auto& [sol, count] : some_solutions)
         a_stream << sol << "\n";

      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const all_solutions_t& some_solutions)
   {
      a_stream << L"solutions: " << some_solutions.size() << L"\n";
      for (const auto& [sol, count] : some_solutions)
         a_stream << sol << L"\n";

      return a_stream;
   }

   std::istream& operator>>(std::istream& a_stream, all_solutions_t& some_solutions)
   {
      some_solutions.clear();

      enum { none, found_validity } state = none;

      std::string solution_buffer;
      char line_buffer[256] = { 0 };

      while (a_stream.getline(line_buffer, sizeof(line_buffer) - 1))
      {
         std::string line(line_buffer);
         solution_buffer.append(line);

         if (line.find_first_of("\r\n") == std::string::npos)
            solution_buffer.append("\n");

         if (state == none)
         {
            if (line.find("valid") != std::string::npos)
               state = found_validity;
            else
               solution_buffer.clear();
         }
         else if (state == found_validity)
         {
            if (line.find_first_not_of(" \t\r\n") == std::string::npos)
            {
               std::istringstream sol_stream(solution_buffer);
               solution_t solution;
               sol_stream >> solution;
               some_solutions[std::move(solution)] += 1;
               solution_buffer.clear();
               state = none;
            }
         }
      }

      return a_stream;
   }

   std::wistream& operator>>(std::wistream& a_stream, all_solutions_t& some_solutions)
   {
      some_solutions.clear();

      enum { none, found_validity } state = none;

      std::wstring solution_buffer;
      wchar_t line_buffer[256] = { 0 };

      while (a_stream.getline(line_buffer, sizeof(line_buffer) - 1))
      {
         std::wstring line(line_buffer);
         solution_buffer.append(line);

         if (line.find_first_of(L"\r\n") == std::wstring::npos)
            solution_buffer.append(L"\n");

         if (state == none)
         {
            if (line.find(L"valid") != std::wstring::npos)
               state = found_validity;
            else
               solution_buffer.clear();
         }
         else if (state == found_validity)
         {
            if (line.find_first_not_of(L" \t\r\n") == std::wstring::npos)
            {
               std::wistringstream sol_stream(solution_buffer);
               solution_t solution;
               sol_stream >> solution;
               some_solutions[std::move(solution)] += 1;
               solution_buffer.clear();
               state = none;
            }
         }
      }

      return a_stream;
   }

   std::ostream& operator<<(std::ostream& a_stream, const puzzle_t& a_puzzle)
   {
      a_stream << "tiles:";
      for (const auto& tile : a_puzzle.initial_tiles())
         a_stream << ' ' << tile.number();
      a_stream << (a_puzzle.must_be_loops() ? "\n" "loops:" : "\n" "lines:");
      for (const auto& color : a_puzzle.line_colors())
         a_stream << ' ' << color;
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const puzzle_t& a_puzzle)
   {
      a_stream << "tiles:";
      for (const auto& tile : a_puzzle.initial_tiles())
         a_stream << ' ' << tile.number();
      a_stream << (a_puzzle.must_be_loops() ? "\n" "loops:" : "\n" "lines:");
      for (const auto& color : a_puzzle.line_colors())
         a_stream << ' ' << color;
      return a_stream;
   }

   std::istream& operator>>(std::istream& a_stream, puzzle_t& a_puzzle)
   {
      enum { none, reading_tiles, reading_lines, reading_loops, reading_shape } state = none;
      bool must_be_loops = false;

      std::vector<tile_t> tiles;
      std::vector<color_t> lines;
      puzzle_t::shape_t shape = puzzle_t::shape_t::any;

      std::string word;
      while (a_stream >> word)
      {
         if (word == "tiles:")
            state = reading_tiles;
         else if (word == "lines:")
            state = reading_lines, must_be_loops = false;
         else if (word == "loops:")
            state = reading_lines, must_be_loops = true;
         else if (word == "shape:")
            state = reading_shape;
         else if (state == reading_tiles)
         {
            int number;
            if (std::istringstream(word) >> number)
            {
               tile_t tile(number);
               if (tile.is_valid())
                  tiles.push_back(tile);
            }
         }
         else if (state == reading_lines)
         {
            color_t color;
            if (std::istringstream(word) >> color)
               lines.push_back(color);
         }
         else if (state == reading_shape)
         {
            if (word == "triangle")
               shape = puzzle_t::shape_t::triangle;
            else if (word == "any")
               shape = puzzle_t::shape_t::any;
         }
      }

      a_puzzle = puzzle_t(tiles, lines, must_be_loops, shape);

      return a_stream;
   }

   std::wistream& operator>>(std::wistream& a_stream, puzzle_t& a_puzzle)
   {
      enum { none, reading_tiles, reading_lines, reading_loops, reading_shape } state = none;
      bool must_be_loops = false;

      std::vector<tile_t> tiles;
      std::vector<color_t> lines;
      puzzle_t::shape_t shape = puzzle_t::shape_t::any;

      std::wstring word;
      while (a_stream >> word)
      {
         if (word == L"tiles:")
            state = reading_tiles;
         else if (word == L"lines:")
            state = reading_lines, must_be_loops = false;
         else if (word == L"loops:")
            state = reading_lines, must_be_loops = true;
         else if (word == L"shape:")
            state = reading_shape;
         else if (state == reading_tiles)
         {
            int number;
            if (std::wistringstream(word) >> number)
            {
               tile_t tile(number);
               if (tile.is_valid())
                  tiles.push_back(tile);
            }
         }
         else if (state == reading_lines)
         {
            color_t color;
            if (std::wistringstream(word) >> color)
               lines.push_back(color);
         }
         else if (state == reading_shape)
         {
            if (word == L"triangle")
               shape = puzzle_t::shape_t::triangle;
            else if (word == L"any")
               shape = puzzle_t::shape_t::any;
         }
      }

      a_puzzle = puzzle_t(tiles, lines, must_be_loops, shape);

      return a_stream;
   }


}
