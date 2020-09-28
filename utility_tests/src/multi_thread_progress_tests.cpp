#include <dak/utility/multi_thread_progress.h>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dak::utility::tests
{
	TEST_CLASS(multi_thread_progress_tests)
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
		
		TEST_METHOD(test_multi_thread_progress)
		{
         dummy_progress_t dummy_prog;
         multi_thread_progress_t mt_prog(dummy_prog);
         per_thread_progress_t pt_prog(mt_prog);

         Assert::AreEqual<size_t>(0, pt_prog.total_progress());
         Assert::AreEqual<size_t>(0, dummy_prog.my_received_progress);

         pt_prog.progress(1);

         // Note: received multi_thread_progress not yet updated due to too-little multi_thread_progress.
         Assert::AreEqual<size_t>(1, pt_prog.total_progress());
         Assert::AreEqual<size_t>(0, dummy_prog.my_received_progress);

         pt_prog.progress(2);

         // Note: received multi_thread_progress not yet updated due to too-little multi_thread_progress.
         Assert::AreEqual<size_t>(3, pt_prog.total_progress());
         Assert::AreEqual<size_t>(0, dummy_prog.my_received_progress);

         pt_prog.progress(progress_t::my_default_report_every);

         // Note: received multi_thread_progress updated with enough multi_thread_progress done.
         // Note: the per-thread progress gets reset to zero.
         Assert::AreEqual<size_t>(0, pt_prog.total_progress());
         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 3, dummy_prog.my_received_progress);

         pt_prog.progress(5);

         // Note: received multi_thread_progress not yet updated due to too-little multi_thread_progress.
         Assert::AreEqual<size_t>(5, pt_prog.total_progress());
         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 3, dummy_prog.my_received_progress);

         pt_prog.flush_progress();

         // Note: multi-thread progress received progress updated due to flush, but not the dummy progress
         Assert::AreEqual<size_t>(0, pt_prog.total_progress());
         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 3, dummy_prog.my_received_progress);

         mt_prog.flush_progress();

         // Note: received multi_thread_progress updated due to flush.
         Assert::AreEqual<size_t>(0, pt_prog.total_progress());
         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 8, dummy_prog.my_received_progress);

         pt_prog.clear_progress();
         pt_prog.flush_progress();

         // Note: received progress updated due to clear.
         Assert::AreEqual<size_t>(0, pt_prog.total_progress());
         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 8, dummy_prog.my_received_progress);

         mt_prog.clear_progress();
         mt_prog.flush_progress();

         // Note: received progress updated due to clear.
         Assert::AreEqual<size_t>(0, pt_prog.total_progress());
         Assert::AreEqual<size_t>(0, dummy_prog.my_received_progress);
      }
	};
}
