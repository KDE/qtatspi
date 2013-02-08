project(QtAtSpi)

cmake_minimum_required(VERSION 2.6)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

# override the KDE defaults and build tests unless explicitly disabled
#option(KDE4_BUILD_TESTS  "Build the tests" ON)

find_package(Qt4 4.8 REQUIRED)
find_package(KDE4)
include(KDE4Defaults)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT4_INCLUDES}  ${QT_INCLUDE_DIR} ${QT_QT_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${KDE4_INCLUDES})

# Require libatspi2-core headers for building:
include(FindPkgConfig)
pkg_check_modules(ATSPI2 REQUIRED atspi-2)
include_directories(${ATSPI2_INCLUDE_DIRS})

add_subdirectory(src)
add_subdirectory(translations)
add_subdirectory(tests)
add_subdirectory(examples)

