#include "tantrix.h"

#include <iostream>

int main()
{
   const auto solutions = solve_genius_puzzle();
   std::cout << solutions.size();
}

