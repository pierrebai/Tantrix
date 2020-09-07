#include "tile.h"
#include "tantrix_tests_helpers.h"

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

			for (std::uint16_t number = 0; number <= 56; ++number)
				Assert::AreEqual(tile_t(number), tile_t(number));

			for (std::uint16_t number = 0; number <= 56; ++number)
				for (std::uint16_t other_number = 0; other_number <= 56; ++other_number)
					if (other_number != number)
						Assert::AreNotEqual(tile_t(number), tile_t(other_number));
		}

		TEST_METHOD(tile_number)
		{
			Assert::AreEqual<int>(tile_t().number(), 0);

			for (std::uint16_t number = 0; number <= 56; ++number)
				Assert::AreEqual<int>(tile_t(number).number(), number);
		}

		TEST_METHOD(rotate_tile)
		{
			for (std::uint16_t number = 0; number <= 56; ++number)
			{
				for (int rot = 1; rot < 20; ++rot)
				{
					Assert::AreEqual(tile_t(number).color(direction_t(0)), tile_t(number).rotate(rot).color(direction_t(rot)));
				}
			}
		}

		TEST_METHOD(is_same_tile)
		{
			for (std::uint16_t number = 0; number <= 56; ++number)
				for (int rot = 1; rot < 6; ++rot)
					Assert::AreEqual(true, tile_t(number).is_same(tile_t(number).rotate(rot)));

			for (std::uint16_t number = 0; number <= 56; ++number)
				for (std::uint16_t other_number = 0; other_number <= 56; ++other_number)
					if (other_number != number)
						for (int rot = 1; rot < 6; ++rot)
							Assert::AreEqual(false, tile_t(number).is_same(tile_t(other_number).rotate(rot)));
		}
	};
}
