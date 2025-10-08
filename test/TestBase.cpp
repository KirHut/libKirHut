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

#include <algorithm>
#include <list>
#include <map>
#include <set>

#include <catch2/catch_test_macros.hpp>
// clazy:excludeall=non-pod-global-static

using namespace KirHut;

struct SomeObject
{
    // No members.
};

struct SomeOtherObject : public SomeObject
{
    int val = 0; // Should have data.
};

struct ThirdObject : public SomeObject
{
    float val = 0.; // other data.
};

struct ConvertibleToString
{
    operator std::string() const
    {
        return "Test String";
    }
};

struct FromInt
{
    FromInt(int)
    {
        // No further implementation.
    }
};

template <typename T>
using Vec = std::vector<T>;

TEST_CASE("Bytes needed for bits", "[base][utility]")
{
	// KirHut stuff does not support non-8-bit bytes as of right now.
	STATIC_REQUIRE(bytesNeededForBits(8) == 1);
	STATIC_REQUIRE(bytesNeededForBits(15) == 2);
	STATIC_REQUIRE(bytesNeededForBits(30) == 4);
	STATIC_REQUIRE(bytesNeededForBits(60) == 8);

    int volatile test1 = 8, test2 = 15, test3 = 30, test4 = 60;

    // Always check both static and dynamic behavior.
    REQUIRE(bytesNeededForBits(test1) == 1);
    REQUIRE(bytesNeededForBits(test2) == 2);
    REQUIRE(bytesNeededForBits(test3) == 4);
    REQUIRE(bytesNeededForBits(test4) == 8);
}

TEST_CASE("The asBytes() and asWritableBytes() functions", "[base][utility]")
{
    u32 testVal    = 0x12'34'56'78;
    auto compareTo = std::bit_cast<array<byte, sizeof(u32)>>(testVal);

    REQUIRE(R::equal(compareTo, asBytes(testVal)));
    auto writableTestVal2 = asWritableBytes(testVal);
    writableTestVal2[2]   = byte{ 0x9A };
    compareTo[2]          = byte{ 0x9A };
    REQUIRE(R::equal(compareTo, writableTestVal2));
}

TEST_CASE("Numeric concept constraints", "[base][concepts]")
{
    STATIC_REQUIRE_FALSE(Numeric<bool>);
    STATIC_REQUIRE(Numeric<double>);
    STATIC_REQUIRE(Numeric<unsigned int>);
    STATIC_REQUIRE_FALSE(Numeric<double *>);
}

TEST_CASE("Raw byteSwap() functions", "[base][utility]")
{
    // We obviously cannot use the byteSwap functions to initialize expectFlip, otherwise we aren't actually testing it.
    constexpr u16 testFlip16 = 0x1234, expectFlip16 = 0x3412;
    constexpr u32 testFlip32 = 0x12'34'56'78, expectFlip32 = 0x78'56'34'12;
    constexpr u64 testFlip64 = 0x01'23'45'67'89'AB'CD'EF, expectFlip64 = 0xEF'CD'AB'89'67'45'23'01;

    STATIC_REQUIRE(byteSwap(testFlip16) == expectFlip16);
    STATIC_REQUIRE(byteSwap(testFlip32) == expectFlip32);
    STATIC_REQUIRE(byteSwap(testFlip64) == expectFlip64);

    u16 volatile vTestFlip16 = testFlip16;
    u32 volatile vTestFlip32 = testFlip32;
    u64 volatile vTestFlip64 = testFlip64;

    REQUIRE(byteSwap(vTestFlip16) == expectFlip16);
    REQUIRE(byteSwap(vTestFlip32) == expectFlip32);
    REQUIRE(byteSwap(vTestFlip64) == expectFlip64);

#if defined(KH_USE_128BIT_TYPES)
    constexpr u128 testFlip128   = (static_cast<u128>(testFlip32) << Platform::bitsInU64) | testFlip64;
    constexpr u128 expectFlip128 = (static_cast<u128>(expectFlip64) << Platform::bitsInU64) |
                                   (static_cast<u128>(expectFlip32) << Platform::bitsInU32);

    STATIC_REQUIRE(byteSwap(testFlip128) == expectFlip128);

    u128 volatile vTestFlip128 = testFlip128;

    REQUIRE(byteSwap(vTestFlip128) == expectFlip128);
#endif
}

TEST_CASE("OneOf concept constraints", "[base][concepts]")
{
    SECTION("Basic OneOf checks between primitive types and pointer types")
    {
        STATIC_REQUIRE(OneOf<int, int>);
        STATIC_REQUIRE(OneOf<int, int, float>);
        STATIC_REQUIRE(OneOf<float, int, float, double>);
        STATIC_REQUIRE(OneOf<bool, char, bool, int>);
        STATIC_REQUIRE(OneOf<byte *, char *, unsigned char *, byte *>);

        STATIC_REQUIRE_FALSE(OneOf<int, float>);
        STATIC_REQUIRE_FALSE(OneOf<int, double, bool>);
        STATIC_REQUIRE_FALSE(OneOf<char, unsigned char, wchar_t>);
        STATIC_REQUIRE_FALSE(OneOf<void *, int *, char *>);
        STATIC_REQUIRE_FALSE(OneOf<byte *, char *, unsigned char *, byte const *>);
    }

    SECTION("Check object types and complex templated types")
    {
        STATIC_REQUIRE(OneOf<SomeObject, SomeObject, SomeOtherObject, ThirdObject>);
        STATIC_REQUIRE_FALSE(OneOf<SomeObject, SomeOtherObject, ThirdObject>);
        STATIC_REQUIRE_FALSE(OneOf<SomeOtherObject, SomeObject>);
        STATIC_REQUIRE_FALSE(OneOf<SomeOtherObject, ThirdObject>);
    }

    SECTION("Check cv-qualifiers and reference types are all distinguished")
    {
        STATIC_REQUIRE(OneOf<int const, int const>);
        STATIC_REQUIRE(OneOf<int, int const, int>);
        STATIC_REQUIRE(OneOf<int &, int &, int const &>);
        STATIC_REQUIRE(OneOf<char &, byte &, unsigned char &, char &>);

        STATIC_REQUIRE_FALSE(OneOf<int const, int>);
        STATIC_REQUIRE_FALSE(OneOf<int, int const>);
        STATIC_REQUIRE_FALSE(OneOf<int &, int>);
        STATIC_REQUIRE_FALSE(OneOf<int, int &>);
        STATIC_REQUIRE_FALSE(OneOf<char &, byte &, unsigned char &>);
        STATIC_REQUIRE_FALSE(OneOf<char &, char, byte, unsigned char>);
        STATIC_REQUIRE_FALSE(OneOf<SomeObject &, SomeObject>);
    }
}

TEST_CASE("ConvertsTo concept constraints", "[base][concepts]")
{
    SECTION("Basic sanity checks, then check numeric conversions and boolean conversions")
    {
        STATIC_REQUIRE(ConvertsTo<int, int>);
        STATIC_REQUIRE(ConvertsTo<int, double>);
        STATIC_REQUIRE(ConvertsTo<float, double>);
        STATIC_REQUIRE(ConvertsTo<char, int, float>);
        STATIC_REQUIRE(ConvertsTo<int, bool>);
        STATIC_REQUIRE(ConvertsTo<void *, bool>);

        STATIC_REQUIRE_FALSE(ConvertsTo<void, bool>);
    }

    SECTION("Check pointer conversions, including object upcasting and downcasting")
    {
        STATIC_REQUIRE(ConvertsTo<int *, void *>);
        STATIC_REQUIRE(ConvertsTo<SomeOtherObject *, SomeObject *>);
        STATIC_REQUIRE(ConvertsTo<ThirdObject *, SomeOtherObject *, SomeObject *>);
        STATIC_REQUIRE(ConvertsTo<ThirdObject *, SomeObject *, SomeOtherObject *>);
        STATIC_REQUIRE(ConvertsTo<byte *, byte const *>);
        STATIC_REQUIRE(ConvertsTo<std::nullptr_t, int *, void *>);

        STATIC_REQUIRE_FALSE(ConvertsTo<void *, int *>);
        STATIC_REQUIRE_FALSE(ConvertsTo<SomeObject *, SomeOtherObject *, ThirdObject *>);
        STATIC_REQUIRE_FALSE(ConvertsTo<byte const *, byte *>);
    }

    SECTION("Check valid and invalid object conversions")
    {
        STATIC_REQUIRE(ConvertsTo<std::unique_ptr<SomeOtherObject>, std::unique_ptr<SomeObject>>);
        STATIC_REQUIRE(ConvertsTo<ConvertibleToString, std::string>);
        STATIC_REQUIRE(ConvertsTo<int, FromInt>);

        STATIC_REQUIRE_FALSE(ConvertsTo<std::unique_ptr<int>, std::unique_ptr<double>>);
        STATIC_REQUIRE_FALSE(ConvertsTo<std::unique_ptr<int>, int *>);
        STATIC_REQUIRE_FALSE(ConvertsTo<std::string, ConvertibleToString>);
        STATIC_REQUIRE_FALSE(ConvertsTo<FromInt, int>);
    }

    SECTION("Other conversion sanity checks")
    {
        STATIC_REQUIRE(ConvertsTo<int, float, double, long double>);
        STATIC_REQUIRE_FALSE(ConvertsTo<void *, int, double>);
        STATIC_REQUIRE(ConvertsTo<int &, int const &>);
        STATIC_REQUIRE_FALSE(ConvertsTo<int const &, int &>);
        STATIC_REQUIRE(ConvertsTo<SomeObject &, SomeObject>);
    }
}

TEST_CASE("InstanceOf concept basic behavior", "[base][concepts]")
{
    SECTION("Basic checks of various template types")
    {
        STATIC_REQUIRE(InstanceOf<std::vector<int>, std::vector>);
        STATIC_REQUIRE(InstanceOf<std::list<float>, std::list>);
        STATIC_REQUIRE(InstanceOf<std::unique_ptr<int>, std::unique_ptr>);
        STATIC_REQUIRE(InstanceOf<std::shared_ptr<double>, std::shared_ptr>);
        STATIC_REQUIRE(InstanceOf<std::set<char>, std::set>);
        STATIC_REQUIRE(InstanceOf<std::vector<int>, std::list, std::vector, std::map>);
        STATIC_REQUIRE(InstanceOf<std::list<int>, std::vector, std::list>);
        STATIC_REQUIRE(InstanceOf<std::vector<std::vector<int>>, std::vector>);
        STATIC_REQUIRE(InstanceOf<std::shared_ptr<std::vector<int>>, std::shared_ptr>);
        STATIC_REQUIRE(InstanceOf<std::string, std::basic_string>);
        STATIC_REQUIRE(InstanceOf<Vec<int>, std::vector>);

        STATIC_REQUIRE_FALSE(InstanceOf<std::string, std::vector, std::list, std::unique_ptr>);
        STATIC_REQUIRE_FALSE(InstanceOf<int, std::vector>);
        STATIC_REQUIRE_FALSE(InstanceOf<std::vector<int>, std::list>);
        STATIC_REQUIRE_FALSE(InstanceOf<std::map<int, int>, std::set>);
    }

    SECTION("Reference and cv-qualification of first type have no effect on identification")
    {
        STATIC_REQUIRE(InstanceOf<std::vector<int> const, std::vector>);
        STATIC_REQUIRE(InstanceOf<std::list<float> const volatile, std::list>);
        STATIC_REQUIRE(InstanceOf<std::unique_ptr<int> volatile, std::unique_ptr>);
        STATIC_REQUIRE(InstanceOf<std::shared_ptr<double> const &, std::shared_ptr>);
        STATIC_REQUIRE(InstanceOf<std::set<char> const volatile &, std::set>);
        STATIC_REQUIRE(InstanceOf<std::vector<int> volatile &, std::list, std::vector, std::map>);
        STATIC_REQUIRE(InstanceOf<std::list<int> &, std::vector, std::list>);
        STATIC_REQUIRE(InstanceOf<std::vector<std::vector<int>> &&, std::vector>);
        STATIC_REQUIRE(InstanceOf<std::shared_ptr<std::vector<int>> const &&, std::shared_ptr>);
        STATIC_REQUIRE(InstanceOf<std::string volatile &&, std::basic_string>);
        STATIC_REQUIRE(InstanceOf<Vec<int> const volatile &&, std::vector>);

        STATIC_REQUIRE_FALSE(InstanceOf<std::string const volatile &, std::vector, std::list, std::unique_ptr>);
        STATIC_REQUIRE_FALSE(InstanceOf<int const, std::vector>);
        STATIC_REQUIRE_FALSE(InstanceOf<std::vector<int> volatile, std::list>);
        STATIC_REQUIRE_FALSE(InstanceOf<std::map<int, int> &, std::set>);
    }
    // The following line fails to compile:
    // STATIC_REQUIRE(InstanceOf<std::array<int, 4>, std::array>);
}

TEST_CASE("The R::getIters() method in ranges.hpp", "[ranges]")
{
    std::vector<int> nums{ 1, 2, 3, 4, 5 };
    auto [f, b] = R::getIters(nums);
    REQUIRE(f == nums.begin());
    REQUIRE(b == nums.end());
}
