
add_library(tantrix_tests SHARED
   src/color_tests.cpp
   src/direction_tests.cpp
   src/position_tests.cpp
   src/solution_tests.cpp
   src/tile_tests.cpp

   include/dak/tantrix_tests/helpers.h
)

target_link_libraries(tantrix_tests PUBLIC
   tantrix
)

target_compile_features(tantrix_tests PUBLIC cxx_std_20)

target_include_directories(tantrix_tests PUBLIC
   "${PROJECT_SOURCE_DIR}/tantrix/include"
   "${PROJECT_SOURCE_DIR}/tantrix_tests/include"
)

