#include <dak/six_eight/stream.h>
#include <dak/six_eight/direction.h>
#include <dak/six_eight/tile.h>

#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicsitem.h>

#include <fstream>
#include <sstream>
#include <iomanip>

namespace dak::tantrix_solver_app
{
   static constexpr double two_pi = 2 * 3.14159265;

   static double dir_to_angle(const six_eight::direction_t& dir)
   {
      double angle = two_pi * dir.as_int() / 4.;
      return angle;
   }

   static QPolygonF create_square(const double tile_radius)
   {
      QPolygonF square;

      for (const auto& dir : six_eight::directions)
      {
         double angle = two_pi / 8. + dir_to_angle(dir);
         QPointF point(std::cos(angle) * tile_radius, std::sin(angle) * tile_radius);
         square << point;
      }

      return square;
   }

   static QPointF convert_tile_pos(const six_eight::position_t& a_pos, const double a_tile_radius)
   {
      const double tile_spacing_x = a_tile_radius * 1.5 + 3;
      const double tile_spacing_y = a_tile_radius * 1.5 + 3;
      const int x = a_pos.x();
      const int y = a_pos.y();
      const double draw_x = x * tile_spacing_x;
      const double draw_y = y * tile_spacing_y;
      return QPointF(draw_x, draw_y);
   }

   static QPointF convert_tile_side(const six_eight::position_t& a_pos, const six_eight::direction_t& a_dir, const double a_tile_radius)
   {
      const QPointF center1 = convert_tile_pos(a_pos,             a_tile_radius);
      const QPointF center2 = convert_tile_pos(a_pos.move(a_dir), a_tile_radius);

      return QPointF(center1.x() * 0.6 + center2.x() * 0.4, center1.y() * 0.6 + center2.y() * 0.4);
   }

   const double tile_radius = 20;

   static std::vector<QColor> color_conversions =
   {
      QColor(240, 40, 40),
      QColor(10, 180, 10),
      QColor(40, 40, 240),
      QColor(220, 220, 10),
      QColor(180, 180, 180),
      QColor(220, 10, 220),
      QColor(70, 70, 70),
      QColor(220, 150, 40),
      QColor(120, 220, 240, 128),
   };

   static void draw_tile_in_scene(const six_eight::tile_t& a_tile, const six_eight::position_t& a_pos, QGraphicsScene& a_scene, size_t a_color)
   {
      QPen tile_pen(QColor(50, 50, 50, 128));
      QBrush tile_brush(color_conversions[a_color]);
      tile_pen.setWidth(3);

      const QPointF tile_center = convert_tile_pos(a_pos, tile_radius);

      const auto& block_positions = a_tile.get_description().block_positions;
      for (const six_eight::position_t& block_pos : block_positions) {
         QPolygonF polygon = create_square(tile_radius);
         polygon.translate(tile_center);
         polygon.translate(convert_tile_pos(block_pos, tile_radius));
         auto sq = new QGraphicsPolygonItem(polygon);
         sq->setPen(tile_pen);
         sq->setBrush(tile_brush);
         a_scene.addItem(sq);
      }
   }

   static void draw_tile_selection(
      const six_eight::tile_t& a_tile,
      const six_eight::position_t& a_pos,
      QGraphicsScene& a_scene,
      bool is_selected)
   {
      const double factor = 1.25;
      QPen selection_pen(    is_selected ? QColor(100, 100, 100,   0) : QColor(100, 100, 100, 0));
      QBrush selection_brush(is_selected ? QColor(120, 220, 240, 128) : QColor(100, 100, 100, 0));
      selection_pen.setWidth(1);

      const auto& block_positions = a_tile.get_description().block_positions;
      for (const six_eight::position_t& block_pos : block_positions) {
         const QPointF tile_center = convert_tile_pos(a_pos.move(block_pos), tile_radius);
   
         auto selection = new QGraphicsEllipseItem(
            tile_center.x() - tile_radius * factor,
            tile_center.y() - tile_radius * factor,
            tile_radius * 2 * factor,
            tile_radius * 2 * factor);

         selection->setPen(selection_pen);
         selection->setBrush(selection_brush);

         a_scene.addItem(selection);
      }
   }

   void draw_six_eight_puzzle_tiles(
      QGraphicsView* a_view,
      const std::shared_ptr<six_eight::puzzle_t>& a_puzzle,
      const std::optional<six_eight::tile_t>& a_selected_tile)
   {
      auto scene = new QGraphicsScene;

      if (!a_puzzle) {
         delete a_view->scene();
         a_view->setScene(scene);
         return;
      }

      const int tiles_per_row = 1 + std::sqrt(a_puzzle->initial_tiles_count());
      int tile_index = 0;

      for (const auto& tile : a_puzzle->initial_tiles())
      {
         const int tile_x = tile_index % tiles_per_row;
         const int tile_y = tile_index / tiles_per_row;
         const six_eight::position_t pos(tile_x * 6, tile_y * 6);

         bool is_selected = a_selected_tile.has_value() && a_selected_tile.value().is_same(tile);

         draw_tile_selection(tile, pos, *scene, is_selected);
         draw_tile_in_scene(tile, pos, *scene, tile_index);

         tile_index += 1;
      }

      delete a_view->scene();
      a_view->setScene(scene);
   }

   void draw_six_eight_solution(
      QGraphicsView* a_view,
      const std::shared_ptr<six_eight::puzzle_t>& /*a_puzzle*/,
      const std::shared_ptr<six_eight::solution_t>& a_solution,
      const std::optional<six_eight::tile_t>& a_selected_tile)
   {
      if (!a_solution)
         return;

      std::optional<six_eight::tile_t> selected_tile;
      std::optional<six_eight::position_t> selected_pos;
      size_t selected_index = 0;

      auto scene = new QGraphicsScene;

      for (size_t i = 0; i < a_solution->tiles_count(); ++i)
      {
         const auto& placed_tile = a_solution->tiles()[i];
         bool is_selected = a_selected_tile.has_value() && a_selected_tile.value().is_same(placed_tile.tile);
         if (is_selected) {
            selected_pos = placed_tile.pos;
            selected_tile = placed_tile.tile;
            selected_index = i;
            continue;
         }

         draw_tile_selection(placed_tile.tile, placed_tile.pos, *scene, false);
         draw_tile_in_scene(placed_tile.tile, placed_tile.pos, *scene, i);
      }

      if (selected_tile.has_value() && selected_pos.has_value()) {
         draw_tile_selection(selected_tile.value(), selected_pos.value(), *scene, true);
         draw_tile_in_scene(selected_tile.value(), selected_pos.value(), *scene, selected_index);
      }

      delete a_view->scene();
      a_view->setScene(scene);
   }
}
