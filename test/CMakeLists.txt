## The KirHut Software Development Library
## CMake Test Folder File
##
## Copyright © KirHut Software Company

cmake_minimum_required(VERSION 3.21)

CPMAddPackage("gh:catchorg/Catch2@3.6.0")

include(CTest)

set(TESTS_TO_EXECUTE
    Global
    Base
    Exceptions
)

if (KH_MD5_SUPPORT)
    list(APPEND TESTS_TO_EXECUTE Md5Hash)
endif()

foreach(TESTNAME ${TESTS_TO_EXECUTE})
    add_executable(KHTest${TESTNAME} Test${TESTNAME}.cpp)
    add_test(NAME ${TESTNAME} COMMAND KHTest${TESTNAME})
    target_include_directories(KHTest${TESTNAME} PRIVATE ${PROJECT_SOURCE_DIR}/extern/include)
    target_link_libraries(KHTest${TESTNAME} PRIVATE libKirHut Catch2::Catch2WithMain)
endforeach()
