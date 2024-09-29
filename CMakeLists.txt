cmake_minimum_required(VERSION 3.21)

project(libKirHut LANGUAGES CXX)

include(cmake/get_cpm.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(KH_USES_QT "Indicate if this library should be built with Qt support." ON)
option(KH_BUILD_SHARED "Build this library as a shared library." ${BUILD_SHARED_LIBS})
option(KH_LIB_BUILD_TESTS "Build the tests for this library." OFF)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(KH_USES_QT)
	find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
	find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
endif()

CPMAddPackage(
	"https://github.com/boostorg/nowide/releases/download/v11.3.0/nowide_standalone_v11.3.0.tar.gz"
)

set(libKirHut_EXTERN_HEADERS
    loguru.hpp
)

set(libKirHut_EXTERN_SOURCES
    loguru.cpp
)

list(TRANSFORM libKirHut_EXTERN_HEADERS PREPEND "include/")

set(libKirHut_EXTERN
    ${libKirHut_EXTERN_HEADERS}
    ${libKirHut_EXTERN_SOURCES}
)

list(TRANSFORM libKirHut_EXTERN PREPEND "extern/")

set(libKirHut_PRIVATE_HEADERS
)

set(libKirHut_LIBRARY_HEADERS
    base.hpp
    exception.hpp
    exceptions.hpp
    global.hpp
    iostream.hpp
    md5hash.hpp
    ranges.hpp
)

list(TRANSFORM libKirHut_PRIVATE_HEADERS PREPEND "priv/")
list(TRANSFORM libKirHut_LIBRARY_HEADERS PREPEND "kh/")

set(libKirHut_SOURCES
    exception.cpp
    base.cpp
    iostream.cpp
    md5hash.cpp
)

set(libKirHut_HEADERS
    ${libKirHut_LIBRARY_HEADERS}
    ${libKirHut_PRIVATE_HEADERS}
)

list(TRANSFORM libKirHut_SOURCES PREPEND "src/")
list(TRANSFORM libKirHut_HEADERS PREPEND "include/")

add_library(libKirHut
    ${libKirHut_HEADERS}
    ${libKirHut_SOURCES}
    ${libKirHut_EXTERN}
)
add_library(KirHut::libKirHut ALIAS libKirHut)

target_include_directories(libKirHut PUBLIC "include")
target_include_directories(libKirHut PRIVATE "extern/include" "include/priv")

set(libKirHut_LINK_LIBRARIES
	$<$<BOOL:${KH_USES_QT}>:Qt${QT_VERSION_MAJOR}::Core>
	nowide::nowide
)

set(libKirHut_COMPILE_DEFS
	$<$<BOOL:${PROJECT_IS_TOP_LEVEL}>:KH_LIBRARY_BUILD>
	KH_MUST_HAVE_8_32_64
	$<$<BOOL:${KH_USES_QT}>:KH_USES_QT>
	$<$<BOOL:${KH_BUILD_SHARED}>:KH_DYNAMIC_LINK>
)

if(KH_LIB_BUILD_TESTS)
    add_subdirectory(test)
endif()

target_link_libraries(libKirHut PRIVATE ${libKirHut_LINK_LIBRARIES})
target_compile_definitions(libKirHut PRIVATE ${libKirHut_COMPILE_DEFS})
