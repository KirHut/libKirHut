## The KirHut Software Development Library
## External dependencies CMake subdirectory file
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

list(TRANSFORM libKirHut_EXTERN_HEADERS PREPEND "include/")

set(libKirHut_EXTERN_SOURCES
    console_buffer.cpp
    cstdio.cpp
    cstdlib.cpp
    filebuf.cpp
    iostream.cpp
    stat.cpp
)

try_compile(NOWIDE_HAS_LFS SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/check_lfs_support.cpp)
try_compile(NOWIDE_HAS_INIT_PRIORITY SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/check_attribute_init_priority.cpp)

add_library(libKirHutExternal OBJECT ${libKirHut_EXTERN_HEADERS} ${libKirHut_EXTERN_SOURCES})
add_library(KirHut::libKirHutExternal ALIAS libKirHutExternal)

target_include_directories(libKirHutExternal PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_definitions(libKirHutExternal PRIVATE
    $<$<BOOL:${NOWIDE_HAS_LFS}>:NOWIDE_HAS_LFS>
    $<$<BOOL:${NOWIDE_HAS_INIT_PRIORITY}>:NOWIDE_HAS_INIT_PRIORITY>
    NOWIDE_NO_LIB
)
target_compile_features(libKirHutExternal PRIVATE cxx_std_20)
set_target_properties(libKirHutExternal PROPERTIES
    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)
