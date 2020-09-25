
add_library(utility_tests SHARED
   src/stopwatch_tests.cpp
   src/progress_tests.cpp
   src/stream_progress_tests.cpp
   src/thread_pool_tests.cpp
   src/threaded_work_tests.cpp
)

target_link_libraries(utility_tests PUBLIC
   utility
)

target_compile_features(utility_tests PUBLIC cxx_std_20)

target_include_directories(utility_tests PUBLIC
   "${PROJECT_SOURCE_DIR}/utility/include"
)

