/*!
 * \mainpage The KirHut Standard Software Development Library
 *
 * This library is designed to provide a variety of useful tools for the purpose of developing KirHut applications,
 * including those found on embedded platforms, on Desktop PCs, and on mobile devices like iPhone and Android. This
 * library was written primarily for KirHut's internal use, however it is written with the hope that it will be
 * useful to others, and is designed to be as flexible as possible for as many different situations as are practical
 * to support. The library is designed to work with at least C++20, and simply does not support compiling with versions
 * of C++ older than that. There is support for additional features or easier implementations when compiled with newer
 * version of C++ like C++23 or the upcoming C++26, but C++20 is the minimal and required standard.
 *
 * This library should be used by all KirHut applications, because it provides the base data types, IO systems, and
 * other functionality that every KirHut application will find useful. This library is not intended to be a replacement
 * of the standard C++ library, but an enhancement to it, so it is completely dependent on the standard C++ library.
 * This library also has additional enhancing functionality when built with Qt support, however there are some caveats
 * to using Qt with this library, mostly to do with memory allocation errors (see oom_exception for more information).
 *
 * ## For Users
 * This library isn't intended for users of KirHut applications, and there is little of value you will find in this
 * documentation. Instead, refer to the user guides of the respective KirHut application found at
 * https://www.kirhut.com/docs you need help with or visit the KirHut help forums to get assistance with the
 * applications. This library is intended for applications developers and people working on maintaining KirHut
 * applications, not users of those software products.
 *
 * ## For Developers
 * If you would like to use this library in your own software, remember that this library is licensed under the terms
 * of the GNU General Public License, version 3. Any software you develop that relies on this library as a derivative
 * work is required to be licensed under a license that is compatible with GPLv3. **No, the KirHut Software Company
 * will not relicense this library under any other terms, do not ask**.
 *
 * Using this library in your own project should be as easy as a single command in your CMake file, assuming you are
 * using CPM.
 *
 * ~~~
 * CPMAddPackage("gh:zekedragon/libKirHut")
 * target_link_libraries(myProject libKirHut)
 * ~~~
 *
 * You are solely responsible for anything that happens while operating your modified versions of the software, and the
 * KirHut Software Company will be fully indemnified of any liabilities or responsibilities with regards to any damages
 * that may occur due to using modified versions of this software. The KirHut Software Company cannot foresee anything
 * that any user may do with the software, and therefore cannot be held liable for the results of running modified
 * versions.
 *
 * ## For Maintainers
 * The standards for formatting C++ code in this library are found in the library's .clang-format file. 
 */