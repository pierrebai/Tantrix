#include "dak/tantrix/tile.h"
#include "dak/tantrix_tests/helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::tantrix;

namespace dak::tantrix::tests
{
	TEST_CLASS(tile_tests)
	{
	public:
		
		TEST_METHOD(compare_tiles)
		{
			Assert::AreEqual(tile_t(), tile_t(0));

			for (std::uint8_t number = 0; number <= 56; ++number)
				Assert::AreEqual(tile_t(number), tile_t(number));

			for (std::uint8_t number = 0; number <= 56; ++number)
				for (std::uint8_t other_number = 0; other_number <= 56; ++other_number)
					if (other_number != number)
						Assert::AreNotEqual(tile_t(number), tile_t(other_number));
		}

		TEST_METHOD(tile_number)
		{
			Assert::AreEqual<int>(tile_t().number(), 0);

			for (std::uint8_t number = 0; number <= 56; ++number)
				Assert::AreEqual<int>(tile_t(number).number(), number);
		}

      TEST_METHOD(rotate_tile)
      {
         for (std::uint8_t number = 1; number <= 56; ++number)
         {
            for (int rot = 1; rot < 20; ++rot)
            {
               const auto t1 = tile_t(number);
               const auto c1 = t1.color(direction_t(0));
               const auto t2 = tile_t(number).rotate(rot);
               const auto c2 = t2.color(direction_t(rot));
               Assert::AreEqual(c1, c2);
            }
         }
      }

      TEST_METHOD(rotate_in_place_tile)
		{
			for (std::uint8_t number = 1; number <= 56; ++number)
			{
				for (int rot = 1; rot < 20; ++rot)
				{
					auto t1 = tile_t(number);
					const auto c1 = t1.color(direction_t(0));
					t1.rotate_in_place(rot);
					const auto c2 = t1.color(direction_t(rot));
					Assert::AreEqual(c1, c2);
				}
			}
		}

		TEST_METHOD(is_same_tile)
		{
			for (std::uint8_t number = 0; number <= 56; ++number)
				for (int rot = 1; rot < 6; ++rot)
					Assert::AreEqual(true, tile_t(number).is_same(tile_t(number).rotate(rot)));

			for (std::uint8_t number = 0; number <= 56; ++number)
				for (std::uint8_t other_number = 0; other_number <= 56; ++other_number)
					if (other_number != number)
						for (int rot = 1; rot < 6; ++rot)
							Assert::AreEqual(false, tile_t(number).is_same(tile_t(other_number).rotate(rot)));
		}

      TEST_METHOD(tile_find_color)
      {
         for (std::uint8_t number = 0; number <= 56; ++number)
         {
            const tile_t tile(number);
            for (auto dir : directions)
            {
               color_t color_in_dir = tile.color(dir);
               Assert::AreEqual(tile.find_color(color_in_dir, dir), dir);
               Assert::AreNotEqual(tile.find_color(color_in_dir, dir.rotate(1)), dir);
            }
         }
      }


      TEST_METHOD(tile_has_color)
      {
         for (std::uint8_t number = 1; number <= 56; ++number)
         {
            const tile_t tile(number);
            for (auto dir : directions)
            {
               Assert::IsTrue(tile.has_color(tile.color(dir)));
            }
         }

         for (std::uint8_t number = 1; number <= 56; ++number)
         {
            const tile_t tile(number);
            int colors_count = 0;
            for (auto color : { color_t::red(), color_t::green(), color_t::blue(), color_t::yellow() })
               if (tile.has_color(color))
                  colors_count += 1;
            Assert::AreEqual(3, colors_count);
         }
      }
   };
}
