#pragma once

#ifndef DAK_UTILITY_WORKER_H
#define DAK_UTILITY_WORKER_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <functional>


namespace dak::utility
{
   ////////////////////////////////////////////////////////////////////////////
   //
   // A worker interface to be added and taken from a worker provider.
   //
   // A worker executes an algorithm on a work item to produce a result.

   template <class WORK_ITEM, class RESULT>
   struct worker_t
   {
      using work_item_t = WORK_ITEM;
      using result_t = RESULT;

      virtual ~worker_t() = default;

      // Execute an algorithm on a work item to produce a result.
      virtual result_t execute(const work_item_t& an_item) = 0;
   };


   ////////////////////////////////////////////////////////////////////////////
   //
   // A concrete worker calling a lambda when executed.

   template <class WORK_ITEM, class RESULT>
   struct lambda_worker_t : worker_t<WORK_ITEM, RESULT>

   {
      using typename worker_t<WORK_ITEM, RESULT>::work_item_t;
      using typename worker_t<WORK_ITEM, RESULT>::result_t;

      lambda_worker_t() = default;
      lambda_worker_t(std::function<result_t(const work_item_t& an_item)> a_function) : my_function(a_function) {}

      // Execute an algorithm in a lambda on a work item to produce a result.
      result_t execute(const work_item_t& an_item) override
      {
         if (!my_function)
            return {};
         
         return my_function(an_item);
      }

   private:
      std::function<result_t(const work_item_t& an_item)> my_function;
   };
}

#endif /* DAK_UTILITY_WORKER_H */
