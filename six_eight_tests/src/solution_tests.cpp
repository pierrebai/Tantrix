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

         Assert::AreNotEqual(solution_t(), solution_t(tile_t('a'), position_t(0, 0)));
      }

      TEST_METHOD(add_tile_to_solution)
      {
         solution_t sol;
         sol.add_tile(tile_t('a'), position_t(0, 0));
         Assert::AreEqual<size_t>(1, sol.tiles_count());
         Assert::AreEqual(sol.tile_at(position_t(0, 0)), tile_t('a'));

         sol.add_tile(tile_t('b'), position_t(4, 0));
         Assert::AreEqual<size_t>(2, sol.tiles_count());
         Assert::AreEqual(sol.tile_at(position_t(4, 0)), tile_t('b'));

         sol.add_tile(tile_t('c'), position_t(0, 4));
         Assert::AreEqual<size_t>(3, sol.tiles_count());
         Assert::AreEqual(sol.tile_at(position_t(0,  4)), tile_t('c'));

         sol.add_tile(tile_t('d'), position_t(4, 4));
         Assert::AreEqual<size_t>(4, sol.tiles_count());
         Assert::AreEqual(sol.tile_at(position_t(4, 4)), tile_t('d'));
      }

      TEST_METHOD(solution_is_occupied)
      {
         solution_t sol;
         Assert::IsFalse(sol.is_occupied(position_t(0, 0)));

         sol.add_tile(tile_t('a'), position_t(0, 0));
         Assert::IsTrue( sol.is_occupied(position_t( 0,  0)));
         Assert::IsTrue( sol.is_occupied(position_t(-1,  0)));
         Assert::IsTrue( sol.is_occupied(position_t( 0, -1)));
         Assert::IsTrue( sol.is_occupied(position_t( 1, -1)));
         Assert::IsTrue( sol.is_occupied(position_t( 1,  0)));
         Assert::IsFalse(sol.is_occupied(position_t( 1,  1)));
         Assert::IsTrue( sol.is_occupied(position_t(-1,  1)));

         sol.add_tile(tile_t('b'), position_t(5, 0));
         Assert::IsTrue( sol.is_occupied(position_t( 5,  0)));
         Assert::IsTrue( sol.is_occupied(position_t( 6,  0)));
         Assert::IsTrue( sol.is_occupied(position_t( 0, -1)));
         Assert::IsTrue( sol.is_occupied(position_t( 1, -1)));
         Assert::IsTrue( sol.is_occupied(position_t( 7,  1)));
         Assert::IsTrue( sol.is_occupied(position_t( 7,  0)));
         Assert::IsTrue( sol.is_occupied(position_t( 5,  1)));
      }

      TEST_METHOD(solution_is_compatible)
      {
         {
            solution_t sol;
            Assert::IsTrue(sol.is_compatible(placed_tile_t::make(tile_t('B'), position_t(0, 0))));

            sol.add_tile(tile_t('B'), position_t(0, 0));
            Assert::IsFalse(sol.is_compatible(placed_tile_t::make(tile_t('^'), position_t( 0,  0))));
            Assert::IsTrue( sol.is_compatible(placed_tile_t::make(tile_t('^'), position_t( 1,  0))));
            Assert::IsTrue( sol.is_compatible(placed_tile_t::make(tile_t('^'), position_t( 2,  0))));
            Assert::IsFalse(sol.is_compatible(placed_tile_t::make(tile_t('^'), position_t( 1,  1))));
            Assert::IsFalse(sol.is_compatible(placed_tile_t::make(tile_t('^'), position_t( 1,  1))));
            Assert::IsTrue( sol.is_compatible(placed_tile_t::make(tile_t('^'), position_t( 2,  1))));
            Assert::IsFalse(sol.is_compatible(placed_tile_t::make(tile_t('^'), position_t(-1,  1))));
         }
         {
            solution_t sol;

            Assert::IsTrue(sol.is_compatible(placed_tile_t::make(tile_t('8').rotate(1), position_t(0, 0))));
            sol.add_tile(tile_t('8').rotate(1), position_t(0, 0));

            Assert::IsTrue(sol.is_compatible(placed_tile_t::make(tile_t('f').rotate(2), position_t(0, 1))));
            sol.add_tile(tile_t('f').rotate(1), position_t(0, 1));

            Assert::IsTrue(sol.is_compatible(placed_tile_t::make(tile_t('d').rotate(3), position_t(2, 1))));
            sol.add_tile(tile_t('d').rotate(1), position_t(2, 1));
         }
      }

      TEST_METHOD(normalize_solution)
      {
         solution_t sol;
         sol.add_tile(tile_t('A').rotate(1), position_t(2, 5));
         sol.add_tile(tile_t('d').rotate(2), position_t(0, 0));
         sol.add_tile(tile_t('m'), position_t(2, 6));
         sol.add_tile(tile_t('q'), position_t(0, 4));
         sol.add_tile(tile_t('s'), position_t(3, 2));
         sol.add_tile(tile_t('u').rotate(2), position_t(3, 0));
         sol.add_tile(tile_t('x').rotate(2), position_t(1, 0));
         sol.add_tile(tile_t('z').rotate(2), position_t(1, 3));
         sol.normalize();

         solution_t sol_unordered;
         sol_unordered.add_tile(tile_t('u').rotate(2), position_t(3, 0));
         sol_unordered.add_tile(tile_t('x').rotate(2), position_t(1, 0));
         sol_unordered.add_tile(tile_t('m'), position_t(2, 6));
         sol_unordered.add_tile(tile_t('z').rotate(2), position_t(1, 3));
         sol_unordered.add_tile(tile_t('A').rotate(1), position_t(2, 5));
         sol_unordered.add_tile(tile_t('d').rotate(2), position_t(0, 0));
         sol_unordered.add_tile(tile_t('s'), position_t(3, 2));
         sol_unordered.add_tile(tile_t('q'), position_t(0, 4));

         sol_unordered.normalize();
         Assert::AreEqual(sol, sol_unordered);
      }

      TEST_METHOD(solution_is_valid)
      {
         {
            solution_t sol;
            sol.add_tile(tile_t('A').rotate(1), position_t(2, 5));
            sol.add_tile(tile_t('d').rotate(2), position_t(0, 0));
            sol.add_tile(tile_t('m'), position_t(2, 6));
            sol.add_tile(tile_t('q'), position_t(0, 4));

            Assert::IsFalse(sol.is_valid());

            sol.add_tile(tile_t('s'), position_t(3, 2));
            sol.add_tile(tile_t('u').rotate(2), position_t(3, 0));
            sol.add_tile(tile_t('x').rotate(2), position_t(1, 0));
            sol.add_tile(tile_t('z').rotate(2), position_t(1, 3));

            Assert::IsTrue(sol.is_valid());
         }
      }
   };
}
