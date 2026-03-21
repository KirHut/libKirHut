/***********************************************************************************************************************
** The KirHut Application Development Library
** TestPlatform.cpp
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
#include "kh/platform.hpp"

#include <catch2/catch_test_macros.hpp>
// clazy:excludeall=non-pod-global-static

using namespace KirHut;

consteval bool sizesCheck()
{
    using namespace KirHut::Platform;
    // clang-format off
    return Limits<i8>::max()      == i8Max 
       and Limits<i16>::max()     == i16Max
       and Limits<i32>::max()     == i32Max
       and Limits<i64>::max()     == i64Max
       and Limits<i128>::max()    == i128Max
       and Limits<iWidest>::max() == iWidestMax
       and Limits<i8>::min()      == i8Min
       and Limits<i16>::min()     == i16Min
       and Limits<i32>::min()     == i32Min
       and Limits<i64>::min()     == i64Min
       and Limits<i128>::min()    == i128Min
       and Limits<iWidest>::min() == iWidestMin
       and Limits<u8>::max()      == u8Max
       and Limits<u16>::max()     == u16Max
       and Limits<u32>::max()     == u32Max
       and Limits<u64>::max()     == u64Max
       and Limits<u128>::max()    == u128Max
       and Limits<uWidest>::max() == uWidestMax
       and Limits<e8>::max()      == e8Max
       and Limits<e16>::max()     == e16Max
       and Limits<e32>::max()     == e32Max
       and Limits<e64>::max()     == e64Max
       and Limits<e8>::min()      == e8Min
       and Limits<e16>::min()     == e16Min
       and Limits<e32>::min()     == e32Min
       and Limits<e64>::min()     == e64Min
       and Limits<ue8>::max()     == ue8Max
       and Limits<ue16>::max()    == ue16Max
       and Limits<ue32>::max()    == ue32Max
       and Limits<ue64>::max()    == ue64Max;
    // clang-format on
}

consteval bool hasTypesCheck()
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

TEST_CASE("Ensure MIN and MAX sizes are correct", "[platform][Limits]")
{
    STATIC_REQUIRE(sizesCheck());
}

TEST_CASE("Ensure type info is correct", "[platform][Limits]")
{
    STATIC_REQUIRE(hasTypesCheck());
}
