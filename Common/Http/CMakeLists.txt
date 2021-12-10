cmake_minimum_required(VERSION 3.14)

project(Http LANGUAGES CXX)

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
    inc/export.h
    inc/http_configuration.h
    inc/http_connection_handler.h
    inc/http_connection_handler_pool.h
    inc/http_cookie.h
    inc/http_listener.h
    inc/http_request.h
    inc/http_request_handler.h
    inc/http_response.h
)

set(SRC
    src/http_configuration.cpp
    src/http_connection_handler.cpp
    src/http_connection_handler_pool.cpp
    src/http_cookie.cpp
    src/http_listener.cpp
    src/http_request.cpp
    src/http_request_handler.cpp
    src/http_response.cpp
)

if(WIN32)
    add_definitions(-DHTTPLIB_EXPORT)
endif()

add_library(Http SHARED ${HEADERS} ${SRC})

#install(TARGETS Http
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)

target_link_libraries(Http Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(Http Qt${QT_VERSION_MAJOR}::Network)

include_directories(inc)

add_subdirectory(test)
