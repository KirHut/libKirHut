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

TEST_CASE("Simple Invalid Construction State", "[invalid][constructor]")
{
    Invalid in{ WhyInvalid::Unknown };
    REQUIRE(in.why() == WhyInvalid::Unknown);
    REQUIRE(in.info().empty());
}

TEST_CASE("Invalid Copy Constuction State", "[invalid][constructor]")
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

TEST_CASE("Invalid Move Construction State", "[invalid][constructor]")
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
