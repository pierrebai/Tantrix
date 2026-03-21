#include <six_eight_puzzle_api.h>

#include <dak/solver/problem.h>
#include <dak/solver/solution.h>
#include <dak/solver/solve.h>
#include <dak/six_eight/solution.h>
#include <dak/six_eight/stream.h>
#include <dak/six_eight/puzzle.h>

#include <fstream>
#include <sstream>
#include <iomanip>

namespace dak::tantrix_solver_app
{
   solver::solution_t::ptr_t six_eight_puzzle_api_t::make_initial_solution(const solver::problem_t::ptr_t& a_puzzle)
   {
      auto puzzle = std::dynamic_pointer_cast<six_eight::puzzle_t>(a_puzzle);
      if (!puzzle)
         return {};
         
      return std::make_shared<six_eight::solution_t>();
   }

   static std::shared_ptr<six_eight::puzzle_t> load_six_eight_puzzle(std::istream& a_stream)
   {
      std::shared_ptr<six_eight::puzzle_t> puzzle;
      a_stream >> puzzle;
      return puzzle;
   }

   solver::problem_t::ptr_t six_eight_puzzle_api_t::convert_text_to_puzzle(const std::string& a_puzzle_desc)
   {
      std::vector<six_eight::tile_t> tiles;
      {
         std::istringstream stream(a_puzzle_desc);
         for (int i = 0; i < 8; ++i) {
            six_eight::tile_t::id_t tile_id;
            stream >> tile_id;
            tiles.emplace_back(six_eight::tile_t(tile_id));
         }

         if (!stream)
            return {};
      }

      return std::make_shared<six_eight::puzzle_t>(tiles);
   }

   std::string six_eight_puzzle_api_t::convert_puzzle_to_text(const solver::problem_t::ptr_t & a_puzzle)
   {
      std::ostringstream stream;
      auto puzzle = std::dynamic_pointer_cast<six_eight::puzzle_t>(a_puzzle);
      if (puzzle) {
         for (const auto& tile : puzzle->initial_tiles())
            stream << tile.id() << " ";
      }
      return stream.str();
   }

   static std::shared_ptr<six_eight::puzzle_t> load_six_eight_puzzle(const std::filesystem::path& a_path)
   {
      if (a_path.empty())
         return{};

      std::ifstream puzzle_stream(a_path);
      return load_six_eight_puzzle(puzzle_stream);
   }

   static std::vector<std::string> load_six_eight_puzzle_descriptions(std::istream& a_stream)
   {
      std::vector<std::string> puzzles;
      while (a_stream) {
         std::shared_ptr<six_eight::puzzle_t> puzzle;
         a_stream >> puzzle;
         if (puzzle) {
            std::ostringstream stream;
            for (const auto& tile : puzzle->initial_tiles())
               stream <<tile.id() << " ";
            puzzles.emplace_back(stream.str());
         }
      }
      return puzzles;
   }

   std::vector<std::pair<std::string, std::string>> six_eight_puzzle_api_t::load_puzzle_descriptions(const std::filesystem::path& a_path)
   {
      if (a_path.empty())
         return{};

      std::ifstream puzzle_stream(a_path);
      auto descriptions = load_six_eight_puzzle_descriptions(puzzle_stream);
      std::vector<std::pair<std::string, std::string>> label_and_descs;
      for (const auto& desc : descriptions)
         label_and_descs.emplace_back(std::string("6x8 ") + desc, std::string("6x8 ") + desc);
      return label_and_descs;
   }

   solver::problem_t::ptr_t six_eight_puzzle_api_t::load_puzzle_from_description(const std::string& a_desc)
   {
      std::istringstream stream(a_desc);
      std::string prefix;
      stream >> prefix;
      if (prefix != "6x8")
         return {};
      return load_six_eight_puzzle(stream);
   }

   static void save_six_eight_puzzle(std::ostream& a_stream, const solver::problem_t::ptr_t& a_puzzle)
   {
      a_stream << std::dynamic_pointer_cast<six_eight::puzzle_t>(a_puzzle);
   }

   void six_eight_puzzle_api_t::save_puzzle(const std::filesystem::path& a_path, const solver::problem_t::ptr_t& a_puzzle)
   {
      if (a_path.empty())
         return;

      std::ofstream puzzle_stream(a_path);
      save_six_eight_puzzle(puzzle_stream, a_puzzle);
   }

   void six_eight_puzzle_api_t::save_solutions(const std::filesystem::path& a_path, const solver::all_solutions_t& some_solutions)
   {
      if (a_path.empty())
         return;

      std::ofstream solutions_stream(a_path);

      for (const auto& solution : some_solutions) {
         auto puzzle_solution = std::dynamic_pointer_cast<six_eight::solution_t>(solution.first);
         if (!puzzle_solution)
            continue;

         solutions_stream << *puzzle_solution << std::endl;
      }
   }

   solver::all_solutions_t six_eight_puzzle_api_t::load_solutions(const std::filesystem::path& a_path)
   {
      if (a_path.empty())
         return {};

      solver::all_solutions_t solver_solutions;

      six_eight::all_solutions_t solutions;
      std::ifstream solutions_stream(a_path);
      solutions_stream >> solutions;
      for (auto& [solution, count] : solutions)
            solver_solutions[std::make_shared<six_eight::solution_t>(solution)] = count;

      return solver_solutions;
   }

   std::vector<std::string> six_eight_puzzle_api_t::get_puzzle_description(const solver::problem_t::ptr_t& a_problem)
   {
      auto puzzle = std::dynamic_pointer_cast<six_eight::puzzle_t>(a_problem);
      if (!puzzle)
         return {};

      std::vector<std::string> description;

      for (const auto& tile : puzzle->initial_tiles()) {
         std::ostringstream stream;
         stream << "Tile " << tile.id();
         description.emplace_back(stream.str().c_str());
      }

      return description;
   }

   std::vector<std::string> six_eight_puzzle_api_t::get_solutions_description(const solver::all_solutions_t& some_solutions)
   {
      std::vector<std::string> description;

      size_t solution_index = 0;
      for (const auto& [abstract_solution, count] : some_solutions)
      {
         auto solution = std::dynamic_pointer_cast<six_eight::solution_t>(abstract_solution);
         if (!solution)
            return {};

         std::ostringstream stream;

         if (solution_index == 1000 && some_solutions.size() > 1010)
         {
            stream << "And " << some_solutions.size() - solution_index << " other solutions";
            description.emplace_back(stream.str().c_str());
            break;
         }

         stream << "Solution #" << ++solution_index << " found "<< count << " times" << ":\n";
         for (size_t i = 0; i < solution->tiles_count(); ++i)
         {
            const auto& placed_tile = solution->tiles()[i];
            stream << "    "
                   << std::setw(3) << placed_tile.pos.x()
                   << " / "
                   << std::setw(3) << placed_tile.pos.y()
                   << " : tile" << std::setw(2) << placed_tile.tile.id() << "\n";
         }
         description.emplace_back(stream.str().c_str());
      }

      return description;
   }
}
