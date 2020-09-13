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
      auto puzzle_layout = new QHBoxLayout(puzzle_container);

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

      my_solving_attempts_label = new QLabel();
      my_solving_attempts_label->hide();
      solutions_layout->addWidget(my_solving_attempts_label);

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
         update_puzzle();
      }
      catch (const std::exception&)
      {
         // TODO: exception handling when loading a puzzle.
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // UI updates from data.

   void main_window_t::update_puzzle()
   {
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
      my_stop_solving = true;

      update_solving_attempts();
      update_toolbar();
   }

   void main_window_t::update_solutions()
   {
      my_solutions_list->clear();

      size_t solution_index = 1;
      for (const auto& solution : my_solutions)
      {
         std::ostringstream stream;
         stream << "Solution #" << solution_index++ << ":\n";
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

   void main_window_t::update_solving_attempts()
   {
      if (my_solving_attempts)
      {
         ostringstream stream;
         stream << "Attempts: " << my_solving_attempts;
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
      my_solve_puzzle_action->setEnabled(my_puzzle.line_colors().size() > 0);
      my_stop_puzzle_action->setEnabled(my_async_solving.valid());
   }

   static QPolygonF create_hexagon(const double tile_radius)
   {
      QPolygonF hexagon;

      for (int i = 0; i < 6; ++i)
      {
         const double two_pi = 2 * 3.14159265;
         double angle = (two_pi / 12.) + (two_pi * i / 6.);
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

      // TODO: drawing a solution in the graphics view.
      const double tile_radius = 50;
      QPen tile_pen(QColor(0, 0, 0));
      tile_pen.setWidth(3);

      std::map<tantrix::color_t, QColor> colors =
      {
         { tantrix::color_t::red(),    QColor(240, 0, 0) },
         { tantrix::color_t::green(),  QColor(0, 240, 0) },
         { tantrix::color_t::blue(),   QColor(0, 0, 240) },
         { tantrix::color_t::yellow(), QColor(240, 240, 0) },
      };

      for (size_t i = 0; i < solution->tiles_count(); ++i)
      {
         const auto& placed_tile = solution->tiles()[i];
         QPolygonF polygon = create_hexagon(tile_radius);

         const QPointF tile_center = convert_tile_pos(placed_tile.pos, tile_radius);
         polygon.translate(tile_center);

         auto hex = new QGraphicsPolygonItem(polygon);
         hex->setPen(tile_pen);
         scene->addItem(hex);

         for (const auto& [tc, qc] : colors)
         {
            if (!placed_tile.tile.has_color(tc))
               continue;

            auto dir1 = placed_tile.tile.find_color(tc, 0);
            auto dir2 = placed_tile.tile.find_color(tc, dir1.rotate(1));

            QPainterPath path;

            path.moveTo(convert_tile_side(placed_tile.pos, dir1, tile_radius));
            path.cubicTo(tile_center, tile_center, convert_tile_side(placed_tile.pos, dir2, tile_radius));

            auto line = new QGraphicsPathItem(path);
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
