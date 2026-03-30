#pragma once

#include <puzzle_api.h>

#include <dak/tantrix/puzzle.h>
#include <dak/tantrix/solution.h>

#include <future>

namespace dak::tantrix_solver_app
{
   struct tantrix_puzzle_api_t : puzzle_api_t
   {
      ~tantrix_puzzle_api_t() = default;

      bool start_solving(const solver::problem_t::ptr_t& a_puzzle) override;
      void stop_solving() override;
      bool is_solved() override;
      all_solutions_t get_solutions() const override;

      solver::problem_t::ptr_t convert_text_to_puzzle(const std::string& a_puzzle_desc) override;
      std::string convert_puzzle_to_text(const solver::problem_t::ptr_t & a_puzzle) override;
      
      std::vector<std::pair<std::string, std::string>> load_puzzle_descriptions(const std::filesystem::path& a_path) override;
      solver::problem_t::ptr_t load_puzzle_from_description(const std::string& a_desc) override;
      void save_puzzle(const std::filesystem::path& a_path, const solver::problem_t::ptr_t& a_puzzle) override;

      void save_solutions(const std::filesystem::path& a_path, const all_solutions_t& some_solutions) override;
      all_solutions_t load_solutions(const std::filesystem::path& a_path) override;

      std::vector<std::string> get_puzzle_description(const solver::problem_t::ptr_t& a_problem) override;
      std::vector<std::string> get_solutions_description(const all_solutions_t& some_solutions) override;

      void draw_puzzle_tiles(
         QGraphicsView* a_view,
         const solver::problem_t::ptr_t& a_puzzle,
         const std::string& a_selected_tile) override;

      void draw_solution(
         QGraphicsView* a_view,
         const solver::problem_t::ptr_t& a_puzzle,
         const solver::solution_t::ptr_t& a_solution,
         const std::string& a_selected_tile) override;

   private:
      std::future<std::set<tantrix::solution_t>> my_async_solving;
      std::set<tantrix::solution_t>              my_solutions;
   };
}
