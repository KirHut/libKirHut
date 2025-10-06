function(kh_set_compiler_info arg_COMPILER_NAME arg_DISPLAY_NAME)
    string(REGEX MATCH "^[0-9]+(\\.[0-9]+)?" var_COMPILER_VERSION_CLEAN "${CMAKE_CXX_COMPILER_VERSION}")
    string(REPLACE "." ";" var_COMPILER_VERSION_LIST "${var_COMPILER_VERSION_CLEAN}")

    list(GET var_COMPILER_VERSION_LIST 0 var_COMPILER_VERSION_MAJOR)
    list(GET var_COMPILER_VERSION_LIST 1 var_COMPILER_VERSION_MINOR)

    string(REGEX REPLACE "^0+" "" var_COMPILER_VERSION_MAJOR "${var_COMPILER_VERSION_MAJOR}")
    string(REGEX REPLACE "^0+" "" var_COMPILER_VERSION_MINOR "${var_COMPILER_VERSION_MINOR}")

    if(var_COMPILER_VERSION_MAJOR STREQUAL "")
        set(var_COMPILER_VERSION_MAJOR 0)
    endif()
    if(var_COMPILER_VERSION_MINOR STREQUAL "")
        set(var_COMPILER_VERSION_MINOR 0)
    endif()

    math(EXPR var_COMPILER_VERSION_NUMBER
        "${var_COMPILER_VERSION_MAJOR} * 100 + ${var_COMPILER_VERSION_MINOR}"
    )

    set(KH_COMPILED_WITH_NAME KH_COMPILED_WITH_${arg_COMPILER_NAME} PARENT_SCOPE)
    set(KH_COMPILED_WITH_${arg_COMPILER_NAME} ${var_COMPILER_VERSION_NUMBER} PARENT_SCOPE)
    set(KH_COMPILER_DISPLAY_STRING "${arg_DISPLAY_NAME} ${CMAKE_CXX_COMPILER_VERSION}" PARENT_SCOPE)
    if(arg_COMPILER_NAME STREQUAL "GCC" OR
       arg_COMPILER_NAME STREQUAL "CLANG" OR
       arg_COMPILER_NAME STREQUAL "APPLECLANG" OR
       arg_COMPILER_NAME STREQUAL "ARMCLANG" OR
       arg_COMPILER_NAME STREQUAL "ICX" OR
       arg_COMPILER_NAME STREQUAL "IBMXL" OR
       arg_COMPILER_NAME STREQUAL "CRAY" OR
       arg_COMPILER_NAME STREQUAL "MCST")
        set(KH_CLANG_GCC_COMPATIBLE YES PARENT_SCOPE)
    endif()
endfunction()

function(kh_find_compiler_info)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        kh_set_compiler_info("GCC" "GNU C++ Compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        kh_set_compiler_info("CLANG" "LLVM Clang Compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        kh_set_compiler_info("APPLECLANG" "Apple Clang Compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        kh_set_compiler_info("MSVC" "Microsoft Visual C++ Compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
        kh_set_compiler_info("ICX" "Intel oneAPI DPC++/C++ Compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "NVHPC")
        kh_set_compiler_info("NVHPC" "NVidia HPC SDK")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "NVIDIA")
        kh_set_compiler_info("NVIDIA" "NVidia CUDA C++ Compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "XLClang")
        kh_set_compiler_info("IBMXL" "IBM Open XL C++ Compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "ARMClang")
        kh_set_compiler_info("ARMCLANG" "ARMClang Compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "CrayClang")
        kh_set_compiler_info("CRAY" "Cray CCE LLVM Compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "QCC")
        kh_set_compiler_info("QNX" "QNX C++ Compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Diab")
        kh_set_compiler_info("DIAB" "Wind River Systems Diab Compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "LCC")
        kh_set_compiler_info("MCST" "MCST Elbrus C++ Compiler")
    else()
        kh_set_compiler_info("UNKNOWN" "Unknown C++ Compiler")
    endif()

    set(KH_COMPILED_WITH_NAME ${KH_COMPILED_WITH_NAME} PARENT_SCOPE)
    set(${KH_COMPILED_WITH_NAME} ${${KH_COMPILED_WITH_NAME}} PARENT_SCOPE)
    set(KH_COMPILER_DISPLAY_STRING "${KH_COMPILER_DISPLAY_STRING}" PARENT_SCOPE)

    if(KH_CLANG_GCC_COMPATIBLE)
        set(KH_CLANG_GCC_COMPATIBLE YES PARENT_SCOPE)
    endif()
endfunction()
