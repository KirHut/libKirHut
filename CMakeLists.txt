cmake_minimum_required(VERSION 3.14)

project(libKirHut LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(libKirHut SHARED
  exception.cpp
  exception.hpp
  khglobal.hpp
  khbase.hpp
  khbase.cpp
)

target_link_libraries(libKirHut PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(libKirHut PRIVATE KH_LIBRARY_BUILD KH_MUST_HAVE_8_32_64 KH_USES_QT)
