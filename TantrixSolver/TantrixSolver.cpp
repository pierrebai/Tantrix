#include "tantrix.h"

#include <iostream>

using namespace std;

std::ostream& operator <<(ostream& stream, const color_t& color)
{
   if (color == color_t::red())
      stream << "R";
   else if (color == color_t::green())
      stream << "G";
   else if (color == color_t::blue())
      stream << "B";
   else if (color == color_t::yellow())
      stream << "Y";
   return stream;
}

int main()
{
   const auto solutions = solve_genius_puzzle();
   cout << solutions.size() << endl;
   if (solutions.size() < 100)
   {
      for (const auto& sol : solutions)
      {
         cout << "----------------------------" << endl;
         for (const auto& [pos, tile] : sol.tiles())
         {
            cout << pos.x() << " / " << pos.y() << " : " << tile.number();
            for (const direction_t& dir : directions)
            {
               cout << " " << tile.color(dir);
            }
            cout << endl;
         }
      }
   }
}

