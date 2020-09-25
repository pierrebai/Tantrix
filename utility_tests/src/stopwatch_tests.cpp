#include <dak/utility/stopwatch.h>

#include "CppUnitTest.h"

#include <thread>
#include <chrono>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono_literals;

namespace dak::utility::tests
{
	TEST_CLASS(stopwatch_tests)
	{
	public:
		
		TEST_METHOD(test_stopwatch)
		{
         stopwatch_t stopwatch;

         std::this_thread::sleep_for(1ms);

         Assert::AreNotEqual(0., stopwatch.elapsed());

         const double t1 = stopwatch.elapsed();
         std::this_thread::sleep_for(1ms);
         const double t2 = stopwatch.elapsed();

         Assert::AreNotEqual(t1, t2);

         stopwatch.stop();

		}

      TEST_METHOD(test_stopwatch_time_buffer)
      {
         double time = 0;
         stopwatch_t stopwatch(time);

         std::this_thread::sleep_for(1ms);

         stopwatch.elapsed();
         Assert::AreNotEqual(0., time);

         stopwatch.elapsed();
         const double t1 = time;
         std::this_thread::sleep_for(1ms);
         stopwatch.elapsed();
         const double t2 = time;

         Assert::AreNotEqual(t1, t2);

         // Stop updates the time buffer a last time and after stop,
         // the time buffer is no longer updated by the stopwatch.
         stopwatch.stop();

         stopwatch.elapsed();
         const double t3 = time;
         std::this_thread::sleep_for(1ms);
         stopwatch.elapsed();
         const double t4 = time;

         Assert::AreEqual(t3, t4);

      }
   };
}
