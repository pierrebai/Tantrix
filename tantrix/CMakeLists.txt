
add_library(tantrix
   include/dak/tantrix/tantrix.h
   include/dak/tantrix/any_shape_puzzle.h       src/any_shape_puzzle.cpp
   include/dak/tantrix/color.h
   include/dak/tantrix/direction.h              src/direction.cpp
   include/dak/tantrix/position.h               src/position.cpp
   include/dak/tantrix/puzzle.h                 src/puzzle.cpp
   include/dak/tantrix/solution.h               src/solution.cpp
   include/dak/tantrix/solve.h                  src/solve.cpp
   include/dak/tantrix/stream.h                 src/stream.cpp
   include/dak/tantrix/tile.h                   src/tile.cpp
   include/dak/tantrix/triangle_puzzle.h        src/triangle_puzzle.cpp
)

target_include_directories(tantrix PUBLIC
   include
)

target_link_libraries(tantrix dak_utility)

target_compile_features(tantrix PUBLIC cxx_std_20)

