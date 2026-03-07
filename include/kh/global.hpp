/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/global.hpp
** Copyright © KirHut Software Company
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** conditions found in the BSD 3-Clause License are met.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** You should have received a copy of the BSD 3-Clause license along with this program.  If not, see
** <https://opensource.org/license/bsd-3-clause>.
***********************************************************************************************************************/
#pragma once

/*!
 * \file kh/global.hpp
 *
 * The KirHut Global include file.
 *
 * This file handles all of the most very basic set-up of libKirHut, defining the preprocessor directives and the base
 * types, as well as performing some compilation checks.
 *
 * This file provides all of the compilation environment detection directives that libKirHut has documented here. This
 * file can be included in your project to detect the platform, processor hardware, and build type. This file gives
 * simple, short names for all detections that are unambiguous. All directives within the same group are mutually
 * exclusive, meaning that if one is defined the others are by necessity not. A Windows PC is not a Linux PC.
 *
 * This file also provides base types that are used all over KirHut software applications. These types should be used in
 * *conjunction with* rather than in *preference of* the standard types like `int` and `long`. The `int` type is
 * perfectly fine for situations where the fastest value of at least 16 bits is needed, and so using e16 in those cases
 * wouldn't make sense, but use e16 if you want to prefer 16 bit values if it is equivalently efficient to a 32 bit one.
 *
 * \see Platform
 * \see Instruction_Set_Architecture
 * \see BuildInfo
 * \see TypeRequirements
 */

// The standard headers are actually included **underneath** the preprocessor definitions as a workaround for MinGW
// sucking so hard. MinGW will define _WIN32_WINNT on its own if you #include <cstdint>, which it should never do and
// MSVC doesn't do this at all. To allow this library to define the WINVER and _WIN32_WINNT values (if the builder
// hasn't set them directly already), the standard includes are done after our preprocessor defines are completed.

#include "kh/export.hpp" // IWYU pragma: export
#include "kh/config.hpp" // IWYU pragma: export

/*!
 * \defgroup Platform Operating System Platform
 *
 * The two sets of platform defines below are mutually exclusive, meaning if one of them is defined, it guarantees the
 * rest are not defined.
 * - KH_WINDOWS
 * - KH_LINUX
 * - KH_APPLE
 * - KH_ANDROID
 * - KH_WASM
 * - KH_BSD
 *
 * And the Apple Platform information subset of defines will only be defined if KH_APPLE is also defined:
 * - KH_MACOS
 * - KH_IPHONE
 *
 * There is also some other basic platform information.
 *
 * \{
 */

/*!
 * \def KH_WINDOWS
 *
 * Preprocessor define indicating if the system is compiled on Windows.
 *
 * This is dependent on the `_WIN32` preprocessor definition. This should be defined by the compiler on any Windows
 * platform, both 32 and 64 bit. The value of this define will be equal to the defined value of WINVER, unless that
 * value has not been set by the user, than it will be whatever it is by default in the sdkddkver.h Windows header.
 */

/*!
 * \def KH_LINUX
 *
 * Preprocessor define indicating if the system is compiled on Linux.
 *
 * This is dependent on the `__linux` or `__linux__` preprocessor definitions. This should only be defined on Linux
 * systems that have not been detected as an Android system, as they are treated as completely separate. This is
 * defined on Linux workstations or servers, or embedded systems with a fully effective Linux kernel that are not
 * Android.
 *
 * As kernel versions are not really meaningful or useful at compile time, this value is simply always 1 when defined.
 */

/*!
 * \def KH_APPLE
 *
 * Preprocessor define indicating if the system is compiled for an Apple device.
 *
 * This is dependent on the `__APPLE__` and `__MACH__` preprocessor definitions. These should be defined by the compiler
 * on any Apple device, either Mac OS, iPhone, or iPad.
 */

/*!
 * \def KH_ANDROID
 *
 * Preprocessor define indicating if the system is compiled for an Android device.
 *
 * This is dependent on the `__ANDROID__` or `ANDROID` preprocessor definitions. One of these should be defined by the
 * compiler on any Android device, either 32 or 64 bit.
 */

/*!
 * \def KH_WASM
 *
 * Preprocessor define indicating if the system is compiled using Emscripten for WebAssembly.
 *
 * This is dependent on the `__EMSCRIPTEN__` preprocessor definition, which is defined on the Emscripten compiler used
 * to build Web Assembly binaries.
 */

/*!
 * \def KH_BSD
 *
 * Preprocessor define indicating if the system is compiled on a BSD system.
 *
 * This is dependent on any of the following being defined for determining the operating system:
 * - `__FreeBSD__`
 * - `__DragonFly__`
 * - `__NetBSD__`
 * - `__OpenBSD__`
 *
 * If any of the above are defined, than this is defined.
 */

/*!
 * \def KH_MACOS
 *
 * Preprocessor define indicating if the system is compiled on Mac OS X.
 *
 * This is dependent on Apple's TargetConditionals.h `TARGET_OS_IPHONE` macro. If `TARGET_OS_IPHONE` is not defined or
 * equal to 0, than this is defined. This will also only be defined if KH_APPLE is also defined.
 */

/*!
 * \def KH_IPHONE
 *
 * Preprocessor define indicating if the system is compiled for Apple iPhone or iPad OS.
 *
 * This is dependent on Apple's TargetConditionals.h `TARGET_OS_IPHONE` macro. If `TARGET_OS_IPHONE` is defined, than
 * this is defined. This will also only be defined if KH_APPLE is also defined.
 */

/*!
 * \def KH_DESKTOP
 *
 * Preprocessor define indicating if the system is compiled for a desktop or laptop.
 *
 * This will conventionally be defined if either KH_WINDOWS, KH_LINUX, KH_MACOS, or KH_BSD are defined.
 */

/*!
 * \def KH_MOBILE
 *
 * Preprocessor define indicating if the system is compiled for a mobile device.
 *
 * This will conventionally be defined if either KH_ANDROID or KH_IPHONE are defined.
 */

//! \}

/*!
 * \defgroup Instruction_Set_Architecture Hardware ISA Identification
 *
 * There are several mutually exclusive collections in this group. The primary ISA, some specifiers, etc.:
 * - KH_X64
 * - KH_X32
 * - KH_IA64
 * - KH_ARMv6
 * - KH_ARMv7
 * - KH_ARMv8
 *
 * Many other informational preprocessor defines are also documented in this group.
 *
 * There are many instruction set architectures that are deliberately not supported, mostly due to either being too old
 * to build for or not having an adequate C++20 build toolchain. This library is intended to support embedded devices
 * well, but the nature of what this library requires along with how difficult it is to test support for every platform
 * forces a choice of what hardware platforms will and will not be supported. Here are platforms that will not be:
 * - DEC Alpha
 * - Analog Blackfin
 * - Convex C series
 * - Motorola 68K series
 *
 * More will be added in the future as they are discovered.
 *
 * \{
 */

/*!
 * \def KH_X64
 *
 * Preprocessor define indicating if the system is compiled for 64-bit x86 processors.
 *
 * This is dependent on `__x86_64__` or `_M_X64` being defined by the compiler for determining the processor
 * architecture. If either of those are defined, than this is defined.
 */

/*!
 * \def KH_X32
 *
 * Preprocessor define indicating if the system is compiled for 32-bit x86 processors.
 *
 * This is dependent on any of the following being defined for determining the operating system:
 * - `i386`
 * - `__i386__`
 * - `__i386`
 * - `_M_IX86`
 *
 * If any of the above are defined, and KH_X64 is **not** defined, than this is defined.
 */

/*!
 * \def KH_IA64
 *
 * Preprocessor define indicating if the system is compiled for Intel Itanium processors.
 *
 * This is dependent on any of the following being defined for determining the operating system:
 * - `__ia64`
 * - `__ia64__`
 * - `__IA64__`
 * - `_M_IA64`
 * - `__itanium__`
 *
 * If any of the above are defined, than this is defined.
 */

/*!
 * \def KH_ARMv6
 *
 * Preprocessor define indicating if the system is compiled for ARM v6 processors.
 *
 * This is dependent on the preprocessor define `__ARM_ARCH_6__` being defined or `_M_ARM` being equal to 6.
 */

/*!
 * \def KH_ARMv7
 *
 * Preprocessor define indicating if the system is compiled for ARM v7 processors.
 *
 * This is dependent on the preprocessor define `__ARM_ARCH_7__` being defined or `_M_ARM` being equal to 7.
 */

/*!
 * \def KH_ARMv8
 *
 * Preprocessor define indicating if the system is compiled for ARM v8 processors.
 *
 * This is dependent on the preprocessor defines `__aarch64__` or `_M_ARM64` being defined.
 */

/*!
 * \def KH_ARM32
 *
 * Preprocessor define indicating if the system is compiled for 32-bit ARM processors.
 *
 * This is dependent on QtGlobal's Q_PROCESSOR_ARM_V5, Q_PROCESSOR_ARM_V6, or Q_PROCESSOR_ARM_V7 being true for
 * determining the processor architecture. If any of those are defined, then KH_ARM32 is 1, otherwise it is 0.
 */

/*!
 * \def KH_ARM64
 *
 * Preprocessor define indicating if the system is compiled for 64-bit ARM processors.
 *
 * This is dependent on KH_ARMv8 being defined because the ARM v8 architecture is all 64 bit and all previous versions
 * are 32 bit.
 */

/*!
 * \def KH_X86
 *
 * Preprocessor define indicating if the system is compiled for an x86 processor.
 *
 * This is dependent on KH_X64 or KH_X32 being defined, as there should be a way to determine if you are on either of
 * the x86 processor architectures.
 */

/*!
 * \def KH_RISCV
 *
 * Preprocessor define indicating if the system is compiled for a RISC V processor ("RISC Five").
 *
 * This is dependent on the preprocessor define `__riscv` being defined.
 */

/*!
 * \def KH_RISCV32
 *
 * Preprocessor define indicating if the system is compiled for a RISC V 32-bit processor.
 *
 * This is dependent on the preprocessor define `__riscv` being defined and the define `__riscv_xlen` being equal to 32.
 */

/*!
 * \def KH_RISCV64
 *
 * Preprocessor define indicating if the system is compiled for a RISC V 64-bit processor.
 *
 * This is dependent on the preprocessor define `__riscv` being defined and the define `__riscv_xlen` being equal to 64.
 */

/*!
 * \def KH_RISCV128
 *
 * Preprocessor define indicating if the system is compiled for a RISC V 128-bit processor.
 *
 * This is dependent on the preprocessor define `__riscv` being defined and the define `__riscv_xlen` being equal to
 * 128.
 */

/*!
 * \def KH_128BIT
 *
 * Preprocessor define indicating if the system is compiled for 128-bit systems. Currently, the only hardware platform
 * this will be defined for is RISC-V 128-bit systems.
 */

/*!
 * \def KH_64BIT
 *
 * Preprocessor define indicating if the system is compiled for 64-bit systems.
 */

/*!
 * \def KH_32BIT
 *
 * Preprocessor define indicating if the system is compiled for 32-bit systems.
 */

//! \}

/*!
 * \defgroup BuildFlags Flags Used in the Library Build.
 *
 * This group defines a set of preprocessor flags that are active when certain build options are present, and can be
 * influenced by the user's settings regarding build options. These are the options that should always be present and
 * used to provide build guidance to the compiler, rather than providing information to downstream systems. As such,
 * libraries that want to modify their builds based on this library's build settings can use these flags or the
 * BuildInfo flags for more information.
 *
 * \{
 */

/*!
 * \def KH_EXPORT
 *
 * Preprocessor define to place in front of exported functions and classes in headers.
 *
 * Unlike most of the preprocessor defines, this one is always defined, so it may be safely used in KirHut headers and
 * libraries in all build configurations. The KH_EXPORT macro is supplied by CMake using the generate_export_header
 * function, and is always included in the kh/global.hpp file. Please consult the documentation for CMake's
 * GenerateExportHeader module for more information.
 *
 * On all non-Windows operating systems, KH_EXPORT should just expand to an empty string.
 */

/*!
 * \def KH_NO_EXPORT
 *
 * Preprocessor define to place in front of functions and classes that should have no external linkage in headers.
 *
 * Unlike most of the preprocessor defines, this one is always defined, so it may be safely used in KirHut headers and
 * libraries in all build configurations. The KH_NO_EXPORT macro is supplied by CMake using the generate_export_header
 * function, and is always included in the kh/global.hpp file. Please consult the documentation for CMake's
 * GenerateExportHeader module for more information.
 */

/*!
 * \def KH_DEPRECATED
 *
 * Preprocessor define to place in front of deprecated functions and classes in headers.
 *
 * Unlike most of the preprocessor defines, this one is always defined, so it may be safely used in KirHut headers and
 * libraries in all build configurations. The KH_DEPRECATED macro is supplied by CMake using the generate_export_header
 * function, and is always included in the kh/global.hpp file. Please consult the documentation for CMake's
 * GenerateExportHeader module for more information.
 */

/*!
 * \def KH_DEPRECATED_EXPORT
 *
 * Preprocessor define to place in front of deprecated exported functions and classes in headers.
 *
 * Unlike most of the preprocessor defines, this one is always defined, so it may be safely used in KirHut headers and
 * libraries in all build configurations. The KH_DEPRECATED_EXPORT macro is supplied by CMake using the
 * generate_export_header function, and is always included in the kh/global.hpp file. Please consult the documentation
 * for CMake's GenerateExportHeader module for more information.
 */

/*!
 * \def KH_DEPRECATED_NO_EXPORT
 *
 * Preprocessor define to place in front of deprecated functions and classes that should have no external linkage in
 * headers.
 *
 * Unlike most of the preprocessor defines, this one is always defined, so it may be safely used in KirHut headers and
 * libraries in all build configurations. The KH_DEPRECATED_NO_EXPORT macro is supplied by CMake using the
 * generate_export_header function, and is always included in the kh/global.hpp file. Please consult the documentation
 * for CMake's GenerateExportHeader module for more information.
 */

/*!
 * \def KH_EXPLICIT_TEMPLATE_EXPORT
 *
 * Preprocessor define to place in front of explicit template declarations in a header file.
 *
 * Unlike most of the preprocessor defines, this one is always defined, so it may be safely used in KirHut headers and
 * libraries in all build configurations. The KH_EXPLICIT_TEMPLATE_EXPORT macro is provided by libKirHut directly, but
 * is based on the #KH_EXPORT definition from generate_export_header. Unfortunately, CMake's generate_export_header does
 * not generate appropriate definitions to use explicit templates instances in shared libraries without warnings.
 */

/*!
 * \def KH_EXPLICIT_TEMPLATE_INSTANCE
 *
 * Preprocessor define to place in front of an explicit template definition found in a source file.
 *
 * Unlike most of the preprocessor defines, this one is always defined, so it may be safely used in KirHut headers and
 * libraries in all build configurations. The KH_EXPLICIT_TEMPLATE_INSTANCE macro is provided by libKirHut directly, but
 * is based on the #KH_EXPORT definition from generate_export_header. Unfortunately, CMake's generate_export_header does
 * not generate appropriate definitions to use explicit templates instances in shared libraries without warnings.
 */

/*!
 * \def KH_OVERRIDE_PLATFORM_SAFETY
 *
 * Preprocessor define that communicates if the build is overriding the normal platform safety compile-time checks.
 *
 * This library performs several compile-time checks to prevent building on platforms that are not supported by
 * libKirHut, like checking that the platform has 8 bit bytes, checking that it is building on a supported hardware
 * architecture and OS platform, and that several sanity checks pass (like that integers use two's complement
 * representation and not, for example, sign-magnitude). This is guaranteed to be defined as either 1 or 0, depending on
 * if the user builds with the appropriate CMake option or not. It should always be defined as 0 for basically every
 * build unless the user had some good reason to override it.
 */

/*!
 * \def KH_USES_FMT
 *
 * Preprocessor flag to build libKirHut and applications using libKirHut using {fmt} instead of std::format.
 *
 * This library supports modern C++23-style printing with both {fmt} and std::format, and wraps them in the
 * KirHut::print() functions. By default, libKirHut is always UTF-8 aware and all output operations that aren't raw IO
 * accept the input as UTF-8, even if the output is actually in a different character set. File IO is almost always
 * considered raw binary in libKirHut, but text files are read as UTF-8 documents and output a replacement character for
 * every invalid sequence.
 *
 * The conversion from UTF-8 to the output format is performed by boost::nowide, which is used internally by this
 * library on Windows. Otherwise, UTF-8 is just assumed as a universal standard.
 */

/*!
 * \def KH_NO_EXCEPTIONS
 *
 * Preprocessor flag to build libKirHut without support for throwing any exceptions at all.
 *
 * This will cause all functions and methods to stop throwing exceptions, and they will usually instead either fallback
 * to an alternative or simply crash the application.
 */

/*!
 * \def KH_INCLUDE_TERMINAL_PRINT
 *
 * Preprocessor flag to build libKirHut with the MD5 Hashing object and functions included.
 *
 * The MD5 Hashing functionality of this library is extremely lightweight and there's little reason to want to remove
 * this, but there is also never any real reason you would use this outside of specifically needing to support legacy
 * hashing functions or if you want a kind-of-slow, insecure hashing function to uniquely identify some asset.
 */

//! \}

/*!
 * \defgroup BuildInfo Info On This Library Build
 *
 * This group defines a set of different preprocessor defines with information about how this library was built, such as
 * the version info, the compiler used to build the library, and exposed build settings like debug or release.
 *
 * \{
 */

/*!
 * \def KH_DEBUG
 *
 * Preprocessor define indicating if the library is compiled in debug mode.
 *
 * This is defined if the preprocessor define `NDEBUG` is not defined.
 */

/*!
 * \def KH_RELEASE
 *
 * Preprocessor define indicating if the library is compiled in release mode.
 *
 * This is defined if the preprocessor define `NDEBUG` is defined.
 */

/*!
 * \def KH_CLANG_GNUC_COMPATIBLE
 *
 * Preprocessor define to indicate the compiler used to compile libKirHut is "Clang GNUC compatible."
 *
 * This is defined if this library is built with most compilers that are not MSVC. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant if __GNUC__ is defined. Instead, this information is
 * directly provided by a CMake script in this project.
 *
 * "Clang GNUC" can be thought of as the shittier version of GCC support that Clang provides that justifies defining the
 * __GNUC__ preprocessor macro. It can be useful as a substitute of looking for __GNUC__ directly if you hate that for
 * some reason.
 */

/*!
 * \def KH_COMPILED_WITH_GCC
 *
 * Preprocessor define indicating if the library is compiled using GCC.
 *
 * This is defined if CMake reports that this library is built with GCC. These defines are not based on any preprocessor
 * defines by the compiler itself, so it is irrelevant if __GNUC__ is defined. Instead, this information is directly
 * provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_MSVC
 *
 * Preprocessor define indicating if the library is compiled using MSVC.
 *
 * This is defined if CMake reports that this library is built with MSVC. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant if _MSC_VER is defined. Instead, this information is
 * directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_CLANG
 *
 * Preprocessor define indicating if the library is compiled using Clang.
 *
 * This is defined if CMake reports that this library is built with Clang. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant if __clang__ is defined. Instead, this information
 * is directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_APPLECLANG
 *
 * Preprocessor define indicating if the library is compiled using Apple Clang.
 *
 * This is defined if CMake reports that this library is built with Apple Clang. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant if __clang__ is defined. Instead, this information
 * is directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_ICX
 *
 * Preprocessor define indicating if the library is compiled using the Intel oneAPI DPC++/C++ Compiler.
 *
 * This is defined if CMake reports that this library is built with Intel oneAPI. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant if __INTEL_LLVM_COMPILER is defined. Instead, this
 * information is directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_NVHPC
 *
 * Preprocessor define indicating if the library is compiled using the NVidia HPC SDK C++ Compiler.
 *
 * This is defined if CMake reports that this library is built with NVidia HPC SDK. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant what preprocessor symbols are defined. Instead, this
 * information is directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_NVIDIA
 *
 * Preprocessor define indicating if the library is compiled using the NVidia CUDA C++ Compiler.
 *
 * This is defined if CMake reports that this library is built with NVidia CUDA. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant what preprocessor symbols are defined. Instead, this
 * information is directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_IBMXL
 *
 * Preprocessor define indicating if the library is compiled using the IBM Open XL C++ Compiler.
 *
 * This is defined if CMake reports that this library is built with IBM Open XL. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant what preprocessor symbols are defined. Instead, this
 * information is directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_ARMCLANG
 *
 * Preprocessor define indicating if the library is compiled using the ARM Clang Compiler.
 *
 * This is defined if CMake reports that this library is built with ARM Clang. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant if __clang__ is defined. Instead, this information
 * is directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_CRAY
 *
 * Preprocessor define indicating if the library is compiled using the HPE Cray Clang Compiler.
 *
 * This is defined if CMake reports that this library is built with HPE Cray Clang. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant if __clang__ is defined. Instead, this information
 * is directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_QNX
 *
 * Preprocessor define indicating if the library is compiled using the BlackBerry QNX C/C++ Compiler.
 *
 * This is defined if CMake reports that this library is built with BlackBerry QNX. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant if __QNX__ is defined. Instead, this information is
 * directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_DIAB
 *
 * Preprocessor define indicating if the library is compiled using the Wind River Systems Diab Compiler.
 *
 * This is defined if CMake reports that this library is built with Wind River Diab. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant what preprocessor symbols are defined. Instead, this
 * information is directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_GHS
 *
 * Preprocessor define indicating if the library is compiled using the Green Hills Systems' C++ Compiler.
 *
 * This is defined if CMake reports that this library is built with Green Hills. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant what preprocessor symbols are defined. Instead, this
 * information is directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_MCST
 *
 * Preprocessor define indicating if the library is compiled using the MCST Elbrus C++ Compiler.
 *
 * This is defined if CMake reports that this library is built with Elbrus C++. These defines are not based on any
 * preprocessor defines by the compiler itself, so it is irrelevant what preprocessor symbols are defined. Instead, this
 * information is directly provided by a CMake script in this project.
 */

/*!
 * \def KH_COMPILED_WITH_UNKNOWN
 *
 * Preprocessor define indicating if the library is compiled using an unknown compiler.
 *
 * This is defined if the CMake script used to report the compiler could not determine the compiler based on its CMake
 * compiler ID. The compiler ID is usually one of a fixed set of compilers, found here:
 * https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
 *
 * The only compilers that are C++20 capable or have the potential to be in the future are already detected:
 * - GCC
 * - MSVC
 * - Clang
 * - Apple Clang
 * - ARMClang
 * - Intel oneAPI
 * - IBM Open XL
 * - NVidia HPC SDK
 * - NVidia CUDA
 * - Cray Clang
 * - BlackBerry QNX QCC (future)
 * - Wind River Diab (future)
 * - Green Hills (future)
 * - IAR Embedded Workbench (future)
 * - MCST Elbrus (future)
 *
 * C++ compilers currently detected by CMake but not included in the list have their reasons below:
 * - Analog VisualDSP++: Nowhere close to C++20 support and for hardware architectures unsupported by this library.
 *   https://www.analog.com/en/resources/evaluation-hardware-and-software/software/vdsp-bf-sh-ts.html
 * - ARM C++ Compiler for Embedded: Will never support C++20.
 * - Embarcadero C++ Compiler: Stuck in C++11, doesn't look like it's going to progress.
 * - Fujitsu HPC C++ Compiler: I can't figure out anything about this compiler!
 * - HP C/aC++ Compiler: No longer supported and never got close to C++20.
 * - OrangeC Compiler: Doesn't look like it will ever go beyond C++14.
 * - TIClang: Isn't keeping up with C++ standards, so unless things change, I don't see it.
 * - SCO OpenServer C++ Compiler: It doesn't even support C++98, and will never be updated.
 * - Oracle Developer Studio: Only supports up to C++14, and doesn't look like that will improve.
 * - Renesas C++ Compiler: Still in C++11 despite MISRA supporting C++17. Doesn't look like it will improve.
 * - Tasking Compiler Toolsets: Only seems to support C++03, and doesn't look like it will improve.
 *   https://www.tasking.com/documentation/tricore/ctc/reference/cppcompiler.html
 * - Open Watcom C++ Compiler: It barely supports C++98, and will never update.
 * - PathScale C++ Compiler: Company is defunct, hasn't had an update in over a decade, and isn't C++20.
 * - PGI C++ Compiler: Technically now the NVidia HPC SDK Compiler, so it is supported.
 */

/*!
 * \def KH_COMPILER_DISPLAY_STRING
 *
 * Preprocessor define with the complete "display string" (name and complete version number) of the compiler used to
 * compile libKirHut.
 *
 * This should always be defined, even if #KH_COMPILED_WITH_UNKNOWN is true. This should have a complete compiler info
 * string, including complete version number and build information, as directly provided by the compiler. CMake is
 * responsible for extracting this information and providing it to the library, but libKirHut also simply requires
 * CMake to build at all.
 */

//! \}

/*!
 * \defgroup TypeRequirements Build Requirements for Built-In Types
 *
 * This library allows you to set build requirements for the basic types this libary has, which are documented in this
 * group. Each of the ones that are "higher" in order than the lower ones will take precedence over the lower defines,
 * which means they will be overridden. The hierarchy is relatively simple: any group of types associated with a
 * particular flag that is a complete superset of another flag's group of types is considered a "higher rank" flag and
 * will override anything you set the lower rank flag to. For example: if you set the KH_MUST_HAVE_32_64 flag, then
 * set the KH_MUST_HAVE_32BIT_TYPES flag to 0, it will still require 32 bit types and the rest of your application will
 * still see KH_MUST_HAVE_32BIT_TYPES set to 1.
 *
 * You may directly set any of the type requirements flags in this group when you build with this library. The
 * preferred way to use this functionality is to simply use the available options in this library's CMakeLists.txt file
 * for the application build target, then this will automatically work for all the subsequent flags and you can freely
 * check in your code `#if KH_MUST_HAVE_32BIT_TYPES` before using something that absolutely requires integer rollover or
 * something like that.
 *
 * If the target platform does not conform to the requirements, the application using libKirHut will simply fail to
 * compile.
 *
 * \{
 */

/*!
 * \def KH_MUST_HAVE_16_32_64
 *
 * Preprocessor flag set by user to require the underlying implementation to have 16, 32, and 64 bit types of both
 * signed and unsigned values.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_32_64
 * - KH_MUST_HAVE_16BIT_TYPES
 *
 * As well as any flags that are a consequence of defining those two flags.
 */

/*!
 * \def KH_MUST_HAVE_32_64
 *
 * Preprocessor flag set by user to require the underlying implementation to have 32 and 64 bit types of both signed
 * and unsigned values.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_32BIT_TYPES
 * - KH_MUST_HAVE_64BIT_TYPES
 *
 * As well as any flags that are a consequence of defining those two flags.
 */

/*!
 * \def KH_MUST_HAVE_64BIT_TYPES
 *
 * Preprocessor flag set by user to require the underlying implementation to have 64 bit signed and unsigned types.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<u64>::digits() value is
 * exactly 64. If this is not the case, the library or application will fail to compile.
 */

/*!
 * \def KH_MUST_HAVE_32BIT_TYPES
 *
 * Preprocessor flag set by user to require the underlying implementation to have 32 bit signed and unsigned types.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<u32>::digits() value is
 * exactly 32. If this is not the case, the library or application will fail to compile.
 */

/*!
 * \def KH_MUST_HAVE_16BIT_TYPES
 *
 * Preprocessor flag set by user to require the underlying implementation to have 16 bit signed and unsigned types.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<u16>::digits() value is
 * exactly 16. If this is not the case, the library or application will fail to compile.
 */

//! \}

/*!
 * \def KH_INLINE_NAMESPACE_V1
 *
 * Preprocessor define to begin a v1 inline namespace.
 *
 * This generates `inline namespace v1 {` and little else, unless the library is being built with KH_PRIV_DOCS active,
 * in which case this will just output nothing at all. This is to workaround the problem of Doxygen not ignoring inline
 * namespaces for the purposes of documentation. https://github.com/doxygen/doxygen/issues/5914
 */

/*!
 * \def KH_INLINE_NAMESPACE_V2
 *
 * Preprocessor define to begin a v2 inline namespace.
 *
 * This generates `inline namespace v2 {` and little else, unless the library is being built with KH_PRIV_DOCS active,
 * in which case this will just output nothing at all. This is to workaround the problem of Doxygen not ignoring inline
 * namespaces for the purposes of documentation. https://github.com/doxygen/doxygen/issues/5914
 */

/*!
 * \def KH_INLINE_NAMESPACE_V3
 *
 * Preprocessor define to begin a v3 inline namespace.
 *
 * This generates `inline namespace v3 {` and little else, unless the library is being built with KH_PRIV_DOCS active,
 * in which case this will just output nothing at all. This is to workaround the problem of Doxygen not ignoring inline
 * namespaces for the purposes of documentation. https://github.com/doxygen/doxygen/issues/5914
 */

/*!
 * \def KH_END_INLINE_NAMESPACE
 *
 * Preprocessor define to end an inline namespace.
 *
 * This generates `}` and little else, unless the library is being built with KH_PRIV_DOCS active, in which case this
 * will just output nothing at all. This is to workaround the problem of Doxygen not ignoring inline namespaces for the
 * purposes of documentation. https://github.com/doxygen/doxygen/issues/5914
 */

//* Edit assistant. Qt Creator is slow with this block active, but commenting it out while editing makes editing faster.
//! \cond
#if defined(_WIN32)
// The builder can set _WIN32_WINNT and WINVER to the desired values before compiling, or this library will just use
// whatever default value is decided by the Windows headers.
# include <sdkddkver.h>
# if not defined(KH_WINDOWS)
#  define KH_WINDOWS WINVER
# endif
# if not defined(KH_DESKTOP) and not defined(KH_MOBILE) and not defined(KH_EMBEDDED)
#  define KH_DESKTOP 1
# endif
#elif defined(__APPLE__) or defined(__MACH__)
# include <TargetConditionals.h>
# if not defined(TARGET_OS_MAC)
static_assert(false, "Building for an unsupported Apple OS or OS version.");
# endif
# if not defined(KH_APPLE)
#  define KH_APPLE KH_KERNEL_VERSION
# endif
# if not TARGET_OS_IPHONE
#  if defined(TARGET_OS_DRIVERKIT)
#   if not defined(KH_DESKTOP) and not defined(KH_MOBILE) and not defined(KH_EMBEDDED)
#    define KH_EMBEDDED 1
#   endif
#  else
#   if not defined(KH_MACOS)
#    define KH_MACOS 1
#   endif
#   if not defined(KH_DESKTOP) and not defined(KH_MOBILE) and not defined(KH_EMBEDDED)
#    define KH_DESKTOP 1
#   endif
#  endif // defined(TARGET_OS_DRIVERKIT)
# else
#  if not defined(KH_MOBILE)
#   define KH_MOBILE 1
#  endif
#  if not defined(KH_IPHONE) and not defined(KH_TVOS) and not defined(KH_WATCHOS) and not defined(KH_VISIONOS)
#   if defined(TARGET_OS_IOS)
#    define KH_IPHONE 1
#   elif defined(TARGET_OS_VISION)
#    define KH_VISIONOS 1
#   elif defined(TARGET_OS_TV)
#    define KH_TVOS 1
#   elif defined(TARGET_OS_WATCH)
#    define KH_WATCHOS 1
#   else
static_assert(false, "The iOS hardware platform that this library is being built for is unsupported.");
#   endif
#  endif // not defined(KH_IPHONE) and not defined(KH_TVOS) and not defined(KH_WATCHOS) and not defined(KH_VISIONOS)
# endif // not TARGET_OS_IPHONE
#elif defined(__ANDROID__) or defined(ANDROID)
# if not defined(KH_ANDROID)
#  define KH_ANDROID 1
# endif
# if not defined(KH_DESKTOP) and not defined(KH_MOBILE) and not defined(KH_EMBEDDED)
#  define KH_MOBILE 1
# endif
#elif defined(__EMSCRIPTEN__)
// Emscripten versions prior to 1.3.7 are not supported at all by libKirHut.
# if not defined(KH_WASM)
#  define KH_WASM 1
# endif
#elif defined(__linux) or defined(__linux__)
# if not defined(KH_LINUX)
#  define KH_LINUX 1
# endif
# if not defined(KH_DESKTOP) and not defined(KH_MOBILE) and not defined(KH_EMBEDDED)
// Just take a guess at a desktop if this isn't set by the builder...
#  define KH_DESKTOP 1
# endif
#endif // defined(__linux) or defined(__linux__)

#if defined(__FreeBSD__) or defined(__DragonFly__) or defined(__NetBSD__) or defined(__OpenBSD__)
# undef KH_BSD
# undef KH_DESKTOP
# define KH_BSD 1
# define KH_DESKTOP 1
#endif

#if defined(__sun)
# undef KH_SUN
# undef KH_DESKTOP
# define KH_SUN 1
# define KH_DESKTOP 1
#endif

#if defined(__x86_64__) or defined(_M_X64)
# if not defined(KH_X64)
#  define KH_X64 1
# endif
# if not defined(KH_X86)
#  define KH_X86 1
# endif
# if not defined(KH_32BIT) and not defined(KH_64BIT) and not defined(KH_128BIT)
#  define KH_64BIT 1
# endif
#endif

#if not defined(KH_X64) and (defined(i386) or defined(__i386__) or defined(__i386) or defined(_M_IX86))
# undef KH_X32
# undef KH_X86
# undef KH_32BIT
# define KH_X32 1
# define KH_X86 1
# define KH_32BIT 1
#endif

#if defined(__ia64__) or defined(__ia64) or defined(_M_IA64) or defined(__IA64__) or defined(__itanium__)
# undef KH_IA64
# undef KH_64BIT
# define KH_IA64 1
# define KH_64BIT 1
#endif

#if defined(__ARM_ARCH_6__) or (_M_ARM == 6)
# undef KH_ARMv6
# undef KH_ARM32
# undef KH_32BIT
# define KH_ARMv6 1
# define KH_ARM32 1
# define KH_32BIT 1
#endif

#if defined(__ARM_ARCH_7__) or (_M_ARM == 7)
# undef KH_ARMv7
# undef KH_ARM32
# undef KH_32BIT
# define KH_ARMv7 1
# define KH_ARM32 1
# define KH_32BIT 1
#endif

#if defined(__aarch64__) or defined(_M_ARM64)
# undef KH_ARMv8
# undef KH_ARM64
# undef KH_64BIT
# define KH_ARMv8 1
# define KH_ARM64 1
# define KH_64BIT 1
#endif

#if defined(mips) or defined(__mips__) or defined(__mips)
# undef KH_MIPS
# define KH_MIPS 1
static_assert(KH_OVERRIDE_PLATFORM_SAFETY, "libKirHut currently does not support MIPS architecture builds!");
#endif

#if defined(__sh__)
# undef KH_SUPERH
# undef KH_32BIT
# define KH_SUPERH 1
# define KH_32BIT 1
static_assert(KH_OVERRIDE_PLATFORM_SAFETY, "libKirHut currently does not support Super H architecture builds!");
#endif

#if defined(__PPC64__) or defined(__ppc64__) or defined(_ARCH_PPC64) or defined(__powerpc64__)
# define KH_POWER64 1
# define KH_64BIT 1
static_assert(KH_OVERRIDE_PLATFORM_SAFETY, "libKirHut currently does not support PowerPC architecture builds!");
#endif

#if not KH_POWER64 and (defined(__powerpc) or defined(__powerpc__) or defined(__POWERPC__))
# define KH_POWER32 1
# define KH_32BIT 1
static_assert(KH_OVERRIDE_PLATFORM_SAFETY, "libKirHut currently does not support PowerPC architecture builds!");
#endif

#if not KH_POWER32 and not KH_POWER64 and (defined(__ppc__) or defined(__PPC__) or defined(_ARCH_PPC))
# define KH_POWER32 1
# define KH_32BIT 1
static_assert(KH_OVERRIDE_PLATFORM_SAFETY, "libKirHut currently does not support PowerPC architecture builds!");
#endif

#if defined(__sparc_v9__) or defined(__sparcv9)
# define KH_SPARC64 1
# define KH_64BIT 1
static_assert(KH_OVERRIDE_PLATFORM_SAFETY, "libKirHut currently does not support SPARC architecture builds!");
#endif

#if not defined(KH_SPARC64) and (defined(__sparc__) or defined(__sparc))
# define KH_SPARC32 1
# define KH_32BIT 1
static_assert(KH_OVERRIDE_PLATFORM_SAFETY, "libKirHut currently does not support SPARC architecture builds!");
#endif

#if defined(__riscv)
# undef KH_RISCV
# define KH_RISCV 1
# if defined(__riscv_xlen)
#  if __riscv_xlen == 32
#   define KH_RISCV32 1
#   define KH_32BIT 1
#  elif __riscv_xlen == 64
#   define KH_RISCV64 1
#   define KH_64BIT 1
#  elif __riscv_xlen == 128
#   define KH_RISCV128 1
#   define KH_128BIT 1
static_assert(KH_OVERRIDE_PLATFORM_SAFETY, "libKirHut currently does not support RISC-V 128-bit builds!");
#  endif
# else
// If we've overridden platform safety, just take a guess!
#  define KH_RISCV32 1
#  define KH_32BIT 1
static_assert(KH_OVERRIDE_PLATFORM_SAFETY,
              "libKirHut could not determine the processor bitlength for this RISC-V target.");
# endif
#endif

#if defined(KH_INCLUDE_MD5HASH)
# undef KH_MUST_HAVE_32_64
# define KH_MUST_HAVE_32_64 1
#endif

#if not defined(KH_DEBUG) and not defined(KH_RELEASE) and not defined(NDEBUG)
# define KH_DEBUG 1
#endif

#if not defined(KH_DEBUG) and not defined(KH_RELEASE)
# define KH_RELEASE 1
#endif

#if defined(KH_MUST_HAVE_16_32_64)
# undef KH_MUST_HAVE_32_64
# undef KH_MUST_HAVE_16BIT_TYPES
# define KH_MUST_HAVE_32_64 1
# define KH_MUST_HAVE_16BIT_TYPES 1
#endif

#if defined(KH_MUST_HAVE_32_64)
# undef KH_MUST_HAVE_32BIT_TYPES
# undef KH_MUST_HAVE_64BIT_TYPES
# define KH_MUST_HAVE_32BIT_TYPES 1
# define KH_MUST_HAVE_64BIT_TYPES 1
#endif

#define KH_INLINE_NAMESPACE_V1 \
    inline namespace v1        \
    {
#define KH_INLINE_NAMESPACE_V2 \
    inline namespace v2        \
    {
#define KH_INLINE_NAMESPACE_V3 \
    inline namespace v3        \
    {
#define KH_END_INLINE_NAMESPACE }
//! \endcond

#if defined(KH_PRIV_DOCS)
# define KH_WINDOWS
# define KH_LINUX
# define KH_MACOS
# define KH_APPLE
# define KH_ANDROID
# define KH_WASM
# define KH_BSD
# define KH_IPHONE
# define KH_DESKTOP
# define KH_MOBILE
# define KH_X64
# define KH_X32
# define KH_IA64
# define KH_ARMv6
# define KH_ARMv7
# define KH_ARMv8
# define KH_ARM32
# define KH_ARM64
# define KH_X86
# define KH_RISCV
# define KH_RISCV32
# define KH_RISCV64
# define KH_RISCV128
# define KH_32BIT
# define KH_64BIT
# define KH_128BIT
# define KH_COMPILED_WITH_GCC
# define KH_COMPILED_WITH_MSVC
# define KH_COMPILED_WITH_CLANG
# define KH_COMPILED_WITH_APPLECLANG
# define KH_COMPILED_WITH_ICX
# define KH_COMPILED_WITH_NVHPC
# define KH_COMPILED_WITH_NVIDIA
# define KH_COMPILED_WITH_IBMXL
# define KH_COMPILED_WITH_ARMCLANG
# define KH_COMPILED_WITH_CRAY
# define KH_COMPILED_WITH_QNX
# define KH_COMPILED_WITH_DIAB
# define KH_COMPILED_WITH_GHS
# define KH_COMPILED_WITH_MCST
# define KH_COMPILED_WITH_UNKNOWN
# define KH_CLANG_GNUC_COMPATIBLE
# define KH_EXPORT
# define KH_NO_EXPORT
# define KH_DEPRECATED
# define KH_DEPRECATED_EXPORT
# define KH_DEPRECATED_NO_EXPORT
# define KH_EXPLICIT_TEMPLATE_EXPORT
# define KH_EXPLICIT_TEMPLATE_INSTANCE
# define KH_USES_FMT
# define KH_NO_EXCEPTIONS
# define KH_INLINE_NAMESPACE_V1
# define KH_INLINE_NAMESPACE_V2
# define KH_INLINE_NAMESPACE_V3
# define KH_END_INLINE_NAMESPACE
# define KH_INCLUDE_TERMINAL_PRINT
# define KH_OVERRIDE_PLATFORM_SAFETY
# define KH_DEBUG
# define KH_RELEASE
# define KH_MUST_HAVE_16_32_64
# define KH_MUST_HAVE_32_64
# define KH_MUST_HAVE_64BIT_TYPES
# define KH_MUST_HAVE_32BIT_TYPES
# define KH_MUST_HAVE_16BIT_TYPES
#endif

// */

/*!
 * \def KH_FORCEINLINE
 *
 * Preprocessor define to direct the compiler that a function or method must always be inlined.
 *
 * There are situations with certain compilers (usually MSVC) where an inline function does not inline even when it is
 * demonstrable that inlining the function would result in a runtime speed improvement. When this occurs, you can use
 * this preprocessor define, which is defined to run `__forceinline` on MSVC and `__attribute__((always_inline))` on
 * GCC/Clang, which forces it to inline the appropriate function. This should be the case regardless of the optimization
 * level of the compiled code. On unsupported compilers, this directive expands to nothing.
 *
 * This is always defined after including this header.
 */

#if defined(KH_COMPILED_WITH_MSVC)
# define KH_FORCEINLINE __forceinline
#elif defined(KH_CLANG_GNUC_COMPATIBLE)
# define KH_FORCEINLINE __attribute__((always_inline))
#else
# define KH_FORCEINLINE
#endif

/*!
 * \def KH_EBO_EMPTY_BASES
 *
 * Preprocessor define to fix inheriting from multiple empty base classes not applying EBO to those classes in MSVC.
 *
 * MSVC sucks when it comes to following C++ standards. There was a long standing bug with MSVC's object model with
 * regards to inheriting from multiple empty base objects breaking EBO. This is supposed to work per the C++ standard,
 * however it only works if we use a __declspec(empty_bases) decorator before the name of the class/struct declaration.
 * This should only need to be done in rare conditions, but when you need EBO to work when inheriting from potentially
 * multiple empty base classes, this is a necessity.
 *
 * This is always defined after including this header.
 */
#if defined(KH_COMPILED_WITH_MSVC)
# define KH_EBO_EMPTY_BASES __declspec(empty_bases)
#else
# define KH_EBO_EMPTY_BASES
#endif

/*!
 * \def KH_ATTR_NO_UNIQUE_ADDRESS
 *
 * Preprocessor define to the correct attribute syntax to use no_unique_address for the current compiler.
 *
 * MSVC sucks when it comes to following C++ standards. C++20 added the standard attribute **no_unique_address**,
 * however the MSVC compiler does not have this, and instead requires you to use a namespace enclosed version of this
 * attribute under **msvc::no_unique_address**. This blatantly violates the C++ standards, but we still have to work
 * around it since MSVC support is a must-have target. Other compilers generally follow the C++ standards as they are
 * written.
 *
 * This is always defined after including this header.
 *
 * \hideinitializer
 */

#if __has_cpp_attribute(no_unique_address)
# define KH_ATTR_NO_UNIQUE_ADDRESS no_unique_address
#else
# define KH_ATTR_NO_UNIQUE_ADDRESS msvc::no_unique_address
#endif

/*!
 * \def KH_ATTR_FLATTEN
 *
 * Preprocessor define to the correct attribute syntax to use flatten for the current compiler.
 *
 * The "flatten" directive has not made its way to standard C++, so this attribute is dependent on using a supported
 * compiler. If libKirHut is being compiled on a compiler without a known flatten attribute, this will simply generate
 * "flatten" which should emit a warning and be ignored by all C++ compliant compilers.
 *
 * This is always defined after including this header.
 *
 * \hideinitializer
 */

// Check for __clang__ here as well to prevent clangd from complaining about msvc::flatten when compiling for MSVC.
#if defined(KH_COMPILED_WITH_MSVC) and not defined(__clang__)
# define KH_ATTR_FLATTEN msvc::flatten
#elif defined(KH_CLANG_GNUC_COMPATIBLE)
# define KH_ATTR_FLATTEN gnu::flatten
#else
# define KH_ATTR_FLATTEN flatten
#endif

#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <limits>
#include <bit>

#if defined(KH_COMPILED_WITH_MSVC)
# include <compare> // IWYU pragma: keep
#endif

#if defined(KH_USE_128BIT_TYPES)
# include <cmath>
#endif

namespace KirHut
{

KH_INLINE_NAMESPACE_V1

//! \cond
using std::byte;
using std::size_t;
//! \endcond

/*!
 * The standard 8 bit signed integer type.
 *
 * This type is guaranteed to be at least 8 bits, and for all current supported platforms, is precisely 8 bits.
 */
using i8 = std::int_least8_t;

/*!
 * The standard 16 bit signed integer type.
 *
 * This type is guaranteed to be at least 16 bits.
 */
using i16 = std::int_least16_t;

/*!
 * The standard 32 bit signed integer type.
 *
 * This type is guaranteed to be at least 32 bits, and for all current supported platforms, is precisely 32 bits.
 */
using i32 = std::int_least32_t;

/*!
 * The standard 64 bit signed integer type.
 *
 * This type is guaranteed to be at least 64 bits, and for all current supported platforms, is precisely 64 bits.
 */
using i64 = std::int_least64_t;

/*!
 * A 128-bit integer type available on GCC and Clang only.
 *
 * This type is functional only on the GCC and Clang compilers, so should only be used on software projects that require
 * those specific compilers or in situations where a fallback implementation using 64-bit types is possible. This type
 * uses a fallback implementation on MSVC which is not 128 bits and is instead just an alias for i64.
 *
 * The types and values used with 128 bit types are deliberately always defined, even when there is no support, because
 * this allows using them in if constexpr blocks, as the code must remain legal even if the if constexpr block is never
 * evaluated under those contexts.
 *
 * The type iWidest is defined in terms of this type, but should be used in preference to this if you are just looking
 * for the "widest" type on the platform.
 */
#if KH_PRIV_DOCS or KH_USE_128BIT_TYPES
using i128 = __int128_t;
#else
using i128 = i64;
#endif

/*!
 * The standard maximum bit width signed integer type.
 *
 * This type is guaranteed to be the largest integer type on the platform, and should be at least 64 bits. This type is
 * defined in terms of i128, but i128 is not guaranteed to be 128 bits long, and could be 64 bits with certain compilers
 * or platforms, and so may this type. Unlike the std::intmax_t type, if the platform or compiler supports 128-bit
 * types, this will be 128-bits as well.
 */
using iWidest = i128;

/*!
 * The standard 8 bit unsigned integer type.
 *
 * This type is guaranteed to be at least 8 bits, and for all current supported platforms, is precisely 8 bits.
 */
using u8 = std::uint_least8_t;

/*!
 * The standard 16 bit unsigned integer type.
 *
 * This type is guaranteed to be at least 16 bits.
 */
using u16 = std::uint_least16_t;

/*!
 * The standard 32 bit unsigned integer type.
 *
 * This type is guaranteed to be at least 32 bits, and for all current supported platforms, is precisely 32 bits.
 */
using u32 = std::uint_least32_t;

/*!
 * The standard 64 bit unsigned integer type.
 *
 * This type is guaranteed to be at least 64 bits, and for all current supported platforms, is precisely 64 bits.
 */
using u64 = std::uint_least64_t;

/*!
 * A 128-bit integer type available on GCC and Clang.
 *
 * This type is functional only on the GCC and Clang compilers, so should only be used on software projects that require
 * those specific compilers or in situations where a fallback implementation using 64-bit types is possible. This type
 * uses a fallback implementation on MSVC which is not 128 bits and is instead just an alias for u64.
 *
 * The types and values used with 128 bit types are deliberately always defined, even when there is no support, because
 * this allows using them in if constexpr blocks, as the code must remain legal even if the if constexpr block is never
 * evaluated under those contexts.
 *
 * The type uWidest is defined in terms of this type, but should be used in preference to this if you are just looking
 * for the "widest" type on the platform.
 */
#if KH_PRIV_DOCS or KH_USE_128BIT_TYPES
using u128 = __uint128_t;
#else
using u128 = u64;
#endif

/*!
 * The standard maximum bit width unsigned integer type.
 *
 * This type is guaranteed to be the largest unsigned integer type on the platform, and should be at least 64 bits. This
 * type is defined in terms of u128, but u128 is not guaranteed to be 128 bits long, and could be 64 bits with certain
 * compilers or platforms, and so may this type. Unlike the std::uintmax_t type, if the platform or compiler supports
 * 128-bit types, this will be 128-bits as well.
 */
using uWidest = u128;

/*!
 * The efficient 8 bit signed integer type.
 *
 * This type is guaranteed to be **at least** 8 bits long, not precisely 8 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
using e8 = std::int_fast8_t;
/*!
 * The efficient 16 bit signed integer type.
 *
 * This type is guaranteed to be **at least** 16 bits long, not precisely 16 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
using e16 = std::int_fast16_t;
/*!
 * The efficient 32 bit signed integer type.
 *
 * This type is guaranteed to be **at least** 32 bits long, not precisely 32 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
using e32 = std::int_fast32_t;
/*!
 * The efficient 64 bit signed integer type.
 *
 * This type is guaranteed to be **at least** 64 bits long, not precisely 64 bits long. All supported platforms
 * have a 64 bit integer type, so this is likely to be 64 bits long.
 */
using e64 = std::int_fast64_t;

/*!
 * The efficient 8 bit unsigned integer type.
 *
 * This type is guaranteed to be **at least** 8 bits long, not precisely 8 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
using ue8 = std::uint_fast8_t;
/*!
 * The efficient 16 bit unsigned integer type.
 *
 * This type is guaranteed to be **at least** 16 bits long, not precisely 16 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
using ue16 = std::uint_fast16_t;
/*!
 * The efficient 32 bit unsigned integer type.
 *
 * This type is guaranteed to be **at least** 32 bits long, not precisely 32 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
using ue32 = std::uint_fast32_t;
/*!
 * The efficient 64 bit signed integer type.
 *
 * This type is guaranteed to be **at least** 64 bits long, not precisely 64 bits long. All supported platforms
 * have a 64 bit integer type, so this is likely to be 64 bits long.
 */
using ue64 = std::uint_fast64_t;

/*!
 * KirHut private "Detail" namespace.
 *
 * This namespace, along with the KirHut::*::Detail namespace in each nested namespace, is a private, reserved namespace
 * that should never be used, as it is portions of the library intended to implement functionality within the library
 * and is not stable for use. Any types in this namespace are intentionally left out of the documentation, and anything
 * within this namespace is subject to both ABI and API changes without warning, version number updates, or any other
 * notification whatsoever. If you use these, you are deliberately breaking your own application.
 *
 * That said, nothing should change in such a way that it would break user applications if two different headers of the
 * same minor library version are linked together. So if there is an end-user translation unit that was compiled using
 * a header with this namespace in version 1.1.0, and another library translation unit compiled with 1.1.1 is linked
 * with it, this link is guaranteed to work correctly (both statically and dynamically). If something about this breaks,
 * the minor version number will be incremented. Further, if something about this breaks in a backward-incompatible way
 * (as in, you would need to change your code to accomodate), the major version number will be incremented.
 *
 * This library attempts to maintain ABI compatibility for as long as humanly possible, including over minor version
 * updates and even major version updates. The only reason an incompatible change should be published is because of a
 * security issue, and this will have at least a minor version update to accompany it. There is the possibility of a
 * breaking ABI change being introduced as a bug. If this occurs, the published library will be considered "broken" and
 * it will be revised to match the previous library's ABI. So as a general contract, even if an ABI break occurs, if
 * this ABI break was not previously disclosed, than it is a bug and the library will be fixed to be ABI compatible
 * again.
 *
 * \internal
 * This namespace will have a lot of different objects in it, and each object should be documented properly according to
 * our requirements. The global.hpp file only has two entries for this namespace: IntFinder and staticConstRef. Other
 * objects in this namespace will only appear when the appropriate headers are included, please see the according header
 * or object documentation for which file to include.
 * \endinternal
 *
 * Leave this namespace alone.
 */
namespace Detail
{

/*!
 * \internal
 *
 * A type that is only used by templates to find the appropriate integer type for a given number of bytes.
 *
 * This class exists for the Integer type inside of it, as that is the only public member.
 *
 * \tparam bytes The number of bytes the returned integer must be at least as large as.
 * \tparam isSigned Whether or not the returned integer type should be signed.
 * \tparam isFast Whether or not the returned integer type should be a "fast" type, IE from `std::int_fastXX_t` or not.
 * \tparam mustBeExact Whether or not the returned integer type must be exactly \p bytes size. Defaults to true.
 */
template <size_t bytes, bool isSigned, bool isFast, bool mustBeExact = true>
class IntFinder
{
    /*!
     * \internal
     *
     * A simple std::conditional_t alias that checks if sizeof(Int_T) is the same size or larger than bytes. If it is,
     * it becomes Int_T, and if it isn't, it uses Rest_T instead. Rest_T should usually be another cnd conditional type,
     * unless there is no larger type than the next one.
     *
     * \tparam Int_T The type who's size is being checked if it is large enough for the requested number of \p bytes.
     * \tparam Rest_T
     */
    template <typename Int_T, typename Rest_T>
    using cnd = std::conditional_t<sizeof(Int_T) >= bytes, Int_T, Rest_T>;

    /*!
     * \internal
     *
     * This is a simple alias for all possible unsigned integer sizes on the platform. This will always be the smallest
     * unsigned integer type that fits the number of requested bytes, unless the bytes is larger than the largest
     * unsigned integer type, in which case it will be uWidest.
     */
    using uints = cnd<u8, cnd<u16, cnd<u32, cnd<u64, uWidest>>>>;

    /*!
     * \internal
     *
     * This is a simple alias for all possible signed integer sizes on the platform. This will always be the smallest
     * integer type that fits the number of requested bytes, unless the bytes is larger than the largest signed integer
     * type, in which case it will be iWidest.
     */
    using ints = cnd<i8, cnd<i16, cnd<i32, cnd<i64, iWidest>>>>;

    /*!
     * \internal
     *
     * This is a simple alias for all possible unsigned "fast" integer sizes on the platform. This will always be the
     * smallest unsigned "fast" integer type that fits the number of requested bytes, unless the bytes is larger than
     * the largest unsigned "fast" integer type, in which case it will be ue64.
     */
    using ufints = cnd<ue8, cnd<ue16, cnd<ue32, ue64>>>;

    /*!
     * \internal
     *
     * This is a simple alias for all possible signed "fast" integer sizes on the platform. This will always be the
     * smallest "fast" integer type that fits the number of requested bytes, unless the bytes is larger than the largest
     * signed "fast" integer type, in which case it will be e64.
     */
    using fints = cnd<e8, cnd<e16, cnd<e32, e64>>>;

    /*!
     * \internal
     *
     * Constant evaluated function that checks if bytes is below the maximum amount that can be stored in an integer.
     *
     * The purpose of the IntFinder class is to find an appropriate integer of the smallest size that will fit a given
     * requested number of bytes. Generally speaking, it is assumed that the amount of bytes must be exact. To ensure it
     * is an exact amount of bytes, do a static_assert on the result of this and the amount you want to check it against
     * like so:
     *
     * ~~~
     * static_assert(UInt<T> == sizeof(T), "There is no integer type that is of equivalent size to T.");
     * ~~~
     *
     * This functionality may be integrated into this type in the future.
     *
     * \return If the current IntFinder template's bytes is a valid byte amount or not.
     */
    consteval static bool validByteAmount() noexcept
    {
        if (isSigned and isFast)
        {
            return bytes <= sizeof(e64);
        }
        else if (isFast)
        {
            return bytes <= sizeof(ue64);
        }
        else if (isSigned)
        {
            return bytes <= sizeof(iWidest);
        }
        else
        {
            return bytes <= sizeof(uWidest);
        }
    }

    static_assert(validByteAmount(),
                  "This platform does not have a type large enough to fit the requested number of bytes.");

public:
    /*!
     * \internal
     *
     * Base Integer type that will resolve to the correct Integer size for the given conditions. If isFast is true, this
     * will be one of the "fast" integer types (starting with 'e' or 'ue' in this library). If isSigned is true, this
     * will be a signed integer, otherwise it will be an unsigned integer. In all of those cases, the number of bytes of
     * the returned integer type will always be at least equal to the number of bytes specified in the IntFinder
     * template.
     */
    using Integer = std::
        conditional_t<isFast, std::conditional_t<isSigned, fints, ufints>, std::conditional_t<isSigned, ints, uints>>;

    static_assert(not mustBeExact or sizeof(Integer) == bytes,
                  "This platform does not have a type with an exact number of bytes as requested.");
};

/*!
 * \internal
 *
 * Constant value template for initializing CPO references in anonymous namespaces following N4381 recommendations.
 *
 * Customization Point Objects can be a cause of issues in certain cases when using them instead of a simple function.
 * Since the CPO is deliberately a non-template object, the operator() method, itself, must be templated instead. As
 * such, creating an instance of a CPO in a header would result in violations of the ODR, because each translation unit
 * would instantiate its own copy of the CPO, and they would all be using the same name, giving the linker a bad day. To
 * prevent this, libKirHut uses a template instantiation of the CPO object, which is guaranteed by the language to only
 * generate one instance. This prevents ODR violations without needing to put an extern instance of every CPO inside of
 * a source file.
 */
template <typename T>
constexpr T staticConstRef{};

} // namespace Detail

/*!
 * An universal integer type for all integer sizes.
 *
 * This is a useful template that allows getting the correct integer type, both in size and signedness, for your
 * template's particular use case. The most powerful thing this does is allow selecting an integer size based on the
 * sizeof() another type, which has multiple useful applications. The two bools also allow selecting based on signedness
 * and if you need a fast integer, but these are usually known in advance so you should just use the convenience aliases
 * Int, UInt, FastInt, and UFastInt instead when it is known.
 *
 * The type returned will be the smallest type on the target platform that can fit the number of bytes requested.
 *
 * \tparam bytes Any number of bytes equal to or less than sizeof(conditional_t<isSigned,iWidest,uWidest>).
 * \tparam isSigned Whether or not the resulting type is a signed integer type.
 * \tparam isFast Whether or not the resulting type is from the "fast" integer set.
 * \tparam mustBeExact Whether having a type that is not exactly \p bytes large should be a compile error.
 */
template <size_t bytes = sizeof(int), bool isSigned = true, bool isFast = false, bool mustBeExact = false>
using Integer = Detail::IntFinder<bytes, isSigned, isFast, mustBeExact>::Integer;

/*!
 * A size-based unsigned integer type alias.
 *
 * This is a useful template when you want to use sizeof() to select the size of an integer type instead of needing to
 * explicitly pick one of them. This can be useful in multiple situations involving templates, when you do not know the
 * size of the types or number of bytes when the template is written but it will be determined at compile time.
 *
 * The type returned will be the smallest type on the target platform that can fit the number of bytes requested.
 *
 * \tparam bytes Any number of bytes equal to or less than sizeof(uWidest).
 */
template <size_t bytes = sizeof(unsigned int)>
using UInt = Integer<bytes, false>;

/*!
 * A type-based unsigned integer type alias.
 *
 * This is a useful template when you want to get an integer type that is capable of carrying the entirety of the passed
 * in object's bytes. This can be useful in multiple situations involving templates, when you do not know the size of
 * the types of bytes at the time of writing, but will be determined at compile time. If the size of the type used in
 * this template is larger than any according integer type that the platform supports, this will simply fail to
 * compile.
 *
 * \tparam T Any type that is equal to or smaller than the largest integer on this platform.
 */
template <typename T>
using UIntOf = UInt<sizeof(T)>;

/*!
 * A size-based integer type alias.
 *
 * \copydetails UInt<BYTES>
 */
template <size_t bytes = sizeof(int)>
using Int = Integer<bytes>;

/*!
 * A type-based integer type alias.
 *
 * \copydetails UIntOf<T>
 */
template <typename T>
using IntOf = Int<sizeof(T)>;

/*!
 * A size-based unsigned fast integer type alias.
 *
 * \copydetails UInt<BYTES>
 */
template <size_t bytes = sizeof(unsigned int)>
using UFastInt = Integer<bytes, false, true>;

/*!
 * A type-based unsigned fast integer type alias.
 *
 * \copydetails UIntOf<T>
 */
template <typename T>
using UFastIntOf = UFastInt<sizeof(T)>;

/*!
 * A size-based fast integer type alias.
 *
 * \copydetails UInt<BYTES>
 */
template <size_t bytes = sizeof(int)>
using FastInt = Integer<bytes, true, true>;

/*!
 * A type-based signed fast integer type alias.
 *
 * \copydetails UIntOf<T>
 */
template <typename T>
using FastIntOf = FastInt<sizeof(T)>;

/*!
 * A size-based unsigned integer type alias.
 *
 * This is a useful template when you want to use sizeof() to select the size of an integer type instead of needing to
 * explicitly pick one of them. This can be useful in multiple situations involving templates, when you do not know the
 * size of the types or number of bytes when the template is written but it will be determined at compile time.
 *
 * The type returned will be the smallest type on the target platform that can fit the number of bytes requested.
 *
 * \tparam bytes Any number of bytes equal to or less than sizeof(uWidest).
 */
template <size_t bytes = sizeof(unsigned int)>
using ExactUInt = Integer<bytes, false, false, true>;

/*!
 * A type-based unsigned integer type alias.
 *
 * This is a useful template when you want to get an integer type that is capable of carrying the entirety of the passed
 * in object's bytes. This can be useful in multiple situations involving templates, when you do not know the size of
 * the types of bytes at the time of writing, but will be determined at compile time. If the size of the type used in
 * this template is larger than any according integer type that the platform supports, this will simply fail to
 * compile.
 *
 * \tparam T Any type that is equal to or smaller than the largest integer on this platform.
 */
template <typename T>
using ExactUIntOf = ExactUInt<sizeof(T)>;

/*!
 * A size-based integer type alias.
 *
 * \copydetails ExactUInt<bytes>
 */
template <size_t bytes = sizeof(int)>
using ExactInt = Integer<bytes, true, false, true>;

/*!
 * A type-based integer type alias.
 *
 * \copydetails ExactUIntOf<T>
 */
template <typename T>
using ExactIntOf = Int<sizeof(T)>;

/*!
 *
 */
namespace Constant
{

/*!
 * Basic constant value for Math::log10(2).
 *
 * This constant can be used to derive the maximum number of base 10 digits that can be represented given a number of
 * base 2 digits. Just multiply the number of base 2 digits by this value and you'll have your number of base 10 digits.
 */
[[maybe_unused]] constexpr double log10_2 = 0.30102999566398119521;

/*!
 * An Unknown or Invalid number of bytes to use.
 *
 * There is no actual buffer of bytes or set of data of std::numeric_limits<size_t>::max() size, so this value is used
 * instead to identify an amount that is invalid or otherwise unknown or unknowable. This is the case when a connection
 * has not finished sending data or the data comes from a user source or separate application.
 */
[[maybe_unused]] constexpr size_t amountUnknown = std::numeric_limits<size_t>::max();

/*!
 * The number of bits in a byte.
 *
 * libKirHut is designed in such a way as to effectively require 8 bit bytes on all supported platforms. As such, this
 * should never change on any supported platform, however there is the theoretical possibility that a byte may not be
 * strictly eight bits. Because it is effectively impossible for this not to be the case on supported platforms, this is
 * found in the Constant namespace instead of the Platform namespace.
 *
 * The type used by std::numeric_limits<T> cannot be std::byte because of a limitation where
 * std::numeric_limits<std::byte> is not defined so cannot be used in constexpr expressions.
 */
[[maybe_unused]] constexpr int bitsInByte = std::numeric_limits<unsigned char>::digits;

/*!
 * A byte where all of the bits are set to 1.
 *
 * This is useful when you want a byte that is representative of a "full" byte, or a byte with all of the bits set to 1
 * (or 'on'). This can be both shorter and more descriptive than the alternative used in the initializer.
 *
 * The type used by std::numeric_limits<T> cannot be std::byte because of a limitation where
 * std::numeric_limits<std::byte> is not defined so cannot be used in constexpr expressions.
 */
[[maybe_unused]] constexpr byte fullByte = std::bit_cast<byte>(std::numeric_limits<unsigned char>::max());

/*!
 * A byte where all of the bits are set to 0.
 *
 * This is useful when you want a byte that is representative of an "empty" byte, or a byte with none of the bits set to
 * 1 (or 'on'). This is a bit more descriptive than simply using `byte{0}`.
 */
[[maybe_unused]] constexpr byte emptyByte = byte{ 0 };
}

/*!
 * Enumeration for reasons why a function or command failed or the input was invalid.
 *
 * This base type is used by the Invalid class and it's TaggedInvalid<WHY> subclasses to communicate the reason that a
 * set of code failed to return an expected value. The enumerations are deliberately set up as possible exit codes for
 * the application, so that if the application closes due to an Invalid being returned or thrown with one of these
 * codes, you can simply `return exitCode(invalid.why());` to return the proper exit code from main.
 *
 * libKirHut uses a standard set of exit codes primarily based on sysexits.h and other aspirational exit code
 * standards that are available on the platform. All exit codes are universal across all KirHut applications, so that a
 * single set of exit code documentation will be correct for all operating systems and all KirHut applications. As such,
 * the exit codes of KirHut applications do not fully follow the conventions of any single operating system, but are
 * designed in such a way as to be as close as is feasible for most supported operating systems.
 *
 * You can either simply cast the WhyInvalid to an int to get the exit code, or use the provided exitCode(WhyInvalid)
 * function. The function just performs the cast itself and is constexpr, so there should be no real difference.
 */
enum class WhyInvalid
{
    Success              = 0, //!< 0 - There was no error or failure at all.
    Unknown              = 1, //!< 1 - There was an unknown error or failure.
    IncorrectInput       = 2, //!< 2 - The input provided to the algorithm was invalid.
    FileNotFound         = 3, //!< 3 - A file or folder specified to the algorithm was not found.
    TooManyOpenFiles     = 4, //!< 4 - The process has already opened too many files.
    CouldntOpenFile      = 5, //!< 5 - A file specified to the algorithm could not be opened.
    InvalidHandle        = 6, //!< 6 - The process was provided or has an invalid handle.
    ArenaTrashed         = 7, //!< 7 - An arena buffer used by this algorithm has become invalid.
    OutOfBounds          = 8, //!< 8 - A request for data was made that was out of specified bounds.
    DataRemoved          = 9, //!< 9 - Expected data was removed prior to the algorithm running.
    BadEnvironment       = 10, //!< 10 - An environment variable or requirement was invalid or missing.
    PatternMismatch      = 11, //!< 11 - An expected pattern was not matched against in the process.
    AlreadyInitialized   = 12, //!< 12 - An attempt to initialize something that is already initialized occurred.
    IllegalArgument      = 14, //!< 14 - An illegal argument was passed to the algorithm.
    OutOfMemory          = 15, //!< 15 - This process ran out of memory.
    CurrentDirectory     = 16, //!< 16 - A directory operation cannot be performed because it is the current directory.
    DataUninitialized    = 20, //!< 20 - Data that should be initialized has not yet been initialized.
    InvalidState         = 21, //!< 21 - A state in data is in an invalid state.
    BadCRCResult         = 23, //!< 23 - An essential cyclic redundancy check in the algorithm failed.
    BadHashResult        = 24, //!< 24 - An essential hash function check in the algorithm failed.
    HandleEndOfFile      = 38, //!< 38 - The end of a file provided to the algorithm was unexpectedly reached.
    DiskFullError        = 39, //!< 39 - The disk of a write destination is full.
    ConfigCmdInvalid     = 64, //!< 64 - A command provided in application configuration is incorrectly formatted.
    IncorrectDataFormat  = 65, //!< 65 - Input provided to algorithm is in incorrect format.
    CannotOpenInput      = 66, //!< 66 - Input provided to algorithm cannot be opened.
    UsernameUnknown      = 67, //!< 67 - Username provided as argument or configuration is unknown.
    HostnameUnknown      = 68, //!< 68 - Hostname provided as argument or configuration is unknown.
    ServiceUnavailable   = 69, //!< 69 - An expected system service was not available at time of request.
    SoftwareError        = 70, //!< 70 - A software programming error was encountered in runtime.
    OperatingSystemError = 71, //!< 71 - The operating system failed to provide an essential functionality.
    OSFileMissingError   = 72, //!< 72 - A critical operating system file was missing.
    CannotCreateFile     = 73, //!< 73 - A file this application required could not be created.
    InputOutputError     = 74, //!< 74 - An error was encountered during IO processing.
    TemporaryError       = 75, //!< 75 - A temporary error was encountered.
    ProtocolError        = 76, //!< 76 - An error was encountered with the protocol used in communication.
    PermissionDenied     = 77, //!< 77 - Permission to access a file, folder, or resource was denied.
    ConfigurationError   = 78, //!< 78 - An error was encountered with the configuration of the application.
};

/*!
 * Convert a WhyInvalid type into an integer to return as an exit code from main().
 *
 * You would ordinarily only use this to return from main because this is an exit code function. You can otherwise use
 * the WhyInvalid type itself for basically all use cases. This method only casts the WhyInvalid to an int and returns
 * it, and given that it is constexpr, it will not be any less efficient than casting it yourself.
 *
 * \param why A WhyInvalid type (which is just an integer underneath) to cast to int.
 * \return an int representation of the WhyInvalid exit code.
 */
[[nodiscard]] constexpr int exitCode(WhyInvalid why) noexcept
{
    return static_cast<int>(why);
}

[[nodiscard]] constexpr auto operator<=>(WhyInvalid first, WhyInvalid second)
{
    using Underlying_T = std::underlying_type_t<WhyInvalid>;
    return static_cast<Underlying_T>(first) <=> static_cast<Underlying_T>(second);
}

KH_END_INLINE_NAMESPACE

} // namespace KirHut
