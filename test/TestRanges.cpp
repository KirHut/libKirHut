/***********************************************************************************************************************
** The KirHut Application Development Library
** TestRanges.cpp
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
#include "kh/ranges.hpp"

#include <catch2/catch_test_macros.hpp>
// clazy:excludeall=non-pod-global-static

#include "kh/base.hpp"

#include <algorithm>

using namespace KirHut;

namespace R = Ranges;
namespace V = Views;

TEST_CASE("The R::getIters() method in ranges.hpp", "[ranges][utility][getIters]")
{
    std::vector<int> nums{ 1, 2, 3, 4, 5 };
    auto [f, b] = R::getIters(nums);
    REQUIRE(f == nums.begin());
    REQUIRE(b == nums.end());
}

TEST_CASE("The R::getIters() method and the IterPair object", "[ranges][utility][getIters][IterPair]")
{
    std::vector<int> nums{ 1, 2, 3, 4, 5 };
    auto pair = R::getIters(nums);
    REQUIRE(pair.begin == nums.begin());
    REQUIRE(pair.end == nums.end());
}

TEST_CASE("Separators basic behavior", "[ranges][Separators]")
{
    constexpr R::Separators<char> ws{ " \t\n\r" };
    STATIC_REQUIRE(std::is_trivially_copyable_v<R::Separators<char>>);
    STATIC_REQUIRE(std::is_nothrow_constructible_v<R::Separators<char>, string_view>);
    STATIC_REQUIRE(std::string_view(ws) == " \t\n\r"sv);
}

TEST_CASE("WordView basic construction", "[ranges][WordView]")
{
    SECTION("Construct from string literal with default separators")
    {
        constexpr R::WordView wv("alpha beta gamma");
        STATIC_REQUIRE(R::distance(wv) == 3);
    }

    SECTION("Construct from string and custom separators")
    {
        constexpr R::WordView csv("a,b,c", R::Separators{ "," });
        STATIC_REQUIRE(R::distance(csv) == 3);
    }

    SECTION("Construct with empty separators (whole string is one word)")
    {
        R::WordView wv("no splits", R::Separators{ "" });
        auto words = std::vector<std::string_view>(wv.begin(), wv.end());
        REQUIRE(words.size() == 1);
        REQUIRE(words[0] == "no splits");
    }
}

TEST_CASE("WordView iteration correctness", "[ranges][WordView]")
{
    R::WordView wv("alpha beta  gamma\tdelta");
    std::vector<std::string_view> expected{ "alpha", "beta", "gamma", "delta" };

    std::vector<std::string_view> actual;
    for (auto word : wv)
    {
        actual.push_back(word);
    }

    REQUIRE(actual == expected);
}

TEST_CASE("WordView handles multiple consecutive separators", "[ranges][WordView]")
{
    R::WordView wv("a,,b,,,c", R::Separators{ "," });
    std::vector<std::string_view> words(wv.begin(), wv.end());
    REQUIRE(words == std::vector<std::string_view>{ "a", "b", "c" });
}

TEST_CASE("WordView handles leading and trailing separators", "[ranges][WordView]")
{
    R::WordView wv("   spaced words   ");
    std::vector<std::string_view> words(wv.begin(), wv.end());
    REQUIRE(words == std::vector<std::string_view>{ "spaced", "words" });
}

TEST_CASE("WordView with alternate separator set", "[ranges][WordView]")
{
    R::WordView wv("x-y z", R::Separators{ "- " });
    std::vector<std::string_view> words(wv.begin(), wv.end());
    REQUIRE(words == std::vector<std::string_view>{ "x", "y", "z" });
}

TEST_CASE("WordView iterator operations", "[ranges][WordView][iterator]")
{
    R::WordView wv("one two three");
    auto it = wv.begin();

    REQUIRE(*it == "one");
    ++it;
    REQUIRE(*it == "two");

    auto pre = ++it;
    REQUIRE(*pre == "three");

    auto post = it++;
    REQUIRE(*post == "three");
    REQUIRE(it == wv.end());
}

TEST_CASE("WordView equality comparisons", "[ranges][WordView]")
{
    string s = "a b c";
    R::WordView wv1(s);
    R::WordView wv2(s);
    REQUIRE(wv1 == wv2);

    R::WordView wv3(s, R::Separators{ "," });
    REQUIRE_FALSE(wv1 == wv3);

    string s2 = "a b c";
    R::WordView wv4(s2);
    // Although s2 and s have same content, they are at different addresses
    REQUIRE_FALSE(wv1 == wv4);
}

TEST_CASE("WordView behaves as a forward range", "[ranges][WordView]")
{
    R::WordView wv("alpha beta gamma");
    STATIC_REQUIRE(R::forward_range<R::WordView<char>>);
    STATIC_REQUIRE(R::view<R::WordView<char>>);

    auto count = R::count_if(wv, [](auto sv) { return sv.size() > 4; });
    REQUIRE(count == 2);
}

TEST_CASE("words CPO basic callable behavior", "[ranges][words]")
{
    auto vw = V::words("alpha beta gamma");
    STATIC_REQUIRE(R::view<decltype(vw)>);
    STATIC_REQUIRE(std::same_as<R::range_value_t<decltype(vw)>, string_view>);

    std::vector<string_view> words_vec(vw.begin(), vw.end());
    REQUIRE(words_vec == std::vector<string_view>{ "alpha", "beta", "gamma" });
}

TEST_CASE("words CPO pipe syntax", "[ranges][words][pipe]")
{
    auto vw = "a b c"sv | V::words;
    std::vector<std::string_view> result(vw.begin(), vw.end());
    REQUIRE(result == std::vector<std::string_view>{ "a", "b", "c" });
}

TEST_CASE("words CPO with custom separators callable form", "[ranges][words]")
{
    auto vw = V::words("a,b,c", R::Separators{ "," });
    std::vector<std::string_view> result(vw.begin(), vw.end());
    REQUIRE(result == std::vector<std::string_view>{ "a", "b", "c" });
}

TEST_CASE("words CPO with custom separators via pipe form", "[ranges][words][pipe]")
{
    auto vw = "x-y z"sv | V::words(R::Separators{ "- " });
    std::vector<std::string_view> result(vw.begin(), vw.end());
    REQUIRE(result == std::vector<std::string_view>{ "x", "y", "z" });
}

TEST_CASE("words CPO pipe form handles leading/trailing separators", "[ranges][words][pipe]")
{
    auto vw = "   one  two   three   "sv | V::words;
    std::vector<std::string_view> result(vw.begin(), vw.end());
    REQUIRE(result == std::vector<std::string_view>{ "one", "two", "three" });
}

TEST_CASE("words CPO equivalence to direct WordView construction", "[ranges][WordView][words]")
{
    std::string s  = "a b c";
    auto wv_direct = R::WordView(s);
    auto wv_cpo    = V::words(s);

    std::vector<std::string_view> direct(wv_direct.begin(), wv_direct.end());
    std::vector<std::string_view> via_cpo(wv_cpo.begin(), wv_cpo.end());

    REQUIRE(direct == via_cpo);
    REQUIRE(wv_direct == wv_cpo);
}

TEST_CASE("words CPO constexpr usability", "[ranges][words]")
{
    constexpr auto vw    = V::words("one two");
    constexpr auto first = *vw.begin();
    STATIC_REQUIRE(first == "one");
}

TEST_CASE("words CPO with no separators produces whole string", "[ranges][words]")
{
    auto vw = V::words("nosplit", R::Separators{ "" });
    std::vector<std::string_view> result(vw.begin(), vw.end());
    REQUIRE(result == std::vector<std::string_view>{ "nosplit" });
}
