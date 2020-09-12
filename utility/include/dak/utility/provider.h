#pragma once

#ifndef DAK_UTILITY_PROVIDER_H
#define DAK_UTILITY_PROVIDER_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <vector>
#include <optional>


namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // A provider of items.
   //
   // Item are added and later taken. Not thread-safe.

   template <class ITEM>
   struct provider_t
   {
      provider_t() = default;

      // Add an item that can be taken later.
      void add(ITEM item an_item)
      {
         my_items.emplace_back(an_item);
      }

      // Take an item if available.
      std::optional<ITEM> take()
      {
         if (my_items.size() <= 0)
            return std::optional<ITEM>();

         auto item = std::optional<ITEM>(my_items.back());
         my_item.pop_back();
         return item;
      }

   private:
      std::vector<ITEM> my_items;
   };
}

#endif /* DAK_UTILITY_PROVIDER_H */
