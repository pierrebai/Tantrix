#include <dak/utility/thread_pool.h>

#include <chrono>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dak::utility::tests
{
	TEST_CLASS(thread_pool_tests)
	{
	public:
		
      struct dummy_work_provider_t : work_provider_t
      {
         void stop() override
         {
            my_stop = true;
         }

         // Check if stop was requested.
         bool is_stopped() const
         {
            return my_stop;
         }

         // The wait-or-execute implementation, called in a loop
         // by the threads in the thread pool.
         void wait_or_execute()
         {
            my_execute_count += 1;
            if (my_execute_count >= my_stop_after_n_execute)
               stop();
         }

         size_t                  my_stop_after_n_execute = 800;
         std::atomic<size_t>     my_execute_count = 0;
         std::atomic<bool>       my_stop = false;
      };

		TEST_METHOD(test_thread_pool)
		{
         dummy_work_provider_t dummy_provider;

         Assert::AreEqual<size_t>(0, dummy_provider.my_execute_count);
         Assert::AreNotEqual<size_t>(0, dummy_provider.my_stop_after_n_execute);
         Assert::IsFalse(dummy_provider.my_stop);

         thread_pool_t thread_pool(dummy_provider, 4);

         using namespace std::chrono_literals;

         while (!dummy_provider.is_stopped())
            std::this_thread::sleep_for(1us);

         Assert::IsTrue(dummy_provider.my_execute_count >= dummy_provider.my_stop_after_n_execute);
         Assert::IsTrue(dummy_provider.my_stop);
      }
	};
}
