#include "tantrix.h"
#include "tantrix_stream.h"

#include <iostream>

using namespace std;
using namespace dak::tantrix;

int main()
{
   const auto solutions = solve_genius_puzzle();
   cout << solutions.size() << endl;
   if (solutions.size() < 100)
   {
      for (const auto& sol : solutions)
      {
         cout << "----------------------------" << endl;
         cout << sol << endl;
      }
   }
}

