#include "dak/tantrix/tantrix.h"
#include "dak/tantrix/stream.h"
#include "dak/utility/stream_progress.h"
#include "dak/utility/stopwatch.h"

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace dak::tantrix;
using namespace dak::utility;

int main(int arg_count, char** arg_values)
{
   using clock = chrono::steady_clock;
   using path = filesystem::path;
    
   for (int arg_index = 1; arg_index < arg_count; ++arg_index)
   {
      try
      {
         const path filename(arg_values[arg_index]);
         cout << "solving puzzle: " << filename.filename() << endl;

         std::shared_ptr<puzzle_t> puzzle;
         {
            ifstream puzzle_stream(filename);
            puzzle_stream >> puzzle;
         }

         if (!puzzle)
         {
            cout << "Invalid puzzle file: " << filename.filename() << endl;
            continue;
         }

         cout << puzzle << endl;

         string elapsed_time;
         stopwatch_t stopwatch(elapsed_time);

         stream_progress_t progress(cout);
         const auto solutions = solve(*puzzle, progress);

         stopwatch.stop();

         cout << "\n";
         cout << "time: " << elapsed_time << endl;
         cout << "solutions: " << solutions.size() << endl;

         path solution_filename(filename);
         solution_filename.replace_extension("solutions.txt");

         ofstream solution_stream(solution_filename);
         solution_stream << solutions << endl;
      }
      catch (exception& ex)
      {
         cout << "Error: " << ex.what() << endl;
      }
   }
}

