#pragma once

#include <dak/solver/problem.h>
#include <dak/solver/solution.h>
#include <dak/solver/solve.h>

#include <dak/utility/progress.h>
#include <dak/utility/stopwatch.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

class QGraphicsView;

namespace dak::tantrix_solver_app
{
   struct puzzle_api_t : protected utility::progress_t
   {
      using ptr_t = std::shared_ptr<puzzle_api_t>;
      using all_solutions_t = std::vector<solver::solution_t::ptr_t>;

      puzzle_api_t()
      : progress_t(10000), my_solving_stopwatch(my_solving_time_buffer)
      {}

      virtual ~puzzle_api_t() = default;

      // Solve the given puzzle.
      virtual bool start_solving(const solver::problem_t::ptr_t& a_puzzle) = 0;
      virtual void stop_solving() = 0;
      virtual bool is_solved() = 0;
      virtual all_solutions_t get_solutions() const = 0;

      std::string get_solving_time() { my_solving_stopwatch.elapsed(); return my_solving_time_buffer; }
      size_t get_solving_attempts() const { return my_solving_attempts.load(); }

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
      virtual void save_solutions(const std::filesystem::path& a_path, const all_solutions_t& some_solutions) = 0;
      virtual all_solutions_t load_solutions(const std::filesystem::path& a_path) = 0;

      // Convert a puzzle and its solutions to a human readable format for display to the user.s
      virtual std::vector<std::string> get_puzzle_description(const solver::problem_t::ptr_t& a_problem) = 0;
      virtual std::vector<std::string> get_solutions_description(const all_solutions_t& some_solutions) = 0;

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

      // Asynchronous puzzle solving update from progress_t interface.
      void update_progress(size_t a_total_count_so_far) override
      {
         my_solving_attempts = a_total_count_so_far;
      }

   protected:
      std::atomic<size_t>       my_solving_attempts = 0;
      dak::utility::stopwatch_t my_solving_stopwatch;
      std::string               my_solving_time_buffer;
   };
}
