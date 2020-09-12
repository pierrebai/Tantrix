#include "tantrix.h"
#include "tantrix_stream.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <mutex>

using namespace std;
using namespace dak::tantrix;

struct stream_progress : progress_t
{
   void update_progress(size_t a_total_count_so_far) override
   {
      my_mutex.lock();
      cout << a_total_count_so_far << " / " << estimated_total_count() << "\r";
      my_mutex.unlock();
   }

private:
   mutex my_mutex;
};

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

         puzzle_t puzzle;
         {
            ifstream puzzle_stream(filename);
            puzzle_stream >> puzzle;
         }

         cout << puzzle << endl;

         auto begin_time = clock::now();
         stream_progress progress;
         const auto solutions = solve(puzzle, progress);
         auto end_time = clock::now();

         cout << "\n";
         cout << "time: " << chrono::duration_cast<chrono::milliseconds>(end_time - begin_time).count() / 1000.0 << "s" << endl;
         cout << "solutions: " << solutions.size() << endl;

         path solution_filename(filename);
         solution_filename.replace_extension("solutions.txt");
         ofstream solution_stream(solution_filename);

         for (const auto& sol : solutions)
         {
            solution_stream << sol << "\n";
         }
      }
      catch (exception& ex)
      {
         cout << "Error: " << ex.what() << endl;
      }
   }
}

