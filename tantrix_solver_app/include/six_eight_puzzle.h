#pragma once

#include <dak/solver/problem.h>
#include <dak/solver/solution.h>
#include <dak/solver/solve.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace dak::tantrix_solver_app
{
   solver::problem_t::ptr_t load_six_eight_puzzle(std::istream& a_stream);
   solver::problem_t::ptr_t load_six_eight_puzzle(const std::string& a_puzzle_desc);
   solver::problem_t::ptr_t load_six_eight_puzzle(const std::filesystem::path& a_path);
   std::vector<std::string> load_six_eight_puzzles(std::istream& a_stream);
   std::vector<std::string> load_six_eight_puzzles(const std::filesystem::path& a_path);
   void save_six_eight_puzzle(std::ostream& a_stream, const solver::problem_t::ptr_t& a_puzzle);
   void save_six_eight_puzzle(const std::filesystem::path& a_path, const solver::problem_t::ptr_t& a_puzzle);
   void save_six_eight_solutions(const std::filesystem::path& a_path, const solver::all_solutions_t& some_solutions);
   solver::all_solutions_t load_six_eight_solutions(const std::filesystem::path& a_path);
   std::vector<std::string> get_six_eight_puzzle_description(const solver::problem_t::ptr_t& a_problem);
   std::vector<std::string> get_six_eight_solutions_description(const solver::all_solutions_t& some_solutions);
}
