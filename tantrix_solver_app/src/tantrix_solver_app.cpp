#include "main_window.h"

#include <QtWidgets/qapplication.h>

static HINSTANCE appInstance;

namespace dak::tantrix_solver_app
{
   /////////////////////////////////////////////////////////////////////////
   //
   // Creates the Qt App and a main window.

   int App(int argc, char** argv)
   {
      QScopedPointer<QApplication> app(new QApplication(argc, argv));

      auto main_window = new main_window_t;

      main_window->resize(1000, 800);
      main_window->show();

      return app->exec();
   }
}

int main(int argc, char** argv)
{
   return dak::tantrix_solver_app::App(argc, argv);
}

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* lpCmdLine, int nCmdShow)
{
   appInstance = hInstance;
   return main(0, 0);
}

