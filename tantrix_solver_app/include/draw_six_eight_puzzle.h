#pragma once

#include <dak/six_eight/puzzle.h>
#include <dak/six_eight/solution.h>

#include <memory>
#include <optional>

class QGraphicsView;

namespace dak::tantrix_solver_app
{
   /////////////////////////////////////////////////////////////////////////
   //
   // Draw a six_eight puzzle in the given view.

   void draw_six_eight_puzzle_tiles(
      QGraphicsView* a_view,
      const std::shared_ptr<six_eight::puzzle_t>& a_puzzle,
      const std::optional<six_eight::tile_t>& a_selected_tile);

   void draw_six_eight_solution(
      QGraphicsView* a_view,
      const std::shared_ptr<six_eight::puzzle_t>& a_puzzle,
      const std::shared_ptr<six_eight::solution_t>& a_solution,
      const std::optional<six_eight::tile_t>& a_selected_tile);
}
