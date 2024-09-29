## The KirHut Simple Data Verification Application
## CMake Test Folder File
##
## Copyright (C) 2024 KirHut Software Company

cmake_minimum_required(VERSION 3.21)

CPMAddPackage("gh:catchorg/Catch2@3.6.0")

include(CTest)

set(TESTS_TO_EXECUTE
    Global
    Base
    Exceptions
)

foreach(TESTNAME ${TESTS_TO_EXECUTE})
    add_executable(KHTest${TESTNAME} Test${TESTNAME}.cpp)
    add_test(NAME ${TESTNAME} COMMAND KHTest${TESTNAME})
    target_include_directories(KHTest${TESTNAME} PRIVATE ${REQUIRED_INCLUDES} ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(KHTest${TESTNAME} PRIVATE libKirHut Catch2::Catch2WithMain)
endforeach()
