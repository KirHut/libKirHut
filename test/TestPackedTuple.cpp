/***********************************************************************************************************************
** The KirHut Application Development Library
** TestPackedTuple.cpp
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
#include "kh/packedtuple.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
// clazy:excludeall=non-pod-global-static

using namespace KirHut;

template <typename PT>
constexpr bool constexpr_roundtrip()
{
    PT p;
    p.template set<0>(true);
    p.template set<1>(123);
    p.template set<2>(-56);
    p.template set<3>(false);
    return p.template get<0>() and p.template get<1>() == 123 and p.template get<2>() == -56 and
           not p.template get<3>();
}

TEST_CASE("Basic constant sanity checks for BitFields", "[packedtuple][BitField]")
{
    SECTION("BF:Bool sanity checks")
    {
        STATIC_REQUIRE(BF::Bool::bits() == 1);
        STATIC_REQUIRE(BF::Bool::digits() == 1);
        STATIC_REQUIRE(BF::Bool::min() == false);
        STATIC_REQUIRE(BF::Bool::max() == true);
        STATIC_REQUIRE(std::same_as<bool, BF::Bool::Int>);
    }

    STATIC_REQUIRE(BF::U8<8>::bits() == 8);
    STATIC_REQUIRE(BF::U8<8>::digits() == 8);
    STATIC_REQUIRE(BF::U8<8>::min() == 0);
    STATIC_REQUIRE(BF::U8<8>::max() == 255);
    STATIC_REQUIRE(BF::U8<7>::bits() == 7);
    STATIC_REQUIRE(BF::U8<7>::digits() == 7);
    STATIC_REQUIRE(BF::U8<7>::min() == 0);
    STATIC_REQUIRE(BF::U8<7>::max() == 127);

    STATIC_REQUIRE(BF::I8<8>::bits() == 8);
    STATIC_REQUIRE(BF::I8<8>::digits() == 7);
    STATIC_REQUIRE(BF::I8<8>::min() == -128);
    STATIC_REQUIRE(BF::I8<8>::max() == 127);
    STATIC_REQUIRE(BF::I8<7>::bits() == 7);
    STATIC_REQUIRE(BF::I8<7>::digits() == 6);
    STATIC_REQUIRE(BF::I8<7>::min() == -64);
    STATIC_REQUIRE(BF::I8<7>::max() == 63);
}

TEST_CASE("Basic single-block packing/unpacking", "[packedtuple][PackedTuple]")
{
    using PT = PackedTuple<BF::Bool, BF::I32<10>, BF::U32<6>>;

    PT pack{ true, 511, 63 };

    REQUIRE(pack.get<0>());
    REQUIRE(pack.get<1>() == 511);
    REQUIRE(pack.get<2>() == 63);

    pack.set<1>(-1);
    REQUIRE(pack.get<1>() == -1);
}

TEST_CASE("PackedTuple constructor with braced initializer list", "[packedtuple][PackedTuple]")
{
    using PT = PackedTuple<BF::U32<10>, BF::Bool, BF::Bool, BF::I64<52>>;

    PT data{ 512u, true, false, -375'000 };

    REQUIRE(data.get<0>() == 512u);
    REQUIRE(data.get<1>());
    REQUIRE_FALSE(data.get<2>());
    REQUIRE(data.get<3>() == -375'000);

    STATIC_REQUIRE(sizeof(PT) == sizeof(u64));
}

TEST_CASE("PackedTuple constructor from std::tuple", "[packedtuple][PackedTuple]")
{
    using T = PackedTuple<BF::U32<10>, BF::Bool, BF::Bool, BF::I64<52>>;

    auto tup = std::tuple{ 123u, false, true, -99'999LL };
    T data{ tup };

    REQUIRE(data.get<0>() == 123u);
    REQUIRE_FALSE(data.get<1>());
    REQUIRE(data.get<2>());
    REQUIRE(data.get<3>() == -99'999LL);
}

TEST_CASE("Cross-block packing/unpacking unsigned", "[packedtuple][PackedTuple32]")
{
    using PT = PackedTuple32<BF::U32<30>, BF::U32<10>>;

    PT pack;
    pack.set<0>(0x3f'ff'ff'ffU); // 30 bits all ones
    pack.set<1>(0x3ffU); // 10 bits all ones

    REQUIRE(pack.get<0>() == 0x3f'ff'ff'ffU);
    REQUIRE(pack.get<1>() == 0x3ffU);

    pack.set<0>(0);
    pack.set<1>(512);
    REQUIRE(pack.get<0>() == 0);
    REQUIRE(pack.get<1>() == 512);
}

TEST_CASE("Cross-block packing/unpacking signed", "[packedtuple][PackedTuple32]")
{
    using PT = PackedTuple32<BF::I32<29>, BF::I32<9>>;

    PT pack;
    pack.set<0>(-123'456);
    pack.set<1>(-123);

    REQUIRE(pack.get<0>() == -123'456);
    REQUIRE(pack.get<1>() == -123);

    pack.set<0>(123'456);
    pack.set<1>(123);
    REQUIRE(pack.get<0>() == 123'456);
    REQUIRE(pack.get<1>() == 123);
}

TEST_CASE("Boolean packing in multi-field tuple", "[packedtuple][PackedTuple8]")
{
    using PT = PackedTuple8<BF::Bool, BF::Bool, BF::Bool, BF::Bool, BF::U8<4>>;

    PT pack;
    pack.set<0>(true);
    pack.set<1>(false);
    pack.set<2>(true);
    pack.set<3>(false);
    pack.set<4>(9U);

    REQUIRE(pack.get<0>());
    REQUIRE_FALSE(pack.get<1>());
    REQUIRE(pack.get<2>());
    REQUIRE_FALSE(pack.get<3>());
    REQUIRE(pack.get<4>() == 9U);
    STATIC_REQUIRE(sizeof(PT) == sizeof(u8));
}

TEST_CASE("Boolean packing in multi-field tuple (constexpr)", "[constexpr][packedtuple][PackedTuple8]")
{
    using PT = PackedTuple8<BF::Bool, BF::Bool, BF::Bool, BF::Bool, BF::U8<4>>;

    constexpr PT pack{ true, false, true, false, 9u };

    STATIC_REQUIRE(pack.get<0>());
    STATIC_REQUIRE_FALSE(pack.get<1>());
    STATIC_REQUIRE(pack.get<2>());
    STATIC_REQUIRE_FALSE(pack.get<3>());
    STATIC_REQUIRE(pack.get<4>() == 9U);
    STATIC_REQUIRE(sizeof(PT) == sizeof(u8));
}

TEST_CASE("Round-trip integrity across complex layout", "[packedtuple][PackedTuple32]")
{
    using PT = PackedTuple32<BF::Bool, BF::I32<19>, BF::I32<22>, BF::Bool, BF::Bool, BF::U32<20>>;

    PT testPack;
    testPack.set<0>(true);
    testPack.set<1>(123'456);
    testPack.set<2>(-123'456);
    testPack.set<3>(false);
    testPack.set<4>(true);
    testPack.set<5>(1'048'575U); // max for 20 bits

    REQUIRE(testPack.get<0>() == true);
    REQUIRE(testPack.get<1>() == 123'456);
    REQUIRE(testPack.get<2>() == -123'456);
    REQUIRE(testPack.get<3>() == false);
    REQUIRE(testPack.get<4>() == true);
    REQUIRE(testPack.get<5>() == 1'048'575U);

    STATIC_REQUIRE(sizeof(testPack) == sizeof(u32) * 2);
}

TEST_CASE("PackedTuple works in constexpr contexts", "[packedtuple][PackedTuple]")
{
    using PT = PackedTuple<BF::Bool, BF::I32<12>, BF::I32<8>, BF::U32<6>>;

    STATIC_REQUIRE(constexpr_roundtrip<PT>());

    constexpr PT p{ true, 2047, -12, 63 };

    STATIC_REQUIRE(p.get<0>() == true);
    STATIC_REQUIRE(p.get<1>() == 2047);
    STATIC_REQUIRE(p.get<2>() == -12);
    STATIC_REQUIRE(p.get<3>() == 63);
}

TEMPLATE_TEST_CASE("PackedTuple cross-block and signed behavior is correct",
                   "[packedtuple][BasicPackedTuple]",
                   u8,
                   u16,
                   u32,
                   u64)
{
    using Block = TestType;

    using T = BasicPackedTuple<Block, BF::U8<3>, BF::I8<4>, BF::U8<5>, BF::Bool, BF::U8<7>>;
    T tup{};
    tup.template set<0>(5);
    tup.template set<1>(-3);
    tup.template set<2>(17);
    tup.template set<3>(true);
    tup.template set<4>(92);

    REQUIRE(tup.template get<0>() == 5);
    REQUIRE(tup.template get<1>() == -3);
    REQUIRE(tup.template get<2>() == 17);
    REQUIRE(tup.template get<3>());
    REQUIRE(tup.template get<4>() == 92);

    if constexpr (sizeof(Block) >= 4)
    {
        using T = BasicPackedTuple<Block, BF::Bool, BF::I32<19>, BF::I32<22>, BF::Bool, BF::Bool, BF::U32<20>>;

        T tup{};
        tup.template set<0>(true);
        REQUIRE(tup.template get<0>());

        tup.template set<1>(123'456);
        REQUIRE(tup.template get<1>() == 123'456);

        tup.template set<2>(-123'456);
        REQUIRE(tup.template get<2>() == -123'456);

        tup.template set<3>(false);
        tup.template set<4>(true);
        REQUIRE_FALSE(tup.template get<3>());
        REQUIRE(tup.template get<4>());

        tup.template set<5>(0xF'FF'FF);
        REQUIRE(tup.template get<5>() == 0xF'FF'FF);

        STATIC_REQUIRE(sizeof(tup) == bytesNeededForBits(64));
    }
}
