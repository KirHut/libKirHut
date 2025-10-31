/***********************************************************************************************************************
** The KirHut Application Development Library
** TestErrors.cpp
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
#include "kh/errors.hpp"

#include <catch2/catch_test_macros.hpp>
// clazy:excludeall=non-pod-global-static

using namespace KirHut;

TEST_CASE("Check if Error type is constructible.", "[errors][Error]")
{
    string_view suckerString = "This should construct correctly."sv;

    KirHutSucksAtProgramming sucker(suckerString);

    REQUIRE(sucker.info() == "This should construct correctly.");
    REQUIRE(sucker.why() == WhyInvalid::SoftwareError);
    REQUIRE(std::strncmp(sucker.what(), suckerString.data(), suckerString.size()) == 0);
}

TEST_CASE("Check if Error type returns correct WhyInvalid types.", "[errors][Error]")
{
    KirHutSucksAtProgramming sucker("Test 1");
    BullshitEnvironment bs("Test 2");
    IllegalArgument illegal("Test 3");

    REQUIRE(sucker.why() == WhyInvalid::SoftwareError);
    REQUIRE(bs.why() == WhyInvalid::BadEnvironment);
    REQUIRE(illegal.why() == WhyInvalid::IllegalArgument);
}

TEST_CASE("Check if Error type returns constructed string from .info()", "[errors][Error]")
{
    KirHutSucksAtProgramming sucker("Test 1");
    BullshitEnvironment bs("Test 2");
    IllegalArgument illegal("Test 3");

    REQUIRE(sucker.info() == "Test 1");
    REQUIRE(bs.info() == "Test 2");
    REQUIRE(illegal.info() == "Test 3");
}

TEST_CASE("Check if Error type returns constructed string from .what()", "[errors][Error]")
{
    string_view suckerString  = "Test 1"sv;
    string_view bsString      = "Test 2"sv;
    string_view illegalString = "Test 3"sv;

    KirHutSucksAtProgramming sucker(suckerString);
    BullshitEnvironment bs(bsString);
    IllegalArgument illegal(illegalString);

    REQUIRE(std::strncmp(sucker.what(), suckerString.data(), suckerString.size()) == 0);
    REQUIRE(std::strncmp(bs.what(), bsString.data(), bsString.size()) == 0);
    REQUIRE(std::strncmp(illegal.what(), illegalString.data(), illegalString.size()) == 0);
}
