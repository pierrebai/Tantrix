#pragma once

#include <dak/solver/problem.h>
#include <dak/solver/solution.h>
#include <dak/solver/solve.h>

#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>

class QGraphicsView;

namespace dak::tantrix_solver_app
{
   struct puzzle_api_t
   {
      using ptr_t = std::shared_ptr<puzzle_api_t>;

      virtual ~puzzle_api_t() = default;

      // Create the initial solution for the given puzzle, which is used to start the solving process.
      virtual solver::solution_t::ptr_t make_initial_solution(const solver::problem_t::ptr_t& a_puzzle) = 0;

      // Function to convert to and from text to allow the user to edit the puzzle.
      virtual solver::problem_t::ptr_t convert_text_to_puzzle(const std::string& a_puzzle_desc) = 0;
      virtual std::string convert_puzzle_to_text(const solver::problem_t::ptr_t & a_puzzle) = 0;

      // Load textual description of the puzzles contained in the given file.
      //
      // The first string is a human readable name of the puzzle, and the second string
      // is a machine-parsable description of the puzzle that can be converted to a puzzle
      // using load_puzzle_from_description.
      virtual std::vector<std::pair<std::string, std::string>> load_puzzle_descriptions(const std::filesystem::path& a_path) = 0;
      virtual solver::problem_t::ptr_t load_puzzle_from_description(const std::string& a_desc) = 0;

      // Save a puzzle to a file.
      virtual void save_puzzle(const std::filesystem::path& a_path, const solver::problem_t::ptr_t& a_puzzle) = 0;

      // Load and save puzzle solutions to a file.
      virtual void save_solutions(const std::filesystem::path& a_path, const solver::all_solutions_t& some_solutions) = 0;
      virtual solver::all_solutions_t load_solutions(const std::filesystem::path& a_path) = 0;

      // Convert a puzzle and its solutions to a human readable format for display to the user.s
      virtual std::vector<std::string> get_puzzle_description(const solver::problem_t::ptr_t& a_problem) = 0;
      virtual std::vector<std::string> get_solutions_description(const solver::all_solutions_t& some_solutions) = 0;

      // Draw the puzzle and its solution on the given view.
      virtual void draw_puzzle_tiles(
         QGraphicsView* a_view,
         const solver::problem_t::ptr_t& a_puzzle,
         const std::string& a_selected_tile) = 0;

      virtual void draw_solution(
         QGraphicsView* a_view,
         const solver::problem_t::ptr_t& a_puzzle,
         const solver::solution_t::ptr_t& a_solution,
         const std::string& a_selected_tile) = 0;
   };
}
