#include "dak/six_eight/six_eight.h"
#include "dak/solver/solve.h"
#include "dak/utility/progress.h"
#include "dak/six_eight_tests/helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::six_eight;
using namespace dak::utility;

namespace dak::six_eight::tests
{
   TEST_CLASS(solve_tests)
   {
   public:
      
      TEST_METHOD(solve_simple_puzzle)
      {
         const std::vector<tile_t> simple_tiles = { 'm', 'q', 'd', 'x', 'u', 's', 'A', 'z' };
         auto simple_puzzle = std::make_shared<puzzle_t>(simple_tiles);
         auto initial_solution = std::make_shared<solution_t>();

         struct dummy_progress_t : progress_t
         {
            void update_progress(size_t a_total_count_so_far) override {}
         };
         dummy_progress_t progress;
         auto simple_solutions = solver::solve(simple_puzzle, initial_solution, progress);

         Assert::AreEqual<size_t>(2, simple_solutions.size());
      }
   };
}
