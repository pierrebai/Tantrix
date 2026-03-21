#pragma once

#include <puzzle_api.h>

namespace dak::tantrix_solver_app
{
   struct six_eight_puzzle_api_t : puzzle_api_t
   {
      ~six_eight_puzzle_api_t() = default;

      solver::solution_t::ptr_t make_initial_solution(const solver::problem_t::ptr_t& a_puzzle) override;

      solver::problem_t::ptr_t convert_text_to_puzzle(const std::string& a_puzzle_desc) override;
      std::string convert_puzzle_to_text(const solver::problem_t::ptr_t & a_puzzle) override;

      std::vector<std::pair<std::string, std::string>> load_puzzle_descriptions(const std::filesystem::path& a_path) override;
      solver::problem_t::ptr_t load_puzzle_from_description(const std::string& a_desc) override;
      void save_puzzle(const std::filesystem::path& a_path, const solver::problem_t::ptr_t& a_puzzle) override;

      void save_solutions(const std::filesystem::path& a_path, const solver::all_solutions_t& some_solutions) override;
      solver::all_solutions_t load_solutions(const std::filesystem::path& a_path) override;

      std::vector<std::string> get_puzzle_description(const solver::problem_t::ptr_t& a_problem) override;
      std::vector<std::string> get_solutions_description(const solver::all_solutions_t& some_solutions) override;

      void draw_puzzle_tiles(
         QGraphicsView* a_view,
         const solver::problem_t::ptr_t& a_puzzle,
         const std::string& a_selected_tile) override;

      void draw_solution(
         QGraphicsView* a_view,
         const solver::problem_t::ptr_t& a_puzzle,
         const solver::solution_t::ptr_t& a_solution,
         const std::string& a_selected_tile) override;
   };
}
