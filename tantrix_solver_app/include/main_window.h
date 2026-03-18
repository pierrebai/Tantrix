#pragma once

#include <puzzle_api.h>

#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qlistwidget.h>

#include <dak/solver/solve.h>
#include <dak/utility/progress.h>
#include <dak/utility/stopwatch.h>

#include <chrono>
#include <future>
#include <memory>
#include <filesystem>

class QToolButton;
class QAction;
class QTreeView;
class QDockWidget;
class QTimer;
class QLineEdit;
class QLabel;
class QPushButton;
class QGraphicsView;
class QGraphicsScene;
class QErrorMessage;

namespace dak::tantrix_solver_app
{
   /////////////////////////////////////////////////////////////////////////
   //
   // Main window of the tantrix solver app.

   class main_window_t : public QMainWindow, private utility::progress_t
   {
   public:
      // Create the main window.
      main_window_t();

   protected:
      // Create the UI elements.
      void build_ui();
      void build_toolbar_ui();
      void build_avail_puzzles_ui();
      void build_puzzle_ui();
      void build_solutions_ui();
      void build_solution_canvas();

      // Connect the signals of the UI elements.
      void connect_ui();

      // Fill the UI with the initial data.
      void fill_ui();
      void fill_available_puzzles();

      // Puzzle.
      void load_puzzle();
      void load_puzzle(const std::filesystem::path& a_path);
      void load_puzzle_from_desc(const std::string& a_desc);
      void load_puzzle(const solver::problem_t::ptr_t& a_puzzle);
      void load_puzzle_from_available_puzzle(int a_row);

      void save_puzzle();
      void edit_puzzle();

      void save_solutions();
      void load_solutions();

      // Asynchronous puzzle solving.
      void solve_puzzle();
      void verify_async_puzzle_solving();
      bool is_async_puzzle_solving_done();
      void stop_puzzle();

      // Asynchronous puzzle solving update.
      void update_progress(size_t a_total_count_so_far) override;

      // UI updates from data.
      solver::solution_t::ptr_t get_selected_solution() const;
      std::string get_selected_tile() const;

      void update_puzzle();
      void update_solutions();
      void update_toolbar();
      void update_solving_attempts();
      void update_solving_time();

      void draw_selected_solution();
      void draw_selected_puzzle_tile();

      void showException(const char* message, const std::exception& ex);

      // Toolbar buttons.
      QAction*       my_load_puzzle_action = nullptr;
      QToolButton*   my_load_puzzle_button = nullptr;

      QAction*       my_save_puzzle_action = nullptr;
      QToolButton*   my_save_puzzle_button = nullptr;

      QAction*       my_edit_puzzle_action = nullptr;
      QToolButton*   my_edit_puzzle_button = nullptr;

      QAction*       my_solve_puzzle_action = nullptr;
      QToolButton*   my_solve_puzzle_button = nullptr;

      QAction*       my_stop_puzzle_action = nullptr;
      QToolButton*   my_stop_puzzle_button = nullptr;

      QAction*       my_save_solutions_action = nullptr;
      QToolButton*   my_save_solutions_button = nullptr;

      QAction*       my_load_solutions_action = nullptr;
      QToolButton*   my_load_solutions_button = nullptr;

      // UI elements.
      QGraphicsView*    my_solution_canvas = nullptr;

      QListWidget*      my_avail_puzzles_list = nullptr;
      QLabel*           my_avail_puzzles_label = nullptr;

      QListWidget*      my_puzzle_list = nullptr;
      QLabel*           my_puzzle_label = nullptr;

      QListWidget*      my_solutions_list = nullptr;
      QLabel*           my_solving_attempts_label = nullptr;
      QLabel*           my_solving_time_label = nullptr;

      QTimer*           my_solve_puzzle_timer = nullptr;
      QErrorMessage*    my_error_message = nullptr;

      // Data.
      std::vector<puzzle_api_t::ptr_t>          my_puzzle_apis;
      solver::problem_t::ptr_t                  my_puzzle;
      std::filesystem::path                     my_puzzle_filename;
      solver::all_solutions_t                   my_solutions;

      std::future<solver::all_solutions_t>      my_async_solving;
      std::atomic<size_t>                       my_solving_attempts = 0;
      
      dak::utility::stopwatch_t                 my_solving_stopwatch;
      std::string                               my_solving_time_buffer;

      Q_OBJECT;
   };
}

