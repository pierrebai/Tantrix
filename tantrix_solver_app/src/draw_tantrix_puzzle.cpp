#include <draw_tantrix_puzzle.h>

#include <dak/tantrix/direction.h>
#include <dak/tantrix/color.h>

#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicsitem.h>

#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

namespace dak::tantrix_solver_app
{
   static constexpr double two_pi = 2 * 3.14159265;

   static double dir_to_angle(const tantrix::direction_t& dir)
   {
      double angle = two_pi * dir.as_int() / 6.;
      return angle;
   }

   static QPolygonF create_hexagon(const double tile_radius)
   {
      QPolygonF hexagon;

      for (const auto& dir : tantrix::directions)
      {
         double angle = two_pi / 12. + dir_to_angle(dir);
         QPointF point(std::cos(angle) * tile_radius, std::sin(angle) * tile_radius);
         hexagon << point;
      }

      return hexagon;
   }

   static QPointF convert_tile_pos(const tantrix::position_t& a_pos, const double a_tile_radius)
   {
      const double tile_spacing_x = (a_tile_radius * std::sqrt(0.75) + 2) * 2;
      const double tile_spacing_y = (a_tile_radius * 1.5 + 3);
      const int x = a_pos.x();
      const int y = a_pos.y();
      const double draw_x_offset = y * tile_spacing_x / 2;
      const double draw_x = x * tile_spacing_x + draw_x_offset;
      const double draw_y = y * tile_spacing_y;
      return QPointF(draw_x, draw_y);
   }

   static QPointF convert_tile_side(const tantrix::position_t& a_pos, const tantrix::direction_t& a_dir, const double a_tile_radius)
   {
      const QPointF center1 = convert_tile_pos(a_pos,             a_tile_radius);
      const QPointF center2 = convert_tile_pos(a_pos.move(a_dir), a_tile_radius);

      return QPointF(center1.x() * 0.6 + center2.x() * 0.4, center1.y() * 0.6 + center2.y() * 0.4);
   }

   static QPainterPath convert_tile_line(const tantrix::tile_t& a_tile, const tantrix::position_t& a_pos, const QPointF& a_tile_center, double a_tile_radius, const tantrix::color_t& a_color, const QColor& a_qt_color)
   {
      auto dir1 = a_tile.find_color(a_color, 0);
      auto dir2 = a_tile.find_color(a_color, dir1.rotate(1));

      auto pos1 = convert_tile_side(a_pos, dir1, a_tile_radius);
      auto pos2 = convert_tile_side(a_pos, dir2, a_tile_radius);

      QPainterPath path;

      const int dir_delta = std::abs(dir1.as_int() - dir2.as_int());
      switch (dir_delta)
      {
         case 1:
         case 5:
         {
            auto pos_between_1_center = a_tile_center * 0.02 + pos1 * 0.98;
            auto pos_between_2_center = a_tile_center * 0.02 + pos2 * 0.98;

            auto pos_between_1_2 = a_tile_center * 0.3 + pos_between_1_center * 0.35 + pos_between_2_center * 0.35;

            path.moveTo(pos1);
            path.lineTo(pos_between_1_center);
            path.cubicTo(pos_between_1_2, pos_between_1_2, pos_between_2_center);
            path.lineTo(pos2);

            break;
         }

         case 3:
         {
            path.moveTo(pos1);
            path.lineTo(pos2);

            break;
         }

         case 2:
         case 4:
         {
            auto pos_between_1_center = a_tile_center * 0.02 + pos1 * 0.98;
            auto pos_between_2_center = a_tile_center * 0.02 + pos2 * 0.98;

            auto pos_between_1_2 = a_tile_center * 0.6 + pos_between_1_center * 0.2 + pos_between_2_center * 0.2;


            path.moveTo(pos1);
            path.lineTo(pos_between_1_center);
            path.cubicTo(pos_between_1_2, pos_between_1_2, pos_between_2_center);
            path.lineTo(pos2);

            break;
         }
      }

      return path;
   }

   static std::map<tantrix::color_t, QColor> color_conversions =
   {
      { tantrix::color_t::red(),    QColor(220, 0, 0) },
      { tantrix::color_t::green(),  QColor(0, 220, 0) },
      { tantrix::color_t::blue(),   QColor(10, 10, 240) },
      { tantrix::color_t::yellow(), QColor(220, 220, 0) },
   };

   const double tile_radius = 50;

   static void draw_tile_in_scene(const tantrix::tile_t& a_tile, const tantrix::position_t& a_pos, QGraphicsScene& a_scene)
   {
      QPen tile_pen(QColor(50, 50, 50, 128));
      QBrush tile_brush(QColor(0, 0, 0));
      tile_pen.setWidth(3);

      QPolygonF polygon = create_hexagon(tile_radius);

      const QPointF tile_center = convert_tile_pos(a_pos, tile_radius);
      polygon.translate(tile_center);

      auto hex = new QGraphicsPolygonItem(polygon);
      hex->setPen(tile_pen);
      hex->setBrush(tile_brush);
      a_scene.addItem(hex);

      for (const auto& [tc, qc] : color_conversions)
      {
         if (!a_tile.has_color(tc))
            continue;

         {
            auto line = new QGraphicsPathItem(convert_tile_line(a_tile, a_pos, tile_center, tile_radius, tc, qc));
            QPen line_pen(qc.darker());
            line_pen.setWidth(tile_radius / 3.5);
            line->setPen(line_pen);
            a_scene.addItem(line);
         }

         {
            auto line = new QGraphicsPathItem(convert_tile_line(a_tile, a_pos, tile_center, tile_radius, tc, qc));
            QPen line_pen(qc);
            line_pen.setWidth(tile_radius / 5);
            line->setPen(line_pen);
            a_scene.addItem(line);
         }
      }
   }

   static void draw_tile_selection(bool is_selected, const tantrix::position_t& a_pos, QGraphicsScene& a_scene)
   {
      const QPointF tile_center = convert_tile_pos(a_pos, tile_radius);
      const double factor = 1.25;
      auto selection = new QGraphicsEllipseItem(tile_center.x() - tile_radius * factor, tile_center.y() - tile_radius * factor,
                                                                  tile_radius * 2 * factor,               tile_radius * 2 * factor);

      QPen selection_pen(    is_selected ? QColor(100, 100, 100,   0) : QColor(100, 100, 100, 0));
      QBrush selection_brush(is_selected ? QColor(120, 220, 240, 128) : QColor(100, 100, 100, 0));
      selection_pen.setWidth(1);
      selection->setPen(selection_pen);
      selection->setBrush(selection_brush);

      a_scene.addItem(selection);
   }

   void draw_tantrix_puzzle_tiles(
      QGraphicsView* a_view,
      const std::shared_ptr<tantrix::puzzle_t>& a_puzzle,
      const std::optional<tantrix::tile_t>& a_selected_tile)
   {
      if (!a_view)
         return;

      auto scene = new QGraphicsScene;

      if (!a_puzzle)
      {
         delete a_view->scene();
         a_view->setScene(scene);
         return;
      }

      const int tiles_per_row = std::sqrt(a_puzzle->initial_tiles_count());
      int tile_index = 0;

      for (const auto& tile : a_puzzle->initial_tiles())
      {
         const int tile_x = tile_index % tiles_per_row;
         const int tile_y = tile_index / tiles_per_row;
         const tantrix::position_t pos(tile_x * 2 - tile_y, tile_y * 2);

         bool is_selected = a_selected_tile.has_value() && a_selected_tile.value().is_same(tile);

         draw_tile_selection(is_selected, pos, *scene);
         draw_tile_in_scene(tile, pos, *scene);

         tile_index += 1;
      }

      delete a_view->scene();
      a_view->setScene(scene);
   }

   void draw_tantrix_solution(
      QGraphicsView* a_view,
      const std::shared_ptr<tantrix::puzzle_t>& a_puzzle,
      const std::shared_ptr<tantrix::solution_t>& a_solution,
      const std::optional<tantrix::tile_t>& a_selected_tile)
   {
      if (!a_view)
         return;

      if (!a_solution)
         return;

      std::optional<tantrix::tile_t> selected_tile;
      std::optional<tantrix::position_t> selected_pos;

      auto scene = new QGraphicsScene;

      for (size_t i = 0; i < a_solution->tiles_count(); ++i)
      {
         const auto& placed_tile = a_solution->tiles()[i];
         bool is_selected = a_selected_tile.has_value() && a_selected_tile.value().is_same(placed_tile.tile);
         if (is_selected)
         {
            selected_pos = placed_tile.pos;
            selected_tile = placed_tile.tile;
            continue;
         }

         draw_tile_selection(false, placed_tile.pos, *scene);
         draw_tile_in_scene(placed_tile.tile, placed_tile.pos, *scene);
      }

      if (selected_tile.has_value() && selected_pos.has_value())
      {
         draw_tile_selection(true, selected_pos.value(), *scene);
         draw_tile_in_scene(selected_tile.value(), selected_pos.value(), *scene);
      }

      delete a_view->scene();
      a_view->setScene(scene);
   }
}
