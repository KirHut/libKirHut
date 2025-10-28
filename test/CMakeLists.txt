## The KirHut Software Development Library
## CMake Test Folder File
##
## Copyright © KirHut Software Company

cmake_minimum_required(VERSION 3.27)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG 2b60af89e23d28eefc081bc930831ee9d45ea58b
    FIND_PACKAGE_ARGS 3.8.0
)

FetchContent_MakeAvailable(Catch2)
target_compile_features(Catch2 PUBLIC cxx_std_20)
target_compile_features(Catch2WithMain PUBLIC cxx_std_20)
set_target_properties(Catch2
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
set_target_properties(Catch2WithMain
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

set(TESTS_TO_EXECUTE
    Base
    Global
    Invalid
    Math
    PackedTuple
    Platform
    Ranges
)

if(KH_INCLUDE_ARG_PARSER)
    list(APPEND TESTS_TO_EXECUTE Args)
endif()

if(NOT KH_NO_EXCEPTIONS)
    list(APPEND TESTS_TO_EXECUTE Errors)
endif()

if(KH_INCLUDE_MD5HASH)
    list(APPEND TESTS_TO_EXECUTE Md5Hash)
endif()

if(KH_INCLUDE_TERMINAL_PRINT)
    list(APPEND TESTS_TO_EXECUTE Print)
endif()

if(KH_INCLUDE_TASK_SYSTEM)
    list(APPEND TESTS_TO_EXECUTE Task)
endif()

if(KH_INCLUDE_TOML)
    list(APPEND TESTS_TO_EXECUTE Toml)
endif()

list(TRANSFORM TESTS_TO_EXECUTE PREPEND Test)
list(TRANSFORM TESTS_TO_EXECUTE APPEND .cpp)

add_executable(KHTestBed ${TESTS_TO_EXECUTE})
target_compile_features(KHTestBed PUBLIC cxx_std_20)
target_link_libraries(KHTestBed
    PUBLIC
        KirHut::libKirHut
        libKirHutInterface
        KirHut::libKirHutExternal
        Catch2::Catch2WithMain
)

if(CMAKE_PREFIX_PATH)
    set(KH_QT_DL_PATHS "${CMAKE_PREFIX_PATH}/bin")
else()
    set(KH_QT_DL_PATHS)
endif()
catch_discover_tests(KHTestBed
    DL_PATHS
        ${PROJECT_BINARY_DIR}
        ${KH_QT_DL_PATHS}
)
