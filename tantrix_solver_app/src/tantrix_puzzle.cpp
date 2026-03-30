#include <tantrix_puzzle_api.h>

#include <dak/solver/problem.h>
#include <dak/solver/solution.h>
#include <dak/solver/solve.h>
#include <dak/tantrix/solution.h>
#include <dak/tantrix/stream.h>
#include <dak/tantrix/triangle_puzzle.h>
#include <dak/tantrix/any_shape_puzzle.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <map>

namespace dak::tantrix_solver_app
{
   using namespace std;

   bool tantrix_puzzle_api_t::start_solving(const solver::problem_t::ptr_t& a_puzzle)
   {
      auto puzzle = std::dynamic_pointer_cast<const tantrix::puzzle_t>(a_puzzle);
      if (!puzzle)
         return false;

      stop_progress(false);
      my_solving_attempts = 0;
      my_solving_stopwatch.start();
      my_async_solving = std::async(std::launch::async, [self = this, puzzle = puzzle]()
      {
         try
         {
            if (auto triangle_puzzle = std::dynamic_pointer_cast<const tantrix::triangle_puzzle_t>(puzzle)) {
               tantrix::solution_t initial_solution(*triangle_puzzle);
               return solver::solver_t<tantrix::triangle_puzzle_t, tantrix::solution_t>::solve(*triangle_puzzle, initial_solution, *self);
            }

            if (auto any_shape_puzzle = std::dynamic_pointer_cast<const tantrix::any_shape_puzzle_t>(puzzle)) {
               tantrix::solution_t initial_solution(*any_shape_puzzle);
               return solver::solver_t<tantrix::any_shape_puzzle_t, tantrix::solution_t>::solve(*any_shape_puzzle, initial_solution, *self);
            }
         }
         catch (const std::exception&)
         {
         }
         return std::set<tantrix::solution_t>();
      });
      return true;
   }

   void tantrix_puzzle_api_t::stop_solving()
   {
      stop_progress(true);
   }

   bool tantrix_puzzle_api_t::is_solved()
   {
      if (!my_async_solving.valid())
         return false;

      if (my_async_solving.wait_for(1us) != future_status::ready)
         return false;

      my_solutions = my_async_solving.get();
      return true;
   }

   tantrix_puzzle_api_t::all_solutions_t tantrix_puzzle_api_t::get_solutions() const
   {
      all_solutions_t solver_solutions;
      for (const auto& solution : my_solutions)
         solver_solutions.push_back(std::make_shared<tantrix::solution_t>(solution));
      return solver_solutions;
   }
   static std::shared_ptr<tantrix::puzzle_t> load_tantrix_puzzle(std::istream& a_stream)
   {
      std::shared_ptr<tantrix::puzzle_t> puzzle;
      a_stream >> puzzle;
      return puzzle;
   }

   solver::problem_t::ptr_t tantrix_puzzle_api_t::convert_text_to_puzzle(const std::string& a_puzzle_desc)
   {
      std::istringstream stream(a_puzzle_desc);
      return load_tantrix_puzzle(stream);
   }

   std::string tantrix_puzzle_api_t::convert_puzzle_to_text(const solver::problem_t::ptr_t& a_puzzle)
   {
      auto puzzle = std::dynamic_pointer_cast<tantrix::puzzle_t>(a_puzzle);
      if (!puzzle)
         return {};
      std::ostringstream stream;
      stream << puzzle << std::endl;
      return stream.str();
   }

   std::shared_ptr<tantrix::puzzle_t> load_tantrix_puzzle(const std::filesystem::path& a_path)
   {
      if (a_path.empty())
         return {};

      std::ifstream puzzle_stream(a_path);
      return load_tantrix_puzzle(puzzle_stream);
   }

   std::vector<std::pair<std::string, std::string>> tantrix_puzzle_api_t::load_puzzle_descriptions(const std::filesystem::path& a_path)
   {
      std::shared_ptr<tantrix::puzzle_t> puzzle = load_tantrix_puzzle(a_path);
      if (!puzzle)
         return {};
      return { { a_path.filename().string(), a_path.string() } };
   }

   solver::problem_t::ptr_t tantrix_puzzle_api_t::load_puzzle_from_description(const std::string& a_desc)
   {
      return load_tantrix_puzzle(std::filesystem::path(a_desc));
   }

   static void save_tantrix_puzzle(std::ostream& a_stream, const solver::problem_t::ptr_t& a_puzzle)
   {
      a_stream << std::dynamic_pointer_cast<tantrix::puzzle_t>(a_puzzle);
   }

   void tantrix_puzzle_api_t::save_puzzle(const std::filesystem::path& a_path, const solver::problem_t::ptr_t& a_puzzle)
   {
      if (a_path.empty())
         return;

      std::ofstream puzzle_stream(a_path);
      save_tantrix_puzzle(puzzle_stream, a_puzzle);
   }

   void tantrix_puzzle_api_t::save_solutions(const std::filesystem::path& a_path, const all_solutions_t& some_solutions)
   {
      if (a_path.empty())
         return;

      std::ofstream solutions_stream(a_path);

      for (const auto& solution : some_solutions) {
         auto puzzle_solution = std::dynamic_pointer_cast<tantrix::solution_t>(solution);
         if (!puzzle_solution)
            continue;
         solutions_stream << *puzzle_solution << std::endl;
      }
   }

   tantrix_puzzle_api_t::all_solutions_t tantrix_puzzle_api_t::load_solutions(const std::filesystem::path& a_path)
   {
      if (a_path.empty())
         return {};

      all_solutions_t solver_solutions;

      tantrix::all_solutions_t solutions;
      std::ifstream solutions_stream(a_path);
      solutions_stream >> solutions;
      for (auto& solution : solutions)
            solver_solutions.push_back(std::make_shared<tantrix::solution_t>(solution));

      return solver_solutions;
   }

   std::vector<std::string> tantrix_puzzle_api_t::get_puzzle_description(const solver::problem_t::ptr_t& a_problem)
   {
      auto puzzle = std::dynamic_pointer_cast<tantrix::puzzle_t>(a_problem);
      if (!puzzle)
         return {};

      std::vector<std::string> description;

      for (const auto& tile : puzzle->initial_tiles())
      {
         std::ostringstream stream;
         stream << "Tile #" << tile;
         description.emplace_back(stream.str().c_str());
      }

      static std::map<tantrix::color_t, const char*> color_names =
      {
         { tantrix::color_t::red(),    "Red"    },
         { tantrix::color_t::green(),  "Green"  },
         { tantrix::color_t::blue(),   "Blue"   },
         { tantrix::color_t::yellow(), "Yellow" },
      };

      const char* const line_type = (puzzle->must_be_loops() ? "Loop" : "Line");

      for (const auto& color : puzzle->line_colors())
      {
         std::ostringstream stream;
         stream << color_names[color] << " " << line_type;
         description.emplace_back(stream.str().c_str());
      }

      if (std::dynamic_pointer_cast<tantrix::triangle_puzzle_t>(puzzle))
         description.emplace_back("Must be a triangle");

      if (puzzle->holes_count().has_value())
      {
         std::ostringstream stream;
         stream << "Must have " << puzzle->holes_count().value() << " holes";
         description.emplace_back(stream.str().c_str());
      }

      return description;
   }

   std::vector<std::string> tantrix_puzzle_api_t::get_solutions_description(const all_solutions_t& some_solutions)
   {
      std::vector<std::string> description;

      size_t solution_index = 0;
      for (const auto& abstract_solution : some_solutions)
      {
         auto solution = std::dynamic_pointer_cast<tantrix::solution_t>(abstract_solution);
         if (!solution)
            return {};

         std::ostringstream stream;

         if (solution_index == 1000 && some_solutions.size() > 1010)
         {
            stream << "And " << some_solutions.size() - solution_index << " other solutions";
            description.emplace_back(stream.str().c_str());
            break;
         }

         stream << "Solution #" << ++solution_index << ":\n";
         const size_t similar_count = solution->count_similar_solutions();
         if (similar_count > 0) {
            stream << "(with " << solution->count_similar_solutions() << " similar solution" << (similar_count == 1 ? "" : "s") << ")\n";
         }
         for (size_t i = 0; i < solution->tiles_count(); ++i)
         {
            const auto& placed_tile = solution->tiles()[i];
            stream << "    "
                   << std::setw(3) << placed_tile.pos.x()
                   << " / "
                   << std::setw(3) << placed_tile.pos.y()
                   << " : tile #" << std::setw(2) << placed_tile.tile.number() << "\n";
         }
         description.emplace_back(stream.str().c_str());
      }

      return description;
   }
}
