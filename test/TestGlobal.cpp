/***********************************************************************************************************************
** The KirHut Application Development Library
** TestGlobal.cpp
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

#include "kh/platform.hpp"

#include <catch2/catch_test_macros.hpp>
// clazy:excludeall=non-pod-global-static

using namespace KirHut;

constexpr bool sizesCheck()
{
    using namespace KirHut::Platform;
    // clang-format off
    return Limits<i8>::max()      == i8Max 
	    && Limits<i16>::max()     == i16Max
		&& Limits<i32>::max()     == i32Max
		&& Limits<i64>::max()     == i64Max
		&& Limits<iWidest>::max() == iWidestMax
		&& Limits<i8>::min()      == i8Min
	    && Limits<i16>::min()     == i16Min
		&& Limits<i32>::min()     == i32Min
		&& Limits<i64>::min()     == i64Min
		&& Limits<iWidest>::min() == iWidestMin
		&& Limits<u8>::max()      == u8Max
	    && Limits<u16>::max()     == u16Max
		&& Limits<u32>::max()     == u32Max
		&& Limits<u64>::max()     == u64Max
		&& Limits<uWidest>::max() == uWidestMax
		&& Limits<e8>::max()      == e8Max
	    && Limits<e16>::max()     == e16Max
		&& Limits<e32>::max()     == e32Max
		&& Limits<e64>::max()     == e64Max
		&& Limits<e8>::min()      == e8Min
	    && Limits<e16>::min()     == e16Min
		&& Limits<e32>::min()     == e32Min
		&& Limits<e64>::min()     == e64Min
		&& Limits<ue8>::max()     == ue8Max
	    && Limits<ue16>::max()    == ue16Max
		&& Limits<ue32>::max()    == ue32Max
		&& Limits<ue64>::max()    == ue64Max;
    // clang-format on
}

constexpr bool hasTypesCheck()
{
    using namespace KirHut::Platform;
    // clang-format off
	return i8Is8Bits               == (Limits<i8>::digits  == 7)
       and i16Is16Bits             == (Limits<i16>::digits == 15)
       and i32Is32Bits             == (Limits<i32>::digits == 31)
       and i64Is64Bits             == (Limits<i64>::digits == 63)
       and u8Is8Bits               == (Limits<u8>::digits  == 8)
       and u16Is16Bits             == (Limits<u16>::digits == 16)
       and u32Is32Bits             == (Limits<u32>::digits == 32)
       and u64Is64Bits             == (Limits<u64>::digits == 64)
       and reallyHas8BitTypes      == (i8Is8Bits and u8Is8Bits)
	   and reallyHas16BitTypes     == (i16Is16Bits and u16Is16Bits)
	   and reallyHas32BitTypes     == (i32Is32Bits and u32Is32Bits)
	   and reallyHas64BitTypes     == (i64Is64Bits and u64Is64Bits)
       and i3264Are3264Bits        == (i32Is32Bits and i64Is64Bits)
	   and u3264Are3264Bits        == (u32Is32Bits and u64Is64Bits)
       and reallyHas3264BitTypes   == (reallyHas32BitTypes and reallyHas64BitTypes)
       and i163264Are163264Bits    == (i16Is16Bits and i3264Are3264Bits)
	   and u163264Are163264Bits    == (u16Is16Bits and u3264Are3264Bits)
       and reallyHas163264BitTypes == (reallyHas16BitTypes and reallyHas3264BitTypes);
    // clang-format on
}

TEST_CASE("Ensure MIN and MAX sizes are correct", "[global]")
{
	STATIC_REQUIRE(sizesCheck());
}

TEST_CASE("Ensure type info is correct", "[global]")
{
	STATIC_REQUIRE(hasTypesCheck());
}
