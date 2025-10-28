## The KirHut Software Development Library
## External dependencies CMake subdirectory file
##
## Copyright © KirHut Software Company

set(libKirHut_EXTERN_HEADERS
    BS_thread_pool.hpp
    mio.hpp
    Md51.hpp
    Md51_loc.hpp
    tomlpp.hpp
)

list(TRANSFORM libKirHut_EXTERN_HEADERS PREPEND "include/")

set(libKirHut_EXTERN_SOURCES
    Md51.cpp
)

add_library(libKirHutExternal OBJECT ${libKirHut_EXTERN_HEADERS} ${libKirHut_EXTERN_SOURCES})
add_library(KirHut::libKirHutExternal ALIAS libKirHutExternal)

target_include_directories(libKirHutExternal PUBLIC "include")
target_compile_definitions(libKirHutExternal PRIVATE ${libKirHut_COMPILE_DEFS})
target_compile_features(libKirHutExternal PUBLIC cxx_std_20)
