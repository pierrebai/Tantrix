#include "dak/six_eight/solution.h"
#include "dak/six_eight_tests/helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::six_eight;

namespace dak::six_eight::tests
{
   TEST_CLASS(solution_tests)
   {
   public:
      
      TEST_METHOD(compare_solutions)
      {
         Assert::AreEqual(solution_t(), solution_t());

         Assert::AreNotEqual(solution_t(), solution_t(tile_t(3), position_t(0, 0)));
      }

      TEST_METHOD(add_tile_to_solution)
      {
         solution_t sol;
         sol.add_tile(tile_t(13), position_t(-2,  2));
         Assert::AreEqual<size_t>(1, sol.tiles_count());
         Assert::AreEqual(sol.tile_at(position_t(-2,  2)), tile_t(13));

         sol.add_tile(tile_t(12), position_t(-1, -2));
         Assert::AreEqual<size_t>(2, sol.tiles_count());
         Assert::AreEqual(sol.tile_at(position_t(-1, -2)), tile_t(12));

         sol.add_tile(tile_t( 7), position_t(-1,  1));
         Assert::AreEqual<size_t>(3, sol.tiles_count());
         Assert::AreEqual(sol.tile_at(position_t(-1,  1)), tile_t( 7));

         sol.add_tile(tile_t( 1), position_t( 0, -2));
         Assert::AreEqual<size_t>(4, sol.tiles_count());
         Assert::AreEqual(sol.tile_at(position_t( 0, -2)), tile_t( 1));
      }

      TEST_METHOD(solution_is_occupied)
      {
         solution_t sol;
         Assert::IsFalse(sol.is_occupied(position_t(0, 0)));

         sol.add_tile(tile_t(1), position_t(0, 0));
         Assert::IsTrue( sol.is_occupied(position_t( 0,  0)));
         Assert::IsFalse(sol.is_occupied(position_t(-1,  0)));
         Assert::IsFalse(sol.is_occupied(position_t( 0, -1)));
         Assert::IsFalse(sol.is_occupied(position_t( 1, -1)));
         Assert::IsFalse(sol.is_occupied(position_t( 1,  0)));
         Assert::IsFalse(sol.is_occupied(position_t( 1,  1)));
         Assert::IsFalse(sol.is_occupied(position_t(-1,  1)));

         sol.add_tile(tile_t(4), position_t(1, 0));
         Assert::IsTrue( sol.is_occupied(position_t( 0,  0)));
         Assert::IsFalse(sol.is_occupied(position_t(-1,  0)));
         Assert::IsFalse(sol.is_occupied(position_t( 0, -1)));
         Assert::IsFalse(sol.is_occupied(position_t( 1, -1)));
         Assert::IsTrue( sol.is_occupied(position_t( 1,  0)));
         Assert::IsFalse(sol.is_occupied(position_t( 1,  1)));
         Assert::IsFalse(sol.is_occupied(position_t(-1,  1)));
      }

      TEST_METHOD(solution_is_compatible)
      {
			solution_t sol;
			Assert::IsTrue(sol.is_compatible(placed_tile_t::make(tile_t(4), position_t(0, 0))));

			sol.add_tile(tile_t(1), position_t(0, 0));
			Assert::IsFalse(sol.is_compatible(placed_tile_t::make(tile_t(4), position_t( 0,  0))));
			Assert::IsTrue( sol.is_compatible(placed_tile_t::make(tile_t(4), position_t(-1,  0))));
			Assert::IsFalse(sol.is_compatible(placed_tile_t::make(tile_t(4), position_t( 0, -1))));
			Assert::IsTrue( sol.is_compatible(placed_tile_t::make(tile_t(4), position_t( 1, -1))));
			Assert::IsTrue( sol.is_compatible(placed_tile_t::make(tile_t(4), position_t( 1,  0))));
			Assert::IsFalse(sol.is_compatible(placed_tile_t::make(tile_t(4), position_t( 0,  1))));
			Assert::IsFalse(sol.is_compatible(placed_tile_t::make(tile_t(4), position_t(-1,  1))));

			Assert::IsTrue(sol.is_compatible(placed_tile_t::make(tile_t(4), position_t( 1,  1))));
			Assert::IsTrue(sol.is_compatible(placed_tile_t::make(tile_t(4), position_t( 2,  0))));
			Assert::IsTrue(sol.is_compatible(placed_tile_t::make(tile_t(4), position_t( 2,  1))));
			Assert::IsTrue(sol.is_compatible(placed_tile_t::make(tile_t(4), position_t( 1,  2))));
			Assert::IsTrue(sol.is_compatible(placed_tile_t::make(tile_t(4), position_t(-2, -5))));
      }

      TEST_METHOD(normalize_solution)
      {
         solution_t sol;
         sol.add_tile(tile_t(13), position_t(-2,  4));
         sol.add_tile(tile_t(12), position_t(-1,  0));
         sol.add_tile(tile_t( 7), position_t(-1,  3));
         sol.add_tile(tile_t( 1), position_t( 0,  0));
         sol.add_tile(tile_t( 9), position_t( 0,  1));
         sol.add_tile(tile_t( 3), position_t( 0,  2));
         sol.add_tile(tile_t(10), position_t( 0,  4));
         sol.add_tile(tile_t( 2), position_t( 1,  0));
         sol.add_tile(tile_t( 4), position_t( 1,  2));
         sol.add_tile(tile_t(11), position_t( 1,  4));
         sol.add_tile(tile_t( 8), position_t( 2,  1));
         sol.add_tile(tile_t( 5), position_t( 2,  2));
         sol.add_tile(tile_t( 6), position_t( 3,  2));

         solution_t sol_unordered;
         sol_unordered.add_tile(tile_t( 4), position_t( 1,  2));
         sol_unordered.add_tile(tile_t(12), position_t(-1,  0));
         sol_unordered.add_tile(tile_t( 8), position_t( 2,  1));
         sol_unordered.add_tile(tile_t( 9), position_t( 0,  1));
         sol_unordered.add_tile(tile_t(13), position_t(-2,  4));
         sol_unordered.add_tile(tile_t( 3), position_t( 0,  2));
         sol_unordered.add_tile(tile_t( 1), position_t( 0,  0));
         sol_unordered.add_tile(tile_t( 5), position_t( 2,  2));
         sol_unordered.add_tile(tile_t(10), position_t( 0,  4));
         sol_unordered.add_tile(tile_t( 2), position_t( 1,  0));
         sol_unordered.add_tile(tile_t( 7), position_t(-1,  3));
         sol_unordered.add_tile(tile_t(11), position_t( 1,  4));
         sol_unordered.add_tile(tile_t( 6), position_t( 3,  2));

         sol_unordered.normalize();
         Assert::AreEqual(sol, sol_unordered);
      }

      TEST_METHOD(solution_is_valid)
      {
         {
            solution_t sol;
            sol.add_tile(tile_t(1), position_t( 0,  0));
            sol.add_tile(tile_t(4), position_t( 1,  0));
            sol.add_tile(tile_t(7), position_t( 0,- 1));

            Assert::IsTrue(sol.is_valid());

            // Create four-sided hole.
            sol.add_tile(tile_t(10), position_t(2, -1));

            Assert::IsFalse(sol.is_valid());
         }

         {
            // Create hole with three yellow.
            solution_t sol;
            sol.add_tile(tile_t( 6), position_t( 0,  0));
            sol.add_tile(tile_t( 4), position_t( 1,  0));
            sol.add_tile(tile_t(10), position_t( 2, -1));

            Assert::IsFalse(sol.is_valid());
         }
      }
   };
}
