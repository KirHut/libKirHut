/***********************************************************************************************************************
** The KirHut Application Development Library
** TestArgs.cpp
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

#include "kh/args.hpp"

#include <catch2/catch_test_macros.hpp>
// clazy:excludeall=non-pod-global-static

using namespace KirHut;
using namespace KirHut::CLI;

TEST_CASE("Construct basic Option objects", "[option][constructor]")
{
    Option op("t test-option");
    REQUIRE(op.isFlag);
    REQUIRE(op.opNames == "t test-option");

    Option op2("c config-file", true);
    REQUIRE_FALSE(op2.isFlag);
    REQUIRE(op2.opNames == "c config-file");
}

TEST_CASE("Matching functionality of Option object", "[option]")
{
    Option op("c config-file", true);
    Option flag("f flag", false);

    auto failFlag1 = flag.matches("-n");
    REQUIRE_FALSE(failFlag1.has_value());

    auto failFlag2 = flag.matches("--not-flag");
    REQUIRE_FALSE(failFlag2.has_value());

    auto failFlag3 = flag.matches("flag");
    REQUIRE_FALSE(failFlag3.has_value());

    auto failFlag4 = flag.matches("f");
    REQUIRE_FALSE(failFlag4.has_value());

    auto failMatch1 = op.matches("-e");
    REQUIRE_FALSE(failMatch1.has_value());

    auto failMatch2 = op.matches("--something-else");
    REQUIRE_FALSE(failMatch2.has_value());

    auto failMatch3 = op.matches("-cba");
    REQUIRE_FALSE(failMatch3.has_value());

    auto failMatch4 = op.matches("config-file");
    REQUIRE_FALSE(failMatch4.has_value());

    auto failMatch5 = flag.matches("c");
    REQUIRE_FALSE(failMatch5.has_value());

    auto flag1 = flag.matches("-f");
    REQUIRE(flag1.has_value());
    REQUIRE_FALSE(flag1->parameterString.has_value());
    REQUIRE(flag1->matched[0] == 'f');

    auto flag2 = flag.matches("--flag");
    REQUIRE(flag2.has_value());
    REQUIRE_FALSE(flag2->parameterString.has_value());
    REQUIRE("flag"sv == flag2->matched);

    auto buf   = "-buf"sv;
    auto flag3 = flag.matches(buf);
    REQUIRE(flag3.has_value());
    REQUIRE_FALSE(flag3->parameterString.has_value());
    REQUIRE(flag3->matched[0] == 'f');
    REQUIRE(flag3->matched == buf.substr(3).data());

    auto fun   = "-fun"sv;
    auto flag4 = flag.matches(fun);
    REQUIRE(flag4.has_value());
    REQUIRE_FALSE(flag4->parameterString.has_value());
    REQUIRE(flag4->matched[0] == 'f');
    REQUIRE(flag4->matched == fun.substr(1).data());

    auto match1 = op.matches("-c");
    REQUIRE(match1.has_value());
    REQUIRE(match1->requiresString());
    REQUIRE(match1->matched[0] == 'c');

    auto configFile = "config-file"sv;
    auto match2     = op.matches("--config-file");
    REQUIRE(match2.has_value());
    REQUIRE(match2->requiresString());
    REQUIRE(configFile == match2->matched);

    auto argWithParam = "--config-file=MYFILE"sv;
    auto match3       = op.matches(argWithParam);
    REQUIRE(match3.has_value());
    REQUIRE(match3->parameterString.has_value());
    REQUIRE(match3->parameterString.value() == "MYFILE");
    // I can now do things like "MYFILE"s.size() since C++20 constexpr strings will just compile to the result.
    REQUIRE(match3->parameterString.value().data() == &*(argWithParam.end() - "MYFILE"s.size()));
    REQUIRE(configFile == string_view{ match3->matched, configFile.size() });

    auto abc    = "-abc"sv;
    auto match4 = op.matches(abc);
    REQUIRE(match4.has_value());
    REQUIRE(match4->requiresString());
    REQUIRE(match4->matched[0] == 'c');
    REQUIRE(match4->matched == abc.substr(3).data());
}

TEST_CASE("Construct basic Command objects", "[command][constructor]")
{
    Command cmd1("Test", OptionList{ { "f flag", false }, { "c config-file", true } });
    REQUIRE(cmd1.matches("Test"));
    auto cmd1Ops = cmd1.getOpts();
    REQUIRE(cmd1Ops[0].isFlag);
    REQUIRE(cmd1Ops[0].opNames == "f flag");
    REQUIRE_FALSE(cmd1Ops[1].isFlag);
    REQUIRE(cmd1Ops[1].opNames == "c config-file");

    Command cmd2("Test2");
    REQUIRE(cmd2.matches("Test2"));
    REQUIRE(cmd2.getOpts().empty());
}

TEST_CASE("Throw exceptions with improper Command construction", "[command][constructor]")
{
    REQUIRE_THROWS([] { Command cmd("5"); }());
    REQUIRE_THROWS([] { Command cmd("NameWith\\Backslash"); }());
    REQUIRE_THROWS([] { Command cmd("Name With Spaces"); }());
    REQUIRE_THROWS([] { Command cmd("NameWith\nEndline"); }());
}

TEST_CASE("Matching functionality of Command objects", "[command]")
{
    Command cmd("TestCommand");
    REQUIRE_FALSE(cmd.matches("testcommand"));
    REQUIRE_FALSE(cmd.matches("SomeString"));
    REQUIRE_FALSE(cmd.matches("TestComman"));
    REQUIRE_FALSE(cmd.matches("Test Command"));
    REQUIRE_FALSE(cmd.matches(" TestCommand"));
    REQUIRE_FALSE(cmd.matches("-TestCommand"));
    REQUIRE_FALSE(cmd.matches("--TestCommand"));
    REQUIRE_FALSE(cmd.matches("\"\\;<>/?*!@#$%^&()"));
    REQUIRE(cmd.matches("TestCommand"));
}

TEST_CASE("Matching of Options functionality of Command objects", "[command][option]")
{
    Command cmd("TestCommand", OptionList{ { "f flag", false }, { "c config-file", true } });
    REQUIRE(cmd.matches("TestCommand"));

    auto failMatch1 = cmd.matchOpt("--config-fil");
    REQUIRE_FALSE(failMatch1.has_value());

    auto failMatch2 = cmd.matchOpt("--fla");
    REQUIRE_FALSE(failMatch2.has_value());

    auto failMatch3 = cmd.matchOpt("config-file");
    REQUIRE_FALSE(failMatch3.has_value());

    auto failMatch4 = cmd.matchOpt("-v");
    REQUIRE_FALSE(failMatch4.has_value());

    auto failMatch5 = cmd.matchOpt("f");
    REQUIRE_FALSE(failMatch5.has_value());

    auto match1 = cmd.matchOpt("--flag");
    REQUIRE(match1.has_value());
    REQUIRE_FALSE(match1->parameterString.has_value());
    REQUIRE("flag"sv == match1->matched);

    auto configFile = "config-file"sv;
    auto match2     = cmd.matchOpt("--config-file");
    REQUIRE(match2.has_value());
    REQUIRE(match2->requiresString());
    REQUIRE(configFile == match2->matched);

    auto argWithParam = "--config-file=FILENAME"sv;
    auto match3       = cmd.matchOpt(argWithParam);
    REQUIRE(match3.has_value());
    REQUIRE(match3->parameterString.has_value());
    auto paramView = match3->parameterString.value();
    REQUIRE(paramView == "FILENAME");
    REQUIRE(paramView.data() == &*(argWithParam.end() - paramView.size()));
    REQUIRE(configFile == string_view{ match3->matched, configFile.size() });

    auto match4 = cmd.matchOpt("-f");
    REQUIRE(match4.has_value());
    REQUIRE_FALSE(match4->parameterString.has_value());
    REQUIRE(match4->matched[0] == 'f');

    auto abc    = "-abc"sv;
    auto match5 = cmd.matchOpt(abc);
    REQUIRE(match5.has_value());
    REQUIRE(match5->requiresString());
    REQUIRE(match5->matched[0] == 'c');
    REQUIRE(match5->matched == abc.substr(3).data());
}
