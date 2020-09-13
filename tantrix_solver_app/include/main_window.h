#pragma once

#ifndef DAK_TANTRIX_SOLVER_APP_MAIN_WINDOW_H
#define DAK_TANTRIX_SOLVER_APP_MAIN_WINDOW_H

#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qlistwidget.h>

class QToolButton;
class QAction;
class QTreeView;
class QDockWidget;
class QTimer;
class QLineEdit;
class QPushButton;
class QMdiArea;

namespace dak::tantrix_solver_app
{
   /////////////////////////////////////////////////////////////////////////
   //
   // Main window of the tantrix solver app.

   class main_window_t : public QMainWindow
   {
   public:
      // Create the main window.
      main_window_t();

   protected:
      // Create the UI elements.
      void build_ui();
      void build_toolbar_ui();
      void build_puzzle_ui();
      void build_solutions_ui();
      void build_tabbed_ui();

      // Connect the signals of the UI elements.
      void connect_ui();

      // Fill the UI with the intial data.
      void fill_ui();

      // Puzzle.
      void load_puzzle();
      void solve_puzzle();
      void stop_puzzle();
      void update_active_tab();

      // Toolbar buttons.
      QAction*       my_load_puzzle_action = nullptr;
      QToolButton*   my_load_puzzle_button = nullptr;

      QAction*       my_solve_puzzle_action = nullptr;
      QToolButton*   my_solve_puzzle_button = nullptr;

      QAction*       my_stop_puzzle_action = nullptr;
      QToolButton*   my_stop_puzzle_button = nullptr;

      // UI elements.
      QMdiArea*         my_tabs = nullptr;
      QListWidget*      my_puzzle_list = nullptr;
      QListWidget*      my_solutions_list = nullptr;
   };
}

#endif /* DAK_TANTRIX_SOLVER_APP_MAIN_WINDOW_H */

