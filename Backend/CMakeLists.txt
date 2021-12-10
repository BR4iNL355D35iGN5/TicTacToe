cmake_minimum_required(VERSION 3.14)

project(Backend LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)


set(HEADERS
    inc/board_table.h
    inc/highscores.h
    inc/player.h
    inc/tic_tac_toe.h
    inc/tic_tac_toe_request_handler.h
)

set(SRC
    src/board_table.cpp
    src/highscores.cpp
    src/player.cpp
    src/main.cpp
    src/tic_tac_toe.cpp
    src/tic_tac_toe_request_handler.cpp
)

add_executable(TicTacToeServer ${HEADERS} ${SRC})
target_link_libraries(TicTacToeServer Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(TicTacToeServer Qt${QT_VERSION_MAJOR}::Network)

link_directories(${CMAKE_BINARY_DIR})
target_link_libraries(TicTacToeServer Http)

include_directories(
    inc
    ../Common/Http/inc
)

add_subdirectory(test)
