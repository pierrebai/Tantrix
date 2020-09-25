#include <dak/utility/progress.h>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dak::utility::tests
{
	TEST_CLASS(progress_tests)
	{
	public:
      struct dummy_progress_t : progress_t
      {
         dummy_progress_t() = default;
         dummy_progress_t(size_t a_report_every) : progress_t(a_report_every) {}

         void update_progress(size_t a_total_count_so_far) override
         {
            my_received_progress = a_total_count_so_far;
         }

         size_t my_received_progress = 0;
      };
		
		TEST_METHOD(test_progress)
		{
         dummy_progress_t p1;

         Assert::AreEqual<size_t>(0, p1.total_count_so_far());
         Assert::AreEqual<size_t>(0, p1.my_received_progress);

         p1.progress(1);

         Assert::AreEqual<size_t>(1, p1.total_count_so_far());
         Assert::AreEqual<size_t>(0, p1.my_received_progress);

         p1.progress(2);

         Assert::AreEqual<size_t>(3, p1.total_count_so_far());
         Assert::AreEqual<size_t>(0, p1.my_received_progress);

         p1.progress(progress_t::my_default_report_every);

         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 3, p1.total_count_so_far());
         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 3, p1.my_received_progress);

         p1.progress(5);

         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 8, p1.total_count_so_far());
         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 3, p1.my_received_progress);

         p1.flush_progress();

         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 8, p1.total_count_so_far());
         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 8, p1.my_received_progress);
      }
	};
}
