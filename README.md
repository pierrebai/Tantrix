# Tantrix Solver App
A Windows application to solve Tantrix(tm) puzzles, like the genius puzzle.

# Description
The Tantrix solver can read a description of a puzzle from a text file and then solve it. It will display the solutions. The solutions can be saved and reloaded later.

Some example puzzle description text files are provided in the tantrix_solver/puzzles folder. The format for the puzzle description is two lines:

   * A line with the list of tiles, in this format: tiles: 3 6 17 34
   * A line with the list of lines or loops, in this format: lines: Y R or loops: G B

For example, see the junior puzzle file:

   ```
   tiles: 3 5 8 12 14 43 46 50 52 54
   loops: B
   ```

![User Interface](https://github.com/pierrebai/Tantrix/blob/master/App.png "User Interface")

# Dependencies and Build
The project requires Qt. It was built using Qt 5.15. It uses CMake to build the project. CMake 3.16.4 was used. A C++ compiler supporting C++ 2020 is needed. Visual Studio 2019 was used.

A script to generate a Visual-Studio solution is provided. In order for CMake to find Qt, the environment variable QT5_DIR must be defined and must point to the Qt 5.12 directory. For example:

    QT5_DIR=C:\Outils\Qt\5.15.0\msvc2019_64

Alternatively, to invoke cmake directly, it needs to be told where to find Qt. It needs the environment variable CMAKE_PREFIX_PATH set to the location of Qt. For example:

    CMAKE_PREFIX_PATH=%QT5_DIR%\msvc2019_64

The code was written and tested with Visual Studio 2019, community edition.
