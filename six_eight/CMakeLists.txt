
add_library(six_eight
   include/dak/six_eight/six_eight.h
   include/dak/six_eight/direction.h              src/direction.cpp
   include/dak/six_eight/position.h               src/position.cpp
   include/dak/six_eight/puzzle.h                 src/puzzle.cpp
   include/dak/six_eight/solution.h               src/solution.cpp
   include/dak/six_eight/stream.h                 src/stream.cpp
   include/dak/six_eight/tile.h                   src/tile.cpp
)

target_include_directories(six_eight PUBLIC
   include
)

target_link_libraries(six_eight dak_utility dak_solver)

target_compile_features(six_eight PUBLIC cxx_std_20)

