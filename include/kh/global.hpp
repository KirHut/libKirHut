/***********************************************************************************************************************
** The KirHut Library for the Public Benefit
** khglobal.hpp
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
 * \file khglobal.hpp
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
 * \see iOS_Device
 * \see Instruction_Set_Architecture
 * \see Qt_Version
 * \see BuildType
 */

#include <cstdint>
#include <cstddef>
#include <limits>

/*!
 * \defgroup Platform
 *
 * This is the mutually-exclusive group of Platform providers.
 *
 * \{
 */

/*!
 * Preprocessor define indicating if the system is compiled on Windows.
 *
 * This is dependent on the _WIN32 preprocessor definition. This should be defined by the compiler on any Windows
 * platform, both 32 and 64 bit.
 */
#define KH_WINDOWS 1

/*!
 * Preprocessor define indicating if the system is compiled on Linux.
 *
 * This is dependent on the __linux preprocessor definition. This should be defined by the compiler on any Linux kernel
 * platform, both 32 and 64 bit.
 */
#define KH_LINUX 1

/*!
 * Preprocessor define indicating if the system is compiled for an Apple device.
 *
 * This is dependent on the __APPLE__ and __MACH__ preprocessor definitions. These should be defined by the compiler on
 * any Apple device, either Mac OS, iPhone, or iPad.
 */
#define KH_APPLE 1

/*!
 * Preprocessor define indicating if the system is compiled for an Android device.
 *
 * This is dependent on the __ANDROID__ or ANDROID preprocessor definitions. One of these should be defined by the
 * compiler on any Android device, either 32 or 64 bit.
 */
#define KH_ANDROID 1

/*!
 * Preprocessor define indicating if the system is compiled using Emscripten for WebAssembly.
 *
 * This is dependent on the __EMSCRIPTEN__ preprocessor definition, which is defined on the Emscripten compiler used to
 * build Web Assembly binaries.
 */
#define KH_WASM 1

/*!
 * Preprocessor define indicating if the system is compiled on a BSD system.
 *
 * This is dependent on any of the following being defined for determining the operating system:
 * - __FreeBSD__
 * - __DragonFly__
 * - __NetBSD__
 * - __OpenBSD__
 *
 * If any of the above are defined, than this is defined.
 */
#define KH_BSD 1

//! \}

/*!
 * \defgroup iOS_Device
 *
 * The mutually exclusive group of possible iOS Devices. While it is true that only one of these will be defined, it is
 * not true that one of them must be defined. If #KH_APPLE is not defined, none of these will be defined.
 *
 * \{
 */

/*!
 * Preprocessor define indicating if the system is compiled on Mac OS X.
 *
 * This is dependent on Apple's TargetConditionals.h TARGET_OS_IPHONE macro. If TARGET_OS_IPHONE is not defined or equal
 * to 0, than this is defined.
 */
#define KH_MACOS 1

/*!
 * Preprocessor define indicating if the system is compiled for Apple iPhone or iPad OS.
 *
 * This is dependent on Apple's TargetConditionals.h TARGET_OS_IPHONE macro. If TARGET_OS_IPHONE is defined, than this
 * is defined.
 */
#define KH_IPHONE 1

//! \}

#ifndef _WIN32
#undef KH_WINDOWS
#endif

#ifndef __linux
#undef KH_LINUX
#endif

#if !defined(__APPLE__) && !defined(__MACH__)
#undef KH_APPLE
#undef KH_MACOS
#undef KH_IPHONE
#else
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#undef KH_MACOS
#else
#undef KH_IPHONE
#endif // TARGET_OS_IPHONE
#endif // !defined(__APPLE__) && !defined(__MACH__)

#if !defined(__ANDROID__) && !defined(ANDROID)
#undef KH_ANDROID
#endif

#if !defined(__EMSCRIPTEN__)
#undef KH_WASM
#endif

#if !defined(__FreeBSD__) && !defined(__DragonFly__) && !defined(__NetBSD__) && !defined(__OpenBSD__)
#undef KH_BSD
#endif

/*!
 * Preprocessor define indicating if the system is compiled for a mobile device.
 */
#define KH_MOBILE KH_IPHONE || KH_ANDROID

#if !KH_MOBILE
#undef KH_MOBILE
#endif

/*!
 * \defgroup Instruction_Set_Architecture
 *
 * The mutually exclusive group of what Instruction Set Architecture (or ISA) this library was built for.
 *
 * \{
 */

/*!
 * Preprocessor define indicating if the system is compiled for 64-bit x86 processors.
 *
 * This is dependent on __x86_64__ or _M_X64 being defined by the compiler for determining the processor architecture.
 * If either of those are defined, than this is defined.
 */
#define KH_X64 1

/*!
 * Preprocessor define indicating if the system is compiled for 32-bit x86 processors.
 *
 * This is dependent on any of the following being defined for determining the operating system:
 * - i386
 * - __i386__
 * - __i386
 * - _M_IX86
 *
 * If any of the above are defined, than this is defined.
 */
#define KH_X32 1

/*!
 * Preprocessor define indicating if the system is compiled for Intel Itanium processors.
 *
 * This is dependent on any of the following being defined for determining the operating system:
 * - __ia64
 * - __ia64__
 * - __IA64__
 * - _M_IA64
 * - __itanium__
 *
 * If any of the above are defined, than this is defined.
 */
#define KH_IA64 1

/*!
 * Preprocessor define indicating if the system is compiled for ARM v6 processors.
 *
 * This is dependent on the preprocessor define __ARM_ARCH_6__ being defined or _M_ARM being equal to 6.
 */
#define KH_ARMv6 1

/*!
 * Preprocessor define indicating if the system is compiled for ARM v7 processors.
 *
 * This is dependent on the preprocessor define __ARM_ARCH_7__ being defined or _M_ARM being equal to 7.
 */
#define KH_ARMv7 1

/*!
 * Preprocessor define indicating if the system is compiled for ARM v8 processors.
 *
 * This is dependent on the preprocessor defines __aarch64__ or _M_ARM64 being defined.
 */
#define KH_ARMv8 1

/*!
 * Preprocessor define indicating if the system is compiled for MIPS processors.
 *
 * This is dependent on the preprocessor defines mips, __mips or __mips__ being defined.
 */
#define KH_MIPS 1

/*!
 * Preprocessor define indicating if the system is compiled for Hitachi SuperH processors.
 *
 * This is dependent on the preprocessor define __sh__ being defined.
 */
#define KH_SUPERH 1

/*!
 * Preprocessor define indicating if the system is compiled for 64 bit POWER processors.
 *
 * This is dependent on any of the following being defined for determining the operating system:
 * - __PPC64__
 * - __ppc64__
 * - __powerpc64__
 * - _ARCH_PPC64
 *
 * If any of the above are defined, than this is defined, otherwise it is not.
 */
#define KH_POWER64 1

/*!
 * Preprocessor define indicating if the system is compiled for 32 bit POWER processors.
 *
 *
 * This is dependent on any of the following being defined for determining the operating system:
 * - __PPC__
 * - __ppc__
 * - __powerpc
 * - __powerpc__
 * - __POWERPC__
 * - _ARCH_PPC
 *
 * If any of the above are defined, \b and #KH_POWER64 is \b not defined, than this is defined.
 */
#define KH_POWER32 1

/*!
 * Preprocessor define indicating if the system is compiled for 64 bit SPARC processors.
 *
 * This is dependent on the preprocessor defines __sparcv9 or __sparc_v9__ being defined.
 */
#define KH_SPARC64 1

/*!
 * Preprocessor define indicating if the system is compiled for 32 bit SPARC processors.
 *
 * This is dependent on the preprocessor defines __sparc or __sparc__ being defined \b and #KH_SPARC64 \b not being
 * defined.
 */
#define KH_SPARC32 1

/*!
 * Preprocessor define indicating if the library is compiled for Motorola 68000 processors.
 *
 * This is dependent on the preprocessor define __m68k__ being defined.
 */
#define KH_M68K 1

//! \}

#if !defined(__x86_64__) && !defined(_M_X64)
#undef KH_X64
#endif

#if KH_X64 || (!defined(i386) && !defined(__i386__) && !defined(__i386) && !defined(_M_IX86))
#undef KH_X32
#endif

#if !defined(__ia64__) && !defined(__ia64) && !defined(_M_IA64) && !defined(__IA64__) && !defined(__itanium__)
#undef KH_IA64
#endif

#if !defined(__ARM_ARCH_6__) && (_M_ARM != 6)
#undef KH_ARMv6
#endif

#if !defined(__ARM_ARCH_7__) && (_M_ARM != 7)
#undef KH_ARMv7
#endif

#if !defined(__aarch64__) && !defined(_M_ARM64)
#undef KH_ARMv8
#endif

#if !defined(mips) && !defined(__mips__) && !defined(__mips)
#undef KH_MIPS
#endif

#if !defined(__sh__)
#undef KH_SUPERH
#endif

#if !defined(__PPC64__) && !defined(__ppc64__) && !defined(_ARCH_PPC64) && !defined(__powerpc64__)
#undef KH_POWER64
#endif

#if KH_POWER64 || (!defined(__powerpc) && !defined(__powerpc__) && !defined(__POWERPC__))
#undef KH_POWER32
#endif

#if !KH_POWER32 && !KH_POWER64 && (defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC))
#define KH_POWER32 1
#endif

#if !defined(__sparc_v9__) && !defined(__sparcv9)
#undef KH_SPARC64
#endif

#if KSC_SPARC64 || (!defined(__sparc__) && !defined(__sparc))
#undef KH_SPARC32
#endif

#if !defined(__m68k__)
#undef KH_M68K
#endif

/*!
 * Preprocessor define for builds of this library on ARMv6T2 processors.
 *
 * I have no idea what this shit means. Maybe you do.
 */
#define KH_ARMv6T2 1

/*!
 * Preprocessor define for builds of this library on ARMv6J processors.
 *
 * I have no idea what this shit means. Maybe you do.
 */
#define KH_ARMv6J 1

/*!
 * Preprocessor define for builds of this library on ARMv6K processors.
 *
 * I have no idea what this shit means. Maybe you do.
 */
#define KH_ARMv6K 1

/*!
 * Preprocessor define for builds of this library on ARMv6Z processors.
 *
 * I have no idea what this shit means. Maybe you do.
 */
#define KH_ARMv6Z 1

/*!
 * Preprocessor define for builds of this library on ARMv6ZK processors.
 *
 * I have no idea what this shit means. Maybe you do.
 */
#define KH_ARMv6ZK 1

/*!
 * Preprocessor define for builds of this library on ARMv7A processors.
 *
 * I have no idea what this shit means. Maybe you do.
 */
#define KH_ARMv7A 1

/*!
 * Preprocessor define for builds of this library on ARMv7S processors.
 *
 * I have no idea what this shit means. Maybe you do.
 */
#define KH_ARMv7S 1


/*!
 * Preprocessor define for builds of this library on ARMv7R processors.
 *
 * I have no idea what this shit means. Maybe you do.
 */
#define KH_ARMv7R 1


/*!
 * Preprocessor define for builds of this library on ARMv7M processors.
 *
 * I have no idea what this shit means. Maybe you do.
 */
#define KH_ARMv7M 1

#if !defined(__ARM_ARCH_6T2_)
#undef KH_ARMv6T2
#endif

#if !defined(__ARM_ARCH_6J__)
#undef KH_ARMv6J
#endif

#if !defined(__ARM_ARCH_6K__)
#undef KH_ARMv6K
#endif

#if !defined(__ARM_ARCH_6Z__)
#undef KH_ARMv6Z
#endif

#if !defined(__ARM_ARCH_6ZK__)
#undef KH_ARMv6ZK
#endif

#if !defined(__ARM_ARCH_7S__)
#undef KH_ARMv7S
#endif

#if !defined(__ARM_ARCH_7M__)
#undef KH_ARMv7M
#endif

#if !defined(__ARM_ARCH_7R__)
#undef KH_ARMv7R
#endif

#if !defined(__ARM_ARCH_7A__)
#undef KH_ARMv7A
#endif

/*!
 * Preprocessor define indicating if the system is compiled for 32-bit ARM processors.
 *
 * This is dependent on QtGlobal's Q_PROCESSOR_ARM_V5, Q_PROCESSOR_ARM_V6, or Q_PROCESSOR_ARM_V7 being true for
 * determining the processor architecture. If any of those are defined, then KH_ARM32 is 1, otherwise it is 0.
 */
#define KH_ARM32 KH_ARMv6 || KH_ARMv7

#if !KH_ARM32
#undef KH_ARM32
#endif

/*!
 * Preprocessor define indicating if the system is compiled for 64-bit ARM processors.
 *
 * This is dependent on KH_ARMv8 being defined because the ARM v8 architecture is all 64 bit and all previous versions
 * are 32 bit.
 */
#define KH_ARM64 1

#if !defined(KH_ARMv8)
#undef KH_ARM64
#endif

/*!
 * Preprocessor define indicating if the system is compiled for 64-bit systems.
 */
#define KH_64BIT KH_X64 || KH_ARM64 || KH_POWER64 || KH_SPARC64

#if !KH_64BIT
#undef KH_64BIT
#endif

/*!
 * Preprocessor define indicating if the system is compiled for 32-bit systems.
 */
#define KH_32BIT KH_X32 || KH_ARM32 || KH_POWER32 || KH_SPARC32 || KH_SUPERH || KH_M68K

#if !KH_32BIT
#undef KH_32BIT
#endif

#if KH_USES_QT || defined(KH_PRIV_DOCS)

#include <QtGlobal>
#if QT_VERSION > QT_VERSION_CHECK(6,0,0)
#include <QtProcessorDetection>
#endif

/*!
 * \defgroup Qt_Version
 *
 * This group is of the three major Qt LTS release versions that were supported by the Qt Company when this library was
 * written. This will likely expand and change as time goes on.
 *
 * \{
 */

/*!
 * Preprocessor define indicating if the system is compiled using Qt 5.15.
 *
 * This determines the version with QT_VERSION < QT_VERSION_CHECK(6, 2, 0). This library assumes you are using one of
 * the major LTS versions of Qt if it is used, so compatibility is based on LTS versions.
 */
#define KH_QT5_15 1

#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
#undef KH_QT5_15
#endif

/*!
 * Preprocessor define indicating if the system is compiled using Qt 6.2.
 *
 * This determines the version with QT_VERSION >= QT_VERSION_CHECK(6, 2, 0) and QT_VERSION < QT_VERSION_CHECK(6, 5, 0).
 * This library assumes you are using one of the major LTS versions of Qt if it is used, so compatibility is based on
 * LTS versions.
 */
#define KH_QT6_2 1

#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0) || QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
#undef KH_QT6_2
#endif

/*!
 * Preprocessor define indicating if the system is compiled using Qt 6.5.
 *
 * This determines the version with QT_VERSION >= QT_VERSION_CHECK(6, 5, 0). When using Qt 6.5, this will be 1,
 * otherwise it is 0.
 */
#define KH_QT6_5 1

#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
#undef KH_QT6_5
#endif

//! \}

#if defined(KH_LIBRARY_BUILD)
#define KH_EXPORT Q_DECL_EXPORT
#else
/*!
 * Preprocessor define to place in front of exported functions and classes in headers.
 *
 * On Windows, what this expands to is dependent on two preprocessor defines in the build: KH_LIBRARY_BUILD and
 * KH_USES_QT.
 * | KH_USES_QT  | KH_LIBRARY_BUILD | Expands To            |
 * |-------------|------------------|-----------------------|
 * | Not Defined | Not Defined      | __declspec(dllimport) |
 * | Not Defined | Defined          | __declspec(dllexport) |
 * | Defined     | Not Defined      | Q_DECL_IMPORT         |
 * | Defined     | Defined          | Q_DECL_EXPORT         |
 *
 * This `define` unconditionally expands to __declspec() on Windows without Qt because the MingW and Clang documentation
 * both claim to support this syntax, so there's no reason not to use it (similarly to `pragma once`).
 *
 * On all other operating systems, KH_EXPORT simply expands into nothing at all.
 *
 * \hideinitializer
 */
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

/*!
 * \defgroup BuildType
 *
 * This mutually exclusive group indicates what type of build is being performed. The most important are KH_DEBUG and
 * KH_RELEASE.
 *
 * \{
 */

/*!
 * Preprocessor define indicating if the system is compiled in debug mode.
 *
 * This is defined if the preprocessor define NDEBUG is not defined.
 */
#define KH_DEBUG 1

/*!
 * Preprocessor define indicating if the system is compiled in release mode.
 *
 * This is defined if the preprocessor define NDEBUG is defined.
 */
#define KH_RELEASE 1

#if !defined(NDEBUG)
#undef KH_RELEASE
#else
#undef KH_DEBUG
#endif

//! \}

namespace KirHut
{

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
 * \copydetails windows
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
 * \copydetails windows
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
 * \copydetails windows
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
 * \copydetails windows
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
 * \copydetails windows
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
 * \copydetails windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool mobile = false
#if KH_MOBILE
                                         || true
#endif
                                         ;

} // namespace Platform

namespace Build
{

/*!
 * Non Preprocessor equivalent to #KH_DEBUG.
 *
 * \copydetails windows
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
 * \copydetails windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool release = false
#if KH_RELEASE
                                          || true
#endif
                                          ;

/*!
 * Non Preprocessor equivalent to #KH_TEST_BUILD.
 *
 * \copydetails windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool test = false
#if KH_TEST_BUILD
                                       || true
#endif
                                       ;

/*!
 * Non Preprocessor equivalent to #KH_USES_QT.
 *
 * \copydetails windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool usesQt = false
#if KH_USES_QT
                                         || true
#endif
                                         ;
/*!
 * Non Preprocessor equivalent to #KH_QT5_15.
 *
 * \copydetails windows
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
 * Non Preprocessor equivalent to #KH_QT6_2.
 *
 * \copydetails windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool qt62 = false
#if KH_QT6_2
                                       || usesQt
#endif
                                       ;
/*!
 * Non Preprocessor equivalent to #KH_QT6_5.
 *
 * \copydetails windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool qt65 = false
#if KH_QT6_5
                                       || usesQt
#endif
                                       ;

} // namespace Build

}

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_8_16_32_64)
#undef KH_MUST_HAVE_8_16_32_64
#undef KH_MUST_HAVE_8_32_64
#undef KH_MUST_HAVE_16BIT_TYPES
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 8, 16, 32, and 64 bit types of both
 * signed and unsigned values.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_8_32_64
 * - KH_MUST_HAVE_16BIT_TYPES
 *
 * As well as any flags that are a consequence of defining those two flags.
 */
#define KH_MUST_HAVE_8_16_32_64 1
#define KH_MUST_HAVE_8_32_64 1
#define KH_MUST_HAVE_16BIT_TYPES 1
#endif

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_I8_16_32_64)
#undef KH_MUST_HAVE_I8_16_32_64
#undef KH_MUST_HAVE_I8_32_64
#undef KH_MUST_HAVE_I16_TYPE
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 8, 16, 32, and 64 bit signed types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I8_32_64
 * - KH_MUST_HAVE_I16_TYPE
 *
 * As well as any flags that are a consequence of defining those two flags.
 */
#define KH_MUST_HAVE_I8_16_32_64 1
#define KH_MUST_HAVE_I8_32_64 1
#define KH_MUST_HAVE_I16_TYPE 1
#endif

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_U8_16_32_64)
#undef KH_MUST_HAVE_U8_16_32_64
#undef KH_MUST_HAVE_U8_32_64
#undef KH_MUST_HAVE_U16_TYPE
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 8, 16, 32, and 64 bit unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_U8_32_64
 * - KH_MUST_HAVE_U16_TYPE
 *
 * As well as any flags that are a consequence of defining those two flags.
 */
#define KH_MUST_HAVE_U8_16_32_64 1
#define KH_MUST_HAVE_U8_32_64 1
#define KH_MUST_HAVE_U16_TYPE 1
#endif

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_8_32_64)
#undef KH_MUST_HAVE_8_32_64
#undef KH_MUST_HAVE_8_32
#undef KH_MUST_HAVE_64BIT_TYPES
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 8, 32, and 64 bit types of both signed
 * and unsigned values.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_8_32
 * - KH_MUST_HAVE_64BIT_TYPES
 *
 * As well as any flags that are a consequence of defining those two flags.
 */
#define KH_MUST_HAVE_8_32_64 1
#define KH_MUST_HAVE_8_32 1
#define KH_MUST_HAVE_64BIT_TYPES 1
#endif

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_I8_32_64)
#undef KH_MUST_HAVE_I8_32_64
#undef KH_MUST_HAVE_I8_32
#undef KH_MUST_HAVE_I64_TYPE
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 8, 32, and 64 bit signed types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I8_32
 * - KH_MUST_HAVE_I64_TYPE
 *
 * As well as any flags that are a consequence of defining those two flags.
 */
#define KH_MUST_HAVE_I8_32_64 1
#define KH_MUST_HAVE_I8_32 1
#define KH_MUST_HAVE_I64_TYPE 1
#endif

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_U8_32_64)
#undef KH_MUST_HAVE_U8_32_64
#undef KH_MUST_HAVE_U8_32
#undef KH_MUST_HAVE_U64_TYPE
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 8, 32, and 64 bit unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_U8_32
 * - KH_MUST_HAVE_U64_TYPE
 *
 * As well as any flags that are a consequence of defining those two flags.
 */
#define KH_MUST_HAVE_U8_32_64 1
#define KH_MUST_HAVE_U8_32 1
#define KH_MUST_HAVE_U64_TYPE 1
#endif

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_8_32)
#undef KH_MUST_HAVE_8_32
#undef KH_MUST_HAVE_8BIT_TYPES
#undef KH_MUST_HAVE_32BIT_TYPES
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 8 and 32 bit types of both signed and
 * unsigned values.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_8BIT_TYPES
 * - KH_MUST_HAVE_32BIT_TYPES
 *
 * As well as any flags that are a consequence of defining those two flags.
 */
#define KH_MUST_HAVE_8_32 1
#define KH_MUST_HAVE_8BIT_TYPES 1
#define KH_MUST_HAVE_32BIT_TYPES 1
#endif

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_I8_32)
#undef KH_MUST_HAVE_I8_32
#undef KH_MUST_HAVE_I8_TYPE
#undef KH_MUST_HAVE_I32_TYPE
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 8 and 32 bit signed types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I8_TYPE
 * - KH_MUST_HAVE_I32_TYPE
 */
#define KH_MUST_HAVE_I8_32 1
#define KH_MUST_HAVE_I8_TYPE 1
#define KH_MUST_HAVE_I32_TYPE 1
#endif

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_U8_32)
#undef KH_MUST_HAVE_U8_32
#undef KH_MUST_HAVE_U8_TYPE
#undef KH_MUST_HAVE_U32_TYPE
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 8 and 32 bit unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_U8_TYPE
 * - KH_MUST_HAVE_U32_TYPE
 */
#define KH_MUST_HAVE_U8_32 1
#define KH_MUST_HAVE_U8_TYPE 1
#define KH_MUST_HAVE_U32_TYPE 1
#endif

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_64BIT_TYPES)
#undef KH_MUST_HAVE_64BIT_TYPES
#undef KH_MUST_HAVE_I64_TYPE
#undef KH_MUST_HAVE_U64_TYPE
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 64 bit signed and unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I64_TYPE
 * - KH_MUST_HAVE_U64_TYPE
 */
#define KH_MUST_HAVE_64BIT_TYPES 1
/*!
 * Preprocessor flag set by user to require the underlying implementation to have a 64 bit signed integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<i64>::digits() value is
 * exactly 63. If this is not the case, the library or application will fail to compile.
 */
#define KH_MUST_HAVE_I64_TYPE 1
/*!
 * Preprocessor flag set by user to require the underlying implementation to have a 64 bit unsigned integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<u64>::digits() value is
 * exactly 64. If this is not the case, the library or application will fail to compile.
 */
#define KH_MUST_HAVE_U64_TYPE 1
#endif

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_32BIT_TYPES)
#undef KH_MUST_HAVE_32BIT_TYPES
#undef KH_MUST_HAVE_I32_TYPE
#undef KH_MUST_HAVE_U32_TYPE
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 32 bit signed and unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I32_TYPE
 * - KH_MUST_HAVE_U32_TYPE
 */
#define KH_MUST_HAVE_32BIT_TYPES 1
/*!
 * Preprocessor flag set by user to require the underlying implementation to have a 32 bit signed integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<i32>::digits() value is
 * exactly 31. If this is not the case, the library or application will fail to compile.
 */
#define KH_MUST_HAVE_I32_TYPE 1
/*!
 * Preprocessor flag set by user to require the underlying implementation to have a 32 bit unsigned integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<u32>::digits() value is
 * exactly 32. If this is not the case, the library or application will fail to compile.
 */
#define KH_MUST_HAVE_U32_TYPE 1
#endif

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_16BIT_TYPES)
#undef KH_MUST_HAVE_16BIT_TYPES
#undef KH_MUST_HAVE_I16_TYPE
#undef KH_MUST_HAVE_U16_TYPE
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 16 bit signed and unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I16_TYPE
 * - KH_MUST_HAVE_U16_TYPE
 */
#define KH_MUST_HAVE_16BIT_TYPES 1
/*!
 * Preprocessor flag set by user to require the underlying implementation to have a 16 bit signed integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<i16>::digits() value is
 * exactly 15. If this is not the case, the library or application will fail to compile.
 */
#define KH_MUST_HAVE_I16_TYPE 1
/*!
 * Preprocessor flag set by user to require the underlying implementation to have a 16 bit unsigned integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<u16>::digits() value is
 * exactly 16. If this is not the case, the library or application will fail to compile.
 */
#define KH_MUST_HAVE_U16_TYPE 1
#endif

#if defined(KH_PRIV_DOCS) || defined(KH_MUST_HAVE_8BIT_TYPES)
#undef KH_MUST_HAVE_8BIT_TYPES
#undef KH_MUST_HAVE_I8_TYPE
#undef KH_MUST_HAVE_U8_TYPE
/*!
 * Preprocessor flag set by user to require the underlying implementation to have 8 bit signed and unsigned types.
 *
 * As a consequence of building libKirHut with this flag, the following flags are overridden and defined as 1:
 * - KH_MUST_HAVE_I8_TYPE
 * - KH_MUST_HAVE_U8_TYPE
 */
#define KH_MUST_HAVE_8BIT_TYPES 1
/*!
 * Preprocessor flag set by user to require the underlying implementation to have an 8 bit signed integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<i8>::digits() value is
 * exactly 7. If this is not the case, the library or application will fail to compile.
 */
#define KH_MUST_HAVE_I8_TYPE 1
/*!
 * Preprocessor flag set by user to require the underlying implementation to have an 8 bit unsigned integer type.
 *
 * This flag causes a static_assert to be built in this header that checks that the Limits<u8>::digits() value is
 * exactly 8. If this is not the case, the library or application will fail to compile.
 */
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

//! \cond
/*
 * I use a macro here instead of a constexpr template method because I need the compilation time string literal
 * concatenation abilities of a preprocessor simply putting multiple constant strings together instead of a typechecked
 * and not-yet-working as of C++17 set of constexpr std::string concatenations necessary to make this string. When the
 * constexpr stuff is working better in C++20/23 this may be revised.
 */
#define KH_PRIV_CHECK_REQUIRES_SIZE(NUM, TYPE, TYPEDESC)                                                   \
static_assert(Limits<TYPE>::digits == NUM,                                                                 \
              "Compilation requires " TYPEDESC " type but this compiler or platform does not support it.")
//! \endcond
namespace KirHut
{

#if KH_MUST_HAVE_I8_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(7, i8, "signed 8-bit");
#endif
#if KH_MUST_HAVE_U8_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(8, u8, "unsigned 8-bit");
#endif
#if KH_MUST_HAVE_I16_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(15, i16, "signed 16-bit");
#endif
#if KH_MUST_HAVE_U16_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(16, u16, "unsigned 16-bit");
#endif
#if KH_MUST_HAVE_I32_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(31, i32, "signed 32-bit");
#endif
#if KH_MUST_HAVE_U32_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(32, u32, "unsigned 32-bit");
#endif
#if KH_MUST_HAVE_I64_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(63, i64, "signed 64-bit");
#endif
#if KH_MUST_HAVE_U64_TYPE
KH_PRIV_CHECK_REQUIRES_SIZE(64, u64, "unsigned 64-bit");
#endif

} // namespace KirHut
