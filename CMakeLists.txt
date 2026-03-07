## The KirHut Foundational Development Library - A Universally Useful Set of Tools
## CMake Main File
##
## Copyright © KirHut Software Company

cmake_minimum_required(VERSION 3.27)

# Prevent building in the source tree
if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    message(STATUS "libKirHut prevents in-source builds by default, since this is likely not what you intended.")
    message(STATUS "Use the command `cmake . -B <build-dir>` to set a separate build directory for the library to")
    message(STATUS "build to.")
    message(FATAL_ERROR "In-source builds of libKirHut are prohibited.")
endif()

project(libKirHut
    VERSION 0.6.0
    DESCRIPTION "The KirHut Software Company Foundational C++ Development Library"
    HOMEPAGE_URL "https://www.kirhut.com/libKirHut"
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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
option(KH_MUST_HAVE_32_64 "${KH_MUST_HAVE_32_64_STRING}" ON)
option(KH_MUST_HAVE_16BIT_TYPES "Require that the platform has 16 bit integers." OFF)
option(KH_MUST_HAVE_32BIT_TYPES "Require that the platform has 32 bit integers." OFF)
option(KH_MUST_HAVE_64BIT_TYPES "Require that the platform has 64 bit integers." OFF)

option(KH_USES_FMT "Build libKirHut with {fmt} dependency instead of std::format." ON)
option(KH_BUILD_TESTS "Build the tests for this library (requires KH_ALLOW_FETCHCONTENT)." ${KH_BUILD_TESTS_DEFAULT})
option(BUILD_SHARED_LIBS "Build this library (and its dependencies) as shared libraries." ON)
option(KH_ALLOW_FETCHCONTENT "Allow libKirHut to download dependencies using FetchContent." ON)
option(KH_NO_BADALLOC "Remove support for throwing or catching std::bad_alloc." OFF)
option(KH_NO_EXCEPTIONS "Removes exception support entirely, along with the Error type (implies KH_NO_BADALLOC)." OFF)
option(KH_INCLUDE_TERMINAL_PRINT "Include UTF-8 C++23 style printing support in KirHut::IO." ON)
option(KH_OVERRIDE_PLATFORM_SAFETY "Ignore unsupported platform checks during compile time checking (dangerous)." OFF)
option(CMAKE_EXPORT_PACKAGE_REGISTRY "Register the libKirHut package in the user's package repository." OFF)
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

# Ensure all runtime binaries (executables and DLLs) are placed in a single
# directory per build tree. This simplifies test execution and runtime
# dependency discovery, especially for shared-library builds.
#
# Users or superprojects may override this if they require a different layout.
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

find_package(Doxygen QUIET)
if(Doxygen_FOUND)
    add_subdirectory(docs)
else()
    message(STATUS
        "Doxygen not found on the build host, so libKirHut's documentation target and options will not be available."
    )
endif()

if(KH_USES_FMT)
    if(KH_ALLOW_FETCHCONTENT)
        set(KH_FMT_PACKAGE_ARGS CONFIG QUIET)
    else()
        set(KH_FMT_PACKAGE_ARGS CONFIG REQUIRED)
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

        set_target_properties(fmt
            PROPERTIES
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

set(libKirHut_TARGET_NAME libKirHut)

add_library(${libKirHut_TARGET_NAME}
    ${libKirHut_HEADERS}
    ${libKirHut_SOURCES}
    $<TARGET_OBJECTS:KirHut::libKirHutExternal>
)
add_library(KirHut::${libKirHut_TARGET_NAME} ALIAS ${libKirHut_TARGET_NAME})
set_target_properties(${libKirHut_TARGET_NAME}
    PROPERTIES
        OUTPUT_NAME "KirHut"
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)

if(MINGW AND BUILD_SHARED_LIBS)
    # This is an ugly hack for MinGW because of a long unresolved bug.
    # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=109380
    target_link_options(${libKirHut_TARGET_NAME} PRIVATE LINKER:--export-all-symbols)
endif()

include(GNUInstallDirs)

set(libKirHut_PUBLIC_LIBRARIES
    $<$<BOOL:${KH_USES_FMT}>:fmt::fmt>
)

target_include_directories(${libKirHut_TARGET_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(${libKirHut_TARGET_NAME} PUBLIC ${libKirHut_PUBLIC_LIBRARIES})

# Do NOT use CMAKE_CXX_STANDARD! This library supports building for newer versions than C++20, and could have a slightly
# different API and ABI in those cases. This allows the library builder to set a C++ version, while also requiring at
# least C++20 support.
target_compile_features(${libKirHut_TARGET_NAME} PUBLIC cxx_std_20)

if(KH_BUILD_TESTS)
    add_subdirectory(test)
endif()

target_include_directories(${libKirHut_TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/extern/include")

configure_file(cmake/config.hpp.in include/kh/config.hpp @ONLY)

include(GenerateExportHeader)
generate_export_header(${libKirHut_TARGET_NAME}
    BASE_NAME kh
    EXPORT_FILE_NAME include/kh/export.hpp
)

set(KH_INSTALL_CMAKEDIR
    ${CMAKE_INSTALL_LIBDIR}/cmake/${libKirHut_TARGET_NAME}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/libKirHutConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libKirHutConfig.cmake"
    INSTALL_DESTINATION ${KH_INSTALL_CMAKEDIR}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libKirHutConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

include(InstallRequiredSystemLibraries)

install(TARGETS ${libKirHut_TARGET_NAME}
    EXPORT libKirHutTargets
)

install(DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    "${CMAKE_CURRENT_BINARY_DIR}/include/"
    TYPE INCLUDE
)

install(EXPORT libKirHutTargets
    FILE libKirHutTargets.cmake
    NAMESPACE KirHut::
    DESTINATION ${KH_INSTALL_CMAKEDIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libKirHutConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/libKirHutConfigVersion.cmake"
    DESTINATION ${KH_INSTALL_CMAKEDIR}
)

install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/${libKirHut_TARGET_NAME}"
)

list(POP_BACK CMAKE_MODULE_PATH)
