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
    VERSION 0.1.1
    LANGUAGES CXX
    DESCRIPTION "The KirHut Software Company General C++ Development Library"
)

include(CMakeDependentOption)
include(FetchContent)
include(GenerateExportHeader)
include(cmake/kh_compiler_info.cmake)

kh_find_compiler_info()

if(KH_COMPILED_WITH_MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(KH_SUPPORTS_128_TYPES OFF)
elseif(KH_CLANG_GCC_COMPATIBLE)
    cmake_host_system_information(RESULT KH_SUPPORTS_128_TYPES QUERY IS_64BIT)
endif()

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(KH_LIB_BUILD_TESTS_DEFAULT ON)
else()
    set(KH_LIB_BUILD_TESTS_DEFAULT OFF)
endif()

option(KH_MUST_HAVE_16_32_64 "Require that the platform has 16, 32, and 64 bit integers." OFF)
option(KH_MUST_HAVE_32_64 "Require that the platform has 32 and 64 bit integers." ON)
option(KH_MUST_HAVE_16BIT_TYPES "Require that the platform has 16 bit integers." OFF)
option(KH_MUST_HAVE_32BIT_TYPES "Require that the platform has 32 bit integers." OFF)
option(KH_MUST_HAVE_64BIT_TYPES "Require that the platform has 64 bit integers." OFF)

option(KH_USES_QT "Build libKirHut with Qt extensions enabled." ON)
option(KH_USES_FMT "Build libKirHut with {fmt} dependency instead of std::format." ON)
option(KH_LIB_BUILD_TESTS "Build the tests for this library." ${KH_LIB_BUILD_TESTS_DEFAULT})
option(KH_MD5_SUPPORT "Build this library with MD5 Hash support." ${KH_MUST_HAVE_32_64})
option(KH_TOML_SUPPORT "Build this library with TOML parsing support." ON)
option(KH_PERFORM_COMPILE_CHECKS "Have libKirHut perform compile time checking during this build (recommended)." ON)
option(KH_INCLUDE_FILESYSTEM "Include std::filesystem under the KirHut::FS namespace." ON)
option(KH_OVERRIDE_PLATFORM_SAFETY "Ignore unsupported platform checks during compile time checking." OFF)
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
    KH_PERFORM_COMPILE_CHECKS
    KH_INCLUDE_FILESYSTEM
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
    get_target_property(KH_NOWIDE_BINARY_DIR nowide BINARY_DIR)
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
endif()

set(libKirHut_LIBRARY_HEADERS
    args.hpp
    base.hpp
    error.hpp
    errors.hpp
    filesystem.hpp
    global.hpp
    invalid.hpp
    math.hpp
    md5hash.hpp
    platform.hpp
    print.hpp
    ranges.hpp
    toml.hpp
)

list(TRANSFORM libKirHut_LIBRARY_HEADERS PREPEND "kh/")

set(libKirHut_SOURCES
    args.cpp
    base.cpp
    invalid.cpp
    print.cpp
)

if(KH_MD5_SUPPORT)
    list(APPEND libKirHut_SOURCES md5hash.cpp)
endif()

if(KH_TOML_SUPPORT)
    list(APPEND libKirHut_SOURCES toml.cpp)
endif()

set(libKirHut_HEADERS
    ${libKirHut_LIBRARY_HEADERS}
)

list(TRANSFORM libKirHut_SOURCES PREPEND "src/")
list(TRANSFORM libKirHut_HEADERS PREPEND "include/")

add_library(libKirHut
    ${libKirHut_HEADERS}
    ${libKirHut_SOURCES}
)
add_library(KirHut::libKirHut ALIAS libKirHut)
set_target_properties(libKirHut PROPERTIES OUTPUT_NAME "KirHut")

if(MINGW AND BUILD_SHARED_LIBS)
    # This is an ugly hack for MinGW because of a long unresolved bug.
    # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=109380
    target_link_options(libKirHut PRIVATE LINKER:--export-all-symbols)
endif()

set(libKirHut_COMPILE_DEFS
    $<$<BOOL:${KH_USES_QT}>:KH_USES_QT>
    $<$<BOOL:${KH_USES_FMT}>:KH_USES_FMT>
    $<$<BOOL:${KH_MD5_SUPPORT}>:KH_MD5_SUPPORT>
    $<$<BOOL:${KH_USE_128BIT_TYPES}>:KH_USE_128BIT_TYPES>
    $<$<BOOL:${KH_INCLUDE_FILESYSTEM}>:KH_INCLUDE_FILESYSTEM>
    $<$<BOOL:${KH_CLANG_GCC_COMPATIBLE}>:KH_CLANG_GCC_COMPATIBLE>
    ${KH_COMPILED_WITH_NAME}=${${KH_COMPILED_WITH_NAME}}
    KH_COMPILER_DISPLAY_STRING="${KH_COMPILER_DISPLAY_STRING}"
)

message(DEBUG "Compile Defs: ${libKirHut_COMPILE_DEFS}")

add_subdirectory(extern)

set(libKirHut_PUBLIC_LIBRARIES
    $<$<BOOL:${KH_USES_QT}>:Qt${QT_VERSION_MAJOR}::Core>
    fmt::fmt
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

if(KH_PERFORM_COMPILE_CHECKS)
    add_executable(libKirHutCompileCheck "src/ctchecks.cpp")
    add_custom_target(runCompileChecks ALL
        DEPENDS libKirHutCompileCheck
    )

    target_compile_definitions(libKirHutCompileCheck PRIVATE
        $<$<BOOL:${KH_MUST_HAVE_16_32_64}>:KH_MUST_HAVE_16_32_64>
        $<$<BOOL:${KH_MUST_HAVE_32_64}>:KH_MUST_HAVE_32_64>
        $<$<BOOL:${KH_MUST_HAVE_16BIT_TYPES}>:KH_MUST_HAVE_16BIT_TYPES>
        $<$<BOOL:${KH_MUST_HAVE_32BIT_TYPES}>:KH_MUST_HAVE_32BIT_TYPES>
        $<$<BOOL:${KH_MUST_HAVE_64BIT_TYPES}>:KH_MUST_HAVE_64BIT_TYPES>
        $<$<BOOL:${KH_OVERRIDE_PLATFORM_SAFETY}>:KH_OVERRIDE_PLATFORM_SAFETY>
    )

    target_link_libraries(libKirHutCompileCheck PRIVATE libKirHutInterface)
    add_dependencies(libKirHut libKirHutCompileCheck)
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
