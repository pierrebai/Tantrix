#include <main_window.h>
#include <draw_tantrix_puzzle.h>
#include <tantrix_puzzle.h>
#include <dak/tantrix/puzzle.h>

#include <resource.h>

#include <dak/QtAdditions/QtUtilities.h>

#include <dak/tantrix/stream.h>
#include <dak/tantrix/triangle_puzzle.h>

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
#include <QtWidgets/qinputdialog.h>

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
      : progress_t(10000), my_solving_stopwatch(my_solving_time_buffer)
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
      my_error_message = new QErrorMessage(this);

      build_toolbar_ui();
      build_puzzle_ui();
      build_solutions_ui();
      build_solution_canvas();

      setWindowIcon(QIcon(QtWin::fromHICON((HICON)::LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 256, 256, 0))));
   }

   void main_window_t::build_toolbar_ui()
   {
      auto toolbar = new QToolBar();
      toolbar->setObjectName("Main Toolbar");
      toolbar->setIconSize(QSize(32, 32));

      my_load_puzzle_action = CreateAction(tr("Load Puzzle"), IDB_OPEN_PUZZLE, QKeySequence(QKeySequence::StandardKey::Open));
      my_load_puzzle_button = CreateToolButton(my_load_puzzle_action);
      toolbar->addWidget(my_load_puzzle_button);

      my_save_puzzle_action = CreateAction(tr("Save Puzzle"), IDB_SAVE_PUZZLE, QKeySequence(QKeySequence::StandardKey::Save));
      my_save_puzzle_button = CreateToolButton(my_save_puzzle_action);
      toolbar->addWidget(my_save_puzzle_button);

      my_edit_puzzle_action = CreateAction(tr("Edit Puzzle"), IDB_EDIT_PUZZLE);
      my_edit_puzzle_button = CreateToolButton(my_edit_puzzle_action);
      toolbar->addWidget(my_edit_puzzle_button);

      my_load_solutions_action = CreateAction(tr("Load Solutions"), IDB_LOAD_SOLUTIONS, QKeySequence(QKeySequence::StandardKey::Open));
      my_load_solutions_button = CreateToolButton(my_load_solutions_action);
      toolbar->addWidget(my_load_solutions_button);

      my_save_solutions_action = CreateAction(tr("Save Solutions"), IDB_SAVE_SOLUTIONS, QKeySequence(QKeySequence::StandardKey::Save));
      my_save_solutions_button = CreateToolButton(my_save_solutions_action);
      toolbar->addWidget(my_save_solutions_button);

      my_solve_puzzle_action = CreateAction(tr("Solve Puzzle"), IDB_SOLVE_PUZZLE);
      my_solve_puzzle_button = CreateToolButton(my_solve_puzzle_action);
      toolbar->addWidget(my_solve_puzzle_button);

      my_stop_puzzle_action = CreateAction(tr("Stop Puzzle"), IDB_STOP_PUZZLE);
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
      my_puzzle_list->setSelectionMode(QListWidget::SelectionMode::SingleSelection);
      my_puzzle_list->setSelectionBehavior(QListWidget::SelectionBehavior::SelectRows);
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

      // Puzzle list.

      my_puzzle_list->connect(my_puzzle_list, &QListWidget::currentRowChanged, [self = this]()
      {
         self->draw_selected_puzzle_tile();
      });

      my_puzzle_list->connect(my_puzzle_list, &QListWidget::itemClicked, [self = this]()
      {
         self->draw_selected_puzzle_tile();
      });

      // Solution list.

      my_solutions_list->connect(my_solutions_list, &QListWidget::currentRowChanged, [self = this]()
      {
         self->draw_selected_solution();
      });

      my_solutions_list->connect(my_solutions_list, &QListWidget::itemClicked, [self = this]()
      {
         self->draw_selected_solution();
      });

      // Toolbar.

      my_load_puzzle_action->connect(my_load_puzzle_action, &QAction::triggered, [self = this]()
      {
         self->load_puzzle();
         self->update_toolbar();
      });

      my_save_puzzle_action->connect(my_save_puzzle_action, &QAction::triggered, [self = this]()
      {
         self->save_puzzle();
         self->update_toolbar();
      });

      my_edit_puzzle_action->connect(my_edit_puzzle_action, &QAction::triggered, [self = this]()
      {
         self->edit_puzzle();
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

         my_puzzle = load_tantrix_puzzle(path);
         update_puzzle();
         my_solutions.clear();
         update_solutions();
      }
      catch (const std::exception& ex)
      {
         showException("Could not load the puzzle:", ex);
      }
   }

   void main_window_t::save_puzzle()
   {
      static constexpr char puzzle_file_types[] = "Puzzle Text files (*.txt);;All files (*.*)";

      try
      {
         // We must stop on save because otherwise the threads are preventing the dialog from opening!
         stop_puzzle();

         filesystem::path path = AskSave(tr("Save Puzzle"), tr(puzzle_file_types), my_puzzle_filename.string().c_str(), this);
         if (path.empty())
            return;

         save_tantrix_puzzle(path, my_puzzle);
         update_puzzle();
      }
      catch (const std::exception& ex)
      {
         showException("Could not load the puzzle:", ex);
      }
   }

   void main_window_t::edit_puzzle()
   {
      try
      {
         // We must stop on save because otherwise the threads are preventing the dialog from opening!
         stop_puzzle();

         std::string puzzle_text;
         {
            std::ostringstream puzzle_stream;
            save_tantrix_puzzle(puzzle_stream, my_puzzle);
            puzzle_text = puzzle_stream.str();
         }

         auto new_text = QInputDialog::getMultiLineText(this, "Edit Puzzle", "Modify the puzzle description", puzzle_text.c_str());

         if (!new_text.isEmpty())
         {
            puzzle_text = new_text.toStdString();
            std::istringstream puzzle_stream(puzzle_text);
            my_puzzle = load_tantrix_puzzle(puzzle_stream);
         }

         update_puzzle();
      }
      catch (const std::exception& ex)
      {
         showException("Could not edit the puzzle:", ex);
      }
   }

   void main_window_t::showException(const char* message, const std::exception& ex)
   {
      if (!my_error_message)
         return;

      std::ostringstream stream;
      stream << message << "\n" << ex.what();
      my_error_message->showMessage(stream.str().c_str());
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

         save_tantrix_solutions(path, my_solutions);
      }
      catch (const std::exception& ex)
      {
         showException("Could not save the solutions:", ex);
      }
   }

   void main_window_t::load_solutions()
   {
      static constexpr char solutions_file_types[] = "Solutions Text files (*.solutions.txt);;All files (*.*)";

      try
      {
         // We must stop on load because otherwise the threads are preventing the dialog from opening!
         stop_puzzle();

         filesystem::path path = AskOpen(tr("Load Solutions"), tr(solutions_file_types), this);
         if (path.empty())
            return;

         my_solutions = load_tantrix_solutions(path);
         update_solutions();
      }
      catch (const std::exception& ex)
      {
         showException("Could not load the solutions:", ex);
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // UI updates from data.

   void main_window_t::update_puzzle()
   {
      if (my_puzzle_filename.filename().string().size() > 0)
      {
         my_puzzle_label->setText(my_puzzle_filename.filename().string().c_str());
         my_puzzle_label->show();
      }
      else
      {
         my_puzzle_label->hide();
      }

      if (!my_puzzle)
         return;

      my_puzzle_list->clear();

      for (const std::string& item : get_tantrix_puzzle_description(my_puzzle))
         my_puzzle_list->addItem(item.c_str());

      my_solving_attempts = 0;
      my_solving_stopwatch.elapsed();
      stop_progress(true);

      update_solving_attempts();
      update_solving_time();
      update_toolbar();
   }

   void main_window_t::update_solutions()
   {
      if (!my_solutions_list)
         return;

      const int old_current_row = my_solutions_list->currentRow();
      my_solutions_list->clear();

      for (const std::string& item : get_tantrix_solutions_description(my_solutions))
         my_solutions_list->addItem(item.c_str());

      if (old_current_row >= 0 && old_current_row < my_solutions_list->count())
         my_solutions_list->setCurrentRow(old_current_row);
      else
         my_solutions_list->setCurrentRow(0);

      update_toolbar();
   }

   void main_window_t::update_solving_time()
   {
      if (my_solving_attempts)
      {
         my_solving_stopwatch.elapsed();

         my_solving_time_label->setText((std::string("Time: ") + my_solving_time_buffer).c_str());

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
      auto puzzle = std::dynamic_pointer_cast<tantrix::puzzle_t>(my_puzzle);

      my_load_puzzle_action->setEnabled(true);
      my_save_puzzle_action->setEnabled(my_puzzle != nullptr);
      my_edit_puzzle_action->setEnabled(my_puzzle != nullptr);

      my_save_solutions_action->setEnabled(my_solutions.size() > 0);
      my_load_solutions_action->setEnabled(true);

      my_solve_puzzle_action->setEnabled(puzzle && puzzle->line_colors().size() > 0);
      my_stop_puzzle_action->setEnabled(my_async_solving.valid());
   }

   std::shared_ptr<tantrix::solution_t> main_window_t::get_selected_solution() const
   {
      const int index = my_solutions_list->currentRow();

      if (index < 0 || index >= my_solutions.size())
         return {};

      auto solution_it = my_solutions.begin();
      std::advance(solution_it, index);

      return std::dynamic_pointer_cast<tantrix::solution_t>(solution_it->first);
   }

   std::optional<tantrix::tile_t> main_window_t::get_selected_tile() const
   {
      auto puzzle = std::dynamic_pointer_cast<tantrix::puzzle_t>(my_puzzle);
      if (!puzzle)
         return {};

      auto selection_model = my_puzzle_list->selectionModel();
      if (!selection_model)
         return {};

      if (selection_model->selectedRows().size() <= 0)
         return {};

      const int index = selection_model->selectedRows().at(0).row();

      if (index < 0 || index >= puzzle->initial_tiles().size())
         return {};

      return std::optional<tantrix::tile_t>(puzzle->initial_tiles()[index]);
   }

   void main_window_t::draw_selected_puzzle_tile()
   {
      auto solution = get_selected_solution();
      if (solution)
         return draw_selected_solution();

      auto puzzle = std::dynamic_pointer_cast<tantrix::puzzle_t>(my_puzzle);
      draw_tantrix_puzzle_tiles(my_solution_canvas, puzzle, get_selected_tile());
   }

   void main_window_t::draw_selected_solution()
   {
      auto solution = get_selected_solution();
      if (!solution)
         return;

      auto puzzle = std::dynamic_pointer_cast<tantrix::puzzle_t>(my_puzzle);
      draw_tantrix_solution(my_solution_canvas, puzzle, solution, get_selected_tile());
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Asynchornous puzzle solving update.

   void main_window_t::update_progress(size_t a_total_count_so_far)
   {
      my_solving_attempts = a_total_count_so_far;
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Asynchronous puzzle solving.

   void main_window_t::solve_puzzle()
   {
      if (!my_puzzle)
         return;

      stop_progress(false);
      my_solving_attempts = 0;
      my_solving_stopwatch.start();
      my_async_solving = std::async(std::launch::async, [self = this, puzzle = my_puzzle]()
      {
         try
         {
            auto initial_solution = std::make_shared<tantrix::solution_t>();
            return solver::solve(puzzle, initial_solution, *self);
         }
         catch (const std::exception&)
         {
            return solver::all_solutions_t();
         }
      });
      my_solve_puzzle_timer->start(500);
   }

   bool main_window_t::is_async_puzzle_solving_done()
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

      if (!is_async_puzzle_solving_done())
      {
         my_solve_puzzle_timer->start(500);
         return;
      }

      try
      {
         my_solutions = my_async_solving.get();
      }
      catch (const std::exception& ex)
      {
         showException("Could not solve the puzzle:", ex);
      }
      update_solutions();
   }

   void main_window_t::stop_puzzle()
   {
      stop_progress(true);
   }
}
