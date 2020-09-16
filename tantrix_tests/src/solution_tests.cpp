#include "dak/tantrix/solution.h"
#include "dak/tantrix_tests/helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::tantrix;

namespace dak::tantrix::tests
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
			Assert::AreEqual(sol.tile_at(position_t(-2,  2)), tile_t(13));

			sol.add_tile(tile_t(12), position_t(-1, -2));
			Assert::AreEqual(sol.tile_at(position_t(-1, -2)), tile_t(12));

			sol.add_tile(tile_t( 7), position_t(-1,  1));
			Assert::AreEqual(sol.tile_at(position_t(-1,  1)), tile_t( 7));

			sol.add_tile(tile_t( 1), position_t( 0, -2));
			Assert::AreEqual(sol.tile_at(position_t( 0, -2)), tile_t( 1));
		}

		TEST_METHOD(get_borders_solution)
		{
			solution_t sol;
			Assert::AreEqual<size_t>(0, sol.get_borders(color_t::red()).size());

			solution_t sol2;
			sol2.add_tile(tile_t(1), position_t(0, 0));
			Assert::AreEqual<size_t>(2, sol2.get_borders(color_t::red()).size());
			Assert::AreEqual<size_t>(2, sol2.get_borders(color_t::blue()).size());
			Assert::AreEqual<size_t>(2, sol2.get_borders(color_t::yellow()).size());

			sol2.add_tile(tile_t(4), position_t(1, 0));
			Assert::AreEqual<size_t>(2, sol2.get_borders(color_t::red()).size());
			Assert::AreEqual<size_t>(3, sol2.get_borders(color_t::blue()).size());
			Assert::AreEqual<size_t>(4, sol2.get_borders(color_t::yellow()).size());
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
			Assert::IsTrue(sol.is_compatible(tile_t(4), position_t(0, 0)));

			sol.add_tile(tile_t(1), position_t(0, 0));
			Assert::IsFalse(sol.is_compatible(tile_t(4), position_t( 0,  0)));
			Assert::IsTrue( sol.is_compatible(tile_t(4), position_t(-1,  0)));
			Assert::IsFalse(sol.is_compatible(tile_t(4), position_t( 0, -1)));
			Assert::IsTrue( sol.is_compatible(tile_t(4), position_t( 1, -1)));
			Assert::IsTrue( sol.is_compatible(tile_t(4), position_t( 1,  0)));
			Assert::IsFalse(sol.is_compatible(tile_t(4), position_t( 0,  1)));
			Assert::IsFalse(sol.is_compatible(tile_t(4), position_t(-1,  1)));

			Assert::IsTrue(sol.is_compatible(tile_t(4), position_t(1, 1)));
			Assert::IsTrue(sol.is_compatible(tile_t(4), position_t(2, 0)));
			Assert::IsTrue(sol.is_compatible(tile_t(4), position_t(2, 1)));
			Assert::IsTrue(sol.is_compatible(tile_t(4), position_t(1, 2)));
			Assert::IsTrue(sol.is_compatible(tile_t(4), position_t(-2, -5)));
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

		TEST_METHOD(solution_has_line)
		{
			{
				solution_t sol;
				sol.add_tile(tile_t( 1), position_t(0, 0));
				sol.add_tile(tile_t( 4), position_t(1, 0));
				sol.add_tile(tile_t(13), position_t(1, 1));

				Assert::IsTrue( sol.has_line(color_t::red(), false));
				Assert::IsFalse(sol.has_line(color_t::blue(), false));
				Assert::IsFalse(sol.has_line(color_t::yellow(), false));

				Assert::IsFalse(sol.has_line(color_t::red(), true));
				Assert::IsFalse(sol.has_line(color_t::blue(), true));
				Assert::IsFalse(sol.has_line(color_t::yellow(), true));

				// Add a tile the will not link red.
				sol.add_tile(tile_t(9), position_t(-1, 0));

				Assert::IsFalse(sol.has_line(color_t::red(), false));
				Assert::IsFalse(sol.has_line(color_t::blue(), false));
				Assert::IsFalse(sol.has_line(color_t::yellow(), false));
			}
		}
	};
}
