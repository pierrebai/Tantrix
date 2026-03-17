
add_library(six_eight_tests SHARED
   src/direction_tests.cpp
   src/position_tests.cpp
   src/solution_tests.cpp
   src/solve_tests.cpp
   src/tile_tests.cpp

   include/dak/six_eight_tests/helpers.h
)

target_link_libraries(six_eight_tests PUBLIC
   six_eight
   dak_utility
)

target_compile_features(six_eight_tests PUBLIC cxx_std_20)

target_include_directories(six_eight_tests PRIVATE
   "${PROJECT_SOURCE_DIR}/six_eight_tests/include"
)

