add_executable(six_eight_solver
   src/six_eight_solver.cpp
   "puzzles/Puzzles.txt"
)

target_link_libraries(six_eight_solver PUBLIC
   six_eight
   dak_utility
)

target_compile_features(six_eight_solver PUBLIC
   cxx_std_20)

