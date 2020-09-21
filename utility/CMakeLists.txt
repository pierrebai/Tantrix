
add_library(utility
   include/dak/utility/all.h
   include/dak/utility/progress.h                  src/progress.cpp
   include/dak/utility/stream_progress.h           src/stream_progress.cpp
   include/dak/utility/multi_thread_progress.h     src/multi_thread_progress.cpp
   include/dak/utility/stopwatch.h                 src/stopwatch.cpp
   include/dak/utility/thread_pool.h               src/thread_pool.cpp
   include/dak/utility/threaded_work.h
)

target_include_directories(utility PUBLIC
   include
)

target_compile_features(utility PUBLIC
   cxx_std_20
)

