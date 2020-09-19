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

   static void solve_partial(puzzle_threaded_work_t& threaded_work, const puzzle_t& a_puzzle, const sub_puzzle_t& a_sub_puzzle, all_solutions_t& solutions, const solution_t& partial_solution, per_thread_progress_t& a_progress);

   static void solve_recursion(
      puzzle_threaded_work_t& threaded_work,
      const puzzle_t& a_puzzle,
      const sub_puzzle_t& a_sub_puzzle,
      all_solutions_t& solutions,
      const solution_t& partial_solution,
      const tile_t& new_tile, position_t new_pos,
      per_thread_progress_t& a_progress)
   {
      solution_t new_partial = partial_solution;
      new_partial.add_tile(new_tile, new_pos);

      if (a_puzzle.has_more_sub_puzzles(a_sub_puzzle))
      {
         solve_partial(threaded_work, a_puzzle, a_sub_puzzle, solutions, new_partial, a_progress);
      }
      else if (a_puzzle.is_solution_valid(new_partial))
      {
         add_solution(solutions, std::move(new_partial));
      }
   }

   static void solve_sub_puzzle_with_tile(puzzle_threaded_work_t& threaded_work, const puzzle_t& a_puzzle, const sub_puzzle_t& a_sub_puzzle, all_solutions_t& solutions, const solution_t& partial_solution, per_thread_progress_t& a_progress)
   {
      a_progress.progress(1);

      const auto next_positions = a_puzzle.get_sub_puzzle_positions(a_sub_puzzle, partial_solution);
      for (const auto new_pos : next_positions)
      {
         for (int selected_orientation = 0; selected_orientation < 6; ++selected_orientation)
         {
            const tile_t tile = a_sub_puzzle.tile_to_place.rotate(selected_orientation);
            if (partial_solution.is_compatible(tile, new_pos))
            {
               solve_recursion(threaded_work, a_puzzle, a_sub_puzzle, solutions, partial_solution, tile, new_pos, a_progress);
            }
         }
      }
   }


   static void solve_partial(puzzle_threaded_work_t& threaded_work, const puzzle_t& a_puzzle, const sub_puzzle_t& a_sub_puzzle, all_solutions_t& solutions, const solution_t& partial_solution, per_thread_progress_t& a_progress)
   {
      std::vector<puzzle_threaded_work_t::token_t> solutions_tokens;

      const auto sub_sub_puzzles = a_puzzle.create_sub_puzzles(a_sub_puzzle, partial_solution);
      for (const sub_puzzle_t& sub_sub_puzzle : sub_sub_puzzles)
      {
         #ifndef _DEBUG
         if (sub_sub_puzzle.other_tiles.size() > 8)
         {
            // Note: a_progress is passed by value volontarily so that a new one will be created for the sub-thread.
            auto token = threaded_work.add_work(sub_sub_puzzle, [&threaded_work, &a_puzzle, partial_solution, a_progress](sub_puzzle_t a_sub_sub_puzzle) -> all_solutions_t
            {
               all_solutions_t solutions;
               solve_sub_puzzle_with_tile(threaded_work, a_puzzle, a_sub_sub_puzzle, solutions, partial_solution, const_cast<per_thread_progress_t&>(a_progress));
               return solutions;
            });
            solutions_tokens.emplace_back(token);
         }
         else
         #endif
         {
            solve_sub_puzzle_with_tile(threaded_work, a_puzzle, sub_sub_puzzle, solutions, partial_solution, a_progress);
         }
      }

      for (auto& token : solutions_tokens)
      {
         auto partial_solutions = threaded_work.wait_for(token);
         add_solutions(solutions, std::move(partial_solutions));
      }
   }

   all_solutions_t solve(const puzzle_t& a_puzzle, progress_t& a_progress)
   {
      all_solutions_t all_solutions;

      // Protect the normal non-thread-safe progress against multi-threading.
      multi_thread_progress_t mt_progress(a_progress);

      // The first tile can be chosen arbitrarily and placed.
      // This will force the orientation of the solution, so
      // we won't have to compare with rotations or translations.

      #ifdef _DEBUG

         for (const auto& sub_puzzle : a_puzzle.create_initial_sub_puzzles())
         {
            all_solutions_t solutions;
            solution_t initial_partial_solution(sub_puzzle.tile_to_place, position_t(0, 0));
            solve_partial(&a_puzzle, sub_puzzle, solutions, initial_partial_solution, per_thread_progress_t(mt_progress));
            add_solutions(all_solutions, std::move(solutions));
         }

      #else

         puzzle_threaded_work_t threaded_work;

         std::vector<puzzle_threaded_work_t::token_t> solutions_tokens;

         for (const auto& sub_puzzle : a_puzzle.create_initial_sub_puzzles())
         {
            auto token = threaded_work.add_work(sub_puzzle, [&threaded_work , &a_puzzle, &mt_progress](sub_puzzle_t sub_puzzle) -> all_solutions_t
            {
               all_solutions_t solutions;
               solution_t initial_partial_solution(sub_puzzle.tile_to_place, position_t(0, 0));
               solve_partial(threaded_work, a_puzzle, sub_puzzle, solutions, initial_partial_solution, per_thread_progress_t(mt_progress));
               return solutions;
            });
            solutions_tokens.emplace_back(token);
         }

         for (auto& token : solutions_tokens)
         {
            auto partial_solutions = threaded_work.wait_for(token);
            add_solutions(all_solutions, std::move(partial_solutions));
         }

      #endif

      return all_solutions;
   }

}
