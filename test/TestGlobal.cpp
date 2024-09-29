/***********************************************************************************************************************
** The KirHut Application Development Library
** TestGlobal.cpp
** Copyright (C) 2024 KirHut Software Company
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

#include "kh/global.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <algorithm>

using namespace KirHut;

constexpr bool sizesCheck()
{
	// clang-format off
	return Limits<i8>::max()      == I8_MAX 
	    && Limits<i16>::max()     == I16_MAX
		&& Limits<i32>::max()     == I32_MAX
		&& Limits<i64>::max()     == I64_MAX
		&& Limits<iWidest>::max() == IWIDEST_MAX
		&& Limits<i8>::min()      == I8_MIN
	    && Limits<i16>::min()     == I16_MIN
		&& Limits<i32>::min()     == I32_MIN
		&& Limits<i64>::min()     == I64_MIN
		&& Limits<iWidest>::min() == IWIDEST_MIN
		&& Limits<u8>::max()      == U8_MAX
	    && Limits<u16>::max()     == U16_MAX
		&& Limits<u32>::max()     == U32_MAX
		&& Limits<u64>::max()     == U64_MAX
		&& Limits<uWidest>::max() == UWIDEST_MAX
		&& Limits<e8>::max()      == E8_MAX
	    && Limits<e16>::max()     == E16_MAX
		&& Limits<e32>::max()     == E32_MAX
		&& Limits<e64>::max()     == E64_MAX
		&& Limits<e8>::min()      == E8_MIN
	    && Limits<e16>::min()     == E16_MIN
		&& Limits<e32>::min()     == E32_MIN
		&& Limits<e64>::min()     == E64_MIN
		&& Limits<ue8>::max()     == UE8_MAX
	    && Limits<ue16>::max()    == UE16_MAX
		&& Limits<ue32>::max()    == UE32_MAX
		&& Limits<ue64>::max()    == UE64_MAX;
	// clang-format on
}

constexpr bool platformCheck()
{
	using namespace KirHut::Platform;
	auto platforms = std::array { windows, linux, apple, android };
	bool once = false;
	auto comp = [&once](bool plat)
	{
		if (plat)
		{
			if (once)
				return false;
			once = true;
		}
		
		return true;
	};
	
	// The apple and mobile checks are at the end because this will ensure the array is checked first regardless of
	// the results of the apple and mobile platform checks.
	return std::all_of(platforms.begin(), platforms.end(), comp) &&
		   (apple ? macOS != iPhone : true) &&
		   (mobile ? iPhone != android : true);
}

constexpr bool hasTypesCheck()
{
	using namespace KirHut::Build;
	// clang-format off
	return hasI8       == (Limits<i8>::digits  == 7)
	    && hasI16      == (Limits<i16>::digits == 15)
	    && hasI32      == (Limits<i32>::digits == 31)
	    && hasI64      == (Limits<i64>::digits == 63)
	    && hasU8       == (Limits<u8>::digits  == 8)
		&& hasU16      == (Limits<u16>::digits == 16)
		&& hasU32      == (Limits<u32>::digits == 32)
		&& hasU64      == (Limits<u64>::digits == 64)
		&& has8        == (hasI8  && hasU8)
		&& has16       == (hasI16 && hasU16)
		&& has32       == (hasI32 && hasU32)
		&& has64       == (hasI64 && hasU64)
		&& hasI832     == (hasI8 && hasI32)
		&& hasU832     == (hasU8 && hasU32)
		&& has832      == (has8 && has32)
		&& hasI83264   == (hasI832 && hasI64)
		&& hasU83264   == (hasU832 && hasU64)
		&& has83264    == (has832 && has64)
		&& hasI8163264 == (hasI83264 && hasI16)
		&& hasU8163264 == (hasU83264 && hasU16)
		&& has8163264  == (has83264 && has16);
	// clang-format on
}

TEST_CASE("Ensure MIN and MAX sizes are correct", "[global][asserted]")
{
	STATIC_REQUIRE(sizesCheck());
}

TEST_CASE("Ensure platform info is correct", "[global][asserted]")
{
	STATIC_REQUIRE(platformCheck());
}

TEST_CASE("Ensure type info is correct", "[global][asserted]")
{
	STATIC_REQUIRE(hasTypesCheck());
}
