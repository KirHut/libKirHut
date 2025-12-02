# The KirHut Application Development Library

This library is designed to provide a variety of useful tools for the purpose of developing KirHut applications,
including those found on embedded platforms, on Desktop PCs, and on mobile devices like iPhone and Android. This
library was written primarily for KirHut's internal use, however it is written with the hope that it will be
useful to others, and is designed to be as flexible as possible for as many different situations as are practical
to support. The library is designed to work best with modern C++, and requires at least C++20, and there are many
enhancements in the library available when using newer versions of C++, like C++23 or C++26.

This library should be used by all KirHut applications, because it provides the base data types, IO systems, and
other functionality that every KirHut application will find useful. This library is not intended to be a replacement
of the standard C++ library, but an enhancement to it, so it is completely dependent on the standard C++ library.
This library also has additional enhancing functionality when built with Qt support, however there are some caveats
to using Qt with this library, mostly to do with memory allocation errors (see oom_exception for more information).

## For KirHut Software Users
This library isn't intended for users of KirHut applications, and there is little of value you will find in this
documentation. Instead, refer to the user guides of the respective KirHut application found at
https://www.kirhut.com/docs you need help with or visit the KirHut help forums to get assistance with the
applications. This library is intended for applications developers and people working on maintaining KirHut
applications, not users of those software products.

## For Application Developers
If you would like to use this library in your own software, remember that this library is licensed under the terms
of the 3-Clause BSD license. The full text of the license is below:

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Using this library in your own project should be as easy as installing the library and including it using
find_package(). The library is almost certainly not available on your package manager, so you will need to install from
source.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
find_package(libKirHut 0.3 REQUIRED)

target_link_libraries(MyProject KirHut::libKirHut)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you prefer building from source, fetching the source and making it available using CMake's FetchContent is also
supported.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FetchContent_Declare(libKirHut
    GIT_REPOSITORY https://github.com/KirHut/libKirHut.git
    GIT_TAG #Insert appropriate version tag here.
)
FetchContent_MakeAvailable(libKirHut)
target_link_libraries(MyProject KirHut::libKirHut)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## For Package Maintainers


## For Library Maintainers
The standards for formatting C++ code in this library are found in the library's .clang-format file. 