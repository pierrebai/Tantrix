#include "dak/tantrix/puzzle.h"
#include "dak/tantrix/solve.h"
#include "dak/utility/progress.h"
#include "dak/utility/multi_thread_progress.h"
#include "dak/utility/threaded_work.h"


namespace dak::tantrix
{
   using multi_thread_progress_t = dak::utility::multi_thread_progress_t;
   using per_thread_progress_t = dak::utility::per_thread_progress_t;
   using namespace dak::utility;
   using puzzle_threaded_work_t = threaded_work_t<sub_puzzle_t, all_solutions_t>;


   ////////////////////////////////////////////////////////////////////////////
   //
   // Keeper of all solutions.

   // Normalize and add a solution if it is not already known.
   static void add_solution(all_solutions_t& all_solutions, solution_t&& a_solution)
   {
      a_solution.normalize();
      all_solutions[a_solution] += 1;
   }

   // Add many solutions, we assume they have been already normalized.
   static void add_solutions(all_solutions_t& all_solutions, all_solutions_t&& other_solutions)
   {
      for (auto& [solution, count] : other_solutions)
         all_solutions[solution] += count;
   }


   ////////////////////////////////////////////////////////////////////////////
   //
   // Solve the placement of all given tiles.

   struct solve_context_t
   {
      solve_context_t(
         puzzle_threaded_work_t& a_threaded_work,
         const puzzle_t& a_puzzle,
         multi_thread_progress_t& a_progress,
         size_t a_depth)
         : threaded_work(a_threaded_work)
         , puzzle(a_puzzle)
         , progress(a_progress)
         , recursion_depth(a_depth)
      {
      }

      solve_context_t(const solve_context_t& a_ctx)
         : threaded_work(a_ctx.threaded_work)
         , puzzle(a_ctx.puzzle)
         , progress(a_ctx.progress)
         , recursion_depth(a_ctx.recursion_depth)
      {
         // Note: do *not* copy solutions!
      }

      puzzle_threaded_work_t&    threaded_work;
      const puzzle_t&            puzzle;
      per_thread_progress_t      progress;
      size_t                     recursion_depth;
      all_solutions_t            solutions;
   };

   static void solve_partial(const sub_puzzle_t& a_sub_puzzle, const solution_t& partial_solution, solve_context_t& a_ctx);

   static void solve_recursion(
      const sub_puzzle_t& a_sub_puzzle,
      const solution_t& partial_solution,
      const tile_t& new_tile, position_t new_pos,
      solve_context_t& a_ctx)
   {
      solution_t new_partial = partial_solution;
      new_partial.add_tile(new_tile, new_pos);

      if (a_ctx.puzzle.has_more_sub_puzzles(a_sub_puzzle))
      {
         solve_partial(a_sub_puzzle, new_partial, a_ctx);
      }
      else if (a_ctx.puzzle.is_solution_valid(new_partial))
      {
         add_solution(a_ctx.solutions, std::move(new_partial));
      }
   }

   static void solve_sub_puzzle_with_tile(const sub_puzzle_t& a_sub_puzzle, const solution_t& partial_solution, solve_context_t& a_ctx)
   {
      a_ctx.progress.progress(1);

      const auto next_positions = a_ctx.puzzle.get_sub_puzzle_positions(a_sub_puzzle, partial_solution);
      for (const auto new_pos : next_positions)
      {
         for (int selected_orientation = 0; selected_orientation < 6; ++selected_orientation)
         {
            const tile_t tile = a_sub_puzzle.tile_to_place.rotate(selected_orientation);
            if (partial_solution.is_compatible(tile, new_pos))
            {
               solve_recursion(a_sub_puzzle, partial_solution, tile, new_pos, a_ctx);
            }
         }
      }
   }


   static void solve_partial(const sub_puzzle_t& a_sub_puzzle, const solution_t& partial_solution, solve_context_t& a_ctx)
   {
      std::vector<std::future<all_solutions_t>> solutions_futures;

      const auto sub_sub_puzzles = a_ctx.puzzle.create_sub_puzzles(a_sub_puzzle, partial_solution);
      for (const sub_puzzle_t& sub_sub_puzzle : sub_sub_puzzles)
      {
         #ifndef _DEBUG
         // Avoid queuing sub-sub-puzzle that are too simple or if there are already enough
         // parallel tasks, to avoid overflowing the work queue.
         if (partial_solution.tiles_count() < 3)
         {
            // Note: a_progress is passed by value volontarily so that a new one will be created for the sub-thread.
            auto new_future = a_ctx.threaded_work.add_work(sub_sub_puzzle, a_ctx.recursion_depth, [&a_ctx, partial_solution](sub_puzzle_t a_sub_sub_puzzle, size_t a_depth) -> all_solutions_t
            {
               solve_context_t ctx(a_ctx);
               try
               {
                  ctx.recursion_depth = a_depth;
                  solve_sub_puzzle_with_tile(a_sub_sub_puzzle, partial_solution, ctx);
               }
               catch (...)
               {
                  ctx.threaded_work.stop();
               }
               return ctx.solutions;
            });
            solutions_futures.emplace_back(std::move(new_future));
         }
         else
         #endif
         {
            solve_sub_puzzle_with_tile(sub_sub_puzzle, partial_solution, a_ctx);
         }
      }

      for (auto& new_future : solutions_futures)
      {
         auto partial_solutions = a_ctx.threaded_work.wait_for(new_future, a_ctx.recursion_depth);
         add_solutions(a_ctx.solutions, std::move(partial_solutions));
      }
   }

   all_solutions_t solve(const puzzle_t& a_puzzle, progress_t& a_progress)
   {
      all_solutions_t all_solutions;

      // Protect the normal non-thread-safe progress against multi-threading.
      multi_thread_progress_t mt_progress(a_progress);

      puzzle_threaded_work_t threaded_work(3, 1);

      // The first tile can be chosen arbitrarily and placed.
      // This will force the orientation of the solution, so
      // we won't have to compare with rotations or translations.

      #ifdef _DEBUG_WAWA

         for (const auto& sub_puzzle : a_puzzle.create_initial_sub_puzzles())
         {
            all_solutions_t solutions;
            solution_t initial_partial_solution(sub_puzzle.tile_to_place, position_t(0, 0));
            solve_partial(threaded_work, a_puzzle, sub_puzzle, solutions, initial_partial_solution, per_thread_progress_t(mt_progress));
            add_solutions(all_solutions, std::move(solutions));
         }

      #else

         std::vector<std::future<all_solutions_t>> solutions_futures;

         for (const auto& sub_puzzle : a_puzzle.create_initial_sub_puzzles())
         {
            auto new_future = threaded_work.add_work(sub_puzzle, 0, [&threaded_work , &a_puzzle, &mt_progress](sub_puzzle_t sub_puzzle, size_t a_depth) -> all_solutions_t
            {
               solve_context_t ctx(threaded_work, a_puzzle, mt_progress, a_depth);
               try
               {
                  solution_t initial_partial_solution(sub_puzzle.tile_to_place, position_t(0, 0));
                  solve_partial(sub_puzzle, initial_partial_solution, ctx);
               }
               catch (...)
               {
                  ctx.threaded_work.stop();
               }
               return ctx.solutions;
            });
            solutions_futures.emplace_back(std::move(new_future));
         }

         for (auto& new_future : solutions_futures)
         {
            auto partial_solutions = threaded_work.wait_for(new_future, 0);
            add_solutions(all_solutions, std::move(partial_solutions));
         }

      #endif

      return all_solutions;
   }

}
