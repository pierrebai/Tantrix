#include "dak/six_eight/six_eight.h"
#include "dak/six_eight/stream.h"

#include <sstream>
#include <string>

namespace dak::six_eight
{
   std::ostream& operator<<(std::ostream& a_stream, const direction_t& a_direction)
   {
      switch (a_direction.as_int())
      {
         case 0:  return a_stream << "left";
         case 1:  return a_stream << "top";
         case 3:  return a_stream << "right";
         case 5:  return a_stream << "bottom";
         default: return a_stream;
      }
   }

   std::wostream& operator<<(std::wostream& a_stream, const direction_t& a_direction)
   {
      switch (a_direction.as_int())
      {
         case 0:  return a_stream << "left";
         case 1:  return a_stream << "top";
         case 3:  return a_stream << "right";
         case 5:  return a_stream << "bottom";
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

   std::ostream& operator<<(std::ostream& a_stream, const tile_t& a_tile)
   {
      a_stream << a_tile.id() << " rotated " << a_tile.get_rotation() << " times ";
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const tile_t& a_tile)
   {
      a_stream << a_tile.id() << " rotated " << a_tile.get_rotation() << " times ";
      return a_stream;
   }

   std::istream& operator>>(std::istream& a_stream, tile_t& a_tile)
   {
      int tile_id = 0;
      int rotation = 0;
      std::string word1;
      std::string word2;
      a_stream >> tile_id >> word1 >> word2 >> rotation;
      if (a_stream)
         a_tile = tile_t(tile_id).rotate_in_place(rotation);
      return a_stream;
   }

   std::wistream& operator>>(std::wistream& a_stream, tile_t& a_tile)
   {
      wchar_t tile_id = 0;
      int rotation = 0;
      std::wstring word1;
      std::wstring word2;
      a_stream >> tile_id >> word1 >> word2 >> rotation;
      if (a_stream)
         a_tile = tile_t(char(tile_id)).rotate_in_place(rotation);
      return a_stream;
   }

   std::ostream& operator<<(std::ostream& a_stream, const solution_t& a_solution)
   {
      for (int y = 0; y < 8; ++y) {
         for (int x = 0; x < 6; ++x) {
            tile_t::id_t id = a_solution.tile_at(x, y).id();
            a_stream << (id ? id : ' ');
         }
         
         if (y < a_solution.tiles_count()) {
            const placed_tile_t& placed = a_solution.tiles()[y];
            a_stream << "    " << placed.pos << " " << placed.tile;
         }
         a_stream << std::endl;
      }
      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const solution_t& a_solution)
   {
      for (int y = 0; y < 8; ++y) {
         for (int x = 0; x < 6; ++x) {
            tile_t::id_t id = a_solution.tile_at(x, y).id();
            a_stream << (id ? id : ' ');
         }
         
         if (y < a_solution.tiles_count()) {
            const placed_tile_t& placed = a_solution.tiles()[y];
            a_stream << "    " << placed.pos << " " << placed.tile;
         }
         a_stream << std::endl;
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

   std::ostream& operator<<(std::ostream& a_stream, const std::shared_ptr<puzzle_t>& a_puzzle)
   {
      if (!a_puzzle)
         return a_stream;

      for (const auto& tile : a_puzzle->initial_tiles())
         a_stream << tile.id() << ' ';

      return a_stream;
   }

   std::wostream& operator<<(std::wostream& a_stream, const std::shared_ptr<puzzle_t>& a_puzzle)
   {
      if (!a_puzzle)
         return a_stream;

      for (const auto& tile : a_puzzle->initial_tiles())
         a_stream << ' ' << tile.id();

      return a_stream;
   }

   std::istream& operator>>(std::istream& a_stream, std::shared_ptr<puzzle_t>& a_puzzle)
   {
      std::vector<tile_t> tiles;

      std::string word;
      while (a_stream >> word)
      {
         char id;
         if (std::istringstream(word) >> id)
         {
            tile_t tile(id);
            if (tile.is_valid())
               tiles.push_back(tile);
         }
      }

      a_puzzle.reset(new puzzle_t(tiles));

      return a_stream;
   }

   std::wistream& operator>>(std::wistream& a_stream, std::shared_ptr<puzzle_t>& a_puzzle)
   {
      std::vector<tile_t> tiles;

      std::wstring word;
      while (a_stream >> word)
      {
         int id;
         if (std::wistringstream(word) >> id)
         {
            tile_t tile(id);
            if (tile.is_valid())
               tiles.push_back(tile);
         }
      }

      a_puzzle.reset(new puzzle_t(tiles));

      return a_stream;
   }


}
