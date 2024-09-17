cmake_minimum_required(VERSION 3.16)

project(LyricProcessor LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

FILE(GLOB HEADERS ./*.h)
FILE(GLOB SOURCES ./*.cpp)
add_library(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    LYRICPROCESSOR_LIBRARY
)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_SHARED_LIBS)
endif()

add_executable(test
    test/main.cpp
)
target_link_libraries(test PRIVATE ${PROJECT_NAME})

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
