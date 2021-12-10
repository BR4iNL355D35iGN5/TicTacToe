cmake_minimum_required(VERSION 3.14)

project(HttpTest LANGUAGES CXX)

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

add_executable(HttpTest
  src/test_http_listener.cpp
)

target_link_libraries(HttpTest Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(HttpTest Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(HttpTest Qt${QT_VERSION_MAJOR}::Test)

#find_library(Http REQUIRED)
link_directories(${CMAKE_BINARY_DIR})
target_link_libraries(HttpTest Http)

add_test(NAME HttpTest COMMAND HttpTest)

include_directories(
  ../inc
)
