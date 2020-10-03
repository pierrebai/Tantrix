cmake_minimum_required(VERSION 3.7.0)

# The dak/tantrix library, solver and its tests.

project(tantrix)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

add_subdirectory(tantrix)
add_subdirectory(dak_utility/utility)
add_subdirectory(QtAdditions)
add_subdirectory(tantrix_solver)
add_subdirectory(tantrix_solver_app)
add_subdirectory(tantrix_tests)

