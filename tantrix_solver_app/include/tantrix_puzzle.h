#pragma once

#include <dak/solver/problem.h>
#include <dak/solver/solution.h>
#include <dak/solver/solve.h>

#include <filesystem>
#include <iostream>

namespace dak::tantrix_solver_app
{
   solver::problem_t::ptr_t load_tantrix_puzzle(std::istream& a_stream);
   solver::problem_t::ptr_t load_tantrix_puzzle(const std::filesystem::path& a_path);
   void save_tantrix_puzzle(std::ostream& a_stream, const solver::problem_t::ptr_t& a_puzzle);
   void save_tantrix_puzzle(const std::filesystem::path& a_path, const solver::problem_t::ptr_t& a_puzzle);
   void save_tantrix_solutions(const std::filesystem::path& a_path, const solver::all_solutions_t& some_solutions);
   solver::all_solutions_t load_tantrix_solutions(const std::filesystem::path& a_path);
   std::vector<std::string> get_tantrix_puzzle_description(const solver::problem_t::ptr_t& a_problem);
   std::vector<std::string> get_tantrix_solutions_description(const solver::all_solutions_t& some_solutions);
}
