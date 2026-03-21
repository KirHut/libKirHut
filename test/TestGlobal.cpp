/***********************************************************************************************************************
** The KirHut Application Development Library
** TestGlobal.cpp
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
#include "kh/global.hpp"

#include <array>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
// clazy:excludeall=non-pod-global-static

using namespace KirHut;

consteval auto generateWhyInvalidList() noexcept
{
    using enum WhyInvalid;

    return std::array{
        Success,
        Unknown,
        IncorrectInput,
        FileNotFound,
        TooManyOpenFiles,
        CouldntOpenFile,
        InvalidHandle,
        ArenaTrashed,
        OutOfBounds,
        DataRemoved,
        BadEnvironment,
        PatternMismatch,
        AlreadyInitialized,
        IllegalArgument,
        OutOfMemory,
        CurrentDirectory,
        DataUninitialized,
        InvalidState,
        BadCRCResult,
        BadHashResult,
        IllegalOverflow,
        CausesUndefined,
        HandleEndOfFile,
        DiskFullError,
        ConfigCmdInvalid,
        IncorrectDataFormat,
        CannotOpenInput,
        UsernameUnknown,
        HostnameUnknown,
        ServiceUnavailable,
        SoftwareError,
        OperatingSystemError,
        OSFileMissingError,
        CannotCreateFile,
        InputOutputError,
        TemporaryError,
        ProtocolError,
        PermissionDenied,
        ConfigurationError,
    };
}

constexpr auto whyInvalidList = generateWhyInvalidList();

TEST_CASE("Test return values of exitCode()", "[global][exitCode]")
{
    auto why       = GENERATE(from_range(whyInvalidList));
    int integerVal = static_cast<int>(why);
    REQUIRE(KirHut::exitCode(why) == integerVal);
}

TEST_CASE("Test constexpr WhyInvalid comparison operators", "[global][WhyInvalid]")
{
    constexpr auto first = WhyInvalid::Success, second = WhyInvalid::ArenaTrashed;
    STATIC_REQUIRE(first != second);
    STATIC_REQUIRE(first < second);
    STATIC_REQUIRE(first <= second);
    STATIC_REQUIRE((first <=> second) == std::strong_ordering::less);

    constexpr auto third = WhyInvalid::DiskFullError, fourth = WhyInvalid::IllegalArgument;
    STATIC_REQUIRE(third != fourth);
    STATIC_REQUIRE(third > fourth);
    STATIC_REQUIRE(third >= fourth);
    STATIC_REQUIRE((third <=> fourth) == std::strong_ordering::greater);

    constexpr auto fifth = WhyInvalid::OutOfBounds, sixth = WhyInvalid::OutOfBounds;
    STATIC_REQUIRE(fifth == sixth);
    STATIC_REQUIRE(fifth <= sixth);
    STATIC_REQUIRE(fifth >= sixth);
    STATIC_REQUIRE((fifth <=> sixth) == std::strong_ordering::equal);
}

TEST_CASE("Test WhyInvalid comparison operators", "[global][WhyInvalid]")
{
    auto first = WhyInvalid::Success, second = WhyInvalid::ArenaTrashed;
    REQUIRE(first != second);
    REQUIRE(first < second);
    REQUIRE(first <= second);
    REQUIRE((first <=> second) == std::strong_ordering::less);

    first = WhyInvalid::DiskFullError, second = WhyInvalid::IllegalArgument;
    REQUIRE(first != second);
    REQUIRE(first > second);
    REQUIRE(first >= second);
    REQUIRE((first <=> second) == std::strong_ordering::greater);

    first = WhyInvalid::OutOfBounds, second = WhyInvalid::OutOfBounds;
    REQUIRE(first == second);
    REQUIRE(first <= second);
    REQUIRE(first >= second);
    REQUIRE((first <=> second) == std::strong_ordering::equal);
}

TEST_CASE("Ensure correct integer types from Integer", "[global][Integer]")
{
    SECTION("Direct Integer type checks")
    {
        STATIC_REQUIRE(std::same_as<Integer<>, int>);
        STATIC_REQUIRE(std::same_as<Integer<1>, signed char>);
        STATIC_REQUIRE(std::same_as<Integer<1, false>, unsigned char>);
    }
}
