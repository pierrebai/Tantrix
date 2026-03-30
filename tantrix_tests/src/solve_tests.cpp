#include "dak/tantrix/tantrix.h"
#include "dak/solver/solve.h"
#include "dak/utility/progress.h"
#include "dak/tantrix_tests/helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::tantrix;
using namespace dak::utility;

namespace dak::tantrix::tests
{
	TEST_CLASS(solve_tests)
	{
	public:
		
		TEST_METHOD(solve_junior_puzzle)
		{
         const std::vector<tile_t> junior_tiles = { 3, 5, 8, 12, 14, 43, 46, 50, 52, 54, };
         const std::vector<color_t> junior_loops = { color_t::blue(), };
         const bool junior_must_be_loops = true;
         auto junior_puzzle = any_shape_puzzle_t(junior_tiles, junior_loops, junior_must_be_loops);
         auto initial_solution = solution_t(junior_puzzle);

         struct dummy_progress_t : progress_t
         {
            void update_progress(size_t a_total_count_so_far) override {}
         };
         dummy_progress_t progress;
         auto junior_solutions = dak::solver::solver_t<any_shape_puzzle_t, solution_t>::solve(junior_puzzle, initial_solution, progress);

         Assert::AreEqual<size_t>(3, junior_solutions.size());
		}

      TEST_METHOD(solve_professor_puzzle)
      {
         const std::vector<tile_t> professor_tiles = { 2, 11, 15, 17, 20, 30, 38, 39, 44, 45, 51, 56, };
         const std::vector<color_t> professor_loops = { color_t::blue(), color_t::yellow(), };
         const bool professor_must_be_loops = true;
         auto professor_puzzle = any_shape_puzzle_t(professor_tiles, professor_loops, professor_must_be_loops);
         auto initial_solution = solution_t(professor_puzzle);

         struct dummy_progress_t : progress_t
         {
            void update_progress(size_t a_total_count_so_far) override {}
         };
         dummy_progress_t progress;
         auto professor_solutions = dak::solver::solver_t<any_shape_puzzle_t, solution_t>::solve(professor_puzzle, initial_solution, progress);

         Assert::AreEqual<size_t>(1, professor_solutions.size());
      }
   };
}
