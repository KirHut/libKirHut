## The KirHut Software Development Library
## CMake Main File
##
## Copyright © KirHut Software Company

cmake_minimum_required(VERSION 3.27)

# Enable Hot Reload for MSVC compilers if supported.
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    string(CONCAT KIRHUT_MSVC_DEBUG_HOTRELOAD
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,"
        "$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
    )
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        ${KIRHUT_MSVC_DEBUG_HOTRELOAD})
endif()

project(libKirHut
    VERSION 0.2.0
    LANGUAGES CXX
    DESCRIPTION "The KirHut Software Company General C++ Development Library"
)

include(CMakeDependentOption)
include(FetchContent)
include(GenerateExportHeader)
include(cmake/kh_compiler_info.cmake)

kh_find_compiler_info()

if(KH_CLANG_GNUC_COMPATIBLE)
    cmake_host_system_information(RESULT KH_SUPPORTS_128_TYPES QUERY IS_64BIT)
else()
    set(KH_SUPPORTS_128_TYPES OFF)
endif()

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(KH_LIB_BUILD_TESTS_DEFAULT ON)
else()
    set(KH_LIB_BUILD_TESTS_DEFAULT OFF)
endif()

string(CONCAT KH_MUST_HAVE_16_32_64_STRING
    "Require that the platform has 16, 32, and 64 bit integers "
    "(implies KH_MUST_HAVE_32_64 and KH_MUST_HAVE_16BIT_TYPES)."
)

string(CONCAT KH_MUST_HAVE_32_64_STRING
    "Require that the platform has 32 and 64 bit integers "
    "(implies KH_MUST_HAVE_32BIT_TYPES and KH_MUST_HAVE_64BIT_TYPES)."
)

option(KH_MUST_HAVE_16_32_64 "${KH_MUST_HAVE_16_32_64_STRING}" OFF)
option(KH_MUST_HAVE_32_64 "${KH_MUST_HAVE_32_64_STRING}" OFF)
option(KH_MUST_HAVE_16BIT_TYPES "Require that the platform has 16 bit integers." OFF)
option(KH_MUST_HAVE_32BIT_TYPES "Require that the platform has 32 bit integers." OFF)
option(KH_MUST_HAVE_64BIT_TYPES "Require that the platform has 64 bit integers." OFF)

option(KH_USES_QT "Build libKirHut with Qt extensions enabled (implies KH_NO_BADALLOC)." OFF)
option(KH_USES_FMT "Build libKirHut with {fmt} dependency instead of std::format." ON)
option(KH_LIB_BUILD_TESTS "Build the tests for this library." ${KH_LIB_BUILD_TESTS_DEFAULT})
option(BUILD_SHARED_LIBS "Build this library (and its dependencies) as shared libraries." ON)
option(KH_NO_BADALLOC "Remove support for throwing or catching std::bad_alloc." OFF)
option(KH_NO_EXCEPTIONS "Removes exception support entirely, along with the Error type (implies KH_NO_BADALLOC)." OFF)
option(KH_INCLUDE_ARG_PARSER "Include KirHut::CLI command line argument parsing functionality." ON)
option(KH_INCLUDE_FILESYSTEM "Include std::filesystem under the KirHut::FS namespace." ON)
option(KH_INCLUDE_MD5HASH "Build this library with MD5Hash support (implies KH_MUST_HAVE_32_64)." ON)
option(KH_INCLUDE_TASK_SYSTEM "Include controllable multithreaded task system." ON)
option(KH_INCLUDE_TERMINAL_PRINT "Include UTF-8 C++23 style printing support in KirHut::IO." ON)
option(KH_INCLUDE_TOML "Build this library with TOML parsing support." ON)
option(KH_OVERRIDE_PLATFORM_SAFETY "Ignore unsupported platform checks during compile time checking (dangerous)." OFF)
cmake_dependent_option(KH_USE_128BIT_TYPES
    "Use 128 bit types on platforms that support it."
    ON
    KH_SUPPORTS_128_TYPES
    OFF
)

mark_as_advanced(
    KH_MUST_HAVE_16_32_64
    KH_MUST_HAVE_32_64
    KH_MUST_HAVE_16BIT_TYPES
    KH_MUST_HAVE_32BIT_TYPES
    KH_MUST_HAVE_64BIT_TYPES
    KH_LIB_BUILD_TESTS
    KH_NO_BADALLOC
    KH_NO_EXCEPTIONS
    KH_INCLUDE_ARG_PARSER
    KH_INCLUDE_FILESYSTEM
    KH_INCLUDE_MD5HASH
    KH_INCLUDE_TASK_SYSTEM
    KH_INCLUDE_TERMINAL_PRINT
    KH_INCLUDE_TOML
    KH_OVERRIDE_PLATFORM_SAFETY
)

find_package(Doxygen QUIET)
if(Doxygen_FOUND)
    add_subdirectory(docs)
else()
    message(STATUS
        "Doxygen not found on the build host, so the documentation target and options will not be available."
    )
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})

if(KH_USES_QT)
    # Qt must be installed on the platform prior to building. This can be done using the Qt Installer tool on Windows or
    # using a package manager on Unix. This may also be done with the Installer tool or Homebrew on MacOS.
    find_package(QT NAMES Qt6 Qt5 REQUIRED
        COMPONENTS
            Core
    )
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED
        COMPONENTS
            Core
    )
endif()

set(KH_BINARY_DIR ${PROJECT_BINARY_DIR})

if(WIN32)
    # Boost::nowide is unconditionally downloaded and built, unlike other dependencies that check find_package first.
    # This is because this library uses the standalone version of nowide, which is almost never distributed using
    # packaging systems, so to prevent dragging in all of Boost, this is done.
    FetchContent_Declare(
        nowide
        URL "https://github.com/boostorg/nowide/releases/download/v11.3.0/nowide_standalone_v11.3.0.tar.gz"
        URL_HASH SHA256=153ac93173c8de9c08e7701e471fa750f84c27e51fe329570c5aa06016591f8c
    )

    FetchContent_MakeAvailable(nowide)
    target_compile_features(nowide PUBLIC cxx_std_20)
    set_target_properties(nowide
        PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
            LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
            RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
endif()

if(KH_USES_FMT)
    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 40626af88bd7df9a5fb80be7b25ac85b122d6c21
        FIND_PACKAGE_ARGS 11.2.0
    )

    FetchContent_MakeAvailable(fmt)
    target_compile_features(fmt PUBLIC cxx_std_20)
    set_target_properties(fmt
        PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
            LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
            RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
endif()

set(libKirHut_HEADERS
    args.hpp
    base.hpp
    error.hpp
    errors.hpp
    filesystem.hpp
    global.hpp
    invalid.hpp
    math.hpp
    md5hash.hpp
    packedtuple.hpp
    platform.hpp
    print.hpp
    ranges.hpp
    task.hpp
    toml.hpp
)

set(libKirHut_PRIV_HEADERS
    format.hpp
)

set(libKirHut_SOURCES
    base.cpp
    invalid.cpp
)

if(KH_INCLUDE_ARG_PARSER)
    list(APPEND libKirHut_SOURCES args.cpp)
endif()

if(KH_INCLUDE_MD5HASH)
    list(APPEND libKirHut_SOURCES md5hash.cpp)
endif()

if(KH_INCLUDE_TASK_SYSTEM)
    list(APPEND libKirHut_SOURCES task.cpp)
endif()

if(KH_INCLUDE_TERMINAL_PRINT)
    list(APPEND libKirHut_SOURCES print.cpp)
endif()

if(KH_INCLUDE_TOML)
    list(APPEND libKirHut_SOURCES toml.cpp)
endif()

if(APPLE)
    set(KH_KERNEL_VERSION ${CMAKE_SYSTEM_VERSION})
endif()

list(TRANSFORM libKirHut_SOURCES PREPEND "src/")
list(TRANSFORM libKirHut_PRIV_HEADERS PREPEND "priv/")
list(APPEND libKirHut_HEADERS ${libKirHut_PRIV_HEADERS})
list(TRANSFORM libKirHut_HEADERS PREPEND "include/kh/")

set(libKirHut_COMPILE_DEFS
    $<$<BOOL:${KH_USES_QT}>:KH_USES_QT>
    $<$<BOOL:${KH_USES_FMT}>:KH_USES_FMT>
    $<$<BOOL:${KH_USE_128BIT_TYPES}>:KH_USE_128BIT_TYPES>
    $<$<BOOL:${KH_NO_EXCEPTIONS}>:KH_NO_EXCEPTIONS>
    $<$<BOOL:${KH_NO_BADALLOC}>:KH_NO_BADALLOC>
    $<$<BOOL:${KH_INCLUDE_ARG_PARSER}>:KH_INCLUDE_ARG_PARSER>
    $<$<BOOL:${KH_INCLUDE_FILESYSTEM}>:KH_INCLUDE_FILESYSTEM>
    $<$<BOOL:${KH_INCLUDE_MD5HASH}>:KH_INCLUDE_MD5HASH>
    $<$<BOOL:${KH_INCLUDE_TASK_SYSTEM}>:KH_INCLUDE_TASK_SYSTEM>
    $<$<BOOL:${KH_INCLUDE_TERMINAL_PRINT}>:KH_INCLUDE_TERMINAL_PRINT>
    $<$<BOOL:${KH_INCLUDE_TOML}>:KH_INCLUDE_TOML>
    $<$<BOOL:${KH_MUST_HAVE_16_32_64}>:KH_MUST_HAVE_16_32_64>
    $<$<BOOL:${KH_MUST_HAVE_32_64}>:KH_MUST_HAVE_32_64>
    $<$<BOOL:${KH_MUST_HAVE_16BIT_TYPES}>:KH_MUST_HAVE_16BIT_TYPES>
    $<$<BOOL:${KH_MUST_HAVE_32BIT_TYPES}>:KH_MUST_HAVE_32BIT_TYPES>
    $<$<BOOL:${KH_MUST_HAVE_64BIT_TYPES}>:KH_MUST_HAVE_64BIT_TYPES>
    $<$<BOOL:${KH_OVERRIDE_PLATFORM_SAFETY}>:KH_OVERRIDE_PLATFORM_SAFETY>
    $<$<BOOL:${KH_CLANG_GNUC_COMPATIBLE}>:KH_CLANG_GNUC_COMPATIBLE>
    ${KH_COMPILED_WITH_NAME}=${${KH_COMPILED_WITH_NAME}}
    $<$<BOOL:${KH_KERNEL_VERSION}>:KH_KERNEL_VERSION=${KH_KERNEL_VERSION}>
    KH_COMPILER_DISPLAY_STRING="${KH_COMPILER_DISPLAY_STRING}"
)

message(DEBUG "Compile Defs: ${libKirHut_COMPILE_DEFS}")

add_subdirectory(extern)

add_library(libKirHut
    ${libKirHut_HEADERS}
    ${libKirHut_SOURCES}
)
add_library(KirHut::libKirHut ALIAS libKirHut)
set_target_properties(libKirHut
    PROPERTIES
        OUTPUT_NAME "KirHut"
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

if(MINGW AND BUILD_SHARED_LIBS)
    # This is an ugly hack for MinGW because of a long unresolved bug.
    # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=109380
    target_link_options(libKirHut PRIVATE LINKER:--export-all-symbols)
endif()

set(libKirHut_PUBLIC_LIBRARIES
    $<$<BOOL:${KH_USES_QT}>:Qt${QT_VERSION_MAJOR}::Core>
    $<$<BOOL:${KH_USES_FMT}>:fmt::fmt>
)

message(DEBUG "Public Libs: ${libKirHut_PUBLIC_LIBRARIES}")

set(libKirHut_LINK_LIBRARIES
    $<$<BOOL:${WIN32}>:nowide::nowide>
    KirHut::libKirHutExternal
)

message(DEBUG "Private Libs: ${libKirHut_LINK_LIBRARIES}")

add_library(libKirHutInterface INTERFACE)

target_include_directories(libKirHutInterface INTERFACE "include" "${CMAKE_CURRENT_BINARY_DIR}/include")
target_compile_definitions(libKirHutInterface INTERFACE ${libKirHut_COMPILE_DEFS})
target_link_libraries(libKirHutInterface INTERFACE ${libKirHut_PUBLIC_LIBRARIES})
if(WIN32)
    target_link_directories(libKirHutInterface INTERFACE ${KH_NOWIDE_BINARY_DIR})
endif()

# Do NOT use CMAKE_CXX_STANDARD! This library supports building for newer versions than C++20, and could have a slightly
# different API and ABI in those cases. This allows the library builder to set a C++ version, while also requiring at
# least C++20 support.
target_compile_features(libKirHutInterface INTERFACE cxx_std_20)

if(KH_LIB_BUILD_TESTS)
    add_subdirectory(test)
endif()

target_link_libraries(libKirHut PUBLIC libKirHutInterface)
target_link_libraries(libKirHut PRIVATE ${libKirHut_LINK_LIBRARIES})

string(CONCAT KH_EXPORT_HEADER_ADDON
    "\n"
    "#ifdef KH_COMPILED_WITH_MSVC\n"
    "#  ifdef libKirHut_EXPORTS\n"
    "#    define KH_EXPLICIT_TEMPLATE_EXPORT\n"
    "#    define KH_EXPLICIT_TEMPLATE_INSTANCE KH_EXPORT\n"
    "#  else\n"
    "#    define KH_EXPLICIT_TEMPLATE_EXPORT KH_EXPORT\n"
    "#    define KH_EXPLICIT_TEMPLATE_INSTANCE\n"
    "#  endif\n"
    "#else\n"
    "#  define KH_EXPLICIT_TEMPLATE_EXPORT KH_EXPORT\n"
    "#  define KH_EXPLICIT_TEMPLATE_INSTANCE\n"
    "#endif\n"
)

generate_export_header(libKirHut
    BASE_NAME kh
    EXPORT_FILE_NAME include/kh/export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE KH_EXPORT_HEADER_ADDON
)
