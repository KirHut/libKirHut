/***********************************************************************************************************************
** The KirHut Application Development Library
** TestErrors.cpp
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
    EnvironmentProblem ep("Test 2");
    IllegalArgument illegal("Test 3");

    REQUIRE(sucker.why() == WhyInvalid::SoftwareError);
    REQUIRE(ep.why() == WhyInvalid::BadEnvironment);
    REQUIRE(illegal.why() == WhyInvalid::IllegalArgument);
}

TEST_CASE("Check if Error type returns constructed string from .info()", "[errors][Error]")
{
    KirHutSucksAtProgramming sucker("Test 1");
    EnvironmentProblem ep("Test 2");
    IllegalArgument illegal("Test 3");

    REQUIRE(sucker.info() == "Test 1");
    REQUIRE(ep.info() == "Test 2");
    REQUIRE(illegal.info() == "Test 3");
}

TEST_CASE("Check if Error type returns constructed string from .what()", "[errors][Error]")
{
    string_view suckerString  = "Test 1"sv;
    string_view epString      = "Test 2"sv;
    string_view illegalString = "Test 3"sv;

    KirHutSucksAtProgramming sucker(suckerString);
    EnvironmentProblem ep(epString);
    IllegalArgument illegal(illegalString);

    REQUIRE(std::strncmp(sucker.what(), suckerString.data(), suckerString.size()) == 0);
    REQUIRE(std::strncmp(ep.what(), epString.data(), epString.size()) == 0);
    REQUIRE(std::strncmp(illegal.what(), illegalString.data(), illegalString.size()) == 0);
}
