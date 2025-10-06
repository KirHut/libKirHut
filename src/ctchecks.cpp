/***********************************************************************************************************************
** The KirHut Application Development Library
** ctchecks.cpp
** Copyright © KirHut Software Company
**
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
** License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
** version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
** warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
** details.
**
** You should have received a copy of the GNU General Public License along with this program.  If not, see
** <http://www.gnu.org/licenses/>.
***********************************************************************************************************************/
#include "kh/base.hpp"

using namespace KirHut;

#if KH_USE_128BIT_TYPES and not defined(__SIZEOF_INT128__)
static_assert(false,
              "libKirHut is being built with 128-bit integer support, but the compiler or platform does not seem to "
              "support that type (as __SIZEOF_INT128__ is not defined). In order to build with 128-bit integers, the "
              "compiler and/or platform must support that type. Please use an up-to-date version of GCC or Clang to "
              "compile this library or another compiler based on an up-to-date version of one of those two compilers.");
#endif // KH_USE_128BIT_TYPES and not defined(__SIZEOF_INT128__)

#if CHAR_BIT != 8
static_assert(false, "libKirHut does not support platforms that do not have 8-bit bytes.");
#endif

// I know of no mixed endian systems in modern use, and this library does not support single type processors.
static_assert(Platform::BigEndian or Platform::LittleEndian, "libKirHut does not support mixed endian systems.");
static_assert(not Platform::BigEndian or not Platform::LittleEndian,
              "libKirHut does not support all integer values being a single byte.");

// I know of no systems in modern use that do not use two's complement binary representations of integers, but if for
// some bizarre reason this libary is built on one, it should be made clear this is not supported.
static_assert(std::bit_cast<std::make_unsigned_t<decltype(0)>>(0) ==
                  std::bit_cast<std::make_unsigned_t<decltype(0)>>(-0),
              "libKirHut does not support compilation to one's complement or sign-magnitude platforms.");

/*
 * There's a need to check the preprocessor defines in the compiler, as it is possible both for a compiler to
 * inadvertently define multiple cores and for a user to define cores themselves (though this shouldn't be supported at
 * all). This header should simply fail to compile when one of these situations occurs.
 *
 * The first set of failure checks are for the hardware ISA, then for the OS platform.
 */

// TODO: Include Hardware ISA checks.
template <size_t N>
consteval bool onlyOneOf(array<bool, N> const &vals)
{
    bool found = false;
    for (bool val : vals)
    {
        if (val)
        {
            if (found)
            {
                return false;
            }
            else
            {
                found = true;
            }
        }
    }

    return found;
}

consteval bool onlyOnePlatform()
{
    using namespace KirHut::Platform;
    array platforms{ windows, macOS, linux, wasm, iPhone, android };
    return onlyOneOf(platforms);
}

consteval bool onlyOneCompiler()
{
    using namespace KirHut::Build;
    array compilers{ gcc, msvc, clang, appleClang, intel, openxl, nvcxx, cray, armClang };
    return onlyOneOf(compilers);
}

#if KH_USES_QT and not defined(KH_QT_VERSION)
static_assert(false, "libKirHut is set to compile with Qt, but no Qt version was detected.");
#endif

#if not KH_OVERRIDE_PLATFORM_SAFETY
static_assert(onlyOnePlatform(), "libKirHut cannot be built for more than one target platform.");
static_assert(not Build::unknownCompiler,
              "libKirHut could not successfully detect the compiler used. Compilers currently correctly detected are:\n"
              " - GCC\n - MSVC\n - Clang\n - Apple Clang\n - HPC Cray CCE\n - IBM Open XL\n - Intel oneAPI\n"
              " - ARM Clang\n - NVidia HPC SDK\nIf you want to attempt compiling anyway, define "
              "KH_OVERRIDE_PLATFORM_SAFETY.");
# if defined(KH_QT_VERSION)
static_assert(KH_QT_VERSION, "libKirHut does not support building for versions of Qt lower than Qt 5.15.");
# endif
#endif

#if not KH_COMPILED_WITH_UNKNOWN
static_assert(onlyOneCompiler(),
              "libKirHut has identified two compilers being used, which should not be possible. Please report this as "
              "a bug to the developers at the libKirHut GitHub repository!");
#endif

/*
 * I use a macro here instead of a constexpr template method because I need the compilation time string literal
 * concatenation abilities of a preprocessor simply putting multiple constant strings together. This requires something
 * like std::string and some constexpr wizardry to make sure it works correctly, but this header deliberately does not
 * include the <string> header. As such, this simply goes with a macro, and all KH_PRIV_* macros are assumed to be
 * unusable anyway for all library users.
 */
#define KH_PRIV_CHECK_REQUIRES_SIZE(COND, TYPEDESC) \
    static_assert(COND, "Compilation requires " TYPEDESC " type but this compiler or platform does not support it.")

// 8-bit types are always required.
KH_PRIV_CHECK_REQUIRES_SIZE(Platform::i8Is8Bits, "signed 8-bit");
KH_PRIV_CHECK_REQUIRES_SIZE(Platform::u8Is8Bits, "unsigned 8-bit");
#if KH_MUST_HAVE_16BIT_TYPES
KH_PRIV_CHECK_REQUIRES_SIZE(Platform::i16Is16Bits, "signed 16-bit");
#endif
#if KH_MUST_HAVE_16BIT_TYPES
KH_PRIV_CHECK_REQUIRES_SIZE(Platform::u16Is16Bits, "unsigned 16-bit");
#endif
#if KH_MUST_HAVE_32BIT_TYPES
KH_PRIV_CHECK_REQUIRES_SIZE(Platform::i32Is32Bits, "signed 32-bit");
#endif
#if KH_MUST_HAVE_32BIT_TYPES
KH_PRIV_CHECK_REQUIRES_SIZE(Platform::u32Is32Bits, "unsigned 32-bit");
#endif
#if KH_MUST_HAVE_64BIT_TYPES
KH_PRIV_CHECK_REQUIRES_SIZE(Platform::i64Is64Bits, "signed 64-bit");
#endif
#if KH_MUST_HAVE_64BIT_TYPES
KH_PRIV_CHECK_REQUIRES_SIZE(Platform::u64Is64Bits, "unsigned 64-bit");
#endif

constexpr array<byte, sizeof(u32)> privTestArray = { byte{'\x12'}, byte{'\x34'}, byte{'\x56'}, byte{'\x78'} };

static_assert(Detail::fromEndian<u32, std::endian::big>(privTestArray.data()) == static_cast<u32>(0x12345678),
              "libKirHut's fromEndian method failed a required static assertion. Please report this as a bug to "
              "the KirHut library developers!");
static_assert(Detail::fromEndian<u32, std::endian::little>(privTestArray.data()) == static_cast<u32>(0x78563412),
              "libKirHut's fromEndian method failed a required static assertion. Please report this as a bug to "
              "the KirHut library developers!");

consteval bool toEndianTest() noexcept
{
    array<byte, sizeof(u32)> destArray;

    byte *end = Detail::toEndian<std::endian::big>(static_cast<u32>(0x12345678), destArray.data());
    if (destArray != privTestArray or end != destArray.data() + sizeof(u32))
    {
        return false;
    }

    end = Detail::toEndian<std::endian::little>(static_cast<u32>(0x78563412), destArray.data());
    return destArray == privTestArray and end == destArray.data() + sizeof(u32);
}

static_assert(toEndianTest(),
              "libKirHut's toEndian method failed a required static assertion. Please report this as a bug to "
              "the KirHut library developers!");

int main()
{
    return 0;
}
