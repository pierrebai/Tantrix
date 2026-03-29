#include "dak/six_eight/six_eight.h"
#include "dak/six_eight/stream.h"
#include "dak/solver/solve.h"
#include "dak/utility/stream_progress.h"
#include "dak/utility/stopwatch.h"

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace dak::six_eight;
using namespace dak::utility;

int main(int arg_count, char** arg_values)
{
   using clock = chrono::steady_clock;
   using path = filesystem::path;

   if (arg_count < 2) {
         cout << "Provide a puzzle file name." << endl;
         return 1;
   }
    
   for (int arg_index = 1; arg_index < arg_count; ++arg_index)
   {
      try
      {
         const path filename(arg_values[arg_index]);
         cout << "Solving puzzle: " << filename.filename() << endl;

         ifstream puzzle_stream(filename);

         path solution_filename(filename);
         solution_filename.replace_extension("solutions.txt");
         ofstream solution_stream(solution_filename);

         while (puzzle_stream) {
            std::shared_ptr<puzzle_t> puzzle;
            puzzle_stream >> puzzle;
            
            auto initial_solution = std::make_shared<solution_t>();
   
            if (!puzzle)
               break;

            cout << "Puzzle: " << puzzle << endl;

            string elapsed_time;
            stopwatch_t stopwatch(elapsed_time);

            stream_progress_t progress(cout);
            const auto solutions = dak::solver::solve(puzzle, initial_solution, progress);

            stopwatch.stop();

            cout << "\n";
            cout << "time: " << elapsed_time << endl;
            cout << "solutions: " << solutions.size() << endl;

            for (const auto& solution : solutions) {
               auto puzzle_solution = std::dynamic_pointer_cast<solution_t>(solution);
               if (!puzzle_solution)
                  continue;
               cout << *puzzle_solution << endl;
               solution_stream << *puzzle_solution << endl;
            }
         }
      }
      catch (exception& ex)
      {
         cout << "Error: " << ex.what() << endl;
      }
   }
}

