#include "position.h"
#include "tantrix_tests_helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::tantrix;

namespace dak::tantrix::tests
{
	TEST_CLASS(position_tests)
	{
	public:
		
		TEST_METHOD(compare_positions)
		{
			Assert::AreEqual(position_t(), position_t(0, 0));

			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y), position_t(x, y));

			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreNotEqual(position_t(x, y), position_t(x + 1, y));

			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreNotEqual(position_t(x, y), position_t(x - 1, y));

			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreNotEqual(position_t(x, y), position_t(x, y + 1));

			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreNotEqual(position_t(x, y), position_t(x, y - 1));
		}

		TEST_METHOD(move_position)
		{
			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).move(0, 0), position_t(x, y));

			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).move(1, 0), position_t(x + 1, y));

			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).move(-1, 0), position_t(x - 1, y));

			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).move(0, 1), position_t(x, y + 1));

			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).move(0, -1), position_t(x, y - 1));

			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).move(7, -5), position_t(x + 7, y - 5));
		}

		TEST_METHOD(move_position_in_dir)
		{
			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					for (direction_t dir : directions)
						Assert::AreEqual(position_t(x, y).move(dir), position_t(x, y).move(dir.delta_x(), dir.delta_y()));
		}

		TEST_METHOD(rotate_position)
		{
			for (std::int8_t rot = 0; rot < 20; ++rot)
				Assert::AreEqual(position_t(0, 0), position_t(0, 0).rotate(rot));

			Assert::AreEqual(position_t(-1,  0), position_t(-1, 0).rotate(0));

			Assert::AreEqual(position_t( 0, -1), position_t(-1, 0).rotate(1));
			Assert::AreEqual(position_t( 1, -1), position_t(-1, 0).rotate(2));
			Assert::AreEqual(position_t( 1,  0), position_t(-1, 0).rotate(3));
			Assert::AreEqual(position_t( 0,  1), position_t(-1, 0).rotate(4));
			Assert::AreEqual(position_t(-1,  1), position_t(-1, 0).rotate(5));

			Assert::AreEqual(position_t(-1,  0), position_t(-1, 0).rotate(6));

			Assert::AreEqual(position_t(-1,  1), position_t(-1, 1).rotate(0));

			Assert::AreEqual(position_t(-1,  0), position_t(-1, 1).rotate(1));
			Assert::AreEqual(position_t( 0, -1), position_t(-1, 1).rotate(2));
			Assert::AreEqual(position_t( 1, -1), position_t(-1, 1).rotate(3));
			Assert::AreEqual(position_t( 1,  0), position_t(-1, 1).rotate(4));
			Assert::AreEqual(position_t( 0,  1), position_t(-1, 1).rotate(5));

			Assert::AreEqual(position_t(-1,  1), position_t(-1, 1).rotate(6));


			for (std::int8_t scale = -7; scale < 7; ++scale)
			{
				if (scale == 0)
					continue;

				Assert::AreEqual(position_t(-1 * scale,  0 * scale), position_t(-1 * scale, 0 * scale).rotate(0));

				Assert::AreEqual(position_t( 0 * scale, -1 * scale), position_t(-1 * scale, 0 * scale).rotate(1));
				Assert::AreEqual(position_t( 1 * scale, -1 * scale), position_t(-1 * scale, 0 * scale).rotate(2));
				Assert::AreEqual(position_t( 1 * scale,  0 * scale), position_t(-1 * scale, 0 * scale).rotate(3));
				Assert::AreEqual(position_t( 0 * scale,  1 * scale), position_t(-1 * scale, 0 * scale).rotate(4));
				Assert::AreEqual(position_t(-1 * scale,  1 * scale), position_t(-1 * scale, 0 * scale).rotate(5));

				Assert::AreEqual(position_t(-1 * scale,  0 * scale), position_t(-1 * scale, 0 * scale).rotate(6));

				Assert::AreEqual(position_t(-1 * scale,  1 * scale), position_t(-1 * scale, 1 * scale).rotate(0));

				Assert::AreEqual(position_t(-1 * scale,  0 * scale), position_t(-1 * scale, 1 * scale).rotate(1));
				Assert::AreEqual(position_t( 0 * scale, -1 * scale), position_t(-1 * scale, 1 * scale).rotate(2));
				Assert::AreEqual(position_t( 1 * scale, -1 * scale), position_t(-1 * scale, 1 * scale).rotate(3));
				Assert::AreEqual(position_t( 1 * scale,  0 * scale), position_t(-1 * scale, 1 * scale).rotate(4));
				Assert::AreEqual(position_t( 0 * scale,  1 * scale), position_t(-1 * scale, 1 * scale).rotate(5));

				Assert::AreEqual(position_t(-1 * scale,  1 * scale), position_t(-1 * scale, 1 * scale).rotate(6));
			}
		}

		TEST_METHOD(position_coordinates)
		{
			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).x(), x);

			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).y(), y);
		}

		TEST_METHOD(position_calculations)
		{
			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y) += position_t(5, 7), position_t(x + 5, y + 7));

			for (std::int8_t x = -6; x < 6; ++x)
				for (std::int8_t y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y) -= position_t(5, 7), position_t(x - 5, y - 7));
		}

		TEST_METHOD(position_relative)
		{
			Assert::AreEqual(true, position_t(0, 0).relative(position_t(-1, 0)).has_value());
			Assert::AreEqual(direction_t(0), position_t(0, 0).relative(position_t(-1, 0)).value());

			Assert::AreEqual(true, position_t(0, 0).relative(position_t(0, -1)).has_value());
			Assert::AreEqual(direction_t(1), position_t(0, 0).relative(position_t(0, -1)).value());

			Assert::AreEqual(true, position_t(0, 0).relative(position_t(1, -1)).has_value());
			Assert::AreEqual(direction_t(2), position_t(0, 0).relative(position_t(1, -1)).value());

			Assert::AreEqual(true, position_t(0, 0).relative(position_t(1, 0)).has_value());
			Assert::AreEqual(direction_t(3), position_t(0, 0).relative(position_t(1, 0)).value());

			Assert::AreEqual(true, position_t(0, 0).relative(position_t(0, 1)).has_value());
			Assert::AreEqual(direction_t(4), position_t(0, 0).relative(position_t(0, 1)).value());

			Assert::AreEqual(true, position_t(0, 0).relative(position_t(-1, 1)).has_value());
			Assert::AreEqual(direction_t(5), position_t(0, 0).relative(position_t(-1, 1)).value());

		}

	};
}
