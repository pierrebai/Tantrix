#include "dak/six_eight/direction.h"
#include "dak/six_eight_tests/helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::six_eight;

namespace dak::six_eight::tests
{
   TEST_CLASS(direction_tests)
   {
   public:
      
      TEST_METHOD(compare_directions)
      {
         Assert::AreEqual(direction_t(), direction_t(0));

         for (int dir = 0; dir < 4; ++dir)
            Assert::AreEqual(direction_t(dir), direction_t(dir));

         for (int dir = 0; dir < 4; ++dir)
            Assert::AreEqual(direction_t(dir), direction_t(dir + 4));

         for (int dir = 0; dir < 4; ++dir)
         {
            Assert::AreNotEqual(direction_t(dir), direction_t(dir + 1));
            Assert::AreNotEqual(direction_t(dir), direction_t(dir + 2));
            Assert::AreNotEqual(direction_t(dir), direction_t(dir + 3));
         }
      }

      TEST_METHOD(rotate_direction)
      {
         for (int dir = 0; dir < 4; ++dir)
            for (int rot = 0; rot < 20; ++rot)
               Assert::AreEqual(direction_t(dir).rotate(rot), direction_t(dir + rot));
      }

      TEST_METHOD(direction_as_int)
      {
         for (int dir = 0; dir < 4; ++dir)
            Assert::AreEqual(direction_t(dir).as_int(), dir);
      }

      TEST_METHOD(direction_deltas)
      {
         Assert::AreEqual(direction_t(0).delta_x(), -1);
         Assert::AreEqual(direction_t(0).delta_y(),  0);

         Assert::AreEqual(direction_t(1).delta_x(),  0);
         Assert::AreEqual(direction_t(1).delta_y(), -1);

         Assert::AreEqual(direction_t(2).delta_x(),  1);
         Assert::AreEqual(direction_t(2).delta_y(),  0);

         Assert::AreEqual(direction_t(3).delta_x(),  0);
         Assert::AreEqual(direction_t(3).delta_y(),  1);

         for (int dir = 0; dir < 4; ++dir)
         {
            Assert::AreEqual(direction_t(dir).delta_x(), -direction_t(dir + 2).delta_x());
            Assert::AreEqual(direction_t(dir).delta_y(), -direction_t(dir + 2).delta_y());
         }
      }

   };
}
