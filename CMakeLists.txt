## The KirHut Software Development Library
## CMake Main File
##
## Copyright © KirHut Software Company

cmake_minimum_required(VERSION 3.21)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    string(CONCAT KIRHUT_MSVC_DEBUG_HOTRELOAD
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,"
        "$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
    )
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        ${KIRHUT_MSVC_DEBUG_HOTRELOAD})
endif()

project(libKirHut
    VERSION 0.1
    LANGUAGES CXX
    DESCRIPTION "The KirHut Software Company General Development Library"
)

include(cmake/get_cpm.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(KH_USES_QT "Indicate if this library should be built with Qt support." ON)
option(KH_BUILD_SHARED "Build this library as a shared library." ${BUILD_SHARED_LIBS})
option(KH_LIB_BUILD_TESTS "Build the tests for this library." OFF)
option(KH_REQUIRE_BITLENGTHS "Require that the platform has 8, 32, and 64 bit integers." ON)
option(KH_MD5_SUPPORT "Build this library with MD5 Hash support." ON)

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

CPMAddPackage(
    NAME                spdlog
    VERSION             1.14.1
    GITHUB_REPOSITORY   gabime/spdlog
    OPTIONS
        "SPDLOG_USE_STD_FORMAT ON"
)

set(libKirHut_EXTERN_HEADERS
    mio.hpp
)

set(libKirHut_EXTERN_SOURCES
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
    args.hpp
    base.hpp
    baseio.hpp
    exception.hpp
    exceptions.hpp
    filesystem.hpp
    fstream.hpp
    global.hpp
    input.hpp
    iostream.hpp
    iotools.hpp
    logging.hpp
    output.hpp
    ranges.hpp
)

list(TRANSFORM libKirHut_PRIVATE_HEADERS PREPEND "priv/")
list(TRANSFORM libKirHut_LIBRARY_HEADERS PREPEND "kh/")

set(libKirHut_SOURCES
    args.cpp
    base.cpp
    exception.cpp
    fstream.cpp
    input.cpp
    iostream.cpp
    iotools.cpp
    logging.cpp
    output.cpp
)

if (KH_MD5_SUPPORT)
    list(APPEND libKirHut_LIBRARY_HEADERS kh/md5hash.hpp)
    list(APPEND libKirHut_SOURCES md5hash.cpp)
endif()

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

set(libKirHut_PUBLIC_LIBRARIES
    spdlog::spdlog
)

set(libKirHut_LINK_LIBRARIES
	$<$<BOOL:${KH_USES_QT}>:Qt${QT_VERSION_MAJOR}::Core>
	nowide::nowide
)

set(libKirHut_COMPILE_DEFS
	$<$<BOOL:${PROJECT_IS_TOP_LEVEL}>:KH_LIBRARY_BUILD>
    $<$<BOOL:${KH_REQUIRE_BITLENGTHS}>:KH_MUST_HAVE_8_32_64>
	$<$<BOOL:${KH_USES_QT}>:KH_USES_QT>
	$<$<BOOL:${KH_BUILD_SHARED}>:KH_DYNAMIC_LINK>
)

if(KH_LIB_BUILD_TESTS)
    add_subdirectory(test)
endif()

target_link_libraries(libKirHut PUBLIC ${libKirHut_PUBLIC_LIBRARIES})
target_link_libraries(libKirHut PRIVATE ${libKirHut_LINK_LIBRARIES})
target_compile_definitions(libKirHut PRIVATE ${libKirHut_COMPILE_DEFS})
