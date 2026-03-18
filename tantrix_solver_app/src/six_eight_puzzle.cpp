#include <six_eight_puzzle.h>

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
   solver::problem_t::ptr_t load_six_eight_puzzle(std::istream& a_stream)
   {
      std::shared_ptr<six_eight::puzzle_t> puzzle;
      a_stream >> puzzle;
      return puzzle;
   }

   solver::problem_t::ptr_t load_six_eight_puzzle(const std::string& a_puzzle_desc)
   {
      std::istringstream stream(a_puzzle_desc);
      std::string word;
      stream >> word;
      if (word != "6x8")
         return {};
      return load_six_eight_puzzle(stream);
   }

   solver::problem_t::ptr_t load_six_eight_puzzle(const std::filesystem::path& a_path)
   {
      if (a_path.empty())
         return{};

      std::ifstream puzzle_stream(a_path);
      return load_six_eight_puzzle(puzzle_stream);
   }

   std::vector<std::string> load_six_eight_puzzles(std::istream& a_stream)
   {
      std::vector<std::string> puzzles;
      while (a_stream) {
         std::shared_ptr<six_eight::puzzle_t> puzzle;
         a_stream >> puzzle;
         if (puzzle) {
            std::string desc;
            std::ostringstream stream;
            stream << "6x8 ";
            for (const auto& tile : puzzle->initial_tiles())
               stream <<tile.id() << " ";
            puzzles.emplace_back(stream.str());
         }
      }
      return puzzles;
   }

   std::vector<std::string> load_six_eight_puzzles(const std::filesystem::path& a_path)
   {
      if (a_path.empty())
         return{};

      std::ifstream puzzle_stream(a_path);
      return load_six_eight_puzzles(puzzle_stream);
   }

   void save_six_eight_puzzle(std::ostream& a_stream, const solver::problem_t::ptr_t& a_puzzle)
   {
      a_stream << std::dynamic_pointer_cast<six_eight::puzzle_t>(a_puzzle);
   }

   void save_six_eight_puzzle(const std::filesystem::path& a_path, const solver::problem_t::ptr_t& a_puzzle)
   {
      if (a_path.empty())
         return;

      std::ofstream puzzle_stream(a_path);
      save_six_eight_puzzle(puzzle_stream, a_puzzle);
   }

   void save_six_eight_solutions(const std::filesystem::path& a_path, const solver::all_solutions_t& some_solutions)
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

   solver::all_solutions_t load_six_eight_solutions(const std::filesystem::path& a_path)
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

   std::vector<std::string> get_six_eight_puzzle_description(const solver::problem_t::ptr_t& a_problem)
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

   std::vector<std::string> get_six_eight_solutions_description(const solver::all_solutions_t& some_solutions)
   {
      std::vector<std::string> description;

      size_t solution_index = 0;
      for (const auto& [abstract_solution, count] : some_solutions)
      {
         std::ostringstream stream;

         if (solution_index == 1000 && some_solutions.size() > 1010)
         {
            stream << "And " << some_solutions.size() - solution_index << " other solutions";
            description.emplace_back(stream.str().c_str());
            break;
         }

         auto solution = std::dynamic_pointer_cast<six_eight::solution_t>(abstract_solution);

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
