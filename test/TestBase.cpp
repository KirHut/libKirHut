/***********************************************************************************************************************
** The KirHut Application Development Library
** TestBase.cpp
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

#include "kh/base.hpp"
#include "kh/ranges.hpp"

#include <catch2/catch_test_macros.hpp>
// clazy:excludeall=non-pod-global-static

using namespace KirHut;

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

TEST_CASE("Bytes needed for bits", "[utility]")
{
	// KirHut stuff does not support non-8-bit bytes as of right now.
	STATIC_REQUIRE(bytesNeededForBits(8) == 1);
	STATIC_REQUIRE(bytesNeededForBits(15) == 2);
	STATIC_REQUIRE(bytesNeededForBits(30) == 4);
	STATIC_REQUIRE(bytesNeededForBits(60) == 8);

    // Always check both static and dynamic behavior.
    REQUIRE(bytesNeededForBits(8) == 1);
    REQUIRE(bytesNeededForBits(15) == 2);
    REQUIRE(bytesNeededForBits(30) == 4);
    REQUIRE(bytesNeededForBits(60) == 8);
}

TEST_CASE("The asBytes() and asWritableBytes() functions", "[utility]")
{
    u32 testVal    = 0x12'34'56'78;
    auto compareTo = std::bit_cast<array<byte, sizeof(u32)>>(testVal);

    REQUIRE(R::equal(compareTo, asBytes(testVal)));
    auto writableTestVal2 = asWritableBytes(testVal);
    writableTestVal2[2]   = byte{ 0x9A };
    compareTo[2]          = byte{ 0x9A };
    REQUIRE(R::equal(compareTo, writableTestVal2));
}

TEST_CASE("Numeric concept constraints", "[base]")
{
    STATIC_REQUIRE_FALSE(Numeric<bool>);
    STATIC_REQUIRE(Numeric<double>);
    STATIC_REQUIRE(Numeric<unsigned int>);
    STATIC_REQUIRE_FALSE(Numeric<double *>);
}

TEST_CASE("The R::getIters() method in ranges.hpp", "[ranges]")
{
    std::vector<int> nums{ 1, 2, 3, 4, 5 };
    auto [f, b] = R::getIters(nums);
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
