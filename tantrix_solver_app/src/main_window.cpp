#include <main_window.h>
#include <six_eight_puzzle_api.h>
#include <tantrix_puzzle_api.h>

#include <resource.h>

#include <dak/QtAdditions/QtUtilities.h>

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
      my_puzzle_apis.push_back(std::make_shared<tantrix_puzzle_api_t>());
      my_puzzle_apis.push_back(std::make_shared<six_eight_puzzle_api_t>());

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
      build_avail_puzzles_ui();
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

   void main_window_t::build_avail_puzzles_ui()
   {
      auto avail_puzzles_dock = new QDockWidget(tr("Available Puzzles"));
      avail_puzzles_dock->setObjectName("avail_puzzles");
      avail_puzzles_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
      auto avail_puzzles_container = new QWidget();
      auto avail_puzzles_layout = new QVBoxLayout(avail_puzzles_container);

      my_avail_puzzles_list = new QListWidget();
      my_avail_puzzles_list->setMinimumWidth(200);
      my_avail_puzzles_list->setSelectionMode(QListWidget::SelectionMode::SingleSelection);
      my_avail_puzzles_list->setSelectionBehavior(QListWidget::SelectionBehavior::SelectRows);
      avail_puzzles_layout->addWidget(my_avail_puzzles_list);

      avail_puzzles_dock->setWidget(avail_puzzles_container);

      addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, avail_puzzles_dock);
   }

   void main_window_t::build_puzzle_ui()
   {
      auto puzzle_dock = new QDockWidget(tr("Puzzle Description"));
      puzzle_dock->setObjectName("Puzzle_Description");
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

      my_avail_puzzles_list->connect(my_avail_puzzles_list, &QListWidget::currentRowChanged, [self = this](int a_row)
      {
         self->load_puzzle_from_available_puzzle(a_row);
         self->update_toolbar();
      });

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
      fill_available_puzzles();
      update_toolbar();
   }

   void main_window_t::fill_available_puzzles()
   {
      std::filesystem::path puzzles_folder = std::filesystem::current_path();
      puzzles_folder.append("puzzles");
      if (!std::filesystem::exists(puzzles_folder))
         return;

      for (const auto& entry : std::filesystem::directory_iterator(puzzles_folder)) {
         if (entry.path().string().find(".solutions.txt") != std::string::npos)
            continue;
         
         for (const auto& puzzle_api : my_puzzle_apis) {
            try {
               auto puzzle_descriptions = puzzle_api->load_puzzle_descriptions(entry);
               for (const std::pair<std::string, std::string>& label_and_desc : puzzle_descriptions) {
                  my_avail_puzzles_list->addItem(label_and_desc.first.c_str());
                  auto item = my_avail_puzzles_list->item(my_avail_puzzles_list->count() - 1);
                  item->setData(Qt::UserRole, QVariant(label_and_desc.second.c_str()));
               }
               if (puzzle_descriptions.size() > 0)
                  break;
            } catch (const std::exception&) {
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////
   //
   // Puzzle.

   void main_window_t::load_puzzle()
   {
      static constexpr char puzzle_file_types[] = "Puzzle Text files (*.txt);;All files (*.*)";

      // We must stop on load because otherwise the threads are preventing the dialog from opening!
      stop_puzzle();

      filesystem::path path = AskOpen(tr("Load Puzzle"), tr(puzzle_file_types), this);
      load_puzzle(path);
   }

   void main_window_t::load_puzzle(const solver::problem_t::ptr_t& a_puzzle)
   {
         if (!a_puzzle)
            return

         stop_puzzle();
         my_puzzle = a_puzzle;
         update_puzzle();
         my_solutions.clear();
         update_solutions();
         draw_selected_puzzle_tile();
   }

   void main_window_t::load_puzzle_from_desc(const std::string& a_desc)
   {
      for (const auto& puzzle_api : my_puzzle_apis) {
         try {
            auto puzzle = puzzle_api->load_puzzle_from_description(a_desc);
            if (puzzle) {
               return load_puzzle(puzzle);
            }
         }
         catch(const std::exception&) {
         }
      }
   }

   void main_window_t::load_puzzle(const std::filesystem::path& a_path)
   {
      if (a_path.empty())
         return;

      for (const auto& puzzle_api : my_puzzle_apis) {
         try {
            auto puzzle_descriptions = puzzle_api->load_puzzle_descriptions(a_path);
            if (puzzle_descriptions.size() > 0) {
               return load_puzzle_from_desc(puzzle_descriptions[0].second);
            }
         }
         catch(const std::exception&) {
         }
      }
   }

   void main_window_t::load_puzzle_from_available_puzzle(int a_row)
   {
      auto item = my_avail_puzzles_list->item(a_row);
      if (!item)
         return;

      load_puzzle_from_desc(item->data(Qt::UserRole).toString().toStdString());
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

         for (const auto& puzzle_api : my_puzzle_apis) {
            puzzle_api->save_puzzle(path, my_puzzle);
         }
         update_puzzle();
      }
      catch (const std::exception& ex)
      {
         showException("Could not save the puzzle:", ex);
      }
   }

   void main_window_t::edit_puzzle()
   {
      try
      {
         // We must stop on save because otherwise the threads are preventing the dialog from opening!
         stop_puzzle();

         std::string puzzle_text;
         std::shared_ptr<puzzle_api_t> good_puzzle_api;
         for (const auto& puzzle_api : my_puzzle_apis) {
            puzzle_text = puzzle_api->convert_puzzle_to_text(my_puzzle);
            if (!puzzle_text.empty()) {
               good_puzzle_api = puzzle_api;
               break;
            }
         }

         auto new_text = QInputDialog::getMultiLineText(this, "Edit Puzzle", "Modify the puzzle description", puzzle_text.c_str());

         if (!new_text.isEmpty())
         {
            puzzle_text = new_text.toStdString();
            my_puzzle = good_puzzle_api->convert_text_to_puzzle(puzzle_text);
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
         if (my_solutions.size() <= 0)
            return;

         // We must stop on load because otherwise the threads are preventing the dialog from opening!
         stop_puzzle();

         filesystem::path path = AskSave(tr("Save Solutions"), tr(solutions_file_types), "", this);
         if (path.empty())
            return;

         for (const auto& puzzle_api : my_puzzle_apis) {
            puzzle_api->save_solutions(path, my_solutions);
         }
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

         for (const auto& puzzle_api : my_puzzle_apis) {
            my_solutions = puzzle_api->load_solutions(path);
            if (my_solutions.size() > 0)
               break;
         }

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

      for (const auto& puzzle_api : my_puzzle_apis)
         for (const std::string& item : puzzle_api->get_puzzle_description(my_puzzle))
            if (item.size() > 0)
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

      if (my_solutions.size() > 0) {
         for (const auto& puzzle_api : my_puzzle_apis)
            for (const std::string& item : puzzle_api->get_solutions_description(my_solutions))
            if (item.size() > 0)
                  my_solutions_list->addItem(item.c_str());
      }
      else {
         my_solutions_list->addItem("No solutions found.");
      }

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
      my_load_puzzle_action->setEnabled(true);
      my_save_puzzle_action->setEnabled(my_puzzle != nullptr);
      my_edit_puzzle_action->setEnabled(my_puzzle != nullptr);

      my_save_solutions_action->setEnabled(my_solutions.size() > 0);
      my_load_solutions_action->setEnabled(true);

      my_solve_puzzle_action->setEnabled(my_puzzle && my_puzzle->is_valid());
      my_stop_puzzle_action->setEnabled(my_async_solving.valid());
   }

   solver::solution_t::ptr_t main_window_t::get_selected_solution() const
   {
      const int index = my_solutions_list->currentRow();

      if (index < 0 || index >= my_solutions.size())
         return {};

      auto solution_it = my_solutions.begin();
      std::advance(solution_it, index);

      return *solution_it;
   }

   std::string main_window_t::get_selected_tile() const
   {
      return my_puzzle_list->currentItem() ? my_puzzle_list->currentItem()->text().toStdString() : "";
   }

   void main_window_t::draw_selected_puzzle_tile()
   {
      auto solution = get_selected_solution();
      if (solution)
         return draw_selected_solution();

      for (const auto& puzzle_api : my_puzzle_apis)
         puzzle_api->draw_puzzle_tiles(my_solution_canvas, my_puzzle, get_selected_tile());
   }

   void main_window_t::draw_selected_solution()
   {
      auto solution = get_selected_solution();
      if (!solution)
         return;

      for (const auto& puzzle_api : my_puzzle_apis)
         puzzle_api->draw_solution(my_solution_canvas, my_puzzle, solution, get_selected_tile());
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
            for (const auto& puzzle_api : self->my_puzzle_apis) {
               auto initial_solution = puzzle_api->make_initial_solution(puzzle);
               if (initial_solution)
                  return solver::solve(puzzle, initial_solution, *self);
            }
         }
         catch (const std::exception&)
         {
         }
         return solver::all_solutions_t();
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
