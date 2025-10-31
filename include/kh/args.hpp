/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/args.hpp
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
#pragma once

/*!
 * \file kh/args.hpp
 *
 * File System Header that includes the KirHut Command Line Parser and associated classes.
 */

#include <vector>
#include <algorithm>

#include "kh/base.hpp"
#include "kh/ranges.hpp"

/*!
 * Namespace used for command line operations used in libKirHut.
 *
 * This namespace should contain any functionality that is meant for command line interfaces for KirHut applications,
 * which includes things like argument parsing, CLI output, and anything else. Currently the only thing in here is the
 * command line parsing objects like Option, Command, and Parser. These are the fundamental building-blocks of the
 * argument parsing system and are well-documented in their according objects.
 */
namespace KirHut::CLI
{

/*!
 * Indication boolean for when command line argument parsing support has been included in libKirHut.
 *
 * When you need to check if this library includes argument parsing support using an if constexpr expression rather than
 * the preprocessor, you can use this to check if the library is built with support.
 *
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool hasArgParser = false
#if defined(KH_INCLUDE_ARG_PARSER)
                                               or true
#endif
    ;

#if defined(KH_INCLUDE_ARG_PARSER) or defined(KH_PRIV_DOCS)
/*!
 * \internal
 *
 * Detail namespace for KirHut::CLI.
 *
 * Just another Detail namespace, see KirHut::Detail for information.
 *
 * \see KirHut::Detail
 */
namespace Detail
{

/*!
 * \internal
 *
 * Throws an exception for having an illegal character in the validateOptionString() function.
 *
 * This method is not constexpr to cause a compile error when attempting to use an invalid constant string expression
 * in the Option type constructor. All of these throw methods throw the same thing: IllegalArgument. The only real
 * distinction is the message the user receives when it is thrown.
 */
[[noreturn]] KH_EXPORT void throwIllegalCharacter(char whichOne);

/*!
 * \internal
 *
 * Throws an exception for having no valid tokens available in the validateOptionString() function.
 *
 * \copydetails KirHut::Detail::throwIllegalCharacter()
 */
[[noreturn]] KH_EXPORT void throwNoValidToken(string_view opStr);

/*!
 * \internal
 *
 * Throws an exception for a token starting with + or - in the validateOptionString() function.
 *
 * \copydetails KirHut::Detail::throwIllegalCharacter()
 */
[[noreturn]] KH_EXPORT void throwNoPlusMinusBegin(char whichOne);

/*!
 * \internal
 *
 * Throws an exception for having a token made of just digits in the validateOptionString() function.
 *
 * \copydetails KirHut::Detail::throwIllegalCharacter()
 */
[[noreturn]] KH_EXPORT void throwNotJustDigits(string_view opStr);

/*!
 * \internal
 *
 * \brief validateOptionString
 * \param str
 */
constexpr void validateOptionString(string_view str)
{
    constexpr auto isDigit    = [](char c) -> bool { return c >= '0' and c <= '9'; };
    constexpr auto legalChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -_"sv;

    if (auto pos = str.find_first_not_of(legalChars); pos != string_view::npos)
    {
        throwIllegalCharacter(str[pos]);
    }

    bool ran = false;
    for (string_view word : V::words(str))
    {
        if (word.front() == '-' or word.front() == '+')
        {
            throwNoPlusMinusBegin(word.front());
        }

        if (R::all_of(word, isDigit))
        {
            throwNotJustDigits(word);
        }

        ran = true;
    }

    if (not ran)
    {
        throwNoValidToken(str);
    }
}

/*!
 * \internal
 *
 * \brief The OptionString class
 */
struct OptionString final
{
    /*!
     * \internal
     *
     * \brief data
     */
    string_view data;

    /*!
     * \internal
     *
     * \brief OptionString
     * \param str
     */
    template <std::convertible_to<string_view> S>
    consteval OptionString(S str) : data(str)
    {
        validateOptionString(data);
    }
};

} // namespace Detail

/*!
 * Object representing an option in a particular Command.
 *
 * Options are the building block of all command line parsing functionality, and can either accept input as a string or
 * not. If an option can be combined with other options (IE it doesn't take further input), then the isFlag value will
 * return true. Creating a new option is relatively simple, just pass in the string of all possible option strings
 * (single character or multiple characters, space separated). When you pass a single character, it is considered a
 * "short option" string and may be combined with other option strings AKA `myapplication -abc` and the parser will
 * properly match the Option with 'a', 'b', and 'c' in its option string (so long as 'a' and 'b' both return true for
 * isFlag). See the Option constructor for more information about setting up an Option.
 *
 * An Option may also take an arbitrary argument as a string. This is useful when you are, for example, taking a config
 * filename or some other arbitrary argument that a user may pass. If this is the case, the Option will take **whatever
 * the next string is** as the argument, unless that string begins with a '-' character. If it does, the Parser will
 * fail to parse the string correctly and the Parser::unreadableInput() method will simply return the two arguments
 * instead.
 *
 * The Option object never changes after construction and does not contain anything based on the parsing itself.
 * Instead, all of the actual real-time parsing information will be in the Parser class. This makes the Option class
 * very simple and should allow for compile-time checking of Option strings at a later time.
 *
 * All of the following inputs are properly parsed by this library as an Option, using "f flag" as the example for an
 * Option without an argument string and "c config-file" as an Option that does:
 *
 * ~~~
 * myapplication -f
 * myapplication --flag
 * myapplication --config-file /path/to/config
 * myapplication --config-file=/path/to/config
 * myapplication -c /path/to/config
 * myapplication -fc /path/to/config
 * myapplication -f -c /path/to/config
 * myapplication --flag --config-file /path/to/config
 * myapplication --flag --config-file=/path/to/config
 * myapplication -f --config-file /path/to/config
 * myapplication -f --config-file=/path/to/config
 * myapplication --config-file /path/to/config --flag
 * myapplication Command --config-file /path/to/config
 * myapplication Command --config-file=/path/to/config
 * myapplication Command --flag --config-file /path/to/config
 * myapplication Command --flag --config-file=/path/to/config
 * myapplication Command -c /path/to/config
 * myapplication Command -fc /path/to/config
 * ~~~
 *
 * All of those are valid inputs which will provide the same result for the active Command after the Parser object is
 * constructed. An Option may have an arbitrary number of strings and characters associated with it. The Parser will
 * store the actual positional arguments and option strings that are parsed from the input Command. See the Command
 * object for more information about how Command arguments work with this parser.
 *
 * The Parser will always choose the first Flag or Option that it finds that has the associated string or character, so
 * if you have two or more Option or Flag objects that use the same string or character, the first one on the list in
 * that Command will be the one that is selected.
 */
struct KH_EXPORT Option final
{
    /*!
     * A string variable containing the possible "Option names" that can be used to match with this Option.
     *
     * This string is created upon construction of the Option object and cannot be changed. If you wish to add more
     * strings to the same option, simply create a new Option object that will result in the same functionality changes
     * in your project. The opNames variable will always be a single-spaced set of character strings, the spaces
     * separating different possible flag or option names the user would pass into the command line. For example, if the
     * opNames variable equals "c config-file", then when you pass "-c", "--c", or "--config-file" to the matches()
     * method, this Option will consider it a match. It will also match with "-config-file", but the Parser object will
     * look at that as a series of single character options, and splits it into "-c", "-o", "-n", etc. This allows it
     * to match each character individually with an Option, and if one of them isn't found, it will return a failed
     * parse.
     */
    const string opNames;

    /*!
     * A constant boolean that indicates whether or not this option is a "flag."
     *
     * A "flag", for the purpose of command line parsing, is a type of Option that never requires a user input string.
     * Most Option objects will require another input string that the user should provide. If the Option does not]
     * require any additional input from the user other than the Option existing, it is called a "flag." A flag will
     * never match up with something like "--flag=SOMESTRING".
     */
    const bool isFlag;

    /*!
     * The compile-time checked Option object constructor, with a \p takesArgument that assumes the Option is a flag.
     *
     * This is the main constructor for the Option object, and generally this should be created in an OptionList with
     * a list initialization of bracketed constructor calls, like so:
     *
     * ~~~
     * std::vector<Command> myCommands;
     * myCommands.emplace_back("Command", OptionList {
     *     { "c config-file", true },
     *     { "f flag" }
     * });
     * ~~~
     *
     * This allows you to set up your commands very quickly and efficiently in the code.
     *
     * \param optionNames A constant string of space-separated symbols used to identify this Option in the arguments.
     * \param takesArgument Whether or not this Option requires an argument, or an additional string input.
     */
    Option(Detail::OptionString optionNames, bool takesArgument = false) noexcept;

    /*!
     * The runtime checked Option object constructor, with a \p takesArgument that assumes the Option is a flag.
     *
     * \param rt
     * \param optionNames
     * \param takesArgument
     * \throws IllegalArgument If any of the single character options is a numeral or any symbol contains an illegal
     * character.
     */
    Option(RuntimeFlag rt, string_view optionNames, bool takesArgument = false);

    /*!
     * \brief operator ==
     * \param other
     * \return
     */
    [[nodiscard]] bool operator==(Option const &other) const noexcept;

    /*!
     * Class representing that a match was found using the Option::matches(string_view) method.
     *
     * The Option::matches(string_view) method will return a Maybe<Match> which is either empty (if no match was found)
     * or has one of these objects in it (if a match was found). The Parser is designed in such a way that it does not
     * need to know if a particular argument is an option or a flag, instead the Option class does the actual reading
     *  work and return if an argument matches their requirements. Because of this, some arguments, like
     * `myapplication --config-file=/path/to/config` will return a Match but not require additional arguments, whereas
     * `myapplication --config-file /path/to/config` will return a Match where requiresString will return true and the
     * Parser will store the next string as its according argument. If, like in the former example, the single string
     * argument contains the Option's necessary argument, the argumentString value will be set and not empty.
     *
     * The Match object is a simple data object, and does not have any internal implementation. The Parser object itself
     * takes on the burden of actually containing the matched arguments under the Parser::matchedOptions() method, which
     * returns an object with all of the matched Option object references as well as all the arguments that had matched
     * that Option string, or the number of matches found when the Option object does not take additional arguments.
     * Once a Match that requires a value has been provided one, requiresString value will return false and the
     * setValue() methods will do nothing, similar to a Match that does not require additional arguments. A new Match
     * object is created for every returned value, but this is very cheap as a Match class is really just a wrapper
     * around a pointer. The copy constructor and assignment operators of this class have been deleted, as making copies
     * of this object does not respect the single Match class requirement, but the move constructor and assignment
     * operators are still fine.
     */
    struct Match final
    {
        /*!
         * Reference to the Option that this Match was matched against.
         *
         * When matching functions are called on objects that own Options rather than the Option itself, it can become
         * much less clear which Option was matched when this object is returned. This reference makes it very clear
         * what Option was matched against in that case.
         */
        Option const &op;

        /*!
         * Pointer to the exact location in the passed string_view object where the match was found.
         *
         * This is expressed as a pointer specifically because it is referring to a location in memory and not to a
         * character itself. For multi-character flags and options, this will always point to the first non-dash
         * character in the flag or option, but for single-character flags in a set of flags, this will point to the
         * specific character in that set that this option matches with, and may allow for more advanced parsing of the
         * characters (for example, having multiple of the same flag).
         */
        char const *matched;

        /*!
         * \brief parameterString
         */
        Maybe<string_view> const parameterString = maybeNot;

        /*!
         * Tests if the found match in the command line arguments requires an additional argument string.
         *
         * Since an Option reads command line arguments one argument at a time, it is impossible for the Option to
         * always capture the user's passed in parameters. The parameter may be part of the same argument (IE
         * "--MyOption=Parameter") or the next argument. In the case that the match is expecting another argument, this
         * method will return true.
         *
         * This condition is also met when the parameterString has a value and that value is an empty string. That is
         * actually what this method checks for directly, as that is the state that means a parameter is required.
         *
         * \return Whether or not this Match is expecting an additional parameter from the user.
         */
        [[nodiscard]] constexpr bool requiresString() const noexcept
        {
            return parameterString.has_value() and parameterString->empty();
        }
    };

    /*!
     * \brief singleMatches
     * \param toCheck
     * \return
     */
    [[nodiscard]] Maybe<Match> singleMatches(string_view toCheck) const noexcept;

    /*!
     * \brief multiMatches
     * \param toCheck
     * \return
     */
    [[nodiscard]] Maybe<Match> multiMatches(string_view toCheck) const noexcept;

    /*!
     * \brief matches
     * \param toCheck
     * \return A Maybe with a Match if the match succeeded, or an empty Maybe if it does not.
     */
    [[nodiscard]] Maybe<Match> matches(string_view toCheck) const noexcept;
};

/*!
 * An alias for a std::vector of Option objects.
 *
 * This is useful for quickly declaring a list of Options, which should be extremely common while creating Command
 * objects to put into a Parser.
 */
using OptionList = std::vector<Option>;

/*!
 * An alias for a std::span of Option objects, with both the span and Option objects as const.
 *
 * This is useful as a return value for objects that wish to display the current Options that are in use, while
 * preventing them from being modified. If they should be modified, return a reference of OptionList.
 */
using OptionView = span<Option const> const;

/*!
 * Class representing a subset of functionality found on the command line for an application.
 *
 * This class allows you to create one or more "subcommands," which are similar to the command structure found on
 * applications like Git, where "git commit" will perform commits and "git init" initializes a new function. Every
 * application that parses the command line must create at least one of these objects that parses the commands that the
 * user inputs. It is possible to create a Command object with an empty string (or using the no-argument constructor)
 * that parses when no command is given but the application is still run with arguments. You may or may not include a
 * Command object without a command name, depending on how your application is intended to work.
 *
 * If the first string found does not match any of the Command objects passed in the Parser object constructor, and that
 * string does not start with a '-' symbol, it will be treated as a positional argument in the Parser object. Any string
 * found after the first argument that matches a Command string will always be treated as a positional argument, so the
 * command string must be the first argument given in a particular invocation of the application.
 */
class KH_EXPORT Command final
{
    /*!
     * \internal
     *
     * \brief options
     */
    OptionList options;

public:
    /*!
     * \brief commandString
     */
    string_view const commandString;

    /*!
     * \brief Command
     * \param commandName
     * \throws IllegalArgument If the passed commandName begins with or is only numerals or contains any illegal
     * characters.
     * \throws std::bad_alloc If allocating memory for the underlying vector of Option objects fails.
     */
    Command(string_view commandName);

    Command(char const *commandName) : Command(string_view{ commandName })
    {
        // No implementation
    }

    /*!
     * \brief Command
     * \param commandName
     * \param ops A std::vector of Option objects that are moved to this Command object.
     * \throws IllegalArgument If the passed commandName begins with or is only numerals or contains any illegal
     * characters.
     * \throws std::bad_alloc If allocating memory for the underlying vector of Option objects fails.
     */
    Command(string_view commandName, OptionList &&ops);

    /*!
     * \brief Command
     * \param commandName
     * \param ops
     * \throws IllegalArgument If the passed commandName begins with or is only numerals or contains any illegal
     * characters.
     * \throws std::bad_alloc If allocating memory for the underlying vector of Option objects fails.
     */
    Command(string_view commandName, OptionView ops);

    /*!
     * \brief addOpt
     * \param option
     * \throws std::bad_alloc If allocating memory for the new Option object fails.
     */
    void addOpt(Option const &option);

    /*!
     * \brief addOpt
     * \param option
     * \throws std::bad_alloc If allocating memory for the new Option object fails.
     */
    void addOpt(Option &&option);

    /*!
     * \brief addOpt
     * \param optionNames
     * \param takesArgument
     * \throws IllegalArgument If any of the single character options is a numeral or any argument contains an illegal
     * character.
     * \throws std::bad_alloc If allocating memory for the new Option object fails.
     */
    void addOpt(Detail::OptionString optionNames, bool takesArgument);

    /*!
     * \brief getOpts
     * \return
     */
    [[nodiscard]] OptionView getOpts() const noexcept;

    /*!
     * \brief matches
     * \param toCheck
     * \return
     */
    [[nodiscard]] bool matches(string_view toCheck) const noexcept;

    /*!
     * \brief matchOpt
     * \param toCheck
     * \return
     */
    [[nodiscard]] Maybe<Option::Match> matchOpt(string_view toCheck) const noexcept;
};

/*!
 * \brief The OptionMatches class
 */
class KH_EXPORT OptionMatches final
{
    using MatchData = Var<u32, std::vector<string_view>>;
    MatchData matches;

public:
    /*!
     * \brief option
     */
    Option const &option;

    /*!
     * \brief matchesFound
     * \return
     */
    [[nodiscard]] u32 matchesFound() const noexcept;

    /*!
     * \brief argumentsFound
     * \return
     */
    [[nodiscard]] StringViews argumentsFound() const noexcept;

private:
    friend class Parser;

    /*!
     * \internal
     *
     * \brief OptionMatches
     * \param option
     * \param hasArguments
     */
    OptionMatches(Option const &option, bool hasArguments) noexcept;

    /*!
     * \internal
     *
     * Private match incrementing function that is only used by Parser to indicate another match was found.
     *
     *
     */
    void matchFound() noexcept;

    /*!
     * \internal
     *
     * \brief matchFound
     * \param argument
     */
    void matchFound(string_view argument);
};

/*!
 * Class that performs the parsing of the command line arguments and provides the result of that parsing.
 *
 * This is the primary class for reading the input the user has provided through the terminal interface. A terminal
 * application will be provided a set of space separated strings through the \b argc and \b argv variables. The way they
 * are provided is acutally OS dependent, with UNIX based systems like Mac, Linux, and others providing data in UTF-8
 * text format already, and Windows providing the data in a locale dependent character set. This class will
 * automatically convert the character encoding to UTF-8, regardless of the OS, and read the input to a high-level
 * dataset of what application-provided Commands and Options have been selected by the user.
 */
class KH_EXPORT Parser final
{
    struct Impl;

    /*!
     * \internal
     *
     * \brief im
     */
    UPtr<Impl> im;

public:
    /*!
     * Construct a new Parser object using argc and argv, and optionally envp.
     *
     * This is the only constructor provided for this object, and this also deliberately makes a copy of the argc and
     * argv pointer values. This is because argc and argv are *not* modified themselves, instead, a copy of the
     * arguments is made internally and then that is modified. You can, therefore, see the raw input provided to the
     * application at any time without needing to coerce that information from this Arguments object. This is the same
     * with the optional envp pointer, it will create a separate set and copy it over.
     *
     * This constructor will perform the parsing of the command line arguments, however this is not guaranteed to work
     * correctly. Instead of throwing an exception (which is the worst-case scenario), instead you should check the
     * success() method to ensure the parsing succeeded. If it does not, the parser will still do the best it can to
     * parse the user's input, includuing determing the user's preferred language and configuration settings, during
     * this constructor. This allows the application to control how to handle failed parses rather than simply calling
     * std::terminate(), throwing an exception, or some other method of hard terminating the application.
     *
     * \param argc The count of arguments as passed to the application through main(). Must be >= 1.
     * \param argv A valid pointer to the array of argument character data as passed to the application through main().
     * \param envp An optional pointer to an array of environment data as passed to the application through main().
     * \throws IllegalArgument If the list of \p commands is empty, \p argc is less than 1, or \p argv is nullptr.
     * \throws std::runtime_error If there is any issue on Windows calling the necessary system calls to generate the
     * UTF-8 arguments.
     * \throws std::bad_alloc If there is an error allocating the necessary memory to create the Parser.
     */
    Parser(span<Command> commands, int argc, char **argv, char **envp = nullptr);

    //! \cond
    Parser(Parser const &)            = delete;
    Parser &operator=(Parser const &) = delete;
    //! \endcond

    /*!
     * Default move constructor.
     *
     * Moving a Parser object is fine, but it cannot be copied.
     */
    Parser(Parser &&) noexcept = default;

    /*!
     * Default move assignment operator.
     *
     * Moving a Parser object is fine, but it cannot be copied. The assigned from Parser object will be left in an
     * unspecified invalid state. It may be move assigned to by another Parser object, but all other calls on the
     * resulting moved-from Parser object are undefined behavior until reassigned to another Parser.
     *
     * \return A reference to the current Parser object being assigned to.
     */
    Parser &operator=(Parser &&) noexcept = default;

    /*!
     * \pimpldestructor
     */
    ~Parser() noexcept;

    /*!
     * Returns whether or not the command line parse that occurred during construction succeeded.
     *
     * This allows for a more graceful handling of improperly input command line arguments. Most parsers will instead
     * simply output a predesigned help text message and call std::terminate(), which this Argument parsing header
     * explicitly avoids. Instead, it is possible for a user to have specific language settings or other settings that
     * may influence the output, so if a parse fails, it should produce a message that is controlled by the application
     * instead of by the header.
     * \return
     */
    [[nodiscard]] bool success() const noexcept;

    /*!
     * \brief positionalArguments
     * \return
     */
    [[nodiscard]] StringViews positionalArguments() const noexcept;

    /*!
     * \brief activeCommand
     * \return
     */
    [[nodiscard]] Command const &activeCommand() const noexcept;

    /*!
     * \brief unreadableInput
     * \return
     */
    [[nodiscard]] StringViews unreadableInput() const noexcept;

    /*!
     * \brief matchedOptions
     * \return
     */
    [[nodiscard]] span<OptionMatches const> const &matchedOptions() const noexcept;

    /*!
     * \brief argc
     * \return
     */
    [[nodiscard]] int argc() const noexcept;

    /*!
     * \brief argv
     * \return
     */
    [[nodiscard]] char const **argv() const noexcept;

    /*!
     * Return a pointer to an array of UTF-8 C strings that
     *
     * Unless this object was constructed with the optional envp pointer, this method will always return nullptr. On
     * Windows, the pointer passed is basically irrelevant since it will be replaced anyway, but on other OS's it must
     * be the envp pointer passed in to main().
     *
     * \return
     */
    [[nodiscard]] char const **envp() const noexcept;
};
#endif // defined(KH_INCLUDE_ARG_PARSER) or defined(KH_PRIV_DOCS)

} // namespace KirHut::CLI
