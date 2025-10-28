/***********************************************************************************************************************
** The KirHut Application Development Library
** TestInvalid.cpp
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
#include "kh/invalid.hpp"

#include <catch2/catch_test_macros.hpp>
// clazy:excludeall=non-pod-global-static

using namespace KirHut;

struct MissingCharType
{
    WhyInvalid getWhyInvalid() const noexcept
    {
        return WhyInvalid::Success;
    }

    std::string_view getInvalidInfo() const noexcept
    {
        return "ok";
    }
};

struct MissingGetWhyInvalid
{
    using CharType = char;

    std::string_view getInvalidInfo() const noexcept
    {
        return "ok";
    }
};

struct WrongReturnTypes
{
    using CharType = char;

    constexpr int getWhyInvalid() const noexcept
    {
        return 42;
    }

    constexpr std::string getInvalidInfo() const noexcept
    {
        return "ok";
    }
};

struct CorrectWhyObject
{
    using CharType                    = char;
    constexpr static bool isQuickCopy = true;

    constexpr WhyInvalid getWhyInvalid() const noexcept
    {
        return WhyInvalid::SoftwareError;
    }

    constexpr std::basic_string_view<CharType> getInvalidInfo() const noexcept
    {
        return "error";
    }
};

TEST_CASE("MessageViewWhy object basic behavior", "[invalid][utility]")
{
    SECTION("MessageViewWhy expected invariants")
    {
        // MessageViewWhy should be trivially copyable, but Clang does not implement std::is_trivially_copyable
        // correctly: https://github.com/llvm/llvm-project/issues/59624
        // This uses std::is_trivially_copy_constructible instead, since that is what ultimately matters.
        STATIC_REQUIRE(std::is_trivially_copy_constructible_v<MessageViewWhy<char>>);
        STATIC_REQUIRE(std::is_trivially_destructible_v<MessageViewWhy<char>>);
        STATIC_REQUIRE(std::is_trivially_copy_constructible_v<MessageViewWhy<wchar_t>>);
        STATIC_REQUIRE(std::is_trivially_destructible_v<MessageViewWhy<wchar_t>>);
    }

    SECTION("Construction and member access (char)")
    {
        constexpr MessageViewWhy<char> msg{ WhyInvalid::FileNotFound, "File not found"sv };

        STATIC_REQUIRE(std::is_same_v<decltype(msg)::CharType, char>);
        STATIC_REQUIRE(msg.isQuickCopy);
        STATIC_REQUIRE(msg.why == WhyInvalid::FileNotFound);
        STATIC_REQUIRE(msg.info == "File not found"sv);

        STATIC_REQUIRE(msg.getWhyInvalid() == WhyInvalid::FileNotFound);
        STATIC_REQUIRE(msg.getInvalidInfo() == "File not found"sv);
    }

    SECTION("Construction and member access (wchar_t)")
    {
        constexpr MessageViewWhy<wchar_t> wmsg{ WhyInvalid::IncorrectInput, L"Invalid input"sv };

        STATIC_REQUIRE(std::is_same_v<decltype(wmsg)::CharType, wchar_t>);
        STATIC_REQUIRE(wmsg.isQuickCopy);
        STATIC_REQUIRE(wmsg.why == WhyInvalid::IncorrectInput);
        STATIC_REQUIRE(wmsg.info == L"Invalid input"sv);

        STATIC_REQUIRE(wmsg.getWhyInvalid() == WhyInvalid::IncorrectInput);
        STATIC_REQUIRE(wmsg.getInvalidInfo() == L"Invalid input"sv);
    }

    SECTION("Runtime construction and access")
    {
        string_view const text = "Something went wrong";
        MessageViewWhy<char> const msg{ WhyInvalid::SoftwareError, text };

        REQUIRE(msg.why == WhyInvalid::SoftwareError);
        REQUIRE(msg.info == text);
        REQUIRE(msg.getWhyInvalid() == WhyInvalid::SoftwareError);
        REQUIRE(msg.getInvalidInfo() == text);
    }
}

TEST_CASE("WhyObject concept constraints", "[invalid][concepts]")
{
    STATIC_REQUIRE(WhyObject<MessageViewWhy<char>>);
    STATIC_REQUIRE(WhyObject<MessageViewWhy<wchar_t>>);
    STATIC_REQUIRE(WhyObject<CorrectWhyObject>);

    STATIC_REQUIRE_FALSE(WhyObject<int>);
    STATIC_REQUIRE_FALSE(WhyObject<MissingCharType>);
    STATIC_REQUIRE_FALSE(WhyObject<MissingGetWhyInvalid>);
    STATIC_REQUIRE_FALSE(WhyObject<WrongReturnTypes>);
}

// The getWhyInvalid function is actually a CPO, so requires will accept every type you pass to it. You have to check
// the overloads directly in the Detail namespace to get a requires clause correctly set to true/false.
template <typename T>
constexpr bool testWhyInvalid = requires { Detail::getWhyInvalid(std::declval<T>()); };

TEST_CASE("getWhyInvalid overloads", "[invalid][concepts]")
{
    SECTION("getWhyInvalid(WhyInvalid) exact overload")
    {
        STATIC_REQUIRE(getWhyInvalid(WhyInvalid::IllegalArgument) == WhyInvalid::IllegalArgument);
        STATIC_REQUIRE(getWhyInvalid(WhyInvalid::Success) == WhyInvalid::Success);
    }

    SECTION("getWhyInvalid(std::basic_string_view<Char_T>) overload")
    {
        constexpr string_view sv        = "test";
        constexpr std::wstring_view wsv = L"wide";
        STATIC_REQUIRE(getWhyInvalid(sv) == WhyInvalid::Unknown);
        STATIC_REQUIRE(getWhyInvalid(wsv) == WhyInvalid::Unknown);
    }

    SECTION("getWhyInvalid(std::basic_string<Char_T> const&) overload")
    {
        string const s        = "test";
        std::wstring const ws = L"wide";
        STATIC_REQUIRE(getWhyInvalid(s) == WhyInvalid::Unknown);
        STATIC_REQUIRE(getWhyInvalid(ws) == WhyInvalid::Unknown);
    }

    SECTION("getWhyInvalid(WhyObject const&) overload")
    {
        constexpr CorrectWhyObject obj{};
        STATIC_REQUIRE(getWhyInvalid(obj) == WhyInvalid::SoftwareError);
    }

    SECTION("No unintended overloads")
    {
        STATIC_REQUIRE_FALSE(testWhyInvalid<int>);
        STATIC_REQUIRE_FALSE(testWhyInvalid<float>);
        STATIC_REQUIRE_FALSE(testWhyInvalid<char>);
        STATIC_REQUIRE_FALSE(testWhyInvalid<std::nullptr_t>);
        STATIC_REQUIRE_FALSE(testWhyInvalid<void *>);
    }
}

// The getWhyInvalid function is actually a CPO, so requires will accept every type you pass to it. You have to check
// the overloads directly in the Detail namespace to get a requires clause correctly set to true/false.
template <typename T>
constexpr bool testInvalidInfo = requires { Detail::getInvalidInfo(std::declval<T>()); };

template <OneOf<string, std::wstring> Str_T>
consteval bool getInvalidInfoStringTest(typename Str_T::value_type const (&data)[])
{
    Str_T str = data;
    return getInvalidInfo(str) == str;
}

TEST_CASE("getInvalidInfo overloads", "[invalid][concepts]")
{
    SECTION("getInvalidInfo(WhyInvalid) exact overload")
    {
        STATIC_REQUIRE(getInvalidInfo(WhyInvalid::Success) == "The operation was successful."sv);
        STATIC_REQUIRE(getInvalidInfo(WhyInvalid::IncorrectInput) == "The input provided was incorrect."sv);
        STATIC_REQUIRE(getInvalidInfo(WhyInvalid::Unknown) == "There was an unknown error or failure."sv);
    }

    SECTION("getInvalidInfo(std::basic_string_view<Char_T>) overload")
    {
        constexpr string_view sv        = "simple view";
        constexpr std::wstring_view wsv = L"wide view";
        STATIC_REQUIRE(getInvalidInfo(sv) == sv);
        STATIC_REQUIRE(getInvalidInfo(wsv) == wsv);
    }

    SECTION("getInvalidInfo(std::basic_string<Char_T> const&) overload")
    {
        STATIC_REQUIRE(getInvalidInfoStringTest<string>("string content"));
        STATIC_REQUIRE(getInvalidInfoStringTest<std::wstring>(L"wide content"));
    }

    SECTION("getInvalidInfo(WhyObject const&) overload")
    {
        constexpr CorrectWhyObject obj{};
        STATIC_REQUIRE(getInvalidInfo(obj) == obj.getInvalidInfo());
    }

    SECTION("No unintended overloads")
    {
        STATIC_REQUIRE_FALSE(testInvalidInfo<int>);
        STATIC_REQUIRE_FALSE(testInvalidInfo<float>);
        STATIC_REQUIRE_FALSE(testInvalidInfo<char>);
        STATIC_REQUIRE_FALSE(testInvalidInfo<std::nullptr_t>);
        STATIC_REQUIRE_FALSE(testInvalidInfo<void *>);
    }
}

TEST_CASE("WhyTypeTraits specializations", "[invalid][traits]")
{
    SECTION("Default WhyTypeTraits<T>")
    {
        STATIC_REQUIRE_FALSE(WhyTypeTraits<int>::isImplemented);
        STATIC_REQUIRE_FALSE(WhyTypeTraits<float>::isImplemented);
        STATIC_REQUIRE_FALSE(WhyTypeTraits<int>::isQuickCopy);
        STATIC_REQUIRE(std::is_same_v<typename WhyTypeTraits<int>::CharType, char>);
    }

    SECTION("WhyTypeTraits<WhyInvalid>")
    {
        STATIC_REQUIRE(WhyTypeTraits<WhyInvalid>::isImplemented);
        STATIC_REQUIRE(WhyTypeTraits<WhyInvalid>::isQuickCopy);
        STATIC_REQUIRE(std::is_same_v<typename WhyTypeTraits<WhyInvalid>::CharType, char>);
    }

    SECTION("WhyTypeTraits<std::basic_string_view<Char_T>> specializations")
    {
        STATIC_REQUIRE(WhyTypeTraits<string_view>::isImplemented);
        STATIC_REQUIRE(WhyTypeTraits<string_view>::isQuickCopy);
        STATIC_REQUIRE(std::is_same_v<typename WhyTypeTraits<string_view>::CharType, char>);

        STATIC_REQUIRE(WhyTypeTraits<std::wstring_view>::isImplemented);
        STATIC_REQUIRE(WhyTypeTraits<std::wstring_view>::isQuickCopy);
        STATIC_REQUIRE(std::is_same_v<typename WhyTypeTraits<std::wstring_view>::CharType, wchar_t>);
    }

    SECTION("WhyTypeTraits<std::basic_string<Char_T>> specializations")
    {
        STATIC_REQUIRE(WhyTypeTraits<string>::isImplemented);
        STATIC_REQUIRE_FALSE(WhyTypeTraits<string>::isQuickCopy);
        STATIC_REQUIRE(std::is_same_v<typename WhyTypeTraits<string>::CharType, char>);

        STATIC_REQUIRE(WhyTypeTraits<std::wstring>::isImplemented);
        STATIC_REQUIRE_FALSE(WhyTypeTraits<std::wstring>::isQuickCopy);
        STATIC_REQUIRE(std::is_same_v<typename WhyTypeTraits<std::wstring>::CharType, wchar_t>);
    }

    SECTION("WhyTypeTraits<WhyObject> specialization")
    {
        using Traits = WhyTypeTraits<CorrectWhyObject>;

        STATIC_REQUIRE(Traits::isImplemented);
        STATIC_REQUIRE(Traits::isQuickCopy);
        STATIC_REQUIRE(std::is_same_v<typename Traits::CharType, char>);
    }
}

struct ValidWhyObject
{
    using CharType = char;

    WhyInvalid getWhyInvalid() const noexcept
    {
        return WhyInvalid::Unknown;
    }

    string_view getInvalidInfo() const noexcept
    {
        return "";
    }
};

struct ValidQuickWhyObject
{
    using CharType                    = char;
    constexpr static bool isQuickCopy = true;

    WhyInvalid getWhyInvalid() const noexcept
    {
        return WhyInvalid::Unknown;
    }

    string_view getInvalidInfo() const noexcept
    {
        return "";
    }
};

TEST_CASE("ValidWhyType concept constraints", "[invalid][concepts]")
{
    SECTION("Valid Why types")
    {
        STATIC_REQUIRE(ValidWhyType<WhyInvalid>);
        STATIC_REQUIRE(ValidWhyType<string_view>);
        STATIC_REQUIRE(ValidWhyType<std::wstring_view>);
        STATIC_REQUIRE(ValidWhyType<string>);
        STATIC_REQUIRE(ValidWhyType<std::wstring>);
        STATIC_REQUIRE(ValidWhyType<ValidWhyObject>);
    }

    SECTION("Invalid because not implemented in traits")
    {
        STATIC_REQUIRE_FALSE(ValidWhyType<int>);
        STATIC_REQUIRE_FALSE(ValidWhyType<void *>);
        STATIC_REQUIRE_FALSE(ValidWhyType<double>);
    }

    SECTION("Invalid because reference or function types")
    {
        STATIC_REQUIRE_FALSE(ValidWhyType<int &>);
        STATIC_REQUIRE_FALSE(ValidWhyType<int const &>);
        STATIC_REQUIRE_FALSE(ValidWhyType<int &&>);
        STATIC_REQUIRE_FALSE(ValidWhyType<void()>);
    }

    SECTION("Custom Why-like objects that fail requirements")
    {
        struct MissingCharType
        {
            WhyInvalid getWhyInvalid() const noexcept
            {
                return WhyInvalid::Unknown;
            }
            string_view getInvalidInfo() const noexcept
            {
                return "";
            }
        };

        struct MissingGetWhyInvalid
        {
            using CharType = char;
            string_view getInvalidInfo() const noexcept
            {
                return "";
            }
        };

        struct MissingGetInvalidInfo
        {
            using CharType = char;
            WhyInvalid getWhyInvalid() const noexcept
            {
                return WhyInvalid::Unknown;
            }
        };

        STATIC_REQUIRE_FALSE(ValidWhyType<MissingCharType>);
        STATIC_REQUIRE_FALSE(ValidWhyType<MissingGetWhyInvalid>);
        STATIC_REQUIRE_FALSE(ValidWhyType<MissingGetInvalidInfo>);
    }

    SECTION("WhyObject concept types are also ValidWhyType")
    {
        STATIC_REQUIRE(WhyObject<ValidWhyObject>);
        STATIC_REQUIRE(ValidWhyType<ValidWhyObject>);
    }
}

TEST_CASE("QuickWhyType concept constraints", "[invalid][concepts]")
{
    SECTION("Valid quick copy types")
    {
        STATIC_REQUIRE(QuickWhyType<WhyInvalid>);
        STATIC_REQUIRE(QuickWhyType<string_view>);
        STATIC_REQUIRE(QuickWhyType<std::wstring_view>);
        STATIC_REQUIRE(QuickWhyType<ValidQuickWhyObject>);
    }

    SECTION("ValidWhyType but not quick copy")
    {
        STATIC_REQUIRE(ValidWhyType<string>);
        STATIC_REQUIRE_FALSE(QuickWhyType<string>);
        STATIC_REQUIRE(ValidWhyType<std::wstring>);
        STATIC_REQUIRE_FALSE(QuickWhyType<std::wstring>);
        STATIC_REQUIRE(ValidWhyType<ValidWhyObject>);
        STATIC_REQUIRE_FALSE(QuickWhyType<ValidWhyObject>);
    }

    SECTION("Invalid types")
    {
        STATIC_REQUIRE_FALSE(QuickWhyType<int>);
        STATIC_REQUIRE_FALSE(QuickWhyType<void *>);
        STATIC_REQUIRE_FALSE(QuickWhyType<double>);
    }

    SECTION("Invalid references and functions")
    {
        STATIC_REQUIRE_FALSE(QuickWhyType<int &>);
        STATIC_REQUIRE_FALSE(QuickWhyType<int const &>);
        STATIC_REQUIRE_FALSE(QuickWhyType<int &&>);
        STATIC_REQUIRE_FALSE(QuickWhyType<void()>);
    }
}

TEST_CASE("Simple BasicInvalid constructor overloads", "[invalid][constructor]")
{
    using TestInvalid = BasicInvalid<string_view>;

    SECTION("construction using argument forwarding constructor")
    {
        MessageInvalid test(WhyInvalid::Success, "My test invalid.");
        REQUIRE(test.why() == WhyInvalid::Success);
        REQUIRE(test.info() == "My test invalid.");
    }

    SECTION("Construction using Why_T copy constructor.")
    {
        auto const view = "My test invalid."sv;
        TestInvalid test(view);
        REQUIRE(test.why() == WhyInvalid::Unknown);
        REQUIRE(test.info() == "My test invalid.");
    }

    SECTION("Construction using Why_T rvalue constructor.")
    {
        auto view = "My test invalid."sv;
        TestInvalid test(std::move(view));
        REQUIRE(test.why() == WhyInvalid::Unknown);
        REQUIRE(test.info() == "My test invalid.");
    }
}

struct ComplexWhyType
{
    using CharType = char;
    WhyInvalid why;
    string message;

    WhyInvalid getWhyInvalid() const noexcept
    {
        return why;
    }

    string_view getInvalidInfo() const noexcept
    {
        return message;
    }
};

TEST_CASE("Complex BasicInvalid constructor overloads", "[invalid][constructor]")
{
    using TestInvalid = BasicInvalid<string>;

    SECTION("Construction using argument forwarding constructor")
    {
        BasicInvalid<ComplexWhyType> test(WhyInvalid::Success, "My test invalid.");
        REQUIRE(test.why() == WhyInvalid::Success);
        REQUIRE(test.info() == "My test invalid.");
    }

    SECTION("Construction using Why_T copy constructor.")
    {
        auto const view = "My test invalid."s;
        TestInvalid test(view);
        REQUIRE(test.why() == WhyInvalid::Unknown);
        REQUIRE(test.info() == "My test invalid.");
    }

    SECTION("Construction using Why_T rvalue constructor.")
    {
        auto view = "My test invalid."s;
        TestInvalid test(std::move(view));
        REQUIRE(test.why() == WhyInvalid::Unknown);
        REQUIRE(test.info() == "My test invalid.");
    }
}

TEST_CASE("Simple BasicInvalid Copy Constuction State", "[invalid][constructor]")
{
    Invalid first{ WhyInvalid::Success };

    REQUIRE(first.why() == WhyInvalid::Success);
    REQUIRE_FALSE(first.info().empty());

    Invalid second(first);

    REQUIRE(first.why() == WhyInvalid::Success);
    REQUIRE_FALSE(first.info().empty());
    REQUIRE(second.why() == WhyInvalid::Success);
    REQUIRE_FALSE(second.info().empty());
    REQUIRE(first.info() == second.info());
}

TEST_CASE("Complex BasicInvalid Copy Constuction State", "[invalid][constructor]")
{
    BasicInvalid<std::string> first{ "Test String."s };

    REQUIRE(first.why() == WhyInvalid::Unknown);
    REQUIRE(first.info() == "Test String.");

    BasicInvalid<std::string> second(first);

    REQUIRE(first.why() == WhyInvalid::Unknown);
    REQUIRE(first.info() == "Test String.");
    REQUIRE(second.why() == WhyInvalid::Unknown);
    REQUIRE(second.info() == "Test String.");
    REQUIRE(second == first);
}

TEST_CASE("Simple Invalid Move Construction State", "[invalid][constructor]")
{
    Invalid first{ WhyInvalid::Success };

    REQUIRE(first.why() == WhyInvalid::Success);
    REQUIRE_FALSE(first.info().empty());

    Invalid second(std::move(first));

    REQUIRE(first.why() == WhyInvalid::Success);
    REQUIRE_FALSE(first.info().empty());
    REQUIRE(second.why() == WhyInvalid::Success);
    REQUIRE_FALSE(second.info().empty());
    REQUIRE(first.info() == second.info());
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
    ThrowTest([[maybe_unused]] ThrowTest const &other)
    {
    }
    ThrowTest([[maybe_unused]] ThrowTest &&other)
    {
    }

    bool should = false;
};

static_assert(std::is_nothrow_copy_constructible_v<NothrowTest>);
static_assert(std::is_nothrow_move_constructible_v<NothrowTest>);
static_assert(!std::is_nothrow_copy_constructible_v<ThrowTest>);
static_assert(!std::is_nothrow_move_constructible_v<ThrowTest>);

TEST_CASE("Good Data MaybeInv Construction State", "[maybeinv][constructor]")
{
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
    auto tform = [](NothrowTest const &nt, int another) -> MaybeInv<NothrowTest> { return NothrowTest{ another }; };
    MaybeInv<NothrowTest> next = good.then(tform, 10);
    REQUIRE(good);
    REQUIRE(next);
    REQUIRE_FALSE(good.get() == nullptr);
    REQUIRE_FALSE(next.get() == nullptr);
    REQUIRE(good.get()->id == 5);
    REQUIRE(next.get()->id == 10);
    auto diffType      = [](NothrowTest const &nt) -> MaybeInv<int> { return nt.id; };
    MaybeInv<int> diff = next.then(diffType);
    REQUIRE(diff);
    REQUIRE_FALSE(diff.get() == nullptr);
    REQUIRE(*diff.get() == 10);
}
