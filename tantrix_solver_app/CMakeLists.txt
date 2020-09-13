# Qt 5 stuff

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS WinExtras REQUIRED)

add_executable(tantrix_solver_app WIN32
   src/tantrix_solver_app.cpp
   src/main_window.cpp                  include/main_window.h

   resources/resource.rc                resources/resource.h
)

target_link_libraries(tantrix_solver_app PUBLIC
   tantrix
   utility
   QtAdditions
   Qt5::Widgets Qt5::Gui Qt5::Core Qt5::WinExtras
)

target_compile_features(tantrix_solver_app PUBLIC
   cxx_std_20
)

target_include_directories(tantrix_solver_app PUBLIC
   include
   resources
   "${PROJECT_SOURCE_DIR}/tantrix/include"
   "${PROJECT_SOURCE_DIR}/utility/include"
   "${PROJECT_SOURCE_DIR}/QtAdditions/include"
)

add_custom_command(TARGET tantrix_solver_app POST_BUILD
   COMMAND "${Qt5_DIR}/../../../bin/windeployqt.exe" --no-webkit2 --no-angle --no-system-d3d-compiler --no-opengl-sw "$<TARGET_FILE:tantrix_solver_app>"
)

install(
   DIRECTORY
   "$<TARGET_FILE_DIR:tantrix_solver_app>/"
   DESTINATION .
   COMPONENT application
   PATTERN "*.pdb" EXCLUDE 
   PATTERN "*.ilk" EXCLUDE 
)

set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_NAME "TantrixSolverApp")
set(CPACK_PACKAGE_VENDOR "Pierre Baillargeon")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tantrix Solver App - An application to solve Tantrix(tm) puzzles.")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Tantrix Solver App")
# set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/tantrix_solver_app/resources/icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/tantrix_solver_app/resources/icon.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "tantrix_solver_app.exe")
set(CPACK_NSIS_DISPLAY_NAME "Tantrix Solver App")

INCLUDE(CPack)
