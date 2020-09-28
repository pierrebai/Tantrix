#include <dak/utility/stream_progress.h>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dak::utility::tests
{
	TEST_CLASS(stream_progress_tests)
	{
	public:
		
		TEST_METHOD(test_stream_progress)
		{
         std::ostringstream stream;
         stream_progress_t p1(stream);

         Assert::AreEqual<size_t>(0, p1.total_progress());
         Assert::AreEqual(std::string(), stream.str());

         p1.progress(1);

         // Note received progress not yet updated due to too-little progress.
         Assert::AreEqual<size_t>(1, p1.total_progress());
         Assert::AreEqual(std::string(), stream.str());

         p1.progress(2);

         // Note received progress not yet updated due to too-little progress.
         Assert::AreEqual<size_t>(3, p1.total_progress());
         Assert::AreEqual(std::string(), stream.str());

         p1.progress(progress_t::my_default_report_every);

         // Note received progress updated with enough progress done.
         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 3, p1.total_progress());
         Assert::IsTrue(stream.str().find("3") != std::string::npos);

         p1.progress(5);

         // Note received progress not yet updated due to too-little progress.
         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 8, p1.total_progress());
         Assert::IsTrue(stream.str().find("3") != std::string::npos);
         Assert::IsFalse(stream.str().find("8") != std::string::npos);

         p1.flush_progress();

         // Note received progress updated due to flush.
         Assert::AreEqual<size_t>(progress_t::my_default_report_every + 8, p1.total_progress());
         Assert::IsTrue(stream.str().find("3") != std::string::npos);
         Assert::IsTrue(stream.str().find("8") != std::string::npos);

         p1.clear_progress();
         p1.flush_progress();

         // Note received progress updated due to clear.
         Assert::AreEqual<size_t>(0, p1.total_progress());
         Assert::IsTrue(stream.str().find("0") != std::string::npos);
      }
	};
}
