cmake_minimum_required(VERSION 3.14)

project(BackendTest LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

enable_testing(true)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

add_executable(BoardTableTest
  ../src/board_table.cpp
  src/test_board_table.cpp
)

target_link_libraries(BoardTableTest Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(BoardTableTest Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME BoardTableTest COMMAND BoardTableTest)

add_executable(HighscoresTest
  ../src/highscores.cpp
  src/test_highscores.cpp
)

target_link_libraries(HighscoresTest Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(HighscoresTest Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME HighscoresTest COMMAND HighscoresTest)


#link_directories(${CMAKE_BINARY_DIR})
#target_link_libraries(BackendTest Http)

include_directories(
  ../inc
)
