#pragma once

#include <QtWidgets/qgraphicsscene.h>
#include <dak/tantrix/puzzle.h>
#include <dak/tantrix/solution.h>

#include <optional>

class QGraphicsView;
class QGraphicsScene;

namespace dak::tantrix_solver_app
{
   /////////////////////////////////////////////////////////////////////////
   //
   // Draw a Tantrix puzzle in the given view.

   void draw_tantrix_puzzle_tiles(
      QGraphicsView* a_view,
      const std::shared_ptr<tantrix::puzzle_t>& a_puzzle,
      const std::optional<tantrix::tile_t>& a_selected_tile);

   void draw_tantrix_solution(
      QGraphicsView* a_view,
      const std::shared_ptr<tantrix::puzzle_t>& a_puzzle,
      const std::shared_ptr<tantrix::solution_t>& a_solution,
      const std::optional<tantrix::tile_t>& a_selected_tile);
}
