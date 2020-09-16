#include "dak/tantrix/tantrix.h"
#include "dak/tantrix/stream.h"
#include "dak/utility/stream_progress.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <mutex>

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

         auto begin_time = clock::now();
         stream_progress_t progress(cout);
         const auto solutions = solve(*puzzle, progress);
         auto end_time = clock::now();

         cout << "\n";
         cout << "time: " << chrono::duration_cast<chrono::milliseconds>(end_time - begin_time).count() / 1000.0 << "s" << endl;
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

