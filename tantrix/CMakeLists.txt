
add_library(tantrix
   include/dak/tantrix/tantrix.h
   include/dak/tantrix/direction.h              src/direction.cpp
   include/dak/tantrix/position.h               src/position.cpp
   include/dak/tantrix/puzzle.h                 src/puzzle.cpp
   include/dak/tantrix/solution.h               src/solution.cpp
   include/dak/tantrix/solve.h                  src/solve.cpp
   include/dak/tantrix/stream.h                 src/stream.cpp
   include/dak/tantrix/tile.h                   src/tile.cpp
)

target_include_directories(tantrix PUBLIC
   include
   "${PROJECT_SOURCE_DIR}/utility/include"
)

target_compile_features(tantrix PUBLIC cxx_std_20)

