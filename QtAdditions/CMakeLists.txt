# Qt 5 stuff

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS WinExtras REQUIRED)

add_library(QtAdditions
   src/QtUtilities.cpp               include/dak/QtAdditions/QtUtilities.h
)

target_compile_features(QtAdditions PUBLIC cxx_std_20)

target_include_directories(QtAdditions PUBLIC
   include
)

target_include_directories(QtAdditions PUBLIC
   "${Qt5Core_INCLUDE_DIRS}"
   "${Qt5Widgets_INCLUDE_DIRS}"
   "${Qt5Gui_INCLUDE_DIRS}"
   "${Qt5WinExtras_INCLUDE_DIRS}"
)

