/***********************************************************************************************************************
** The KirHut Application Development Library
** base.cpp
** Copyright © KirHut Software Company
**
** Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
** the License. You may obtain a copy of the License at
**
**   http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
** an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
** specific language governing permissions and limitations under the License.
***********************************************************************************************************************/
#include "kh/base.hpp"

#include "kh/errors.hpp"

#include <chrono>
#include <cassert>
#include <numeric>

namespace KirHut
{

void Detail::throwTooSmallSpan(string_view message)
{
    throw IllegalArgument(message);
}

u64 v1::currentTicks() noexcept
{
    return static_cast<u64>(std::chrono::steady_clock::now().time_since_epoch().count());
}

/*
 * There's a need to check the preprocessor defines in the compiler, as it is possible both for a compiler to
 * inadvertently define multiple cores and for a user to define cores themselves (though this shouldn't be supported at
 * all). This header should simply fail to compile when one of these situations occurs.
 *
 * The first set of failure checks are for the hardware ISA, then for the OS platform.
 */

consteval int arrayCount(auto const &arr)
{
    return std::accumulate(arr.begin(), arr.end(), 0, [](int acc, bool arrBool) { return acc + (arrBool ? 1 : 0); });
}

consteval int platformCount()
{
    using namespace KirHut::Platform;
    array platforms{ windows, macOS, linuxos, wasm, iPhone, android };
    return arrayCount(platforms);
}

consteval int hardwareIsaCount()
{
    return 1;
}

consteval int compilerCount()
{
    using namespace KirHut::Build;
    array compilers{ gcc, msvc, clang, appleClang, intel, openxl, nvcxx, cray, armClang };
    return arrayCount(compilers);
}

#if not KH_OVERRIDE_PLATFORM_SAFETY

# if defined(KH_USE_128BIT_TYPES) and not defined(__SIZEOF_INT128__)
static_assert(false,
              "libKirHut is being built with 128-bit integer support, but the compiler or platform does not seem to "
              "support that type (as __SIZEOF_INT128__ is not defined). In order to build with 128-bit integers, the "
              "compiler and/or platform must support that type. Please use an up-to-date version of GCC or Clang to "
              "compile this library or another compiler based on an up-to-date version of one of those two compilers.");
# endif // defined(KH_USE_128BIT_TYPES) and not defined(__SIZEOF_INT128__)

// I know of no platforms that support C++20 and have bytes that aren't 8 bits, but this checks for that anyway.
static_assert(Platform::reallyHas8BitTypes, "libKirHut does not support platforms that do not have 8-bit bytes.");

// I know of no mixed endian systems in modern use, and this library does not support single type processors.
static_assert(Platform::BigEndian or Platform::LittleEndian, "libKirHut does not support mixed endian systems.");
static_assert(not Platform::BigEndian or not Platform::LittleEndian,
              "libKirHut does not support all integer values being a single byte.");

// I know of no systems in modern use that do not use two's complement binary representations of integers, but if for
// some bizarre reason this libary is built on one, it should be made clear this is not supported.
static_assert(0u == std::bit_cast<unsigned int>(-0),
              "libKirHut does not support compilation to one's complement or sign-magnitude platforms.");

static_assert(platformCount() < 2,
              "libKirHut has detected more than one target platform being built for. If, for some reason, this is "
              "intentional, than please set KH_OVERRIDE_PLATFORM_SAFETY before building. If not, please report this "
              "as a bug to the libKirHut GitHub repository!");
static_assert(platformCount() > 0,
              "libKirHut has not detected a target platform being built for. If this is intentional, than please set "
              "KH_OVERRIDE_PLATFORM_SAFETY before building. If not, please report this as a bug to the libKirHut "
              "GitHub repository!");

static_assert(not Build::unknownCompiler,
              "libKirHut could not successfully detect the compiler used. Compilers currently correctly detected are:\n"
              " - GCC\n - MSVC\n - Clang\n - Apple Clang\n - HPC Cray CCE\n - IBM Open XL\n - Intel ICX oneAPI\n"
              " - ARM Clang\n - NVidia HPC SDK\n - NVidia CUDA\n - QNX\n - Wind River Diab\n - Green Hills\n - Elbrus\n"
              "If you want to attempt compiling anyway, define KH_OVERRIDE_PLATFORM_SAFETY.");

# if not defined(KH_COMPILED_WITH_UNKNOWN)
static_assert(compilerCount() < 2,
              "libKirHut has identified two compilers being used, which should not be possible. Please report this as "
              "a bug to the developers at the libKirHut GitHub repository!");

static_assert(compilerCount() > 0,
              "libKirHut has both not identified a compiler and not set the compiler as unknown, which should not be"
              "possible. Please report this as a bug to the developers at the libKirHut GitHub repository!");
# endif // not defined(KH_COMPILED_WITH_UNKNOWN)

#endif // not KH_OVERRIDE_PLATFORM_SAFETY

/*
 * I use a macro here instead of a constexpr template method because I need the compilation time string literal
 * concatenation abilities of a preprocessor simply putting multiple constant strings together. This requires something
 * like std::string and some constexpr wizardry to make sure it works correctly, but this header deliberately does not
 * include the <string> header. As such, this simply goes with a macro, and all KH_PRIV_* macros are assumed to be
 * unusable anyway for all library users.
 */
#define KH_PRIV_CHECK_REQUIRES_SIZE(COND, TYPEDESC) \
    static_assert(COND, "Compilation requires " TYPEDESC " types but this compiler or platform does not support it.")

#if KH_MUST_HAVE_16BIT_TYPES
KH_PRIV_CHECK_REQUIRES_SIZE(Platform::reallyHas16BitTypes, "16-bit");
#endif
#if KH_MUST_HAVE_32BIT_TYPES
KH_PRIV_CHECK_REQUIRES_SIZE(Platform::reallyHas32BitTypes, "32-bit");
#endif
#if KH_MUST_HAVE_64BIT_TYPES
KH_PRIV_CHECK_REQUIRES_SIZE(Platform::reallyHas64BitTypes, "64-bit");
#endif

} // namespace KirHut
