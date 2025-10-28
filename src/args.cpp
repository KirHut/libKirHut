/***********************************************************************************************************************
** The KirHut Application Development Library
** args.cpp
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

#if KH_WINDOWS
# include "nowide/args.hpp"
#endif

#include "kh/errors.hpp"

#include <algorithm>

namespace KirHut::CLI
{

string readOptionString(string_view toTest)
{
    // This tester uses spaces to delimit each possible option.
    std::vector<string_view> tokens;
    constexpr string_view illegalSingleChars = "-0123456789";
    constexpr string_view illegalAnyChars    = "=/;,'\"\\\t\n";

    size_t first = 0, last = 0;

    if (toTest.size() < 1)
    {
        throw IllegalArgument("There must be at least one character in an Option constructor's string.");
    }

    while ((first = toTest.find_first_not_of(' ', last)) != string::npos)
    {
        last = toTest.find(' ', first);
        tokens.push_back(toTest.substr(first, last - first));
    }

    if (tokens.size() == 0)
    {
        throw IllegalArgument("An option string must consist of more than just space characters.");
    }

    string retVal;

    for (string_view token : tokens)
    {
        if (token.size() == 1 && illegalSingleChars.find(token[0]) != string::npos)
        {
            string message = "The character ";
            message += token[0];
            message += " is an illegal character to use for a single character Option.";
            throw IllegalArgument(std::move(message));
        }
        else if (token.starts_with('-'))
        {
            throw IllegalArgument("An option cannot begin with the dash \"-\" character.");
        }
        else if (size_t loc; std::any_of(illegalAnyChars.begin(), illegalAnyChars.end(), [&](char illegal) {
                     return (loc = token.find(illegal)) != string::npos;
                 }))
        {
            string message = "The ";
            if (token[loc] == '\n' || token[loc] == '\t')
            {
                message += token[loc] == '\n' ? "endline" : "tab";
            }
            else
            {
                message += token[loc];
            }

            message += " character is illegal to use in an Option string.";
            throw IllegalArgument(std::move(message));
        }

        if (!retVal.empty())
        {
            retVal.push_back(' ');
        }

        retVal.append(token);
    }

    return retVal;
}

string_view readNextOptionValue(string_view optionString, size_t &loc) noexcept
{
    if (loc >= optionString.size())
    {
        return "";
    }

    size_t nextSpace = optionString.find_first_of(' ', loc);
    string_view ret  = optionString.substr(loc, nextSpace - loc);
    loc              = nextSpace == string_view::npos ? optionString.size() : nextSpace;

    return ret;
}

inline char readNextSingleCharOption(string_view optionString, size_t &loc) noexcept
{
    for (size_t nextSpace; loc < optionString.size(); loc = nextSpace + 1)
    {
        nextSpace = optionString.find_first_of(' ', loc);

        if (nextSpace == string_view::npos)
        {
            nextSpace = optionString.size();
        }

        if (nextSpace - loc == 1)
        {
            return optionString[loc++];
        }
    }

    return ' ';
}

void testCommandString(string_view commandString)
{
    constexpr string_view illegalChars = "=/;,'\"\\\t\n ";
    constexpr string_view illegalStartingChars = "-0123456789";

    if (commandString.empty())
    {
        // An empty command string is legitimate in the case of not using a command string at all.
        return;
    }

    if (R::any_of(illegalStartingChars, [&](char illegal) { return commandString.front() == illegal; }))
    {
        throw IllegalArgument("A command cannot begin with any of these characters: \"-0123456789\".");
    }

    for (char check : commandString)
    {
        if (size_t loc = illegalChars.find(check); loc != string_view::npos)
        {
            string message = "The ";
            if (illegalChars[loc] == '\n' || illegalChars[loc] == '\t' || illegalChars[loc] == ' ')
            {
                message += illegalChars[loc] == '\n' ? "endline" : (illegalChars[loc] == '\t' ? "tab" : "space");
            }
            else
            {
                message += illegalChars[loc];
            }

            message += " character is illegal to use in a Command name.";
            throw IllegalArgument(std::move(message));
        }
    }
}

// The next two are set as constexpr to allow for inlining.
inline Maybe<Option::Match> singleMatchesFlagImpl(Option const &op, string_view toCheck) noexcept
{
    size_t const start = toCheck.starts_with('-') ? 1 : 0;
    char check;
    for (size_t pos = 0; (check = readNextSingleCharOption(op.opNames, pos)) != ' '; ++pos)
    {
        if (size_t loc = toCheck.find(check, start); loc != string_view::npos)
        {
            return Option::Match{ op, &toCheck[loc] };
        }
    }

    return maybeNot;
}

inline Maybe<Option::Match> singleMatchesOptionImpl(Option const &op, string_view toCheck) noexcept
{
    char check;
    for (size_t pos = 0; (check = readNextSingleCharOption(op.opNames, pos)) != ' '; ++pos)
    {
        if (toCheck.ends_with(check))
        {
            return Option::Match{ op, &toCheck.back(), "" };
        }
    }

    return maybeNot;
}

Option::Option(Detail::OptionString optionNames, bool takesArgument) noexcept :
    opNames(optionNames.data),
    isFlag(not takesArgument)
{
    // No further implementation.
}

Option::Option([[maybe_unused]] RuntimeFlag rt, string_view optionNames, bool takesArgument) :
    opNames(optionNames),
    isFlag(not takesArgument)
{
    Detail::validateOptionString(optionNames);
}

bool Option::operator==(const Option &other) const noexcept
{
    return opNames == other.opNames and isFlag == other.isFlag;
}

Maybe<Option::Match> Option::singleMatches(string_view toCheck) const noexcept
{
    if (toCheck.empty())
    {
        return maybeNot;
    }

    return isFlag ? singleMatchesFlagImpl(*this, toCheck) : singleMatchesOptionImpl(*this, toCheck);
}

Maybe<Option::Match> Option::multiMatches(string_view toCheck) const noexcept
{
    if (toCheck.starts_with("--"))
    {
        toCheck = toCheck.substr(2);
    }

    if (toCheck.empty())
    {
        return maybeNot;
    }

    string_view opValue;

    size_t equalLoc      = toCheck.find_first_of('=');
    string_view checkArg = toCheck.substr(0, equalLoc);

    if (isFlag and equalLoc != string_view::npos)
    {
        return maybeNot;
    }

    // This will infinite loop if opNames.size() == SIZE_MAX, but that's so unlikely I don't see a reason to worry.
    for (size_t loc = 0; !(opValue = readNextOptionValue(opNames, loc)).empty(); ++loc)
    {
        if (checkArg == opValue)
        {
            if (equalLoc != string_view::npos and ++equalLoc < toCheck.size())
            {
                return Match{ *this, toCheck.data(), toCheck.substr(equalLoc) };
            }

            return Match{ *this, toCheck.data(), isFlag ? maybeNot : std::optional{ "" } };
        }
    }

    return maybeNot;
}

Maybe<Option::Match> Option::matches(string_view toCheck) const noexcept
{
    if (toCheck.size() > 1 and toCheck.starts_with('-'))
    {
        if (toCheck[1] == '-')
        {
            return multiMatches(toCheck);
        }

        return singleMatches(toCheck);
    }

    return maybeNot;
}

Command::Command(string_view commandName) : commandString(commandName)
{
    testCommandString(commandName);
}

Command::Command(string_view commandName, OptionList &&ops) : Command(commandName)
{
    options = std::move(ops);
}

Command::Command(string_view commandName, OptionView ops) : Command(commandName)
{
    options.reserve(ops.size());

    for (Option const &op : ops)
    {
        addOpt(op);
    }
}

void Command::addOpt(Option const &option)
{
    options.push_back(option);
}

void Command::addOpt(Option &&option)
{
    options.push_back(std::move(option));
}

void Command::addOpt(Detail::OptionString optionNames, bool takesArgument)
{
    options.emplace_back(optionNames, takesArgument);
}

OptionView Command::getOpts() const noexcept
{
    return options;
}

bool Command::matches(string_view toCheck) const noexcept
{
    return toCheck == commandString;
}

Maybe<Option::Match> Command::matchOpt(string_view toCheck) const noexcept
{
    for (Option const &op : options)
    {
        if (auto maybeMatch = op.matches(toCheck); maybeMatch.has_value())
        {
            return maybeMatch;
        }
    }

    return maybeNot;
}

OptionMatches::OptionMatches(Option const &option, bool hasArguments) noexcept : option(option)
{
    if (hasArguments)
    {
        matches = std::vector<string_view>{};
    }
}

u32 OptionMatches::matchesFound() const noexcept
{
    u32 const *test = std::get_if<u32>(&matches);
    return test ? *test : std::get<std::vector<string_view>>(matches).size();
}

span<string_view const> const OptionMatches::argumentsFound() const noexcept
{
    static const auto emptySpan = std::vector<string_view>{};
    auto const *test            = std::get_if<std::vector<string_view>>(&matches);
    return test ? *test : emptySpan;
}

void OptionMatches::matchFound() noexcept
{
    u32 *val = std::get_if<u32>(&matches);

    if (!val)
    {
        std::get<std::vector<string_view>>(matches).push_back("");
    }

    ++*val;
}

void OptionMatches::matchFound(string_view argument)
{
    auto *list = std::get_if<std::vector<string_view>>(&matches);

    if (!list)
    {
        ++std::get<u32>(matches);
    }

    list->push_back(argument);
}

struct PrivArgs
{
    constexpr PrivArgs([[maybe_unused]] int, [[maybe_unused]] char **) noexcept
    {
        // No further implementation.
    }

    constexpr PrivArgs([[maybe_unused]] int, [[maybe_unused]] char **, [[maybe_unused]] char **) noexcept
    {
        // No further implementation.
    }
};

#if KH_WINDOWS
using KhArgs = nowide::args;
#else
using KhArgs = PrivArgs;
#endif

struct Parser::Impl
{
    struct ArgTest
    {
        ArgTest(int argc, char **argv)
        {
            if (argc < 1)
            {
                throw IllegalArgument("There must be at least one argument in the Parser's arguments list.");
            }

            if (!argv)
            {
                throw IllegalArgument("The Parser object must be provided a non-null argv pointer.");
            }
        }
        ArgTest(int argc, char **argv, char **envp) : ArgTest(argc, argv)
        {
            if (!envp)
            {
                throw KirHutSucksAtProgramming("The ArgTest constructor should never be called with an invalid envp.");
            }
        }
    };

    [[KH_ATTR_NO_UNIQUE_ADDRESS]] ArgTest test;

    static Command const defaultCmd;

    int numArgs;
    char **args;
    char **env = nullptr;

    KhArgs utf8Args;
    std::vector<string_view> positional;
    std::vector<OptionMatches> matched;
    Maybe<Command const> active;

    bool success = true;

    int extractCommand(span<Command> &commands)
    {
        Command const *emptyCmd = nullptr;
        bool foundEmpty         = false;
        for (Command &cmd : commands)
        {
            if (cmd.matches(""))
            {
                emptyCmd   = &cmd;
                foundEmpty = true;
            }
        }

        // This screams for the elvis operator, but alas I must support MSVC...
        emptyCmd = emptyCmd ? emptyCmd : &defaultCmd;

        if (numArgs > 1)
        {
            for (Command &cmd : commands)
            {
                if (cmd.matches(args[1]))
                {
                    active.emplace(cmd);
                    return 2;
                }
            }
        }

        active.emplace(*emptyCmd);
        success = foundEmpty;
        return success ? 1 : -1;
    }

    void parseArguments(span<Command> &commands)
    {
        if (commands.size() == 0)
        {
            success = false;
            throw IllegalArgument("You must provide at least one Command to the Parser object.");
        }

        int arg = extractCommand(commands);

        if (arg < 1)
        {
            return;
        }

        for (; arg < numArgs; ++arg)
        {
            string_view currentArg = args[arg];
            if (currentArg.size() < 2)
            {
                positional.push_back(currentArg);
                continue;
            }

            if (currentArg.starts_with('-'))
            {
                if (currentArg[1] == '-')
                {
                    if (currentArg.size() == 2)
                    {
                        while (++arg < numArgs)
                        {
                            positional.push_back(args[arg]);
                        }

                        return;
                    }

                    bool matchFound = false;
                    for (Option const &op : active->getOpts())
                    {
                        if (auto match = op.matches(currentArg); match.has_value())
                        {
                            string_view argumentStr;
                            if (match->requiresString())
                            {
                                if (++arg < numArgs)
                                {
                                    argumentStr = args[arg];
                                }
                                else
                                {
                                    success = false;
                                }
                            }
                            else if (match->parameterString.has_value())
                            {
                                argumentStr = match->parameterString.value();
                            }

                            auto foundMatch =
                                R::find_if(matched, [&](OptionMatches const &pot) { return pot.option == op; });
                            if (foundMatch == matched.end())
                            {
                                // The constructor is private so we cannot use emplace_back directly...
                                OptionMatches newMatch(op, op.isFlag);
                                matched.emplace_back(std::move(newMatch));
                                foundMatch = matched.end() - 1;
                            }

                            if (argumentStr.empty())
                            {
                                foundMatch->matchFound();
                            }
                            else
                            {
                                foundMatch->matchFound(argumentStr);
                            }

                            matchFound = true;
                            break;
                        }
                    }

                    if (not matchFound)
                    {
                        // For now I'm considering it a positional argument, but this may want to fail in this case.
                        positional.push_back(currentArg);
                    }
                }
            }
        }
    }

    Impl(int argc, char **argv) : test(argc, argv), numArgs(argc), args(argv), utf8Args(numArgs, args)
    {
        // No implementation.
    }

    Impl(int argc, char **argv, char **envp) :
        test(argc, argv, envp),
        numArgs(argc),
        args(argv),
        env(envp),
        utf8Args(numArgs, args, env)
    {
        // No implementation.
    }
};

Command const Parser::Impl::defaultCmd("");

Parser::Parser(span<Command> commands, int argc, char **argv, char **envp) :
    im(envp ? make_unique<Impl>(argc, argv, envp) : make_unique<Impl>(argc, argv))
{
    im->parseArguments(commands);
}

Parser::~Parser() noexcept
{
    // No implementation.
}

bool Parser::success() const noexcept
{
    return im->success;
}

span<string_view const> const Parser::positionalArguments() const noexcept
{
    return im->positional;
}

const Command &Parser::activeCommand() const noexcept
{
    return im->active.value();
}

void Detail::throwIllegalCharacter(char whichOne)
{
    throw IllegalArgument("An illegal character was passed to the Option constructor: {}\nThese are the legal "
                          "characters: abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -_",
                          whichOne);
}

void Detail::throwNoValidToken(string_view str)
{
    throw IllegalArgument(
        "There must be at least one valid token in an Option string. This was the option string passed:\n\"{}\"",
        str);
}

void Detail::throwNoPlusMinusBegin(char whichOne)
{
    throw IllegalArgument("An option cannot begin with the '{}' character.", whichOne);
}

void Detail::throwNotJustDigits(string_view opStr)
{
    throw IllegalArgument("An option cannot only consist of digits. \"{}\"", opStr);
}

} // namespace KirHut::CLI
