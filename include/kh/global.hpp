/***********************************************************************************************************************
** The KirHut Library for the Public Benefit
** global.hpp
** Copyright (C) 2024 KirHut Security Company
**
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
** later version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
** warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
** details.
**
** You should have received a copy of the GNU Affero General Public License along with this program.  If not, see
** <http://www.gnu.org/licenses/>.
***********************************************************************************************************************/
#pragma once

/*!
 * \file global.hpp
 *
 * The KirHut Global include file.
 *
 * This file handles all of the most very basic set-up of libKirHut, defining the preprocessor directives and the base
 * types, as well as performing some compilation checks.
 *
 * This file provides all of the compilation environment detection directives that libKirHut has documented here. This
 * file can be included in your project to detect the platform, processor hardware, build type and potentially Qt
 * Version (if QT_VERSION is defined before this file is included). This file gives simple, short names for all
 * detections that are unambiguous. All directives within the same group are mutually exclusive, meaning that if one is
 * defined the others are by necessity not. A Windows PC is not a Linux PC.
 *
 * This file also provides base types that are used all over KirHut software applications. These types should be used in
 * *conjuntion with* rather than in *preference of* the standard types like `int` and `long`. The `int` type is
 * perfectly fine for situations where the fastest value of at least 16 bits is needed, and so using e16 in those cases
 * wouldn't make sense, but use e16 if you want to prefer 16 bit values if it is equivalently efficient to a 32 bit one.
 *
 * \see Platform
 * \see Instruction_Set_Architecture
 * \see BuildInfo
 * \see TypeRequirements
 */

#include <cstdint>
#include <cstddef>
#include <limits>

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
 * Preprocessor define indicating if the system is compiled on Windows.
 *
 * This is dependent on the `_WIN32` preprocessor definition. This should be defined by the compiler on any Windows
 * platform, both 32 and 64 bit.
 *
 * \hideinitializer
 */

/*!
 * \def KH_LINUX
 * Preprocessor define indicating if the system is compiled on Linux.
 *
 * This is dependent on the `__linux` preprocessor definition. This should be defined by the compiler on any Linux
 * kernel platform, both 32 and 64 bit.
 *
 * \hideinitializer
 */

/*!
 * \def KH_APPLE
 * Preprocessor define indicating if the system is compiled for an Apple device.
 *
 * This is dependent on the `__APPLE__` and `__MACH__` preprocessor definitions. These should be defined by the compiler
 * on any Apple device, either Mac OS, iPhone, or iPad.
 *
 * \hideinitializer
 */

/*!
 * \def KH_ANDROID
 * Preprocessor define indicating if the system is compiled for an Android device.
 *
 * This is dependent on the `__ANDROID__` or `ANDROID` preprocessor definitions. One of these should be defined by the
 * compiler on any Android device, either 32 or 64 bit.
 *
 * \hideinitializer
 */

/*!
 * \def KH_WASM
 * Preprocessor define indicating if the system is compiled using Emscripten for WebAssembly.
 *
 * This is dependent on the `__EMSCRIPTEN__` preprocessor definition, which is defined on the Emscripten compiler used
 * to build Web Assembly binaries.
 *
 * \hideinitializer
 */

/*!
 * \def KH_BSD
 * Preprocessor define indicating if the system is compiled on a BSD system.
 *
 * This is dependent on any of the following being defined for determining the operating system:
 * - `__FreeBSD__`
 * - `__DragonFly__`
 * - `__NetBSD__`
 * - `__OpenBSD__`
 *
 * If any of the above are defined, than this is defined.
 *
 * \hideinitializer
 */

/*!
 * \def KH_MACOS
 * Preprocessor define indicating if the system is compiled on Mac OS X.
 *
 * This is dependent on Apple's TargetConditionals.h `TARGET_OS_IPHONE` macro. If `TARGET_OS_IPHONE` is not defined or
 * equal to 0, than this is defined.
 *
 * \hideinitializer
 */

/*!
 * \def KH_IPHONE
 * Preprocessor define indicating if the system is compiled for Apple iPhone or iPad OS.
 *
 * This is dependent on Apple's TargetConditionals.h `TARGET_OS_IPHONE` macro. If `TARGET_OS_IPHONE` is defined, than
 * this is defined.
 *
 * \hideinitializer
 */

/*!
 * \def KH_DESKTOP
 * Preprocessor define indicating if the system is compiled for a desktop or laptop.
 *
 * This will be defined if either KH_WINDOWS, KH_LINUX, or KH_MACOS are defined.
 *
 * \hideinitializer
 */

/*!
 * \def KH_MOBILE
 * Preprocessor define indicating if the system is compiled for a mobile device.
 *
 * This will be defined if either KH_ANDROID or KH_IPHONE are defined.
 *
 * \hideinitializer
 */

//! \}

#if KH_PRIV_DOCS || defined(_WIN32)
#define KH_WINDOWS 1
#endif

#if KH_PRIV_DOCS || defined(__linux)
#define KH_LINUX 1
#endif

#if KH_PRIV_DOCS || defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
#define KH_APPLE 1
#if defined(KH_PRIV_DOCS) || !TARGET_OS_IPHONE
#define KH_MACOS 1
#if defined(KH_PRIV_DOCS)
#define KH_IPHONE 1
#endif
#else
#define KH_IPHONE 1
#endif // !TARGET_OS_IPHONE
#endif // defined(__APPLE__) || defined(__MACH__)

#if KH_PRIV_DOCS || defined(__ANDROID__) || defined(ANDROID)
#define KH_ANDROID 1
#endif

#if KH_PRIV_DOCS || defined(__EMSCRIPTEN__)
#define KH_WASM 1
#endif

#if KH_PRIV_DOCS || defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__) || defined(__OpenBSD__)
#define KH_BSD 1
#endif

#define KH_MOBILE KH_IPHONE || KH_ANDROID
#if !KH_PRIV_DOCS && !KH_MOBILE
#undef KH_MOBILE
#endif

#define KH_DESKTOP KH_WINDOWS || KH_LINUX || KH_MACOS
#if !KH_PRIV_DOCS && !KH_DESKTOP
#undef KH_DESKTOP
#endif

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
 * - KH_MIPS
 * - KH_SUPERH
 * - KH_POWER64
 * - KH_POWER32
 * - KH_SPARC64
 * - KH_SPARC32
 * - KH_M68K
 *
 * If the processor is an ARMv6 processor, one of these should be active:
 * - KH_ARMv6T2
 * - KH_ARMv6J
 * - KH_ARMv6K
 * - KH_ARMv6Z
 * - KH_ARMv6ZK
 *
 * If the processor is an ARMv7 processor, one of these should be active:
 * - KH_ARMv7A
 * - KH_ARMv7S
 * - KH_ARMv7R
 * - KH_ARMv7M
 *
 * Many other informational preprocessor defines are also documented in this group.
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
 *
 * \hideinitializer
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
 *
 * \hideinitializer
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
 *
 * \hideinitializer
 */

/*!
 * \def KH_ARMv6
 *
 * Preprocessor define indicating if the system is compiled for ARM v6 processors.
 *
 * This is dependent on the preprocessor define `__ARM_ARCH_6__` being defined or `_M_ARM` being equal to 6.
 *
 * \hideinitializer
 */

/*!
 * \def KH_ARMv7
 *
 * Preprocessor define indicating if the system is compiled for ARM v7 processors.
 *
 * This is dependent on the preprocessor define `__ARM_ARCH_7__` being defined or `_M_ARM` being equal to 7.
 *
 * \hideinitializer
 */

/*!
 * \def KH_ARMv8
 *
 * Preprocessor define indicating if the system is compiled for ARM v8 processors.
 *
 * This is dependent on the preprocessor defines `__aarch64__` or `_M_ARM64` being defined.
 *
 * \hideinitializer
 */

/*!
 * \def KH_MIPS
 *
 * Preprocessor define indicating if the system is compiled for MIPS processors.
 *
 * This is dependent on the preprocessor defines mips, `__mips` or `__mips__` being defined.
 *
 * \hideinitializer
 */

/*!
 * \def KH_SUPERH
 *
 * Preprocessor define indicating if the system is compiled for Hitachi SuperH processors.
 *
 * This is dependent on the preprocessor define `__sh__` being defined. I've never used a Hitachi SuperH processor and
 * cannot imagine this being useful, but it has a preprocessor define so here it is.
 *
 * \hideinitializer
 */

/*!
 * \def KH_POWER64
 *
 * Preprocessor define indicating if the system is compiled for 64 bit POWER processors.
 *
 * This is dependent on any of the following being defined for determining the operating system:
 * - `__PPC64__`
 * - `__ppc64__`
 * - `__powerpc64__`
 * - `_ARCH_PPC64`
 *
 * If any of the above are defined, than this is defined, otherwise it is not.
 *
 * \hideinitializer
 */

/*!
 * \def KH_POWER32
 *
 * Preprocessor define indicating if the system is compiled for 32 bit POWER processors.
 *
 *
 * This is dependent on any of the following being defined for determining the operating system:
 * - `__PPC__`
 * - `__ppc__`
 * - `__powerpc`
 * - `__powerpc__`
 * - `__POWERPC__`
 * - `_ARCH_PPC`
 *
 * If any of the above are defined, and #KH_POWER64 is **not** defined, than this is defined.
 *
 * \hideinitializer
 */

/*!
 * \def KH_SPARC64
 *
 * Preprocessor define indicating if the system is compiled for 64 bit SPARC processors.
 *
 * This is dependent on the preprocessor defines `__sparcv9` or `__sparc_v9__` being defined.
 *
 * \hideinitializer
 */

/*!
 * \def KH_SPARC32
 *
 * Preprocessor define indicating if the system is compiled for 32 bit SPARC processors.
 *
 * This is dependent on the preprocessor defines `__sparc` or `__sparc__` being defined and #KH_SPARC64 **not** being
 * defined.
 *
 * \hideinitializer
 */

/*!
 * \def KH_M68K
 *
 * Preprocessor define indicating if the library is compiled for Motorola 68000 processors.
 *
 * This is dependent on the preprocessor define `__m68k__` being defined.
 *
 * \hideinitializer
 */

/*!
 * \def KH_ARMv6T2
 *
 * Preprocessor define for builds of this library on ARMv6T2 processors.
 *
 * I have no idea what this shit means. Maybe you do?
 *
 * \hideinitializer
 */

/*!
 * \def KH_ARMv6J
 *
 * Preprocessor define for builds of this library on ARMv6J processors.
 *
 * \copydetails KH_ARMv6T2
 * \hideinitializer
 */

/*!
 * \def KH_ARMv6K
 *
 * Preprocessor define for builds of this library on ARMv6K processors.
 *
 * \copydetails KH_ARMv6T2
 * \hideinitializer
 */

/*!
 * \def KH_ARMv6Z
 *
 * Preprocessor define for builds of this library on ARMv6Z processors.
 *
 * \copydetails KH_ARMv6T2
 * \hideinitializer
 */

/*!
 * \def KH_ARMv6ZK
 *
 * Preprocessor define for builds of this library on ARMv6ZK processors.
 *
 * \copydetails KH_ARMv6T2
 * \hideinitializer
 */

/*!
 * \def KH_ARMv7A
 *
 * Preprocessor define for builds of this library on ARMv7A processors.
 *
 * \copydetails KH_ARMv6T2
 * \hideinitializer
 */

/*!
 * \def KH_ARMv7S
 *
 * Preprocessor define for builds of this library on ARMv7S processors.
 *
 * \copydetails KH_ARMv6T2
 * \hideinitializer
 */


/*!
 * \def KH_ARMv7R
 *
 * Preprocessor define for builds of this library on ARMv7R processors.
 *
 * \copydetails KH_ARMv6T2
 * \hideinitializer
 */


/*!
 * \def KH_ARMv7M
 *
 * Preprocessor define for builds of this library on ARMv7M processors.
 *
 * \copydetails KH_ARMv6T2
 * \hideinitializer
 */

/*!
 * \def KH_ARM32
 *
 * Preprocessor define indicating if the system is compiled for 32-bit ARM processors.
 *
 * This is dependent on QtGlobal's Q_PROCESSOR_ARM_V5, Q_PROCESSOR_ARM_V6, or Q_PROCESSOR_ARM_V7 being true for
 * determining the processor architecture. If any of those are defined, then KH_ARM32 is 1, otherwise it is 0.
 *
 * \hideinitializer
 */

/*!
 * \def KH_ARM64
 *
 * Preprocessor define indicating if the system is compiled for 64-bit ARM processors.
 *
 * This is dependent on KH_ARMv8 being defined because the ARM v8 architecture is all 64 bit and all previous versions
 * are 32 bit.
 *
 * \hideinitializer
 */

/*!
 * \def KH_X86
 *
 * Preprocessor define indicating if the system is compiled for an x86 processor.
 *
 * This is dependent on KH_X64 or KH_X32 being defined, as there should be a way to determine if you are on either of
 * the x86 processor architectures.
 *
 * \hideinitializer
 */

/*!
 * \def KH_64BIT
 *
 * Preprocessor define indicating if the system is compiled for 64-bit systems.
 *
 * \hideinitializer
 */

/*!
 * \def KH_32BIT
 *
 * Preprocessor define indicating if the system is compiled for 32-bit systems.
 *
 * \hideinitializer
 */

//! \}

#if KH_PRIV_DOCS || defined(__x86_64__) || defined(_M_X64)
#define KH_X64 1
#endif

#if KH_PRIV_DOCS || !KH_X64 && (defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86))
#define KH_X32 1
#endif

#if KH_PRIV_DOCS || defined(__ia64__) || defined(__ia64) || defined(_M_IA64) || defined(__IA64__) ||defined(__itanium__)
#define KH_IA64 1
#endif

#if KH_PRIV_DOCS || defined(__ARM_ARCH_6__) || (_M_ARM == 6)
#define KH_ARMv6 1
#endif

#if KH_PRIV_DOCS || defined(__ARM_ARCH_7__) || (_M_ARM == 7)
#define KH_ARMv7 1
#endif

#if KH_PRIV_DOCS || defined(__aarch64__) || defined(_M_ARM64)
#define KH_ARMv8 1
#endif

#if KH_PRIV_DOCS || defined(mips) || defined(__mips__) || defined(__mips)
#define KH_MIPS 1
#endif

#if KH_PRIV_DOCS || defined(__sh__)
#define KH_SUPERH 1
#endif

#if KH_PRIV_DOCS || defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64) || defined(__powerpc64__)
#define KH_POWER64 1
#endif

#if KH_PRIV_DOCS || !KH_POWER64 && (defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__))
#define KH_POWER32 1
#endif

#if KH_PRIV_DOCS || !KH_POWER32 && !KH_POWER64 && (defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC))
#define KH_POWER32 1
#endif

#if KH_PRIV_DOCS || defined(__sparc_v9__) || defined(__sparcv9)
#define KH_SPARC64 1
#endif

#if KH_PRIV_DOCS || !KH_SPARC64 && (defined(__sparc__) || defined(__sparc))
#define KH_SPARC32 1
#endif

#if KH_PRIV_DOCS || defined(__m68k__)
#define KH_M68K 1
#endif

#if KH_PRIV_DOCS || defined(__ARM_ARCH_6T2_)
#define KH_ARMv6T2 1
#endif

#if KH_PRIV_DOCS || defined(__ARM_ARCH_6J__)
#define KH_ARMv6J 1
#endif

#if KH_PRIV_DOCS || defined(__ARM_ARCH_6K__)
#define KH_ARMv6K 1
#endif

#if KH_PRIV_DOCS || defined(__ARM_ARCH_6Z__)
#define KH_ARMv6Z 1
#endif

#if KH_PRIV_DOCS || defined(__ARM_ARCH_6ZK__)
#define KH_ARMv6ZK 1
#endif

#if KH_PRIV_DOCS || defined(__ARM_ARCH_7S__)
#define KH_ARMv7S 1
#endif

#if KH_PRIV_DOCS || defined(__ARM_ARCH_7M__)
#define KH_ARMv7M 1
#endif

#if KH_PRIV_DOCS || defined(__ARM_ARCH_7R__)
#define KH_ARMv7R 1
#endif

#if KH_PRIV_DOCS || defined(__ARM_ARCH_7A__)
#define KH_ARMv7A 1
#endif

#define KH_ARM32 KH_ARMv6 || KH_ARMv7
#if !KH_PRIV_DOCS && !KH_ARM32
#undef KH_ARM32
#endif

#if KH_PRIV_DOCS || defined(KH_ARMv8)
#define KH_ARM64 1
#endif

#define KH_X86 KH_X64 || KH_X32
#if !KH_PRIV_DOCS && !KH_X86
#undef KH_X86
#endif

#define KH_64BIT KH_X64 || KH_ARM64 || KH_POWER64 || KH_SPARC64
#if !KH_PRIV_DOCS && !KH_64BIT
#undef KH_64BIT
#endif

#define KH_32BIT KH_X32 || KH_ARM32 || KH_POWER32 || KH_SPARC32 || KH_SUPERH || KH_M68K
#if !KH_PRIV_DOCS && !KH_32BIT
#undef KH_32BIT
#endif

/*!
 * \defgroup BuildInfo Info On This Library Build
 *
 * This group defines a set of different preprocessor defines with information about how this library was built, such as
 * the version info, the Qt version (if any), and the build type.
 *
 * The Qt versions currently have the major Qt LTS release versions that were supported by the Qt Company when this
 * library was written. This will likely expand and change as time goes on.
 *
 * \{
 */

/*!
 * \def KH_QT5_15
 * Preprocessor define indicating if the system is compiled using Qt 5.15.
 *
 * This determines the version with `QT_VERSION < QT_VERSION_CHECK(6, 2, 0)`. This library assumes you are using one of
 * the major LTS versions of Qt if it is used, so compatibility is based on LTS versions.
 *
 * \hideinitializer
 */

/*!
 * \def KH_QT6_2
 * Preprocessor define indicating if the system is compiled using Qt 6.2.
 *
 * This determines the version with `QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)` and
 * `QT_VERSION < QT_VERSION_CHECK(6, 5, 0)`. This library assumes you are using one of the major LTS versions of Qt if
 * it is used, so compatibility is based on LTS versions.
 *
 * \hideinitializer
 */

/*!
 * \def KH_QT6_5
 * Preprocessor define indicating if the system is compiled using Qt 6.5.
 *
 * This determines the version with `QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)`. When using Qt 6.5, this will be 1,
 * otherwise it is 0.
 *
 * \hideinitializer
 */

/*!
 * \def KH_EXPORT
 * Preprocessor define to place in front of exported functions and classes in headers.
 *
 * Unlike most of the preprocessor defines, this one is always defined, so it may be safely used in KirHut headers and
 * libraries even if there is no Qt dependency at all. On Windows, what this expands to is dependent on two preprocessor
 * defines in the build: KH_LIBRARY_BUILD and KH_USES_QT:
 * | KH_USES_QT  | KH_LIBRARY_BUILD | Expands To              |
 * |-------------|------------------|-------------------------|
 * | Not Defined | Not Defined      | `__declspec(dllimport)` |
 * | Not Defined | Defined          | `__declspec(dllexport)` |
 * | Defined     | Not Defined      | #Q_DECL_IMPORT          |
 * | Defined     | Defined          | #Q_DECL_EXPORT          |
 *
 * This `define` unconditionally expands to `__declspec(dllxyz)` on Windows without Qt because the MingW and Clang
 * documentation both claim to support this syntax, so there's no reason not to use it (similarly to `pragma once`).
 * Further, KirHut always builds on Windows using MSVC, so this should never be an issue in the first place.
 *
 * On all other operating systems, KH_EXPORT simply expands into nothing at all.
 *
 * \hideinitializer
 */

/*!
 * \def KH_LIBRARY_BUILD
 *
 * Preprocessor flag set by the user to notify libKirHut headers that it is undergoing a library build.
 *
 * On Windows, shared library symbols need to explicitly export themselves, and those same symbols need to be explicitly
 * imported from a shared library. This flag tells libKirHut that it is undergoing a build into a shared library.
 *
 * \hideinitializer
 */

/*!
 * \def KH_USES_QT
 *
 * Preprocessor flag to build libKirHut and applications using libKirHut using the Qt extensions.
 *
 * This library supports building for both Qt and non-Qt based applications. Almost everything KirHut makes is a Qt
 * application, however there are exceptions and this library should support builds on non-Qt systems.
 *
 * \hideinitializer
 */

/*!
 * \def KH_DEBUG
 * Preprocessor define indicating if the system is compiled in debug mode.
 *
 * This is defined if the preprocessor define `NDEBUG` is not defined.
 *
 * \hideinitializer
 */

/*!
 * \def KH_RELEASE
 * Preprocessor define indicating if the system is compiled in release mode.
 *
 * This is defined if the preprocessor define `NDEBUG` is defined.
 *
 * \hideinitializer
 */

//! \}

#if KH_PRIV_DOCS || KH_USES_QT
#include <QtGlobal>
#if QT_VERSION > QT_VERSION_CHECK(6,0,0)
#include <QtProcessorDetection>
#endif

#if KH_PRIV_DOCS || QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
#define KH_QT5_15 1
#endif

#if KH_PRIV_DOCS || QT_VERSION >= QT_VERSION_CHECK(6, 2, 0) && QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
#define KH_QT6_2 1
#endif

#if KH_PRIV_DOCS || QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
#define KH_QT6_5 1
#endif

#if defined(KH_LIBRARY_BUILD)
#define KH_EXPORT Q_DECL_EXPORT
#else
#define KH_EXPORT Q_DECL_IMPORT
#endif // defined(KH_LIBRARY_BUILD)

#endif // KH_USES_QT || defined(KH_PRIV_DOCS)

#if !defined(KH_EXPORT)
#if defined(KH_WINDOWS)
// The MingW and Clang documentation both claim to outright support building __declspec(dllexport/import), so on
// Windows builds this library just expects them.
#if defined(KH_LIBRARY_BUILD)
#define KH_EXPORT __declspec(dllexport)
#else
#define KH_EXPORT __declspec(dllimport)
#endif // defined(KH_LIBRARY_BUILD)
#else
#define KH_EXPORT
#endif // defined(KH_WINDOWS)
#endif // !defined(KH_EXPORT)

#if KH_PRIV_DOCS || !defined(KH_DEBUG) && !defined(KH_RELEASE) && !defined(NDEBUG)
#define KH_DEBUG 1
#endif

#if KH_PRIV_DOCS || !defined(KH_DEBUG) && !defined(KH_RELEASE)
#define KH_RELEASE 1
#endif

/*!
 * \defgroup TypeRequirements Build Requirements for Built-In Types
 *
 * This library allows you to set build requirements as a build flag of the global.hpp file, which are documented in
 * this group. Each of the ones that are "higher" in order than the lower ones will take precedence over the lower
 * defines, which means they will be overridden. The hierarchy is relatively simple: any group of types associated with
 * a particular flag that is a complete superset of another flag's group of types is considered a "higher rank" flag and
 * will override anything you set the lower rank flag to. For example: if you set the KH_MUST_HAVE_8_32_64 flag, then
 * set the KH_MUST_HAVE_8BIT_TYPES flag to 0, it will still require 8 bit types and the rest of your application will
 * still see KH_MUST_HAVE_8BIT_TYPES set to 1.
 *
 * You may directly set any of the type requirements flags in this group when you build with this library. The
 * preferred way to use this functionality is to add the necessary KH_MUST_HAVE_* flag(s) in your CMakeLists.txt file
 * for the application build target, then this will automatically work for all the subsequent flags and you can freely
 * check in your code `#if KH_MUST_HAVE_U32_TYPE` before using something that absolutely requires integer rollover or
 * something like that.
 *
 * If the target platform does not conform to the requirements, the application using libKirHut will simply fail to
 * compile.
 *
 * \{
 */

/*!
 * \def KH_MUST_HAVE_8_16_32_64
 *
 * Preprocessor flag set by user to require the underlying implementation to have 8, 16, 32, and 64 bit types of both
 * signed and unsigned values.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_8_32_64
 * - KH_MUST_HAVE_16BIT_TYPES
 *
 * As well as any flags that are a consequence of defining those two flags.
 */

/*!
 * \def KH_MUST_HAVE_I8_16_32_64
 *
 * Preprocessor flag set by user to require the underlying implementation to have 8, 16, 32, and 64 bit signed types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I8_32_64
 * - KH_MUST_HAVE_I16_TYPE
 *
 * As well as any flags that are a consequence of defining those two flags.
 */

/*!
 * \def KH_MUST_HAVE_U8_16_32_64
 *
 * Preprocessor flag set by user to require the underlying implementation to have 8, 16, 32, and 64 bit unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_U8_32_64
 * - KH_MUST_HAVE_U16_TYPE
 *
 * As well as any flags that are a consequence of defining those two flags.
 */

/*!
 * \def KH_MUST_HAVE_8_32_64
 *
 * Preprocessor flag set by user to require the underlying implementation to have 8, 32, and 64 bit types of both signed
 * and unsigned values.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_8_32
 * - KH_MUST_HAVE_64BIT_TYPES
 *
 * As well as any flags that are a consequence of defining those two flags.
 */

/*!
 * \def KH_MUST_HAVE_I8_32_64
 *
 * Preprocessor flag set by user to require the underlying implementation to have 8, 32, and 64 bit signed types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I8_32
 * - KH_MUST_HAVE_I64_TYPE
 *
 * As well as any flags that are a consequence of defining those two flags.
 */

/*!
 * \def KH_MUST_HAVE_U8_32_64
 *
 * Preprocessor flag set by user to require the underlying implementation to have 8, 32, and 64 bit unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_U8_32
 * - KH_MUST_HAVE_U64_TYPE
 *
 * As well as any flags that are a consequence of defining those two flags.
 */

/*!
 * \def KH_MUST_HAVE_8_32
 *
 * Preprocessor flag set by user to require the underlying implementation to have 8 and 32 bit types of both signed and
 * unsigned values.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_8BIT_TYPES
 * - KH_MUST_HAVE_32BIT_TYPES
 *
 * As well as any flags that are a consequence of defining those two flags.
 */

/*!
 * \def KH_MUST_HAVE_I8_32
 *
 * Preprocessor flag set by user to require the underlying implementation to have 8 and 32 bit signed types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I8_TYPE
 * - KH_MUST_HAVE_I32_TYPE
 */

/*!
 * \def KH_MUST_HAVE_U8_32
 * Preprocessor flag set by user to require the underlying implementation to have 8 and 32 bit unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_U8_TYPE
 * - KH_MUST_HAVE_U32_TYPE
 */

/*!
 * \def KH_MUST_HAVE_64BIT_TYPES
 *
 * Preprocessor flag set by user to require the underlying implementation to have 64 bit signed and unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I64_TYPE
 * - KH_MUST_HAVE_U64_TYPE
 */

/*!
 * \def KH_MUST_HAVE_I64_TYPE
 *
 * Preprocessor flag set by user to require the underlying implementation to have a 64 bit signed integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<i64>::digits() value is
 * exactly 63. If this is not the case, the library or application will fail to compile.
 */

/*!
 * \def KH_MUST_HAVE_U64_TYPE
 *
 * Preprocessor flag set by user to require the underlying implementation to have a 64 bit unsigned integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<u64>::digits() value is
 * exactly 64. If this is not the case, the library or application will fail to compile.
 */

/*!
 * \def KH_MUST_HAVE_32BIT_TYPES
 *
 * Preprocessor flag set by user to require the underlying implementation to have 32 bit signed and unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I32_TYPE
 * - KH_MUST_HAVE_U32_TYPE
 */

/*!
 * \def KH_MUST_HAVE_I32_TYPE
 *
 * Preprocessor flag set by user to require the underlying implementation to have a 32 bit signed integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<i32>::digits() value is
 * exactly 31. If this is not the case, the library or application will fail to compile.
 */

/*!
 * \def KH_MUST_HAVE_U32_TYPE
 * Preprocessor flag set by user to require the underlying implementation to have a 32 bit unsigned integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<u32>::digits() value is
 * exactly 32. If this is not the case, the library or application will fail to compile.
 */

/*!
 * \def KH_MUST_HAVE_16BIT_TYPES
 *
 * Preprocessor flag set by user to require the underlying implementation to have 16 bit signed and unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I16_TYPE
 * - KH_MUST_HAVE_U16_TYPE
 */

/*!
 * \def KH_MUST_HAVE_I16_TYPE
 *
 * Preprocessor flag set by user to require the underlying implementation to have a 16 bit signed integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<i16>::digits() value is
 * exactly 15. If this is not the case, the library or application will fail to compile.
 */

/*!
 * \def KH_MUST_HAVE_U16_TYPE
 *
 * Preprocessor flag set by user to require the underlying implementation to have a 16 bit unsigned integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<u16>::digits() value is
 * exactly 16. If this is not the case, the library or application will fail to compile.
 */

/*!
 * \def KH_MUST_HAVE_8BIT_TYPES
 *
 * Preprocessor flag set by user to require the underlying implementation to have 8 bit signed and unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I8_TYPE
 * - KH_MUST_HAVE_U8_TYPE
 */

/*!
 * \def KH_MUST_HAVE_I8_TYPE
 *
 * Preprocessor flag set by user to require the underlying implementation to have an 8 bit signed integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<i8>::digits() value is
 * exactly 7. If this is not the case, the library or application will fail to compile.
 */

/*!
 * \def KH_MUST_HAVE_U8_TYPE
 *
 * Preprocessor flag set by user to require the underlying implementation to have an 8 bit unsigned integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<u8>::digits() value is
 * exactly 8. If this is not the case, the library or application will fail to compile.
 */

//! \}

#if KH_PRIV_DOCS || defined(KH_MUST_HAVE_8_16_32_64)
#undef KH_MUST_HAVE_8_16_32_64
#undef KH_MUST_HAVE_8_32_64
#undef KH_MUST_HAVE_16BIT_TYPES
#define KH_MUST_HAVE_8_16_32_64 1
#define KH_MUST_HAVE_8_32_64 1
#define KH_MUST_HAVE_16BIT_TYPES 1
#endif

#if KH_PRIV_DOCS || defined(KH_MUST_HAVE_I8_16_32_64)
#undef KH_MUST_HAVE_I8_16_32_64
#undef KH_MUST_HAVE_I8_32_64
#undef KH_MUST_HAVE_I16_TYPE
#define KH_MUST_HAVE_I8_16_32_64 1
#define KH_MUST_HAVE_I8_32_64 1
#define KH_MUST_HAVE_I16_TYPE 1
#endif

#if KH_PRIV_DOCS || defined(KH_MUST_HAVE_U8_16_32_64)
#undef KH_MUST_HAVE_U8_16_32_64
#undef KH_MUST_HAVE_U8_32_64
#undef KH_MUST_HAVE_U16_TYPE
#define KH_MUST_HAVE_U8_16_32_64 1
#define KH_MUST_HAVE_U8_32_64 1
#define KH_MUST_HAVE_U16_TYPE 1
#endif

#if defined(KH_MUST_HAVE_8_32_64)
#undef KH_MUST_HAVE_8_32_64
#undef KH_MUST_HAVE_8_32
#undef KH_MUST_HAVE_64BIT_TYPES
#define KH_MUST_HAVE_8_32_64 1
#define KH_MUST_HAVE_8_32 1
#define KH_MUST_HAVE_64BIT_TYPES 1
#endif

#if defined(KH_MUST_HAVE_I8_32_64)
#undef KH_MUST_HAVE_I8_32_64
#undef KH_MUST_HAVE_I8_32
#undef KH_MUST_HAVE_I64_TYPE
#define KH_MUST_HAVE_I8_32_64 1
#define KH_MUST_HAVE_I8_32 1
#define KH_MUST_HAVE_I64_TYPE 1
#endif

#if defined(KH_MUST_HAVE_U8_32_64)
#undef KH_MUST_HAVE_U8_32_64
#undef KH_MUST_HAVE_U8_32
#undef KH_MUST_HAVE_U64_TYPE
#define KH_MUST_HAVE_U8_32_64 1
#define KH_MUST_HAVE_U8_32 1
#define KH_MUST_HAVE_U64_TYPE 1
#endif

#if KH_PRIV_DOCS || defined(KH_MUST_HAVE_8_32)
#undef KH_MUST_HAVE_8_32
#undef KH_MUST_HAVE_8BIT_TYPES
#undef KH_MUST_HAVE_32BIT_TYPES
#define KH_MUST_HAVE_8_32 1
#define KH_MUST_HAVE_8BIT_TYPES 1
#define KH_MUST_HAVE_32BIT_TYPES 1
#endif

#if KH_PRIV_DOCS || defined(KH_MUST_HAVE_I8_32)
#undef KH_MUST_HAVE_I8_32
#undef KH_MUST_HAVE_I8_TYPE
#undef KH_MUST_HAVE_I32_TYPE
#define KH_MUST_HAVE_I8_32 1
#define KH_MUST_HAVE_I8_TYPE 1
#define KH_MUST_HAVE_I32_TYPE 1
#endif

#if KH_PRIV_DOCS || defined(KH_MUST_HAVE_U8_32)
#undef KH_MUST_HAVE_U8_32
#undef KH_MUST_HAVE_U8_TYPE
#undef KH_MUST_HAVE_U32_TYPE
#define KH_MUST_HAVE_U8_32 1
#define KH_MUST_HAVE_U8_TYPE 1
#define KH_MUST_HAVE_U32_TYPE 1
#endif

#if KH_PRIV_DOCS || defined(KH_MUST_HAVE_64BIT_TYPES)
#undef KH_MUST_HAVE_64BIT_TYPES
#undef KH_MUST_HAVE_I64_TYPE
#undef KH_MUST_HAVE_U64_TYPE
#define KH_MUST_HAVE_64BIT_TYPES 1
#define KH_MUST_HAVE_I64_TYPE 1
#define KH_MUST_HAVE_U64_TYPE 1
#endif

#if defined(KH_MUST_HAVE_32BIT_TYPES)
#undef KH_MUST_HAVE_32BIT_TYPES
#undef KH_MUST_HAVE_I32_TYPE
#undef KH_MUST_HAVE_U32_TYPE
#define KH_MUST_HAVE_32BIT_TYPES 1
#define KH_MUST_HAVE_I32_TYPE 1
#define KH_MUST_HAVE_U32_TYPE 1
#endif

#if defined(KH_MUST_HAVE_16BIT_TYPES)
#undef KH_MUST_HAVE_16BIT_TYPES
#undef KH_MUST_HAVE_I16_TYPE
#undef KH_MUST_HAVE_U16_TYPE
#define KH_MUST_HAVE_16BIT_TYPES 1
#define KH_MUST_HAVE_I16_TYPE 1
#define KH_MUST_HAVE_U16_TYPE 1
#endif

#if defined(KH_MUST_HAVE_8BIT_TYPES)
#undef KH_MUST_HAVE_8BIT_TYPES
#undef KH_MUST_HAVE_I8_TYPE
#undef KH_MUST_HAVE_U8_TYPE
#define KH_MUST_HAVE_8BIT_TYPES 1
#define KH_MUST_HAVE_I8_TYPE 1
#define KH_MUST_HAVE_U8_TYPE 1
#endif

#if KH_MUST_HAVE_8_16_32_64
#if !defined(KH_MUST_HAVE_U8_16_32_64)
#define KH_MUST_HAVE_U8_16_32_64 1
#endif
#if !defined(KH_MUST_HAVE_I8_16_32_64)
#define KH_MUST_HAVE_I8_16_32_64 1
#endif
#endif

#if KH_MUST_HAVE_8_32_64
#if !defined(KH_MUST_HAVE_U8_32_64)
#define KH_MUST_HAVE_U8_32_64 1
#endif
#if !defined(KH_MUST_HAVE_I8_32_64)
#define KH_MUST_HAVE_I8_32_64 1
#endif
#endif

#if KH_MUST_HAVE_8_32
#if !defined(KH_MUST_HAVE_U8_32)
#define KH_MUST_HAVE_U8_32 1
#endif
#if !defined(KH_MUST_HAVE_I8_32)
#define KH_MUST_HAVE_I8_32 1
#endif
#endif

#if KH_PRIV_DOCS
#define KH_LIBRARY_BUILD 1
#define KH_USES_QT 1
#endif

namespace KirHut
{

/*!
 * Alias for std::numeric_Limits.
 *
 * It is very useful to get to std::numeric_limits frequently, but the name is cumbersome, so this makes it less so.
 */
template <class T>
using Limits = std::numeric_limits<T>;

/*!
 * The std::byte type is also defined in the KirHut namespace.
 */
using std::byte;

/*!
 * The std::size_t type is also defined in the KirHut namespace.
 *
 * Honestly don't know if I will retain this, it is kind of pointless, but there are some compilers that don't have
 * size_t in the global namespace, so it is helpful to avoid std:: all the time.
 */
using std::size_t;

/*!
 * The standard 8 bit signed integer type.
 *
 * This type is guaranteed to be at least 8 bits, and for all current supported platforms, is precisely 8 bits.
 */
using i8 = std::int_least8_t;

/*!
 * The standard 16 bit signed integer type.
 *
 * This type is guaranteed to be at least 16 bits, and for all current supported platforms, is precisely 16 bits.
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
 * The standard maximum bit width signed integer type.
 *
 * This type is guaranteed to be the largest integer type on the platform, and should be at least 64 bits.
 */
using iWidest = std::intmax_t;

/*!
 * The standard 8 bit unsigned integer type.
 *
 * This type is guaranteed to be at least 8 bits, and for all current supported platforms, is precisely 8 bits.
 */
using u8 = std::uint_least8_t;

/*!
 * The standard 16 bit unsigned integer type.
 *
 * This type is guaranteed to be at least 16 bits, and for all current supported platforms, is precisely 16 bits.
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
 * The standard maximum bit width unsigned integer type.
 *
 * This type is guaranteed to be the largest integer type on the platform, and should be at least 64 bits.
 */
using uWidest = std::uintmax_t;

/*!
 * The efficient 8 bit signed integer type.
 *
 * This type is guaranteed to be **at least** 8 bits long, not precisely 8 bits long. Always assume these types can be
 * larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
using e8 = std::int_fast8_t;
/*!
 * The efficient 16 bit signed integer type.
 *
 * This type is guaranteed to be **at least** 16 bits long, not precisely 16 bits long. Always assume these types can be
 * larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
using e16 = std::int_fast16_t;
/*!
 * The efficient 32 bit signed integer type.
 *
 * This type is guaranteed to be **at least** 32 bits long, not precisely 32 bits long. Always assume these types can be
 * larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
using e32 = std::int_fast32_t;
/*!
 * The efficient 64 bit signed integer type.
 *
 * This type is guaranteed to be **at least** 64 bits long, not precisely 64 bits long. All supported platforms have a
 * 64 bit integer type, so this is likely to be 64 bits long.
 */
using e64 = std::int_fast64_t;

/*!
 * The efficient 8 bit unsigned integer type.
 *
 * This type is guaranteed to be <b>at least</b> 8 bits long, not precisely 8 bits long. Always assume these types can
 * be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
using ue8 = std::uint_fast8_t;
/*!
 * The efficient 16 bit unsigned integer type.
 *
 * This type is guaranteed to be <b>at least</b> 16 bits long, not precisely 16 bits long. Always assume these types can
 * be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
using ue16 = std::uint_fast16_t;
/*!
 * The efficient 32 bit unsigned integer type.
 *
 * This type is guaranteed to be <b>at least</b> 32 bits long, not precisely 32 bits long. Always assume these types can
 * be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
using ue32 = std::uint_fast32_t;
/*!
 * The efficient 64 bit signed integer type.
 *
 * This type is guaranteed to be <b>at least</b> 64 bits long, not precisely 64 bits long. All supported platforms have
 * a 64 bit integer type, so this is likely to be 64 bits long.
 */
using ue64 = std::uint_fast64_t;

/*!
 * The number of bits in a byte.
 *
 * Some platforms may have differently sized bytes, and this can be used when that matters. The type used by
 * std::numeric_limits<T> cannot be std::byte because of a C++ limitation where std::numeric_limits<std::byte> is not
 * defined statically so cannot be used in constexpr expressions.
 */
[[maybe_unused]] constexpr int BYTE_BITS = Limits<unsigned char>::digits;
/*!
 * The highest (maximum) value of a byte on the platform.
 *
 * Some platforms may have differently sized bytes, and this can be used when that matters. The type used by
 * std::numeric_limits<T> cannot be std::byte because of a C++ limitation where std::numeric_limits<std::byte> is not
 * defined statically so cannot be used in constexpr expressions.
 */
[[maybe_unused]] constexpr byte BYTE_MAX = byte{Limits<unsigned char>::max()};

/*!
 * The lowest (minimum) value of an i8 integer.
 *
 * The i8 type is guaranteed to be <b>at least</b> 8 bits long, not precisely 8 bits long. Since all currently supported
 * platforms require an 8 bit byte, it is unlikely this will not be 8 bits, but the header supports it.
 */
[[maybe_unused]] constexpr i8 I8_MIN = Limits<i8>::min();
/*!
 * The lowest (minimum) value of an i16 integer.
 *
 * The i16 type is guaranteed to be <b>at least</b> 16 bits long, not precisely 16 bits long. It is unlikely the
 * platform does not support the short int type, however, so this is probably 16 bits on all supported platforms.
 */
[[maybe_unused]] constexpr i16 I16_MIN = Limits<i16>::min();
/*!
 * The lowest (minimum) value of an i32 integer.
 *
 * The i32 type is guaranteed to be <b>at least</b> 32 bits long, not precisely 32 bits long. Since all currently
 * supported platforms require a 32 bit type, it is unlikely this will not be 32 bits, but the header supports it.
 */
[[maybe_unused]] constexpr i32 I32_MIN = Limits<i32>::min();
/*!
 * The lowest (minimum) value of an i64 integer.
 *
 * The i64 type is guaranteed to be <b>at least</b> 64 bits long, not precisely 64 bits long. Since all currently
 * supported platforms require a 64 bit type, it is unlikely this will not be 64 bits, but the header supports it.
 */
[[maybe_unused]] constexpr i64 I64_MIN = Limits<i64>::min();
/*!
 * The lowest (minimum) value of the widest signed integer.
 *
 * The iWidest type is guaranteed to be the largest integer type supported on the platform, and should be at least 64
 * bits on all supported platforms.
 */
[[maybe_unused]] constexpr iWidest IWIDEST_MIN = Limits<iWidest>::min();

/*!
 * The highest (maximum) value of an i8 integer.
 *
 * The i8 type is guaranteed to be <b>at least</b> 8 bits long, not precisely 8 bits long. Since all currently supported
 * platforms require an 8 bit byte, it is unlikely this will not be 8 bits, but the header supports it.
 */
[[maybe_unused]] constexpr i8 I8_MAX = Limits<i8>::max();
/*!
 * The highest (maximum) value of an i16 integer.
 *
 * The i16 type is guaranteed to be <b>at least</b> 16 bits long, not precisely 16 bits long. It is unlikely the
 * platform does not support the short int type, however, so this is probably 16 bits on all supported platforms.
 */
[[maybe_unused]] constexpr i16 I16_MAX = Limits<i16>::max();
/*!
 * The highest (maximum) value of an i32 integer.
 *
 * The i32 type is guaranteed to be <b>at least</b> 32 bits long, not precisely 32 bits long. Since all currently
 * supported platforms require a 32 bit type, it is unlikely this will not be 32 bits, but the header supports it.
 */
[[maybe_unused]] constexpr i32 I32_MAX = Limits<i32>::max();
/*!
 * The highest (maximum) value of an i64 integer.
 *
 * The i64 type is guaranteed to be <b>at least</b> 64 bits long, not precisely 64 bits long. Since all currently
 * supported platforms require a 64 bit type, it is unlikely this will not be 64 bits, but the header supports it.
 */
[[maybe_unused]] constexpr i64 I64_MAX = Limits<i64>::max();
/*!
 * The highest (maximum) value of the widest signed integer.
 *
 * The iWidest type is guaranteed to be the largest integer type supported on the platform, and should be at least 64
 * bits on all supported platforms.
 */
[[maybe_unused]] constexpr iWidest IWIDEST_MAX = Limits<iWidest>::max();

/*!
 * The number of numeric bits found in the i8 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 8 bits long, this should be 7.
 */
[[maybe_unused]] constexpr int I8_BITS = Limits<i8>::digits;
/*!
 * The number of numeric bits found in the i16 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 16 bits long, this should be 15.
 */
[[maybe_unused]] constexpr int I16_BITS = Limits<i16>::digits;
/*!
 * The number of numeric bits found in the i32 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 32 bits long, this should be 31.
 */
[[maybe_unused]] constexpr int I32_BITS = Limits<i32>::digits;
/*!
 * The number of numeric bits found in the i64 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 64 bits long, this should be 63.
 */
[[maybe_unused]] constexpr int I64_BITS = Limits<i64>::digits;
/*!
 * The number of numeric bits found in the widest signed integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 64 bits long, this should be 63.
 */
[[maybe_unused]] constexpr int IWIDEST_BITS = Limits<iWidest>::digits;

/*!
 * The highest (maximum) value of an 8 bit unsigned integer.
 *
 * The u8 type is guaranteed to be <b>at least</b> 8 bits long, not precisely 8 bits long. Since all platforms supported
 * by Qt are dependent on having 8 bit byte types, it is unlikely this will not be 8 bits, but the header supports it.
 */
[[maybe_unused]] constexpr u8 U8_MAX = Limits<u8>::max();
/*!
 * The highest (maximum) value of a 16 bit unsigned integer.
 *
 * The u16 type is guaranteed to be <b>at least</b> 16 bits long, not precisely 16 bits long. It is unlikely the
 * platform does not support the short int type, however, so this is probably 16 bits on all supported platforms.
 */
[[maybe_unused]] constexpr u16 U16_MAX = Limits<u16>::max();
/*!
 * The highest (maximum) value of a 32 bit unsigned integer.
 *
 * The u32 type is guaranteed to be <b>at least</b> 32 bits long, not precisely 32 bits long. Since all platforms
 * supported by Qt require 32 bit types, it is unlikely this will not be 32 bits, but the header supports it.
 */
[[maybe_unused]] constexpr u32 U32_MAX = Limits<u32>::max();
/*!
 * The highest (maximum) value of a 64 bit unsigned integer.
 *
 * The u64 type is guaranteed to be <b>at least</b> 64 bits long, not precisely 64 bits long. Since all platforms
 * supported by Qt require 64 bit types, it is unlikely this will not be 64 bits, but the header supports it.
 */
[[maybe_unused]] constexpr u64 U64_MAX = Limits<u64>::max();
/*!
 * The highest (maximum) value of the widest unsigned integer.
 *
 * The uWidest type is guaranteed to be the largest integer type supported on the platform, and should be at least 64
 * bits on all supported platforms.
 */
[[maybe_unused]] constexpr uWidest UWIDEST_MAX = Limits<uWidest>::max();

/*!
 * The number of numeric bits found in the u8 integer.
 *
 * On platforms with 8-bit std::uint_least8_t integers, this should be 8.
 */
[[maybe_unused]] constexpr int U8_BITS = Limits<u8>::digits;
/*!
 * The number of numeric bits found in the u16 integer.
 *
 * On platforms with 16-bit std::uint_least16_t integers, this should be 16.
 */
[[maybe_unused]] constexpr int U16_BITS = Limits<u16>::digits;
/*!
 * The number of numeric bits found in the u32 integer.
 *
 * On platforms with 32-bit std::uint_least32_t integers, this should be 32.
 */
[[maybe_unused]] constexpr int U32_BITS = Limits<u32>::digits;
/*!
 * The number of numeric bits found in the u64 integer.
 *
 * On platforms with 64-bit std::uint_least64_t integers, this should be 64.
 */
[[maybe_unused]] constexpr int U64_BITS = Limits<u64>::digits;
/*!
 * The number of numeric bits found in the widest unsigned integer.
 *
 * On platforms with 64-bit std::uintmax_t integers, this should be 64.
 */
[[maybe_unused]] constexpr int UWIDEST_BITS = Limits<uWidest>::digits;

/*!
 * The lowest (minimum) value of an efficient 8 bit signed integer.
 *
 * The e8 type is guaranteed to be <b>at least</b> 8 bits long, not precisely 8 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr e8 E8_MIN = Limits<e8>::min();
/*!
 * The lowest (minimum) value of an efficient 16 bit signed integer.
 *
 * The e16 type is guaranteed to be <b>at least</b> 16 bits long, not precisely 16 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr e16 E16_MIN = Limits<e16>::min();
/*!
 * The lowest (minimum) value of an efficient 32 bit signed integer.
 *
 * The e32 type is guaranteed to be <b>at least</b> 32 bits long, not precisely 32 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr e32 E32_MIN = Limits<e32>::min();
/*!
 * The lowest (minimum) value of an efficient 64 bit signed integer.
 *
 * The e64 type is guaranteed to be <b>at least</b> 64 bits long, not precisely 64 bits long. All supported platforms
 * have a 64 bit integer type, so this is likely to be 64 bits long.
 */
[[maybe_unused]] constexpr e64 E64_MIN = Limits<e64>::min();

/*!
 * The number of numeric bits found in the e8 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 8 bits long, this should be 7.
 */
[[maybe_unused]] constexpr int E8_BITS = Limits<e8>::digits;
/*!
 * The number of numeric bits found in the e16 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 16 bits long, this should be 15.
 */
[[maybe_unused]] constexpr int E16_BITS = Limits<e16>::digits;
/*!
 * The number of numeric bits found in the e32 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 32 bits long, this should be 31.
 */
[[maybe_unused]] constexpr int E32_BITS = Limits<e32>::digits;
/*!
 * The number of numeric bits found in the e64 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 64 bits long, this should be 63.
 */
[[maybe_unused]] constexpr int E64_BITS = Limits<e64>::digits;

/*!
 * The highest (maximum) value of an efficient 8 bit signed integer.
 *
 * The e8 type is guaranteed to be <b>at least</b> 8 bits long, not precisely 8 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr e8 E8_MAX = Limits<e8>::max();
/*!
 * The highest (maximum) value of an efficient 16 bit signed integer.
 *
 * The e16 type is guaranteed to be <b>at least</b> 16 bits long, not precisely 16 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr e16 E16_MAX = Limits<e16>::max();
/*!
 * The highest (maximum) value of an efficient 32 bit signed integer.
 *
 * The e32 type is guaranteed to be <b>at least</b> 32 bits long, not precisely 32 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr e32 E32_MAX = Limits<e32>::max();
/*!
 * The highest (maximum) value of an efficient 64 bit signed integer.
 *
 * The e64 type is guaranteed to be <b>at least</b> 64 bits long, not precisely 64 bits long. All supported platforms
 * have a 64 bit integer type, so this is likely to be 64 bits long.
 */
[[maybe_unused]] constexpr e64 E64_MAX = Limits<e64>::max();

/*!
 * The highest (maximum) value of an efficient 8 bit unsigned integer.
 *
 * The ue8 type is guaranteed to be <b>at least</b> 8 bits long, not precisely 8 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr ue8 UE8_MAX = Limits<ue8>::max();
/*!
 * The highest (maximum) value of an efficient 16 bit unsigned integer.
 *
 * The ue16 type is guaranteed to be <b>at least</b> 16 bits long, not precisely 16 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr ue16 UE16_MAX = Limits<ue16>::max();
/*!
 * The highest (maximum) value of an efficient 32 bit unsigned integer.
 *
 * The ue32 type is guaranteed to be <b>at least</b> 32 bits long, not precisely 32 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr ue32 UE32_MAX = Limits<ue32>::max();
/*!
 * The highest (maximum) value of an efficient 64 bit unsigned integer.
 *
 * The ue64 type is guaranteed to be <b>at least</b> 64 bits long, not precisely 64 bits long. All supported platforms
 * have a 64 bit integer type, so this is likely to be 64 bits long.
 */
[[maybe_unused]] constexpr ue64 UE64_MAX = Limits<ue64>::max();

/*!
 * The number of numeric bits found in the ue8 integer.
 *
 * On platforms with 8-bit std::uint_fast8_t integers, this should be 8.
 */
[[maybe_unused]] constexpr int UE8_BITS = Limits<ue8>::digits;
/*!
 * The number of numeric bits found in the ue16 integer.
 *
 * On platforms with 16-bit std::uint_fast16_t integers, this should be 16.
 */
[[maybe_unused]] constexpr int UE16_BITS = Limits<ue16>::digits;
/*!
 * The number of numeric bits found in the ue32 integer.
 *
 * On platforms with 32-bit std::uint_fast32_t integers, this should be 32.
 */
[[maybe_unused]] constexpr int UE32_BITS = Limits<ue32>::digits;
/*!
 * The number of numeric bits found in the ue64 integer.
 *
 * On platforms with 64-bit std::uint_fast64_t integers, this should be 64.
 */
[[maybe_unused]] constexpr int UE64_BITS = Limits<ue64>::digits;

/*!
 * Namespace for types that distinguish the platform at compile time. These are useful for if constexpr and other more
 * modern C++ capabilities that are not dependent on preprocessor defines.
 */
namespace Platform
{

/*!
 * Non Preprocessor equivalent to #KH_WINDOWS.
 *
 * This is useful for static_assert, if constexpr, and other functionality that requires non macro types.
 *
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool windows = false
#if KH_WINDOWS
                                          || true
#endif
    ;
/*!
 * Non Preprocessor equivalent to #KH_LINUX.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool linux = false
#if KH_LINUX
                                        || true
#endif
    ;
/*!
 * Non Preprocessor equivalent to #KH_APPLE.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool apple = false
#if KH_APPLE
                                        || true
#endif
    ;
/*!
 * Non Preprocessor equivalent to #KH_MACOS.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool macOS = false
#if KH_MACOS
                                        || true
#endif
    ;
/*!
 * Non Preprocessor equivalent to #KH_IPHONE.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool iPhone = false
#if KH_IPHONE
                                         || true
#endif
    ;
/*!
 * Non Preprocessor equivalent to #KH_ANDROID.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool android = false
#if KH_ANDROID
                                          || true
#endif
    ;
/*!
 * Non Preprocessor equivalent to #KH_MOBILE.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool mobile = false
#if KH_MOBILE
                                         || true
#endif
    ;

} // namespace Platform

/*!
 * Namespace for Build information regarding libKirHut.
 */
namespace Build
{

/*!
 * Non Preprocessor equivalent to #KH_DEBUG.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool debug = false
#if KH_DEBUG
                                        || true
#endif
    ;
/*!
 * Non Preprocessor equivalent to #KH_RELEASE.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool release = false
#if KH_RELEASE
                                          || true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_USES_QT.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool usesQt = false
#if KH_USES_QT
                                         || true
#endif
    ;
/*!
 * Non Preprocessor equivalent to KH_QT5_15.
 *
 * \copydetails KirHut::Platform::windows
 *
 * These are defined regardless of whether or not Qt is used because if constexpr must still produce a legal expression
 * even if it is always false. See usesQt to check if Qt is included in the build.
 *
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool qt515 = false
#if KH_QT5_15
                                        || usesQt
#endif
    ;
/*!
 * Non Preprocessor equivalent to KH_QT6_2.
 *
 * \copydetails KirHut::Build::qt515
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool qt62 = false
#if KH_QT6_2
                                       || usesQt
#endif
    ;
/*!
 * Non Preprocessor equivalent to KH_QT6_5.
 *
 * \copydetails KirHut::Build::qt515
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool qt65 = false
#if KH_QT6_5
                                       || usesQt
#endif
    ;

/*!
 * Check if the current build has signed 8 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't assert that the platform must have this functionality, instead this
 * value informs you if the platform does have 8 bit integers. This value is therefore always there, even on platforms
 * where there are not 8 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool hasI8 = Limits<i8>::digits == 7;
/*!
 * Check if the current build has signed 16 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't assert that the platform must have this functionality, instead this
 * value informs you if the platform does have 16 bit integers. This value is therefore always there, even on platforms
 * where there are not 16 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool hasI16 = Limits<i16>::digits == 15;
/*!
 * Check if the current build has signed 32 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't assert that the platform must have this functionality, instead this
 * value informs you if the platform does have 32 bit integers. This value is therefore always there, even on platforms
 * where there are not 32 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool hasI32 = Limits<i32>::digits == 31;
/*!
 * Check if the current build has signed 64 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't assert that the platform must have this functionality, instead this
 * value informs you if the platform does have 64 bit integers. This value is therefore always there, even on platforms
 * where there are not 64 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool hasI64 = Limits<i64>::digits == 63;
/*!
 * Check if the current build has unsigned 8 bit integers.
 *
 * \copydetails KirHut::Build::hasI8
 */
[[maybe_unused]] constexpr bool hasU8 = Limits<u8>::digits == 8;
/*!
 * Check if the current build has unsigned 16 bit integers.
 *
 * \copydetails KirHut::Build::hasI16
 */
[[maybe_unused]] constexpr bool hasU16 = Limits<u16>::digits == 16;
/*!
 * Check if the current build has unsigned 32 bit integers.
 *
 * \copydetails KirHut::Build::hasI32
 */
[[maybe_unused]] constexpr bool hasU32 = Limits<u32>::digits == 32;
/*!
 * Check if the current build has unsigned 64 bit integers.
 *
 * \copydetails KirHut::Build::hasI64
 */
[[maybe_unused]] constexpr bool hasU64 = Limits<u64>::digits == 64;
/*!
 * Check if the current build has 8 bit integers.
 *
 * \copydetails KirHut::Build::hasI8
 */
[[maybe_unused]] constexpr bool has8 = hasI8 && hasU8;
/*!
 * Check if the current build has 16 bit integers.
 *
 * \copydetails KirHut::Build::hasI16
 */
[[maybe_unused]] constexpr bool has16 = hasI16 && hasU16;
/*!
 * Check if the current build has 32 bit integers.
 *
 * \copydetails KirHut::Build::hasI32
 */
[[maybe_unused]] constexpr bool has32 = hasI32 && hasU32;
/*!
 * Check if the current build has 64 bit integers.
 *
 * \copydetails KirHut::Build::hasI64
 */
[[maybe_unused]] constexpr bool has64 = hasI64 && hasU64;
/*!
 * Check if the current build has signed 8 and 32 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't assert that the platform must have this functionality, instead this
 * value informs you if the platform does have 8 and 32 bit integers. This value is therefore always there, even on
 * platforms where there are not 8 and 32 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool hasI832 = hasI8 && hasI32;
/*!
 * Check if the current build has unsigned 8 and 32 bit integers.
 *
 * \copydetails KirHut::Build::hasI832
 */
[[maybe_unused]] constexpr bool hasU832 = hasU8 && hasU32;
/*!
 * Check if the current build has 8 and 32 bit integers.
 *
 * \copydetails KirHut::Build::hasI832
 */
[[maybe_unused]] constexpr bool has832 = hasI832 && hasU832;
/*!
 * Check if the current build has signed 8, 32, and 64 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't assert that the platform must have this functionality, instead this
 * value informs you if the platform does have 8, 32, and 64 bit integers. This value is therefore always there, even on
 * platforms where there are not 8, 32, and 64 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool hasI83264 = hasI832 && hasI64;
/*!
 * Check if the current build has unsigned 8, 32, and 64 bit integers.
 *
 * \copydetails KirHut::Build::hasI83264
 */
[[maybe_unused]] constexpr bool hasU83264 = hasU832 && hasU64;
/*!
 * Check if the current build has 8, 32, and 64 bit integers.
 *
 * \copydetails KirHut::Build::hasI83264
 */
[[maybe_unused]] constexpr bool has83264 = hasI83264 && hasU83264;
/*!
 * Check if the current build has signed 8, 16, 32, and 64 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't assert that the platform must have this functionality, instead this
 * value informs you if the platform does have 8, 16, 32, and 64 bit integers. This value is therefore always there,
 * even on platforms where there are not 8, 16, 32, and 64 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool hasI8163264 = hasI83264 && hasI16;
/*!
 * Check if the current build has unsigned 8, 16, 32, and 64 bit integers.
 *
 * \copydetails KirHut::Build::hasI8163264
 */
[[maybe_unused]] constexpr bool hasU8163264 = hasU83264 && hasU16;
/*!
 * Check if the current build has 8, 16, 32, and 64 bit integers.
 *
 * \copydetails KirHut::Build::hasI8163264
 */
[[maybe_unused]] constexpr bool has8163264 = hasI8163264 && hasU8163264;

} // namespace Build

//! \cond
/*
 * I use a macro here instead of a constexpr template method because I need the compilation time string literal
 * concatenation abilities of a preprocessor simply putting multiple constant strings together instead of a typechecked
 * and not-yet-working as of C++17 set of constexpr std::string concatenations necessary to make this string. When the
 * constexpr stuff is working better in C++20/23 this may be revised.
 */
#define KH_PRIV_CHECK_REQUIRES_SIZE(COND, TYPEDESC)                                                              \
static_assert(COND, "Compilation requires " TYPEDESC " type but this compiler or platform does not support it.")

#if KH_MUST_HAVE_I8_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(Build::hasI8, "signed 8-bit");
#endif
#if KH_MUST_HAVE_U8_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(Build::hasU8, "unsigned 8-bit");
#endif
#if KH_MUST_HAVE_I16_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(Build::hasI16, "signed 16-bit");
#endif
#if KH_MUST_HAVE_U16_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(Build::hasU16, "unsigned 16-bit");
#endif
#if KH_MUST_HAVE_I32_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(Build::hasI32, "signed 32-bit");
#endif
#if KH_MUST_HAVE_U32_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(Build::hasU32, "unsigned 32-bit");
#endif
#if KH_MUST_HAVE_I64_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(Build::hasI64, "signed 64-bit");
#endif
#if KH_MUST_HAVE_U64_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(Build::hasU64, "unsigned 64-bit");
#endif
//! \endcond

} // namespace KirHut
