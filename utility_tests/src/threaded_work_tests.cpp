#include <dak/utility/threaded_work.h>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dak::utility::tests
{
	TEST_CLASS(threaded_work_tests)
	{
	public:
		
      TEST_METHOD(test_threaded_work)
      {
         threaded_work_t<size_t, size_t> worker;

         std::vector<std::future<size_t>> future_results;

         for (size_t i = 0; i < 10; ++i)
            future_results.emplace_back(worker.add_work(i, 0, [](size_t a_value, size_t a_recursion_depth)
            {
               return a_value + 7;
            }));

         size_t total = 0;
         for (auto& future_val : future_results)
            total += worker.wait_for(future_val, 0);

         const size_t expected_total = 7 * future_results.size()
                                     + (future_results.size() * (future_results.size() - 1)) / 2;
         Assert::AreEqual<size_t>(expected_total, total);
		}
	};
}
