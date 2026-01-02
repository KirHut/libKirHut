## The KirHut Foundational Development Library - A Universally Useful Set of Tools
## CMake Main File
##
## Copyright © KirHut Software Company

cmake_minimum_required(VERSION 3.27)

project(libKirHut
    VERSION 0.5.0
    DESCRIPTION "The KirHut Software Company Foundational C++ Development Library"
    HOMEPAGE_URL "https://www.kirhut.com/libKirHut"
    LANGUAGES CXX
)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KHCompilerInfo)

kh_find_compiler_info()

if(KH_CLANG_GNUC_COMPATIBLE AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(KH_SUPPORTS_128_TYPES ON)
else()
    set(KH_SUPPORTS_128_TYPES OFF)
endif()

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(KH_BUILD_TESTS_DEFAULT ON)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>")
else()
    set(KH_BUILD_TESTS_DEFAULT OFF)
endif()

string(CONCAT KH_MUST_HAVE_16_32_64_STRING
    "Require that the platform has 16, 32, and 64 bit integers "
    "(implies KH_MUST_HAVE_32_64 and KH_MUST_HAVE_16BIT_TYPES)."
)

string(CONCAT KH_MUST_HAVE_32_64_STRING
    "Require that the platform has 32 and 64 bit integers "
    "(implies KH_MUST_HAVE_32BIT_TYPES and KH_MUST_HAVE_64BIT_TYPES)."
)

include(CMakeDependentOption)

option(KH_MUST_HAVE_16_32_64 "${KH_MUST_HAVE_16_32_64_STRING}" OFF)
option(KH_MUST_HAVE_32_64 "${KH_MUST_HAVE_32_64_STRING}" OFF)
option(KH_MUST_HAVE_16BIT_TYPES "Require that the platform has 16 bit integers." OFF)
option(KH_MUST_HAVE_32BIT_TYPES "Require that the platform has 32 bit integers." OFF)
option(KH_MUST_HAVE_64BIT_TYPES "Require that the platform has 64 bit integers." OFF)

option(KH_USES_FMT "Build libKirHut with {fmt} dependency instead of std::format." ON)
option(KH_BUILD_TESTS "Build the tests for this library (requires KH_ALLOW_FETCHCONTENT)." ${KH_BUILD_TESTS_DEFAULT})
option(BUILD_SHARED_LIBS "Build this library (and its dependencies) as shared libraries." ON)
option(KH_ALLOW_FETCHCONTENT "Allow libKirHut to download {fmt} using FetchContent." ON)
option(KH_NO_BADALLOC "Remove support for throwing or catching std::bad_alloc." OFF)
option(KH_NO_EXCEPTIONS "Removes exception support entirely, along with the Error type (implies KH_NO_BADALLOC)." OFF)
option(KH_INCLUDE_TERMINAL_PRINT "Include UTF-8 C++23 style printing support in KirHut::IO." ON)
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
    KH_NO_BADALLOC
    KH_NO_EXCEPTIONS
    KH_INCLUDE_TERMINAL_PRINT
    KH_OVERRIDE_PLATFORM_SAFETY
)

if(KH_ALLOW_FETCHCONTENT)
    include(FetchContent)
endif()

find_package(Doxygen QUIET)
if(Doxygen_FOUND)
    add_subdirectory(docs)
else()
    message(STATUS
        "Doxygen not found on the build host, so the documentation target and options will not be available."
    )
endif()

if(KH_USES_FMT)
    if(KH_ALLOW_FETCHCONTENT)
        set(KH_FMT_PACKAGE_ARGS CONFIG QUIET)
    else()
        set(KH_FMT_PACKAGE_ARGS REQUIRED)
    endif()

    find_package(fmt 9 ${KH_FMT_PACKAGE_ARGS})

    if(NOT fmt_FOUND)
        # When compiling from source, this uses fmt version 11.2, instead of 9, because 9 has fundamental issues with
        # its build system and emits a lot of deprecation warnings.
        FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG 40626af88bd7df9a5fb80be7b25ac85b122d6c21
        )

        FetchContent_MakeAvailable(fmt)
        target_compile_features(fmt PUBLIC cxx_std_20)

        # Install will put the libraries in the appropriate folder with the application, but this is for convenience of
        # the developer when building without installing.
        set_target_properties(fmt
            PROPERTIES
                ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
                LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
                RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
                POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
        )
    endif()
endif()

set(libKirHut_HEADERS
    base.hpp
    error.hpp
    errors.hpp
    global.hpp
    invalid.hpp
    packedtuple.hpp
    platform.hpp
    print.hpp
    ranges.hpp
)

set(libKirHut_PRIV_HEADERS
    format.hpp
)

set(libKirHut_SOURCES
    base.cpp
    invalid.cpp
)

if(KH_INCLUDE_TERMINAL_PRINT)
    list(APPEND libKirHut_SOURCES print.cpp)
endif()

if(APPLE)
    set(KH_KERNEL_VERSION ${CMAKE_SYSTEM_VERSION})
endif()

list(TRANSFORM libKirHut_SOURCES PREPEND "src/")
list(TRANSFORM libKirHut_PRIV_HEADERS PREPEND "priv/")
list(APPEND libKirHut_HEADERS ${libKirHut_PRIV_HEADERS})
list(TRANSFORM libKirHut_HEADERS PREPEND "include/kh/")

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
        POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)

if(MINGW AND BUILD_SHARED_LIBS)
    # This is an ugly hack for MinGW because of a long unresolved bug.
    # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=109380
    target_link_options(libKirHut PRIVATE LINKER:--export-all-symbols)
endif()

set(libKirHut_PUBLIC_LIBRARIES
    $<$<BOOL:${KH_USES_FMT}>:fmt::fmt>
)

add_library(libKirHutInterface INTERFACE)

target_include_directories(libKirHutInterface INTERFACE "include" "${CMAKE_CURRENT_BINARY_DIR}/include")
target_link_libraries(libKirHutInterface INTERFACE ${libKirHut_PUBLIC_LIBRARIES})

# Do NOT use CMAKE_CXX_STANDARD! This library supports building for newer versions than C++20, and could have a slightly
# different API and ABI in those cases. This allows the library builder to set a C++ version, while also requiring at
# least C++20 support.
target_compile_features(libKirHutInterface INTERFACE cxx_std_20)

if(KH_BUILD_TESTS)
    add_subdirectory(test)
endif()

target_link_libraries(libKirHut PUBLIC libKirHutInterface)
target_link_libraries(libKirHut PRIVATE KirHut::libKirHutExternal)

configure_file(cmake/config.hpp.in include/kh/config.hpp @ONLY)

include(GenerateExportHeader)
generate_export_header(libKirHut
    BASE_NAME kh
    EXPORT_FILE_NAME include/kh/export.hpp
)
