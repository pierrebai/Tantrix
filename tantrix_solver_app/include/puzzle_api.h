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

      virtual solver::solution_t::ptr_t make_initial_solution(const solver::problem_t::ptr_t& a_puzzle) = 0;

      virtual solver::problem_t::ptr_t load_puzzle_from_text(const std::string& a_puzzle_desc) = 0;
      virtual std::string save_puzzle_to_text(const solver::problem_t::ptr_t & a_puzzle) = 0;

      virtual std::vector<std::pair<std::string, std::string>> load_puzzle_descriptions(const std::filesystem::path& a_path) = 0;
      virtual solver::problem_t::ptr_t load_puzzle_from_description(const std::string& a_desc) = 0;
      virtual void save_puzzle(const std::filesystem::path& a_path, const solver::problem_t::ptr_t& a_puzzle) = 0;

      virtual void save_solutions(const std::filesystem::path& a_path, const solver::all_solutions_t& some_solutions) = 0;
      virtual solver::all_solutions_t load_solutions(const std::filesystem::path& a_path) = 0;

      virtual std::vector<std::string> get_puzzle_description(const solver::problem_t::ptr_t& a_problem) = 0;
      virtual std::vector<std::string> get_solutions_description(const solver::all_solutions_t& some_solutions) = 0;

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
