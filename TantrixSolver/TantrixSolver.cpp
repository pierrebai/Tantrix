#include "tantrix.h"
#include "tantrix_stream.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace dak::tantrix;

int main(int arg_count, char** arg_values)
{
   for (int arg_index = 1; arg_index < arg_count; ++arg_index)
   {
      try
      {
         ifstream puzzle_stream(arg_values[arg_index]);
         puzzle_t puzzle;
         puzzle_stream >> puzzle;
         using clock = chrono::steady_clock;

         cout << "solving puzzle:" << endl << puzzle << endl;

         auto begin_time = clock::now();

         const auto solutions = solve(puzzle);

         auto end_time = clock::now();
         cout << "time: " << chrono::duration_cast<chrono::milliseconds>(end_time - begin_time).count() / 1000.0 << "s" << endl;

         size_t to_print_count = solutions.size();

         if (to_print_count > 200)
         {
            to_print_count = 200;
            cout << to_print_count << " solutions out of " << solutions.size() << ":" << endl;
         }
         else
         {
            cout << to_print_count << " solutions:" << endl;
         }

         for (const auto& sol : solutions)
         {
            cout << "----------------------------" << endl;
            cout << sol << endl;

            if (--to_print_count == 0)
               break;
         }
      }
      catch (exception& ex)
      {
         cout << "Error: " << ex.what() << endl;
      }
   }
}

