## The KirHut Software Development Library
## CMake Test Folder File
##
## Copyright © KirHut Software Company

cmake_minimum_required(VERSION 3.27)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG 2b60af89e23d28eefc081bc930831ee9d45ea58b # version 3.8.1
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(Catch2)
target_compile_features(Catch2 PUBLIC cxx_std_20)

include(CTest)

set(TESTS_TO_EXECUTE
    Global
    Base
    Invalid
    Args
)

function(setuptest TESTNAME)
    set(MULTIARGS FILES DIRECTORIES LIBS)
    cmake_parse_arguments(arg "" "" "${MULTIARGS}" ${ARGN})
    add_executable(KHTest${TESTNAME} Test${TESTNAME}.cpp ${arg_FILES})
    add_test(NAME ${TESTNAME} COMMAND KHTest${TESTNAME})
    if(DEFINED arg_DIRECTORIES)
        target_include_directories(KHTest${TESTNAME} PRIVATE ${arg_DIRECTORIES})
    endif()
    target_compile_features(KHTest${TESTNAME} PUBLIC cxx_std_20)
    target_link_libraries(KHTest${TESTNAME} INTERFACE libKirHutInterface)
    target_link_libraries(KHTest${TESTNAME} PRIVATE KirHut::libKirHut Catch2::Catch2WithMain ${arg_LIBS})
endfunction(setuptest)

foreach(TESTNAME ${TESTS_TO_EXECUTE})
    setuptest(${TESTNAME})
endforeach()

if(KH_MD5_SUPPORT)
    setuptest(Md5Hash
        LIBS
            KirHut::libKirHutExternal
    )
endif()
