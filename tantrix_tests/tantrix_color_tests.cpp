#include "color.h"
#include "tantrix_tests_helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::tantrix;

namespace dak::tantrix::tests
{
	TEST_CLASS(color_tests)
	{
	public:
		
		TEST_METHOD(compare_colors)
		{
			Assert::AreEqual(color_t(), color_t::red());

			Assert::AreEqual(color_t::red(), color_t::red());
			Assert::AreEqual(color_t::green(), color_t::green());
			Assert::AreEqual(color_t::blue(), color_t::blue());
			Assert::AreEqual(color_t::yellow(), color_t::yellow());

			Assert::AreNotEqual(color_t::red(), color_t::blue());
			Assert::AreNotEqual(color_t::red(), color_t::green());
			Assert::AreNotEqual(color_t::red(), color_t::yellow());
			Assert::AreNotEqual(color_t::green(), color_t::blue());
			Assert::AreNotEqual(color_t::green(), color_t::yellow());
			Assert::AreNotEqual(color_t::blue(), color_t::yellow());
		}
	};
}
