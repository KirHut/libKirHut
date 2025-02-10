/***********************************************************************************************************************
** The KirHut Application Development Library
** TestBase.cpp
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
#include "kh/ranges.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace KirHut;

TEST_CASE("No Message Invalid Construction State", "[invalid][constructor]")
{
    Invalid in(WhyInvalid::NotFound);
    REQUIRE(in.why() == WhyInvalid::NotFound);
    REQUIRE(in.info().empty());
}

TEST_CASE("With Message Invalid Construction State", "[invalid][constructor]")
{
	char const *cnstCharMsgPtr = "Message of const char.";
	string strMessage = "Message of string.";
	string_view svMessage = "Message of string_view.";
	
	Invalid cnstCharInvalid{ WhyInvalid::NotFound,    cnstCharMsgPtr };
	Invalid strInvalid     { WhyInvalid::CantMatch,   std::move(strMessage) };
	Invalid strViewInvalid { WhyInvalid::CouldntOpen, svMessage };

    REQUIRE(cnstCharInvalid.why() == WhyInvalid::NotFound);
    REQUIRE(strInvalid.why() == WhyInvalid::CantMatch);
    REQUIRE(strViewInvalid.why() == WhyInvalid::CouldntOpen);
    REQUIRE(cnstCharInvalid.info() == "Message of const char."sv);
    REQUIRE(strInvalid.info() == "Message of string."sv);
    REQUIRE(strViewInvalid.info() == "Message of string_view."sv);
}

TEST_CASE("Invalid Copy Constuction State", "[invalid][constructor]")
{
	Invalid first{ WhyInvalid::NotFound, "First Invalid." };

    REQUIRE(first.why() == WhyInvalid::NotFound);
    REQUIRE(first.info() == "First Invalid."sv);

    Invalid second(first);

    REQUIRE(first.why() == WhyInvalid::NotFound);
    REQUIRE(first.info() == "First Invalid."sv);
    REQUIRE(second.why() == WhyInvalid::NotFound);
    REQUIRE(second.info() == "First Invalid."sv);
}

TEST_CASE("Invalid Move Construction State", "[invalid][constructor]")
{
	Invalid first{ WhyInvalid::NotFound, "First Invalid." };

    REQUIRE(first.why() == WhyInvalid::NotFound);
    REQUIRE(first.info() == "First Invalid."sv);

    Invalid second(std::move(first));

    REQUIRE(first.why() == WhyInvalid::NotFound);
    REQUIRE(first.info().empty());
    REQUIRE(second.why() == WhyInvalid::NotFound);
    REQUIRE(second.info() == "First Invalid."sv);
}

TEST_CASE("Set Message in Empty Invalid", "[invalid]")
{
    Invalid in(WhyInvalid::NotFound);
    REQUIRE(in.info().empty());
    in.setInfo("Some Message."sv);
    REQUIRE(in.info() == "Some Message."sv);
    in.setInfo("Ignored Message."sv);
    REQUIRE(in.info() == "Some Message."sv);
}

TEST_CASE("Overwrite Message in Empty Invalid", "[invalid]")
{
    Invalid in(WhyInvalid::NotFound);
    REQUIRE(in.info().empty());
    in.overwriteInfo("First Message."sv);
    REQUIRE(in.info() == "First Message."sv);
    in.overwriteInfo("Next Message."sv);
    REQUIRE(in.info() == "Next Message."sv);
}

TEST_CASE("Set Message in Occupied Invalid", "[invalid]")
{
	Invalid in{ WhyInvalid::NotFound, "Some Message." };
    REQUIRE(in.info() == "Some Message."sv);
    in.setInfo("Ignored Message."sv);
    REQUIRE(in.info() == "Some Message."sv);
}

TEST_CASE("Overwrite Message in Occupied Invalid", "[invalid]")
{
	Invalid in{ WhyInvalid::NotFound, "First Message." };
    REQUIRE(in.info() == "First Message."sv);
    in.overwriteInfo("Next Message."sv);
    REQUIRE(in.info() == "Next Message."sv);
}

struct NothrowTest
{
    NothrowTest() noexcept = default;
    NothrowTest(int id) noexcept : id(id)
    {
    }
    NothrowTest(NothrowTest const &other) noexcept = default;
    NothrowTest(NothrowTest &&other) noexcept      = default;

    int id = 0;
};

struct ThrowTest
{
    ThrowTest() = default;
    ThrowTest(bool s) : should(s)
    {
    }
    ThrowTest(ThrowTest const &other)
    {
    }
    ThrowTest(ThrowTest &&other)
    {
    }

    bool should = false;
};

TEST_CASE("Good Data MaybeInv Construction State", "[maybeinv][constructor]")
{
    STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<NothrowTest>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<NothrowTest>);
    STATIC_REQUIRE_FALSE(std::is_nothrow_copy_constructible_v<ThrowTest>);
    STATIC_REQUIRE_FALSE(std::is_nothrow_move_constructible_v<ThrowTest>);

    MaybeInv<NothrowTest> good = NothrowTest{};

    REQUIRE(good.isValid());
    REQUIRE(good);
    REQUIRE(good.failure() == nullptr);
    REQUIRE_FALSE(good.get() == nullptr);
}

TEST_CASE("MaybeInv take from valid", "[maybeinv]")
{
	MaybeInv<NothrowTest> good = NothrowTest{5};
	REQUIRE(good.isValid());
	REQUIRE_FALSE(good.get() == nullptr);

    NothrowTest alt{ 10 };
    STATIC_REQUIRE(noexcept(good.take(std::declval<NothrowTest>())));
    STATIC_REQUIRE_FALSE(noexcept(MaybeInv{ ThrowTest{} }.take()));
    STATIC_REQUIRE_FALSE(noexcept(MaybeInv{ ThrowTest{} }.take(std::declval<ThrowTest>())));

    NothrowTest res = good.take();
    REQUIRE(res.id == 5);
    REQUIRE_FALSE(good.isValid());
    REQUIRE(good.get() == nullptr);
	REQUIRE_FALSE(good.failure() == nullptr);
    REQUIRE(good.failure()->why() == WhyInvalid::DataRemoved);

    NothrowTest check = good.take(std::move(alt));
    REQUIRE(check.id == 10);
	REQUIRE_FALSE(good.isValid());
	REQUIRE(good.get() == nullptr);
	REQUIRE_FALSE(good.failure() == nullptr);
    REQUIRE(good.failure()->why() == WhyInvalid::DataRemoved);
}

TEST_CASE("MaybeInv monadic transforms good data", "[maybeinv]")
{
    MaybeInv<NothrowTest> good = NothrowTest{ 5 };
    auto tform = [](NothrowTest &nt, int another) -> MaybeInv<NothrowTest> { return NothrowTest{ another }; };
    MaybeInv<NothrowTest> next = good.then(tform, 10);
    REQUIRE(good);
    REQUIRE(next);
    REQUIRE_FALSE(good.get() == nullptr);
    REQUIRE_FALSE(next.get() == nullptr);
    REQUIRE(good.get()->id == 5);
    REQUIRE(next.get()->id == 10);
    auto diffType      = [](NothrowTest &nt) -> MaybeInv<int> { return nt.id; };
    MaybeInv<int> diff = next.then(diffType);
    REQUIRE(diff);
    REQUIRE_FALSE(diff.get() == nullptr);
    REQUIRE(*diff.get() == 10);
}

TEST_CASE("Bytes needed for bits", "[utility]")
{
	// KirHut stuff does not support non-8-bit bytes as of right now.
	STATIC_REQUIRE(bytesNeededForBits(8) == 1);
	STATIC_REQUIRE(bytesNeededForBits(15) == 2);
	STATIC_REQUIRE(bytesNeededForBits(30) == 4);
	STATIC_REQUIRE(bytesNeededForBits(60) == 8);
}

TEST_CASE("The getIters() method in ranges.hpp", "[ranges]")
{
    std::vector<int> nums{ 1, 2, 3, 4, 5 };
    auto [f, b] = getIters(nums);
    REQUIRE(f == nums.begin());
    REQUIRE(b == nums.end());
}

TEST_CASE("Character and Byte conversions", "[charconv]")
{
	NothrowTest first{20};
	alignas(NothrowTest) char second[sizeof(NothrowTest)];
	alignas(NothrowTest) byte third[sizeof(NothrowTest)];
    char const *firstPtr = std::bit_cast<char const *>(&first);
    memcpy(second, firstPtr, sizeof(NothrowTest));
    NothrowTest *secondPtr = std::launder(reinterpret_cast<NothrowTest *>(second));
	REQUIRE(secondPtr->id == 20);
    byte const *firstBytes = std::bit_cast<byte const *>(&first);
    memcpy(third, firstBytes, sizeof(NothrowTest));
    NothrowTest *thirdPtr = std::launder(reinterpret_cast<NothrowTest *>(third));
	REQUIRE(thirdPtr->id == 20);
}
