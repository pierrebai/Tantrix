add_executable(tantrix_solver
   src/tantrix_solver.cpp
   "puzzles/Genius Puzzle Red Blue.txt"
   "puzzles/Genius Puzzle Red Yellow.txt"
   "puzzles/Genius Puzzle Yellow Tiles.txt"
   "puzzles/Junior Puzzle.txt"
   "puzzles/Student Puzzle.txt"
   "puzzles/Professor Puzzle.txt"
)

target_link_libraries(tantrix_solver PUBLIC
   tantrix
)

target_compile_features(tantrix_solver PUBLIC
   cxx_std_20)

target_include_directories(tantrix_solver PUBLIC
   "${PROJECT_SOURCE_DIR}/tantrix/inc"
)

