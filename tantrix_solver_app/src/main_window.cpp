#include <main_window.h>

#include <resource.h>

#include <dak/QtAdditions/QtUtilities.h>

#include <dak/tantrix/stream.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qerrormessage.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtoolbutton.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qgraphicsview.h>

#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

#include <QtWinExtras/qwinfunctions.h>

#include <QtCore/qstandardpaths.h>
#include <QtCore/qtimer.h>

#include <fstream>
#include <sstream>

namespace dak::tantrix_solver_app
{
   using namespace QtAdditions;
   using namespace std;

   /////////////////////////////////////////////////////////////////////////
   //
   // Create the main window.

   main_window_t::main_window_t()
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
      auto solutions_layout = new QHBoxLayout(solutions_container);

      my_solutions_list = new QListWidget();
      my_solutions_list->setMinimumWidth(200);
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

      // Toolbar.

      my_load_puzzle_action->connect(my_load_puzzle_action, &QAction::triggered, [self = this]()
      {
         self->load_puzzle();
      });

      my_solve_puzzle_action->connect(my_solve_puzzle_action, &QAction::triggered, [self = this]()
      {
         self->solve_puzzle();
      });

      my_stop_puzzle_action->connect(my_stop_puzzle_action, &QAction::triggered, [self = this]()
      {
         self->stop_puzzle();
      });

   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Fill the UI with the intial data.

   void main_window_t::fill_ui()
   {
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Puzzle.

   void main_window_t::load_puzzle()
   {
      static constexpr char puzzle_file_types[] = "Puzzle Text files (*.txt);;All files (*.*)";

      try
      {
         filesystem::path path = AskOpen(tr("Load Puzzle"), tr(puzzle_file_types), this);
         std::ifstream puzzle_stream(path);
         puzzle_stream >> my_puzzle;
         update_puzzle();
      }
      catch (const std::exception&)
      {
         // TODO: exception handling.
      }
   }

   void main_window_t::update_puzzle()
   {
      my_puzzle_list->clear();

      for (const auto& [color, tiles] : my_puzzle.tiles())
      {
         for (const auto& tile : tiles)
         {
            std::ostringstream stream;
            stream << tile;
            my_puzzle_list->addItem(stream.str().c_str());
         }
      }

      for (const auto& color : my_puzzle.line_colors())
      {
         std::ostringstream stream;
         stream << color;
         my_puzzle_list->addItem(stream.str().c_str());
      }
   }

   void main_window_t::solve_puzzle()
   {
      //my_async_solving = std::async(std::launch::async, [&this->my_puzzle]()
      //{
      //   tantrix::solve(my_puzzle, *this);
      //});
      my_solve_puzzle_timer->start(10);
   }

   void main_window_t::update_progress(size_t a_total_count_so_far)
   {

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
      if (!is_async_filtering_ready())
         my_solve_puzzle_timer->start(10);

      my_solutions = my_async_solving.get();
      update_solutions();
   }

   void main_window_t::update_solutions()
   {

   }

   void main_window_t::stop_puzzle()
   {

   }

   void main_window_t::draw_selected_solution()
   {
   }
}
