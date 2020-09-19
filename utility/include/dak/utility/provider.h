#pragma once

#ifndef DAK_UTILITY_PROVIDER_H
#define DAK_UTILITY_PROVIDER_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <vector>
#include <optional>
#include <functional>


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
      using item_t = ITEM;
      using selector_t = std::function<bool(const item_t&)>;

      // A selector that returns any item.
      static bool any(const item_t&) { return true; }

      provider_t() = default;

      // Add an item that can be taken later.
      void add(item_t&& an_item)
      {
         my_items.emplace_back(std::move(an_item));
      }

      // Take an item if available.
      std::optional<item_t> take(const selector_t& a_selector = any)
      {
         const auto end = my_items.end();
         for (auto iter = my_items.begin(); iter != end; ++iter)
         {
            if (a_selector(*iter))
            {
               auto item = std::optional<item_t>(std::move(*iter));
               my_items.erase(iter);
               return item;
            }
         }

         return std::optional<item_t>();
      }

   private:
      std::vector<item_t> my_items;
   };
}

#endif /* DAK_UTILITY_PROVIDER_H */
