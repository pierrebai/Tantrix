#include "dak/tantrix/position.h"
#include "dak/tantrix_tests/helpers.h"

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

			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y), position_t(x, y));

			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreNotEqual(position_t(x, y), position_t(x + 1, y));

			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreNotEqual(position_t(x, y), position_t(x - 1, y));

			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreNotEqual(position_t(x, y), position_t(x, y + 1));

			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreNotEqual(position_t(x, y), position_t(x, y - 1));
		}

		TEST_METHOD(move_position)
		{
			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).move(0, 0), position_t(x, y));

			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).move(1, 0), position_t(x + 1, y));

			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).move(-1, 0), position_t(x - 1, y));

			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).move(0, 1), position_t(x, y + 1));

			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).move(0, -1), position_t(x, y - 1));

			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).move(7, -5), position_t(x + 7, y - 5));
		}

		TEST_METHOD(move_position_in_dir)
		{
			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					for (direction_t dir : directions)
						Assert::AreEqual(position_t(x, y).move(dir), position_t(x, y).move(dir.delta_x(), dir.delta_y()));
		}

      TEST_METHOD(add_position)
      {
         for (int x = -6; x < 6; ++x)
            for (int y = -6; y < 6; ++y)
               Assert::AreEqual(position_t(x, y) += position_t(0, 0), position_t(x, y));

         for (int x = -6; x < 6; ++x)
            for (int y = -6; y < 6; ++y)
               Assert::AreEqual(position_t(x, y) += position_t(1, 0), position_t(x + 1, y));

         for (int x = -6; x < 6; ++x)
            for (int y = -6; y < 6; ++y)
               Assert::AreEqual(position_t(x, y) += position_t(-1, 0), position_t(x - 1, y));

         for (int x = -6; x < 6; ++x)
            for (int y = -6; y < 6; ++y)
               Assert::AreEqual(position_t(x, y) += position_t(0, 1), position_t(x, y + 1));

         for (int x = -6; x < 6; ++x)
            for (int y = -6; y < 6; ++y)
               Assert::AreEqual(position_t(x, y) += position_t(0, -1), position_t(x, y - 1));

         for (int x = -6; x < 6; ++x)
            for (int y = -6; y < 6; ++y)
               Assert::AreEqual(position_t(x, y) += position_t(7, -5), position_t(x + 7, y - 5));
      }

      TEST_METHOD(subtract_position)
      {
         for (int x = -6; x < 6; ++x)
            for (int y = -6; y < 6; ++y)
               Assert::AreEqual(position_t(x, y) -= position_t(0, 0), position_t(x, y));

         for (int x = -6; x < 6; ++x)
            for (int y = -6; y < 6; ++y)
               Assert::AreEqual(position_t(x, y) -= position_t(1, 0), position_t(x - 1, y));

         for (int x = -6; x < 6; ++x)
            for (int y = -6; y < 6; ++y)
               Assert::AreEqual(position_t(x, y) -= position_t(-1, 0), position_t(x + 1, y));

         for (int x = -6; x < 6; ++x)
            for (int y = -6; y < 6; ++y)
               Assert::AreEqual(position_t(x, y) -= position_t(0, 1), position_t(x, y - 1));

         for (int x = -6; x < 6; ++x)
            for (int y = -6; y < 6; ++y)
               Assert::AreEqual(position_t(x, y) -= position_t(0, -1), position_t(x, y + 1));

         for (int x = -6; x < 6; ++x)
            for (int y = -6; y < 6; ++y)
               Assert::AreEqual(position_t(x, y) -= position_t(7, -5), position_t(x - 7, y + 5));
      }

      TEST_METHOD(rotate_position)
		{
			for (int rot = 0; rot < 20; ++rot)
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


			for (int scale = -7; scale < 7; ++scale)
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

      TEST_METHOD(rotate_in_place_position)
      {
         for (int rot = 0; rot < 20; ++rot)
            Assert::AreEqual(position_t(0, 0), position_t(0, 0).rotate_in_place(rot));

         Assert::AreEqual(position_t(-1, 0), position_t(-1, 0).rotate_in_place(0));

         Assert::AreEqual(position_t(0, -1), position_t(-1, 0).rotate_in_place(1));
         Assert::AreEqual(position_t(1, -1), position_t(-1, 0).rotate_in_place(2));
         Assert::AreEqual(position_t(1, 0), position_t(-1, 0).rotate_in_place(3));
         Assert::AreEqual(position_t(0, 1), position_t(-1, 0).rotate_in_place(4));
         Assert::AreEqual(position_t(-1, 1), position_t(-1, 0).rotate_in_place(5));

         Assert::AreEqual(position_t(-1, 0), position_t(-1, 0).rotate_in_place(6));

         Assert::AreEqual(position_t(-1, 1), position_t(-1, 1).rotate_in_place(0));

         Assert::AreEqual(position_t(-1, 0), position_t(-1, 1).rotate_in_place(1));
         Assert::AreEqual(position_t(0, -1), position_t(-1, 1).rotate_in_place(2));
         Assert::AreEqual(position_t(1, -1), position_t(-1, 1).rotate_in_place(3));
         Assert::AreEqual(position_t(1, 0), position_t(-1, 1).rotate_in_place(4));
         Assert::AreEqual(position_t(0, 1), position_t(-1, 1).rotate_in_place(5));

         Assert::AreEqual(position_t(-1, 1), position_t(-1, 1).rotate_in_place(6));


         for (int scale = -7; scale < 7; ++scale)
         {
            if (scale == 0)
               continue;

            Assert::AreEqual(position_t(-1 * scale, 0 * scale), position_t(-1 * scale, 0 * scale).rotate_in_place(0));

            Assert::AreEqual(position_t(0 * scale, -1 * scale), position_t(-1 * scale, 0 * scale).rotate_in_place(1));
            Assert::AreEqual(position_t(1 * scale, -1 * scale), position_t(-1 * scale, 0 * scale).rotate_in_place(2));
            Assert::AreEqual(position_t(1 * scale, 0 * scale), position_t(-1 * scale, 0 * scale).rotate_in_place(3));
            Assert::AreEqual(position_t(0 * scale, 1 * scale), position_t(-1 * scale, 0 * scale).rotate_in_place(4));
            Assert::AreEqual(position_t(-1 * scale, 1 * scale), position_t(-1 * scale, 0 * scale).rotate_in_place(5));

            Assert::AreEqual(position_t(-1 * scale, 0 * scale), position_t(-1 * scale, 0 * scale).rotate_in_place(6));

            Assert::AreEqual(position_t(-1 * scale, 1 * scale), position_t(-1 * scale, 1 * scale).rotate_in_place(0));

            Assert::AreEqual(position_t(-1 * scale, 0 * scale), position_t(-1 * scale, 1 * scale).rotate_in_place(1));
            Assert::AreEqual(position_t(0 * scale, -1 * scale), position_t(-1 * scale, 1 * scale).rotate_in_place(2));
            Assert::AreEqual(position_t(1 * scale, -1 * scale), position_t(-1 * scale, 1 * scale).rotate_in_place(3));
            Assert::AreEqual(position_t(1 * scale, 0 * scale), position_t(-1 * scale, 1 * scale).rotate_in_place(4));
            Assert::AreEqual(position_t(0 * scale, 1 * scale), position_t(-1 * scale, 1 * scale).rotate_in_place(5));

            Assert::AreEqual(position_t(-1 * scale, 1 * scale), position_t(-1 * scale, 1 * scale).rotate_in_place(6));
         }
      }

      TEST_METHOD(position_coordinates)
		{
			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).x(), x);

			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y).y(), y);
		}

		TEST_METHOD(position_calculations)
		{
			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
					Assert::AreEqual(position_t(x, y) += position_t(5, 7), position_t(x + 5, y + 7));

			for (int x = -6; x < 6; ++x)
				for (int y = -6; y < 6; ++y)
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
