#include "dak/six_eight/tile.h"
#include "dak/six_eight_tests/helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::six_eight;

namespace dak::six_eight::tests
{
   TEST_CLASS(tile_tests)
   {
   public:
      
      TEST_METHOD(compare_tiles)
      {
         Assert::AreEqual(tile_t(), tile_t(0));

         for (tile_t::id_t id : tile_t::get_all_ids())
            Assert::AreEqual(tile_t(id), tile_t(id));

         for (tile_t::id_t id : tile_t::get_all_ids())
            for (tile_t::id_t other_id : tile_t::get_all_ids())
               if (other_id != id)
                  Assert::AreNotEqual(tile_t(id), tile_t(other_id));
      }

      TEST_METHOD(tile_id)
      {
         Assert::AreEqual<int>(tile_t().id(), 0);

         for (tile_t::id_t id : tile_t::get_all_ids())
            Assert::AreEqual<int>(tile_t(id).id(), id);
      }

      TEST_METHOD(is_same_tile)
      {
         for (tile_t::id_t id : tile_t::get_all_ids())
            for (int rot = 1; rot < 6; ++rot)
               Assert::AreEqual(true, tile_t(id).is_same(tile_t(id).rotate(rot)));

         for (tile_t::id_t id : tile_t::get_all_ids()) {
            for (tile_t::id_t other_id : tile_t::get_all_ids()) {
               if (other_id != id) {
                  const int possible_rotations = tile_t(id).get_description().possible_rotations;
                  for (int rot = 1; rot < possible_rotations; ++rot)
                     Assert::AreEqual(false, tile_t(id).is_same(tile_t(other_id).rotate(rot)));
               }
            }
         }
      }
   };
}
