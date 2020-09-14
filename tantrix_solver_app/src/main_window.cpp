#include <main_window.h>

#include <resource.h>

#include <dak/QtAdditions/QtUtilities.h>

#include <dak/tantrix/stream.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qerrormessage.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtoolbutton.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicsitem.h>

#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

#include <QtWinExtras/qwinfunctions.h>

#include <QtCore/qstandardpaths.h>
#include <QtCore/qtimer.h>

#include <fstream>
#include <sstream>
#include <iomanip>

namespace dak::tantrix_solver_app
{
   using namespace QtAdditions;
   using namespace std;

   /////////////////////////////////////////////////////////////////////////
   //
   // Create the main window.

   main_window_t::main_window_t()
      : progress_t(10000)
   {
      build_ui();
      fill_ui();
      connect_ui();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Create the UI elements.

   void main_window_t::build_ui()
   {
      setCorner(Qt::Corner::TopLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
      setCorner(Qt::Corner::BottomLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
      setCorner(Qt::Corner::TopRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);
      setCorner(Qt::Corner::BottomRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);

      my_solve_puzzle_timer = new QTimer(this);
      my_solve_puzzle_timer->setSingleShot(true);

      build_toolbar_ui();
      build_puzzle_ui();
      build_solutions_ui();
      build_solution_canvas();
   }

   void main_window_t::build_toolbar_ui()
   {
      auto toolbar = new QToolBar();
      toolbar->setObjectName("Main Toolbar");
      toolbar->setIconSize(QSize(32, 32));

      my_load_puzzle_action = CreateAction(tr("Load Puzzle"), IDB_OPEN_PUZZLE, QKeySequence(QKeySequence::StandardKey::Open));
      my_load_puzzle_button = CreateToolButton(my_load_puzzle_action);
      toolbar->addWidget(my_load_puzzle_button);

      my_load_solutions_action = CreateAction(tr("Load Solutions"), IDB_LOAD_SOLUTIONS, QKeySequence(QKeySequence::StandardKey::Open));
      my_load_solutions_button = CreateToolButton(my_load_solutions_action);
      toolbar->addWidget(my_load_solutions_button);

      my_save_solutions_action = CreateAction(tr("Save Solutions"), IDB_SAVE_SOLUTIONS, QKeySequence(QKeySequence::StandardKey::Open));
      my_save_solutions_button = CreateToolButton(my_save_solutions_action);
      toolbar->addWidget(my_save_solutions_button);

      my_solve_puzzle_action = CreateAction(tr("Solve Puzzle"), IDB_SOLVE_PUZZLE, QKeySequence(QKeySequence::StandardKey::Save));
      my_solve_puzzle_button = CreateToolButton(my_solve_puzzle_action);
      toolbar->addWidget(my_solve_puzzle_button);

      my_stop_puzzle_action = CreateAction(tr("Stop Puzzle"), IDB_STOP_PUZZLE, QKeySequence(QKeySequence::StandardKey::Save));
      my_stop_puzzle_button = CreateToolButton(my_stop_puzzle_action);
      toolbar->addWidget(my_stop_puzzle_button);

      addToolBar(toolbar);
   }

   void main_window_t::build_puzzle_ui()
   {
      auto puzzle_dock = new QDockWidget(tr("Puzzle"));
      puzzle_dock->setObjectName("Puzzle");
      puzzle_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
      auto puzzle_container = new QWidget();
      auto puzzle_layout = new QVBoxLayout(puzzle_container);

      my_puzzle_label = new QLabel;
      my_puzzle_label->hide();
      puzzle_layout->addWidget(my_puzzle_label);

      my_puzzle_list = new QListWidget();
      my_puzzle_list->setMinimumWidth(200);
      puzzle_layout->addWidget(my_puzzle_list);

      puzzle_dock->setWidget(puzzle_container);

      addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, puzzle_dock);
   }

   void main_window_t::build_solutions_ui()
   {
      auto solutions_dock = new QDockWidget(tr("Solutions"));
      solutions_dock->setObjectName("Solutions");
      solutions_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
      auto solutions_container = new QWidget();
      auto solutions_layout = new QVBoxLayout(solutions_container);

      auto info_container = new QWidget();
      auto info_layout = new QHBoxLayout(info_container);
      info_layout->setMargin(0);

      my_solving_time_label = new QLabel;
      my_solving_time_label->hide();
      info_layout->addWidget(my_solving_time_label);

      my_solving_attempts_label = new QLabel;
      my_solving_attempts_label->hide();
      info_layout->addWidget(my_solving_attempts_label);

      solutions_layout->addWidget(info_container);

      my_solutions_list = new QListWidget();
      my_solutions_list->setMinimumWidth(200);
      my_solutions_list->setSelectionMode(QListWidget::SelectionMode::SingleSelection);
      my_solutions_list->setSelectionBehavior(QListWidget::SelectionBehavior::SelectRows);
      solutions_layout->addWidget(my_solutions_list);

      solutions_dock->setWidget(solutions_container);

      addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, solutions_dock);
   }

   void main_window_t::build_solution_canvas()
   {
      auto main_container = new QWidget;
      auto main_layout = new QVBoxLayout(main_container);

      my_solution_canvas = new QGraphicsView;
      my_solution_canvas->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

      main_layout->addWidget(my_solution_canvas);

      setCentralWidget(main_container);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Connect the signals of the UI elements.

   void main_window_t::connect_ui()
   {
      // Asynchronous solving.

      my_solve_puzzle_timer->connect(my_solve_puzzle_timer, &QTimer::timeout, [self = this]()
      {
         self->verify_async_puzzle_solving();
      });

      // Solution list.

      my_solutions_list->connect(my_solutions_list, &QListWidget::currentRowChanged, [self = this]()
      {
         self->draw_selected_solution();
      });

      // Toolbar.

      my_load_puzzle_action->connect(my_load_puzzle_action, &QAction::triggered, [self = this]()
      {
         self->load_puzzle();
         self->update_toolbar();
      });

      my_save_solutions_action->connect(my_save_solutions_action, &QAction::triggered, [self = this]()
      {
         self->save_solutions();
         self->update_toolbar();
      });

      my_load_solutions_action->connect(my_load_solutions_action, &QAction::triggered, [self = this]()
      {
         self->load_solutions();
         self->update_toolbar();
      });

      my_solve_puzzle_action->connect(my_solve_puzzle_action, &QAction::triggered, [self = this]()
      {
         self->solve_puzzle();
         self->update_toolbar();
      });

      my_stop_puzzle_action->connect(my_stop_puzzle_action, &QAction::triggered, [self = this]()
      {
         self->stop_puzzle();
         self->update_toolbar();
      });

   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Fill the UI with the intial data.

   void main_window_t::fill_ui()
   {
      // Nothing to fill initially.
      update_toolbar();
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Puzzle.

   void main_window_t::load_puzzle()
   {
      static constexpr char puzzle_file_types[] = "Puzzle Text files (*.txt);;All files (*.*)";

      try
      {
         // We must stop on load because otherwise the threads are preventing the dialog from opening!
         stop_puzzle();

         filesystem::path path = AskOpen(tr("Load Puzzle"), tr(puzzle_file_types), this);
         if (path.empty())
            return;

         std::ifstream puzzle_stream(path);
         puzzle_stream >> my_puzzle;
         update_puzzle(path.filename().string().c_str());
      }
      catch (const std::exception&)
      {
         // TODO: exception handling when loading a puzzle.
      }
   }

   void main_window_t::save_solutions()
   {
      static constexpr char solutions_file_types[] = "Solutions Text files (*.solutions.txt);;All files (*.*)";

      try
      {
         // We must stop on load because otherwise the threads are preventing the dialog from opening!
         stop_puzzle();

         filesystem::path path = AskSave(tr("Save Solutions"), tr(solutions_file_types), "", this);
         if (path.empty())
            return;

         std::ofstream solutions_stream(path);
         solutions_stream << my_solutions;
      }
      catch (const std::exception&)
      {
         // TODO: exception handling when loading a solutions.
      }
   }

   void main_window_t::load_solutions()
   {
      static constexpr char solutions_file_types[] = "Solutions Text files (*.solutions.txt);;All files (*.*)";

      try
      {
         // We must stop on load because otherwise the threads are preventing the dialog from opening!
         stop_puzzle();

         filesystem::path path = AskOpen(tr("Save Solutions"), tr(solutions_file_types), this);
         if (path.empty())
            return;

         std::ifstream solutions_stream(path);
         solutions_stream >> my_solutions;
         update_solutions();
      }
      catch (const std::exception&)
      {
         // TODO: exception handling when loading a solutions.
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // UI updates from data.

   void main_window_t::update_puzzle(const char* filename)
   {
      if (filename && filename[0])
      {
         my_puzzle_label->setText(filename);
         my_puzzle_label->show();
      }
      else
      {
         my_puzzle_label->hide();
      }

      my_puzzle_list->clear();

      for (const auto& [color, tiles] : my_puzzle.tiles())
      {
         for (const auto& tile : tiles)
         {
            std::ostringstream stream;
            stream << "Tile #" << tile;
            my_puzzle_list->addItem(stream.str().c_str());
         }
      }

      static map<tantrix::color_t, const char*> color_names =
      {
         { tantrix::color_t::red(),    "Red"    },
         { tantrix::color_t::green(),  "Green"  },
         { tantrix::color_t::blue(),   "Blue"   },
         { tantrix::color_t::yellow(), "Yellow" },
      };

      const char* const line_type = (my_puzzle.must_be_loops() ? "Loop" : "Line");

      for (const auto& color : my_puzzle.line_colors())
      {
         std::ostringstream stream;
         stream << color_names[color] << " " << line_type;
         my_puzzle_list->addItem(stream.str().c_str());
      }

      my_solving_attempts = 0;
      my_solving_begin_time = clock_t::time_point();
      my_stop_solving = true;

      update_solving_attempts();
      update_solving_time();
      update_toolbar();
   }

   void main_window_t::update_solutions()
   {
      my_solutions_list->clear();

      size_t solution_index = 0;
      //for (const auto& solution : my_solutions)
      for (const auto& [solution, count] : my_solutions)
      {
         std::ostringstream stream;

         if (solution_index == 1000)
         {
            stream << "And " << my_solutions.size() - solution_index << " other solutions";
            my_solutions_list->addItem(stream.str().c_str());
            break;
         }

         stream << "Solution #" << ++solution_index << " found "<< count << " times" << ":\n";
         for (size_t i = 0; i < solution.tiles_count(); ++i)
         {
            const auto& placed_tile = solution.tiles()[i];
            stream << "    "
                   << std::setw(3) << placed_tile.pos.x()
                   << " / "
                   << std::setw(3) << placed_tile.pos.y()
                   << " : tile #" << std::setw(2) << placed_tile.tile << "\n";
         }
         my_solutions_list->addItem(stream.str().c_str());
      }

      update_toolbar();
   }

   void main_window_t::update_solving_time()
   {
      if (my_solving_attempts)
      {
         my_solving_end_time = clock_t::now();
         auto tenth = std::chrono::duration_cast<chrono::milliseconds>(my_solving_end_time - my_solving_begin_time).count() / 100;
         auto seconds = tenth / 10;
         tenth %= 10;


         ostringstream stream;

         if (seconds < 2 * 60)
            stream << "Time: " << seconds << '.' << tenth << "s";
         else
            stream << "Time: " << (seconds / 60) << "min " << (seconds % 60) << "s";

         my_solving_time_label->setText(stream.str().c_str());

         if (!my_solving_time_label->isVisible())
            my_solving_time_label->show();
      }
      else
      {
         if (my_solving_time_label->isVisible())
            my_solving_time_label->hide();
      }
   }

   void main_window_t::update_solving_attempts()
   {
      if (my_solving_attempts)
      {
         ostringstream stream;
         if (my_solving_attempts < 2 * 1000)
            stream << "Attempts: " << my_solving_attempts;
         else if (my_solving_attempts < 2 * 1000 * 1000)
            stream << "Attempts: " << (my_solving_attempts / 1000) << " thousands";
         else
            stream << "Attempts: " << (my_solving_attempts / (1000 * 1000)) << " millions";

         my_solving_attempts_label->setText(stream.str().c_str());
         if (!my_solving_attempts_label->isVisible())
            my_solving_attempts_label->show();
      }
      else
      {
         if (my_solving_attempts_label->isVisible())
            my_solving_attempts_label->hide();
      }
   }

   void main_window_t::update_toolbar()
   {
      my_load_puzzle_action->setEnabled(true);
      my_save_solutions_action->setEnabled(my_solutions.size() > 0);
      my_load_solutions_action->setEnabled(true);
      my_solve_puzzle_action->setEnabled(my_puzzle.line_colors().size() > 0);
      my_stop_puzzle_action->setEnabled(my_async_solving.valid());
   }

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

   static QPainterPath convert_tile_line(const tantrix::placed_tile_t& a_placed_tile, const QPointF& a_tile_center, double a_tile_radius, const tantrix::color_t& a_color, const QColor& a_qt_color)
   {
      auto dir1 = a_placed_tile.tile.find_color(a_color, 0);
      auto dir2 = a_placed_tile.tile.find_color(a_color, dir1.rotate(1));

      auto pos1 = convert_tile_side(a_placed_tile.pos, dir1, a_tile_radius);
      auto pos2 = convert_tile_side(a_placed_tile.pos, dir2, a_tile_radius);

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

            auto pos3 = convert_tile_side(a_placed_tile.pos, dir1.rotate(3), a_tile_radius);
            auto pos4 = convert_tile_side(a_placed_tile.pos, dir2.rotate(3), a_tile_radius);
            auto pos_between_3_center = a_tile_center * 0.7 + pos3 * 0.3;
            auto pos_between_4_center = a_tile_center * 0.7 + pos4 * 0.3;
            auto pos_between_1_2 = pos3 * 0.5 + pos4 * 0.5;


            path.moveTo(pos1);
            path.lineTo(pos_between_1_center);
            path.cubicTo(a_tile_center, a_tile_center, pos_between_2_center);
            path.lineTo(pos2);

            break;
         }
      }

      return path;
   }

   void main_window_t::draw_selected_solution()
   {
      auto scene = new QGraphicsScene;

      const int index = my_solutions_list->currentRow();

      if (index < 0 || index >= my_solutions.size())
      {
         my_solution_canvas->setScene(scene);
         return;
      }

      auto solution = my_solutions.begin();
      std::advance(solution, index);

      const double tile_radius = 50;
      QPen tile_pen(QColor(50, 50, 50, 128));
      QBrush tile_brush(QColor(0, 0, 0));
      tile_pen.setWidth(3);

      std::map<tantrix::color_t, QColor> colors =
      {
         { tantrix::color_t::red(),    QColor(240, 0, 0) },
         { tantrix::color_t::green(),  QColor(0, 240, 0) },
         { tantrix::color_t::blue(),   QColor(0, 0, 240) },
         { tantrix::color_t::yellow(), QColor(240, 240, 0) },
      };

      for (size_t i = 0; i < solution->first.tiles_count(); ++i)
      {
         const auto& placed_tile = solution->first.tiles()[i];
         QPolygonF polygon = create_hexagon(tile_radius);

         const QPointF tile_center = convert_tile_pos(placed_tile.pos, tile_radius);
         polygon.translate(tile_center);

         auto hex = new QGraphicsPolygonItem(polygon);
         hex->setPen(tile_pen);
         hex->setBrush(tile_brush);
         scene->addItem(hex);

         for (const auto& [tc, qc] : colors)
         {
            if (!placed_tile.tile.has_color(tc))
               continue;

            auto line = new QGraphicsPathItem(convert_tile_line(placed_tile, tile_center, tile_radius, tc, qc));
            QPen line_pen(qc);
            line_pen.setWidth(tile_radius / 5);
            line->setPen(line_pen);

            scene->addItem(line);

         }
      }

      my_solution_canvas->setScene(scene);
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Asynchornous puzzle solving update.

   void main_window_t::update_progress(size_t a_total_count_so_far)
   {
      my_solving_attempts = a_total_count_so_far;
      if (my_stop_solving)
         throw std::exception("Stop solving the puzzle.");
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Asynchornous puzzle solving.

   void main_window_t::solve_puzzle()
   {
      my_stop_solving = false;
      my_solving_attempts = 0;
      my_solving_begin_time = clock_t::now();
      my_async_solving = std::async(std::launch::async, [self = this, puzzle = my_puzzle]()
      {
         try
         {
            return tantrix::solve(puzzle, *self);
         }
         catch (const std::exception&)
         {
            return all_solutions_t();
         }
      });
      my_solve_puzzle_timer->start(100);
   }

   bool main_window_t::is_async_filtering_ready()
   {
      if (!my_async_solving.valid())
         return false;

      if (my_async_solving.wait_for(1us) != future_status::ready)
         return false;

      return true;
   }

   void main_window_t::verify_async_puzzle_solving()
   {
      update_solving_attempts();
      update_solving_time();

      if (!is_async_filtering_ready())
      {
         my_solve_puzzle_timer->start(100);
         return;
      }

      try
      {
         my_solutions = my_async_solving.get();
      }
      catch (const std::exception&)
      {
         // TODO: show solving errors to the user.
      }
      update_solutions();
   }

   void main_window_t::stop_puzzle()
   {
      my_stop_solving = true;
   }
}
