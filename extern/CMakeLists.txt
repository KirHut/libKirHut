## The KirHut Software Development Library
## External dependencies CMake subdirectory file
##
## Copyright © KirHut Software Company

include(CheckCXXSourceCompiles)

set(libKirHut_EXTERN_HEADERS
    args.hpp
    config.hpp
    convert.hpp
    cstdio.hpp
    cstdlib.hpp
    filebuf.hpp
    filesystem.hpp
    fstream.hpp
    iostream.hpp
    quoted.hpp
    replacement.hpp
    stackstring.hpp
    stat.hpp
    utf8_codecvt.hpp
    windows.hpp
    detail/convert.hpp
    detail/is_path.hpp
    detail/is_string_container.hpp
    detail/utf.hpp
    utf/convert.hpp
    utf/utf.hpp
)

list(TRANSFORM libKirHut_EXTERN_HEADERS PREPEND "nowide/")

list(APPEND libKirHut_EXTERN_HEADERS
    BS_thread_pool.hpp
    mio.hpp
    Md51.hpp
    Md51_loc.hpp
    tomlpp.hpp
)

list(TRANSFORM libKirHut_EXTERN_HEADERS PREPEND "include/")

set(libKirHut_EXTERN_SOURCES
    console_buffer.cpp
    cstdio.cpp
    cstdlib.cpp
    filebuf.cpp
    iostream.cpp
    Md51.cpp
    stat.cpp
)

try_compile(NOWIDE_HAS_LFS SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/check_lfs_support.cpp)
try_compile(NOWIDE_HAS_INIT_PRIORITY SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/check_attribute_init_priority.cpp)

add_library(libKirHutExternal OBJECT ${libKirHut_EXTERN_HEADERS} ${libKirHut_EXTERN_SOURCES})
add_library(KirHut::libKirHutExternal ALIAS libKirHutExternal)

target_include_directories(libKirHutExternal PUBLIC "include")
target_compile_definitions(libKirHutExternal PRIVATE
    $<$<BOOL:${NOWIDE_HAS_LFS}>:NOWIDE_HAS_LFS>
    $<$<BOOL:${NOWIDE_HAS_INIT_PRIORITY}>:NOWIDE_HAS_INIT_PRIORITY>
    NOWIDE_NO_LIB
)
target_compile_features(libKirHutExternal PUBLIC cxx_std_20)
set_target_properties(libKirHutExternal PROPERTIES
    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)
