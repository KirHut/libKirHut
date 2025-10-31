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
#include "kh/ranges.hpp" // IWYU pragma: export

#include <algorithm>
#include <list>
#include <map>
#include <set>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
// clazy:excludeall=non-pod-global-static

using namespace KirHut;

struct SomeObject
{
    // No members.
};

struct HasEmptyMember
{
    SomeObject e;
};

struct SomeOtherObject : public SomeObject
{
    int val = 0; // Should have data.
};

struct ThirdObject : public SomeObject
{
    float val = 0.; // other data.
};

struct SomeEmptyObject : public SomeObject
{
    // No members.
};

struct ThirdEmptyObject : public SomeEmptyObject
{
    // No members.
};

struct AnotherEmpty
{
    // No members.
};

struct KH_EBO_EMPTY_BASES DerivedEmpty : AnotherEmpty, SomeObject
{
    // No members.
};

struct WithStatic
{
    static int s;
};

struct WithMemberFunc
{
    void foo()
    {
        // No further implementation.
    }
};

struct WithBool
{
    bool b; // Object with sizeof() == 1.
};

struct NonEmpty
{
    int x;
};

struct WithVirtual
{
    virtual void foo()
    {
        // No further implementation.
    }
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

TEST_CASE("Bytes needed for bits", "[base][utility][bytesNeededForBits]")
{
    SECTION("Constexpr bytesNeededForBits functions tests")
    {
        STATIC_REQUIRE(bytesNeededForBits(8) == 1);
        STATIC_REQUIRE(bytesNeededForBits(15) == 2);
        STATIC_REQUIRE(bytesNeededForBits(30) == 4);
        STATIC_REQUIRE(bytesNeededForBits(60) == 8);
    }

    SECTION("Runtime bytesNeededForBits functions tests")
    {
        int volatile test1 = 8, test2 = 15, test3 = 30, test4 = 60;
        REQUIRE(bytesNeededForBits(test1) == 1);
        REQUIRE(bytesNeededForBits(test2) == 2);
        REQUIRE(bytesNeededForBits(test3) == 4);
        REQUIRE(bytesNeededForBits(test4) == 8);
    }
}

TEST_CASE("The asBytes() and asWritableBytes() functions", "[base][utility][asBytes]")
{
    u32 testVal    = 0x12'34'56'78;
    auto compareTo = std::bit_cast<array<byte, sizeof(u32)>>(testVal);

    REQUIRE(R::equal(compareTo, asBytes(testVal)));
    auto writableTestVal2 = asWritableBytes(testVal);
    writableTestVal2[2]   = byte{ 0x9A };
    compareTo[2]          = byte{ 0x9A };
    REQUIRE(R::equal(compareTo, writableTestVal2));
}

TEMPLATE_TEST_CASE("abs() handles all signed types correctly", "[base][utility][abs]", i8, i16, i32, i64, iWidest)
{
    using T               = TestType;
    constexpr auto minVal = Limits<T>::min();

    SECTION("Positive numbers remain unchanged")
    {
        STATIC_REQUIRE(KirHut::abs(static_cast<T>(0)) == static_cast<T>(0));
        STATIC_REQUIRE(KirHut::abs(static_cast<T>(42)) == static_cast<T>(42));
    }

    SECTION("Negative numbers are negated")
    {
        STATIC_REQUIRE(KirHut::abs(static_cast<T>(-1)) == static_cast<T>(1));
        STATIC_REQUIRE(KirHut::abs(static_cast<T>(-42)) == static_cast<T>(42));
    }

    SECTION("Minimum value remains unchanged (no overflow)")
    {
        STATIC_REQUIRE(KirHut::abs(minVal) == minVal);
    }

    SECTION("Symmetry for small range sanity check")
    {
        for (int i = -8; i <= 8; ++i)
        {
            T const value = static_cast<T>(i);
            REQUIRE(abs(value) == (value < 0 ? -value : value));
        }
    }
}

TEMPLATE_TEST_CASE("uabs() returns correct unsigned absolute value for signed types",
                   "[base][utility][uabs]",
                   i8,
                   i16,
                   i32,
                   i64,
                   iWidest)
{
    using T               = TestType;
    using U               = std::make_unsigned_t<T>;
    constexpr auto minVal = Limits<T>::min();
    constexpr auto maxVal = Limits<T>::max();

    SECTION("Type traits")
    {
        STATIC_REQUIRE(std::is_same_v<decltype(uabs(static_cast<T>(0))), U>);
    }

    SECTION("Positive values are unchanged and correctly cast")
    {
        STATIC_REQUIRE(uabs(static_cast<T>(0)) == static_cast<U>(0));
        STATIC_REQUIRE(uabs(static_cast<T>(42)) == static_cast<U>(42));
        STATIC_REQUIRE(uabs(maxVal) == static_cast<U>(maxVal));
    }

    SECTION("Negative values are converted correctly")
    {
        STATIC_REQUIRE(uabs(static_cast<T>(-1)) == static_cast<U>(1));
        STATIC_REQUIRE(uabs(static_cast<T>(-42)) == static_cast<U>(42));
    }

    SECTION("Minimum representable signed value converts to correct magnitude")
    {
        // The magnitude is (abs(min) == max + 1) due to two’s complement representation
        constexpr U expected = static_cast<U>(maxVal) + 1u;
        STATIC_REQUIRE(uabs(minVal) == expected);
    }

    SECTION("Range sanity check")
    {
        for (int i = -64; i <= 64; ++i)
        {
            T const val = static_cast<T>(i);
            U expected  = static_cast<U>(i < 0 ? -static_cast<long long>(i) : i);
            if (val == minVal)
            {
                expected = static_cast<U>(maxVal) + 1u;
            }
            REQUIRE(uabs(val) == expected);
        }
    }
}

TEMPLATE_TEST_CASE("uabs() returns the input unchanged for unsigned types",
                   "[base][utility][uabs]",
                   u8,
                   u16,
                   u32,
                   u64,
                   uWidest)
{
    using T = TestType;

    SECTION("Return type is the same")
    {
        STATIC_REQUIRE(std::is_same_v<decltype(uabs(static_cast<T>(0))), T>);
    }

    SECTION("Values are unchanged")
    {
        constexpr auto values = array{ static_cast<T>(0), static_cast<T>(1), static_cast<T>(42), Limits<T>::max() };
        STATIC_REQUIRE(R::all_of(values, [](auto v) constexpr { return uabs(v) == v; }));
    }
}

template <std::unsigned_integral UInt_T>
consteval UInt_T alternatingBits() noexcept
{
    std::array<unsigned char, sizeof(UInt_T)> byteArray{};
    R::fill(byteArray, 0x55);
    return std::bit_cast<UInt_T>(byteArray);
}

template <std::unsigned_integral auto uInt>
constexpr bool shiftFunction(int pos)
{
    if (uabs(pos) >= sizeof(uInt) * Platform::bitsInByte)
    {
        // This sanity check prevents a warning.
        return false;
    }

    return maskAt(uInt, pos) == static_cast<decltype(uInt)>(pos < 0 ? uInt >> uabs(pos) : uInt << pos);
}

TEMPLATE_TEST_CASE("maskAt() behaves correctly for all supported types",
                   "[base][utility][maskAt]",
                   u8,
                   u16,
                   u32,
                   u64,
                   uWidest)
{
    constexpr int bits         = Limits<TestType>::digits;
    constexpr TestType full    = Limits<TestType>::max();
    constexpr TestType altBits = alternatingBits<TestType>();

    TestType volatile vfull    = full;
    TestType volatile vAltBits = altBits;

    SECTION("Basic constexpr sanity checks")
    {
        STATIC_REQUIRE(maskAt(altBits, 0) == altBits);
        STATIC_REQUIRE(maskAt(altBits, bits) == 0);
        STATIC_REQUIRE(maskAt(altBits, bits + 1) == 0);
        STATIC_REQUIRE(maskAt(altBits, -bits) == 0);
        STATIC_REQUIRE(maskAt(altBits, -bits - 1) == 0);
        STATIC_REQUIRE(maskAt(altBits, Limits<int>::max()) == 0);
        STATIC_REQUIRE(maskAt(altBits, Limits<int>::min()) == 0);
    }

    SECTION("Basic runtime sanity checks")
    {
        REQUIRE(maskAt(vfull, 0) == full);
        REQUIRE(maskAt(vfull, bits) == 0);
        REQUIRE(maskAt(vfull, bits + 1) == 0);
        REQUIRE(maskAt(vfull, -bits) == 0);
        REQUIRE(maskAt(vfull, -bits - 1) == 0);
        REQUIRE(maskAt(vfull, Limits<int>::max()) == 0);
        REQUIRE(maskAt(vfull, Limits<int>::min()) == 0);
    }

    SECTION("location in constexpr maskAt() is shifting correctly for positive and negative values")
    {
        STATIC_REQUIRE(R::all_of(V::iota(-bits + 1, bits), shiftFunction<full>));
        STATIC_REQUIRE(R::all_of(V::iota(-bits + 1, bits), shiftFunction<altBits>));
    }

    SECTION("location in runtime maskAt() is shifting correctly for positive and negative values")
    {
        int pos = GENERATE(range(-bits + 1, bits + 0)); // It will not build without bits + 0 for some reason.
        REQUIRE(maskAt(vfull, pos) == static_cast<TestType>(pos < 0 ? full >> uabs(pos) : full << pos));
        REQUIRE(maskAt(vAltBits, pos) == static_cast<TestType>(pos < 0 ? altBits >> uabs(pos) : altBits << pos));
    }
}

TEST_CASE("Numeric concept constraints", "[base][concepts]")
{
    SECTION("The bool type is not numeric")
    {
        STATIC_REQUIRE_FALSE(Numeric<bool>);
    }

    SECTION("Types that are numeric")
    {
        STATIC_REQUIRE(Numeric<double>);
        STATIC_REQUIRE(Numeric<unsigned int>);
    }

    SECTION("cv-qualifications should pass")
    {
        STATIC_REQUIRE(Numeric<double const>);
        STATIC_REQUIRE(Numeric<int volatile>);
    }

    SECTION("Pointer and reference types are not numeric")
    {
        STATIC_REQUIRE_FALSE(Numeric<double *>);
        STATIC_REQUIRE_FALSE(Numeric<int &>);
    }
}

TEST_CASE("Raw byteSwap() functions", "[base][utility][byteSwap]")
{
    // We obviously cannot use the byteSwap functions to initialize expectFlip, otherwise we aren't actually testing it.
    constexpr u16 testFlip16 = 0x1234, expectFlip16 = 0x3412;
    constexpr u32 testFlip32 = 0x12'34'56'78, expectFlip32 = 0x78'56'34'12;
    constexpr u64 testFlip64 = 0x01'23'45'67'89'AB'CD'EF, expectFlip64 = 0xEF'CD'AB'89'67'45'23'01;

    SECTION("Constexpr byteSwap functions test")
    {
        STATIC_REQUIRE(byteSwap(testFlip16) == expectFlip16);
        STATIC_REQUIRE(byteSwap(testFlip32) == expectFlip32);
        STATIC_REQUIRE(byteSwap(testFlip64) == expectFlip64);
    }

    SECTION("Runtime byteSwap functions test")
    {
        u16 volatile vTestFlip16 = testFlip16;
        u32 volatile vTestFlip32 = testFlip32;
        u64 volatile vTestFlip64 = testFlip64;

        REQUIRE(byteSwap(vTestFlip16) == expectFlip16);
        REQUIRE(byteSwap(vTestFlip32) == expectFlip32);
        REQUIRE(byteSwap(vTestFlip64) == expectFlip64);
    }

#if defined(KH_USE_128BIT_TYPES)
    constexpr u128 testFlip128   = (static_cast<u128>(testFlip32) << Platform::bitsInU64) | testFlip64;
    constexpr u128 expectFlip128 = (static_cast<u128>(expectFlip64) << Platform::bitsInU64) |
                                   (static_cast<u128>(expectFlip32) << Platform::bitsInU32);

    SECTION("128-bit constexpr byteSwap function test")
    {
        STATIC_REQUIRE(byteSwap(testFlip128) == expectFlip128);
    }

    SECTION("128-bit runtime byteSwap function test")
    {
        u128 volatile vTestFlip128 = testFlip128;

        REQUIRE(byteSwap(vTestFlip128) == expectFlip128);
    }
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

TEST_CASE("InstanceOf concept constraints", "[base][concepts]")
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

TEST_CASE("SpanOf concept constraints", "[base][concepts]")
{
    using IntSpan         = span<int>;
    using IntConstSpan    = span<int const>;
    using DoubleSpan      = span<double>;
    using DoubleConstSpan = span<double const>;
    using IntSpanConst    = span<int> const;

    SECTION("Matches writable spans only")
    {
        STATIC_REQUIRE(SpanOf<IntSpan>);
        STATIC_REQUIRE(SpanOf<IntSpan, int>);
        STATIC_REQUIRE(SpanOf<IntConstSpan, int const>);
        STATIC_REQUIRE(SpanOf<DoubleSpan>);
        STATIC_REQUIRE(SpanOf<DoubleConstSpan, double const>);

        STATIC_REQUIRE_FALSE(SpanOf<IntConstSpan>);
        STATIC_REQUIRE_FALSE(SpanOf<DoubleConstSpan, int>);
        STATIC_REQUIRE_FALSE(SpanOf<DoubleConstSpan>);
    }

    SECTION("Rejects cv-qualified or reference span types")
    {
        STATIC_REQUIRE_FALSE(SpanOf<IntSpanConst>);
        STATIC_REQUIRE_FALSE(SpanOf<IntSpan &>);
        STATIC_REQUIRE_FALSE(SpanOf<IntSpan const &>);
    }

    SECTION("Rejects reference element types")
    {
        STATIC_REQUIRE_FALSE(SpanOf<IntSpan, int &>);
        STATIC_REQUIRE_FALSE(SpanOf<IntSpan, int const &>);
    }

    SECTION("Multiple element types")
    {
        STATIC_REQUIRE(SpanOf<IntSpan, int, double, float>);
        STATIC_REQUIRE_FALSE(SpanOf<IntConstSpan, double, float>);
    }

    SECTION("Empty element list implies writable span")
    {
        STATIC_REQUIRE(SpanOf<IntSpan>);
        STATIC_REQUIRE_FALSE(SpanOf<IntConstSpan>);
    }
}

TEST_CASE("ReadableSpanOf concept constraints", "[base][concepts]")
{
    using IntSpan         = span<int>;
    using IntConstSpan    = span<int const>;
    using DoubleSpan      = span<double>;
    using DoubleConstSpan = span<double const>;
    using IntSpanConst    = span<int> const;

    SECTION("Matches readable spans regardless of element constness")
    {
        STATIC_REQUIRE(ReadableSpanOf<IntSpan>);
        STATIC_REQUIRE(ReadableSpanOf<IntConstSpan>);
        STATIC_REQUIRE(ReadableSpanOf<IntConstSpan, int>);
        STATIC_REQUIRE(ReadableSpanOf<IntSpan, int>);
        STATIC_REQUIRE(ReadableSpanOf<DoubleConstSpan, double>);
    }

    SECTION("Rejects cv-qualified or reference span types")
    {
        STATIC_REQUIRE_FALSE(ReadableSpanOf<IntSpanConst>);
        STATIC_REQUIRE_FALSE(ReadableSpanOf<IntSpan &>);
        STATIC_REQUIRE_FALSE(ReadableSpanOf<IntSpan const &>);
    }

    SECTION("Requires Element_Ts to be non-cv-qualified")
    {
        STATIC_REQUIRE(ReadableSpanOf<IntConstSpan, int>);
        STATIC_REQUIRE_FALSE(ReadableSpanOf<IntConstSpan, int const>);
        STATIC_REQUIRE_FALSE(ReadableSpanOf<IntConstSpan, int volatile>);
    }

    SECTION("Rejects reference element types")
    {
        STATIC_REQUIRE_FALSE(ReadableSpanOf<IntSpan, int &>);
        STATIC_REQUIRE_FALSE(ReadableSpanOf<IntSpan, int const &>);
    }

    SECTION("Empty element list means any readable span")
    {
        STATIC_REQUIRE(ReadableSpanOf<IntSpan>);
        STATIC_REQUIRE(ReadableSpanOf<IntConstSpan>);
        STATIC_REQUIRE(ReadableSpanOf<DoubleSpan>);
    }

    SECTION("Multiple element types")
    {
        STATIC_REQUIRE(ReadableSpanOf<IntConstSpan, int, double, float>);
        STATIC_REQUIRE(ReadableSpanOf<DoubleConstSpan, double, float>);
        STATIC_REQUIRE_FALSE(ReadableSpanOf<DoubleConstSpan, int, char>);
    }
}

TEST_CASE("ByteType concept constraints")
{
    SECTION("Accepts standard byte-like types")
    {
        STATIC_REQUIRE(ByteType<char>);
        STATIC_REQUIRE(ByteType<unsigned char>);
        STATIC_REQUIRE(ByteType<std::byte>);
        STATIC_REQUIRE(ByteType<char const>);
        STATIC_REQUIRE(ByteType<unsigned char volatile>);
        STATIC_REQUIRE(ByteType<std::byte const>);
    }

    SECTION("Rejects signed char")
    {
        STATIC_REQUIRE_FALSE(ByteType<signed char>);
        STATIC_REQUIRE_FALSE(ByteType<signed char const>);
    }

    SECTION("Rejects other integer types")
    {
        STATIC_REQUIRE_FALSE(ByteType<int>);
        STATIC_REQUIRE_FALSE(ByteType<short>);
        STATIC_REQUIRE_FALSE(ByteType<long>);
        STATIC_REQUIRE_FALSE(ByteType<bool>);
    }

    SECTION("Rejects unrelated types")
    {
        STATIC_REQUIRE_FALSE(ByteType<void>);
        STATIC_REQUIRE_FALSE(ByteType<float>);
        STATIC_REQUIRE_FALSE(ByteType<double>);
        STATIC_REQUIRE_FALSE(ByteType<void *>);
        STATIC_REQUIRE_FALSE(ByteType<char *>);
        STATIC_REQUIRE_FALSE(ByteType<SomeObject>);
    }

    SECTION("Rejects references to byte types")
    {
        STATIC_REQUIRE_FALSE(ByteType<char &>);
        STATIC_REQUIRE_FALSE(ByteType<unsigned char const &>);
        STATIC_REQUIRE_FALSE(ByteType<std::byte &&>);
    }
}

TEST_CASE("ByteSpan and ReadableByteSpan concept constraints")
{
    SECTION("Accepts spans of writable byte-like types")
    {
        STATIC_REQUIRE(ByteSpan<span<char>>);
        STATIC_REQUIRE(ByteSpan<span<unsigned char>>);
        STATIC_REQUIRE(ByteSpan<span<byte>>);
        STATIC_REQUIRE(ByteSpan<span<std::byte>>);
    }

    SECTION("Rejects spans of const byte-like types for ByteSpan")
    {
        STATIC_REQUIRE_FALSE(ByteSpan<span<char const>>);
        STATIC_REQUIRE_FALSE(ByteSpan<span<unsigned char const>>);
        STATIC_REQUIRE_FALSE(ByteSpan<span<byte const>>);
        STATIC_REQUIRE_FALSE(ByteSpan<span<std::byte const>>);
    }

    SECTION("Accepts spans of readable (const) byte-like types for ReadableByteSpan")
    {
        STATIC_REQUIRE(ReadableByteSpan<span<char const>>);
        STATIC_REQUIRE(ReadableByteSpan<span<unsigned char const>>);
        STATIC_REQUIRE(ReadableByteSpan<span<byte const>>);
        STATIC_REQUIRE(ReadableByteSpan<span<std::byte const>>);
    }

    SECTION("Accepts writable spans for ReadableByteSpan")
    {
        STATIC_REQUIRE(ReadableByteSpan<span<char>>);
        STATIC_REQUIRE(ReadableByteSpan<span<unsigned char>>);
        STATIC_REQUIRE(ReadableByteSpan<span<byte>>);
        STATIC_REQUIRE(ReadableByteSpan<span<std::byte>>);
    }

    SECTION("Rejects spans of unrelated element types")
    {
        STATIC_REQUIRE_FALSE(ByteSpan<span<int>>);
        STATIC_REQUIRE_FALSE(ByteSpan<span<double>>);
        STATIC_REQUIRE_FALSE(ReadableByteSpan<span<int>>);
        STATIC_REQUIRE_FALSE(ReadableByteSpan<span<void *>>);
    }

    SECTION("Rejects completely unrelated types")
    {
        STATIC_REQUIRE_FALSE(ByteSpan<int>);
        STATIC_REQUIRE_FALSE(ReadableByteSpan<int>);
        STATIC_REQUIRE_FALSE(ByteSpan<std::array<char, 4>>);
        STATIC_REQUIRE_FALSE(ReadableByteSpan<std::array<std::byte, 4>>);
        STATIC_REQUIRE_FALSE(ByteSpan<SomeObject>);
        STATIC_REQUIRE_FALSE(ReadableByteSpan<SomeOtherObject>);
    }

    SECTION("Rejects references to spans")
    {
        STATIC_REQUIRE_FALSE(ByteSpan<span<char> &>);
        STATIC_REQUIRE_FALSE(ReadableByteSpan<span<std::byte const> &>);
    }

    SECTION("Rejects volatile spans")
    {
        STATIC_REQUIRE_FALSE(ByteSpan<span<char> volatile>);
        STATIC_REQUIRE_FALSE(ReadableByteSpan<span<std::byte const> volatile>);
    }
}

TEST_CASE("HasTypeOption concept correctness")
{
    using V1 = std::variant<int, float, std::string>;
    using V2 = Var<int, double, char>;

    SECTION("Matches when the type is an exact alternative")
    {
        STATIC_REQUIRE(HasTypeOption<V1, int>);
        STATIC_REQUIRE(HasTypeOption<V1, float>);
        STATIC_REQUIRE(HasTypeOption<V1, std::string>);

        STATIC_REQUIRE(HasTypeOption<V2, double>);
        STATIC_REQUIRE(HasTypeOption<V2, char>);
    }

    SECTION("Matches when any of multiple types are valid alternatives")
    {
        STATIC_REQUIRE(HasTypeOption<V1, double, std::string>);
        STATIC_REQUIRE(HasTypeOption<V2, char, std::byte>);
    }

    SECTION("Rejects when none of the given types match")
    {
        STATIC_REQUIRE_FALSE(HasTypeOption<V1, double>);
        STATIC_REQUIRE_FALSE(HasTypeOption<V1, void *>);
        STATIC_REQUIRE_FALSE(HasTypeOption<V2, long, bool>);
    }

    SECTION("Rejects completely unrelated types")
    {
        STATIC_REQUIRE_FALSE(HasTypeOption<int, int>);
        STATIC_REQUIRE_FALSE(HasTypeOption<std::string, char>);
    }

    SECTION("Works equally for std::variant and alias Var")
    {
        using A = std::variant<int, char>;
        using B = Var<int, char>;
        STATIC_REQUIRE(HasTypeOption<A, int>);
        STATIC_REQUIRE(HasTypeOption<B, char>);
    }

    SECTION("Const-qualified variants still match")
    {
        using CV = std::variant<int, float> const;
        STATIC_REQUIRE(HasTypeOption<CV, int>);
        STATIC_REQUIRE_FALSE(HasTypeOption<CV, double>);
    }

    SECTION("Reference variants are accepted")
    {
        using V = std::variant<int, float>;
        STATIC_REQUIRE(HasTypeOption<V &, int>);
        STATIC_REQUIRE(HasTypeOption<V const &, float>);
    }
}

TEST_CASE("TypeOptionOf concept constraints")
{
    using V1 = std::variant<int, float, std::string>;
    using V2 = Var<char, double, bool>;

    SECTION("Recognizes valid type options for std::variant")
    {
        STATIC_REQUIRE(TypeOptionOf<int, V1>);
        STATIC_REQUIRE(TypeOptionOf<float, V1>);
        STATIC_REQUIRE(TypeOptionOf<std::string, V1>);
    }

    SECTION("Rejects types not in the variant")
    {
        STATIC_REQUIRE_FALSE(TypeOptionOf<char, V1>);
        STATIC_REQUIRE_FALSE(TypeOptionOf<long, V1>);
    }

    SECTION("Recognizes valid type options for Var alias")
    {
        STATIC_REQUIRE(TypeOptionOf<char, V2>);
        STATIC_REQUIRE(TypeOptionOf<double, V2>);
        STATIC_REQUIRE(TypeOptionOf<bool, V2>);
    }

    SECTION("Rejects invalid types for Var alias")
    {
        STATIC_REQUIRE_FALSE(TypeOptionOf<int, V2>);
        STATIC_REQUIRE_FALSE(TypeOptionOf<std::string, V2>);
    }

    SECTION("Works with const and reference variants")
    {
        using CV1 = const V1;
        using RV1 = V1 &;
        STATIC_REQUIRE(TypeOptionOf<float, CV1>);
        STATIC_REQUIRE(TypeOptionOf<int, RV1>);
    }

    SECTION("Works equally for Var and std::variant interchangeably")
    {
        STATIC_REQUIRE(TypeOptionOf<bool, V2>);
        STATIC_REQUIRE(TypeOptionOf<std::string, V1>);
    }
}

TEST_CASE("EmptyClass concept constraints")
{
    SECTION("Fundamental empty classes")
    {
        STATIC_REQUIRE(EmptyClass<SomeObject>);
        STATIC_REQUIRE(EmptyClass<WithStatic>);
        STATIC_REQUIRE(EmptyClass<WithMemberFunc>);
    }

    SECTION("Non-empty classes fail the concept")
    {
        STATIC_REQUIRE_FALSE(EmptyClass<NonEmpty>);
        STATIC_REQUIRE_FALSE(EmptyClass<WithVirtual>);
        STATIC_REQUIRE_FALSE(EmptyClass<WithBool>);
    }

    SECTION("Empty classes with inheritance still satisfy EBO")
    {
        STATIC_REQUIRE(EmptyClass<SomeEmptyObject>);
        STATIC_REQUIRE(EmptyClass<ThirdEmptyObject>);
    }

    SECTION("Multiple empty base classes should still have EBO")
    {
        STATIC_REQUIRE(EmptyClass<DerivedEmpty>);
    }

    SECTION("Classes with empty members fail EBO")
    {
        STATIC_REQUIRE_FALSE(EmptyClass<HasEmptyMember>);
    }

    SECTION("Const/volatile/reference qualified variants")
    {
        STATIC_REQUIRE(EmptyClass<SomeObject const>);
        STATIC_REQUIRE(EmptyClass<SomeObject volatile>);
        STATIC_REQUIRE(EmptyClass<SomeObject const volatile>);
        STATIC_REQUIRE_FALSE(EmptyClass<SomeObject &>);
        STATIC_REQUIRE_FALSE(EmptyClass<SomeObject const &>);
    }

    SECTION("Standard library empty types")
    {
        // These should always be EBO...
        STATIC_REQUIRE(EmptyClass<std::default_delete<int>>);
        STATIC_REQUIRE(EmptyClass<std::allocator<void>>);
    }
}

TEST_CASE("varIndex function returns correct index for std::variant and Var")
{
    using V1 = std::variant<int, float, std::string>;
    using V2 = Var<char, double, bool>;

    constexpr V1 v1{};
    constexpr V2 v2{};

    SECTION("Correctly finds index of existing types")
    {
        STATIC_REQUIRE(varIndex<int>(v1) == 0);
        STATIC_REQUIRE(varIndex<float>(v1) == 1);
        STATIC_REQUIRE(varIndex<std::string>(v1) == 2);

        STATIC_REQUIRE(varIndex<char>(v2) == 0);
        STATIC_REQUIRE(varIndex<double>(v2) == 1);
        STATIC_REQUIRE(varIndex<bool>(v2) == 2);
    }

    SECTION("Works with const-qualified variants")
    {
        constexpr const V1 cv1{};
        STATIC_REQUIRE(varIndex<float>(cv1) == 1);
    }

    SECTION("Works with reference to variant")
    {
        constexpr static V1 v1_local{};
        constexpr V1 const &ref = v1_local;
        STATIC_REQUIRE(varIndex<std::string>(ref) == 2);
    }

    SECTION("Returns sentinel index (variant_size) for types not present")
    {
        constexpr auto missing = varIndex<long>(v1);
        STATIC_REQUIRE(missing == std::variant_size_v<V1>);
        constexpr auto also_missing = varIndex<int>(v2);
        STATIC_REQUIRE_FALSE(also_missing < std::variant_size_v<V2>);
    }

    SECTION("Works equally for Var alias and std::variant")
    {
        STATIC_REQUIRE(varIndex<double>(v2) == 1);
        STATIC_REQUIRE(varIndex<std::string>(v1) == 2);
    }
}

template <std::unsigned_integral UInt_T>
consteval std::pair<UInt_T, std::array<byte, sizeof(UInt_T)>> bePair() noexcept
{
    UInt_T front = 0;
    array<byte, sizeof(UInt_T)> back{};
    unsigned char beByte = 0x01;
    for (int pos = 0; pos < sizeof(UInt_T); ++pos)
    {
        front |= static_cast<UInt_T>(beByte) << (sizeof(UInt_T) - pos - 1) * Platform::bitsInByte;
        back[pos] = static_cast<byte>(beByte);
        beByte += 0x22;
    }

    return { front, back };
}

TEMPLATE_TEST_CASE("fromBigEndian() functions for each type", "[base][endian][utility]", u16, u32, u64, uWidest)
{
    constexpr auto bigEndPair = bePair<TestType>();
    constexpr auto expect     = bigEndPair.first;
    constexpr auto beArray    = bigEndPair.second;

    SECTION("Constexpr overloads")
    {
        STATIC_REQUIRE(fromBigEndian<TestType>(beArray.data()) == expect);
        STATIC_REQUIRE(fromBigEndian<TestType>(std::span<byte const, sizeof(TestType)>(beArray)) == expect);
    }

    SECTION("Runtime overloads")
    {
        auto rtArray      = beArray;
        byte *volatile vp = rtArray.data();
        auto sp           = span{ vp, sizeof(TestType) };
        REQUIRE(fromBigEndian<TestType>(vp) == expect);
        REQUIRE(fromBigEndian<TestType>(sp) == expect);
    }
}

TEST_CASE("fromBigEndian() span too small at runtime throws", "[base][endian][utility]")
{
    std::array<unsigned char, 2> smallBuf{ 0x12, 0x34 };
    auto smallSpan = std::span<unsigned char const>(smallBuf);
    CHECK_THROWS(fromBigEndian<uint32_t>(smallSpan));
}

template <std::unsigned_integral UInt_T>
consteval std::pair<UInt_T, std::array<byte, sizeof(UInt_T)>> lePair() noexcept
{
    UInt_T front = 0;
    array<byte, sizeof(UInt_T)> back{};
    unsigned char beByte = 0x01;
    for (int pos = 0; pos < sizeof(UInt_T); ++pos)
    {
        front |= static_cast<UInt_T>(beByte) << pos * Platform::bitsInByte;
        back[pos] = static_cast<byte>(beByte);
        beByte += 0x22;
    }

    return { front, back };
}

TEMPLATE_TEST_CASE("fromLittleEndian() functions for each type", "[base][endian][utility]", u16, u32, u64, uWidest)
{
    constexpr auto littleEndPair = lePair<TestType>();
    constexpr auto expect        = littleEndPair.first;
    constexpr auto leArray       = littleEndPair.second;

    SECTION("Constexpr overloads")
    {
        STATIC_REQUIRE(fromLittleEndian<TestType>(leArray.data()) == expect);
        STATIC_REQUIRE(fromLittleEndian<TestType>(std::span<byte const, sizeof(TestType)>(leArray)) == expect);
    }

    SECTION("Runtime overloads")
    {
        auto rtArray      = leArray;
        byte *volatile vp = rtArray.data();
        auto sp           = span{ vp, sizeof(TestType) };
        REQUIRE(fromLittleEndian<TestType>(vp) == expect);
        REQUIRE(fromLittleEndian<TestType>(sp) == expect);
    }
}

TEST_CASE("fromLittleEndian() span too small at runtime throws", "[base][endian][utility]")
{
    std::array<unsigned char, 2> smallBuf{ 0x12, 0x34 };
    auto smallSpan = std::span<unsigned char const>(smallBuf);
    CHECK_THROWS(fromLittleEndian<uint32_t>(smallSpan));
}

template <std::unsigned_integral UInt_T>
constexpr array<byte, sizeof(UInt_T)> toBigEndianPtrOverloadTest(UInt_T start)
{
    array<byte, sizeof(UInt_T)> retArray{};
    toBigEndian(start, retArray.data());
    return retArray;
}

template <std::unsigned_integral UInt_T>
constexpr array<byte, sizeof(UInt_T)> toBigEndianSpanOverloadTest(UInt_T start)
{
    array<byte, sizeof(UInt_T)> retArray{};
    toBigEndian(start, span<byte, sizeof(UInt_T)>{ retArray });
    return retArray;
}

TEMPLATE_TEST_CASE("toBigEndian() functions for each type", "[base][endian][utility]", u16, u32, u64, uWidest)
{
    constexpr auto bigEndPair = bePair<TestType>();
    constexpr auto start      = bigEndPair.first;
    constexpr auto expect     = bigEndPair.second;

    SECTION("Constexpr overloads")
    {
        STATIC_REQUIRE(toBigEndianPtrOverloadTest(start) == expect);
        STATIC_REQUIRE(toBigEndian(start) == expect);
        STATIC_REQUIRE(toBigEndianSpanOverloadTest(start) == expect);
    }

    SECTION("Runtime overloads")
    {
        auto volatile vstart = start;

        REQUIRE(toBigEndianPtrOverloadTest(vstart) == expect);
        REQUIRE(toBigEndian(vstart) == expect);
        REQUIRE(toBigEndianSpanOverloadTest(vstart) == expect);
    }
}

template <std::unsigned_integral UInt_T>
constexpr array<byte, sizeof(UInt_T)> toLittleEndianPtrOverloadTest(UInt_T start)
{
    array<byte, sizeof(UInt_T)> retArray{};
    toLittleEndian(start, retArray.data());
    return retArray;
}

template <std::unsigned_integral UInt_T>
constexpr array<byte, sizeof(UInt_T)> toLittleEndianSpanOverloadTest(UInt_T start)
{
    array<byte, sizeof(UInt_T)> retArray{};
    toLittleEndian(start, span<byte, sizeof(UInt_T)>{ retArray });
    return retArray;
}

TEMPLATE_TEST_CASE("toLittleEndian() functions for each type", "[base][endian][utility]", u16, u32, u64, uWidest)
{
    constexpr auto littleEndPair = lePair<TestType>();
    constexpr auto start         = littleEndPair.first;
    constexpr auto expect        = littleEndPair.second;

    SECTION("Constexpr overloads")
    {
        STATIC_REQUIRE(toLittleEndianPtrOverloadTest(start) == expect);
        STATIC_REQUIRE(toLittleEndian(start) == expect);
        STATIC_REQUIRE(toLittleEndianSpanOverloadTest(start) == expect);
    }

    SECTION("Runtime overloads")
    {
        auto volatile vstart = start;

        REQUIRE(toLittleEndianPtrOverloadTest(vstart) == expect);
        REQUIRE(toLittleEndian(vstart) == expect);
        REQUIRE(toLittleEndianSpanOverloadTest(vstart) == expect);
    }
}
