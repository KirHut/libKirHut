## The KirHut Software Development Library
## CMake Test Folder File
##
## Copyright © KirHut Software Company
##
## Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
## the License. You may obtain a copy of the License at
##
##   http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
## an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
## specific language governing permissions and limitations under the License.

cmake_minimum_required(VERSION 3.27)

if(NOT KH_ALLOW_FETCHCONTENT)
    string(APPEND KH_FETCHCONTENT_REQUIRED_MESSAGE
        "The Catch2 library must be custom-built for libKirHut's test bed, because libKirHut uses C++20 features that "
        "are only provided when Catch2 is built from source with them enabled. As such, building the tests for this "
        "library requires allowing the use of FetchContent."
    )
    message(FATAL_ERROR ${KH_FETCHCONTENT_REQUIRED_MESSAGE})
endif()

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG 2b60af89e23d28eefc081bc930831ee9d45ea58b
    FIND_PACKAGE_ARGS 3.8.0
)

FetchContent_MakeAvailable(Catch2)
target_compile_features(Catch2 PUBLIC cxx_std_20)
target_compile_features(Catch2WithMain PUBLIC cxx_std_20)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

set(TESTS_TO_EXECUTE
    Base
    Global
    Invalid
    PackedTuple
    Platform
    Ranges
)

if(NOT KH_NO_EXCEPTIONS)
    list(APPEND TESTS_TO_EXECUTE Errors)
endif()

if(KH_INCLUDE_TERMINAL_PRINT)
    list(APPEND TESTS_TO_EXECUTE Print)
endif()

list(TRANSFORM TESTS_TO_EXECUTE PREPEND Test)
list(TRANSFORM TESTS_TO_EXECUTE APPEND .cpp)

add_executable(KHTestBed ${TESTS_TO_EXECUTE})
target_compile_features(KHTestBed PUBLIC cxx_std_20)
target_link_libraries(KHTestBed
    PUBLIC
        KirHut::${libKirHut_TARGET_NAME}
        KirHut::libKirHutExternal
        Catch2::Catch2WithMain
)

catch_discover_tests(KHTestBed)
