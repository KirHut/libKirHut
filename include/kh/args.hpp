/***********************************************************************************************************************
** The KirHut Application Development Library
** args.hpp
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
#pragma once

/*!
 * \file kh/args.hpp
 *
 * File System Header that includes the KirHut Command Line Parser and associated classes.
 */

#include "base.hpp"
#include <vector>

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
 * Object representing an option in a particular Command.
 *
 * Options are similar to flags except that they also accept a user input string. This makes it possible to do something
 * like "--config-file=/path/to/config" and it will work. Options are parsed according to the rules of GNU command line
 * inputs, so let's say you can add a new config file using either "config-file" or 'c', it would allow the following:
 *
 * ~~~
 * myapplication --config-file /path/to/config
 * myapplication --config-file=/path/to/config
 * myapplication -c /path/to/config
 * myapplication Command --config-file /path/to/config
 * myapplication Command --config-file=/path/to/config
 * myapplication Command -c /path/to/config
 * ~~~
 *
 * All of those are valid inputs which will provide the same result for the active Command after the Parser object is
 * constructed. This includes if other flags (*not options*) are used prior to the Option character in a single-dash set
 * of flags, a 'la `myapplication -abc /path/to/config`. Just like a Flag, an Option may have an arbitrary number of
 * strings and characters associated with it. The Parser will always choose the first Flag or Option that it finds that
 * has the associated string or character, so if you have two or more Option or Flag objects that use the same string or
 * character, the first one on the list in that Command will be the one that is selected.
 */
class Option final
{
    string_view opNames;
    std::vector<string> opValues; // I fucking hate this.

public:
    /*!
     * \brief Option
     * \param optionNames
     * \throws IllegalArgument If any of the single character options is a numeral or any argument contains an illegal
     * character.
     */
    Option(string_view optionNames);

    /*!
     * \brief optionFound
     * \return
     */
    bool optionFound() const noexcept;

    /*!
     * \brief optionValues
     * \return
     */
    span<string_view> optionValues() const noexcept;

    /*!
     * \brief numMatches
     * \return
     */
    u32 numMatches() const noexcept;

    /*!
     * Class representing that a match was found using the Option::matches(string_view) method.
     *
     * The Option::matches(string_view) method will return a Maybe<Match> which is either empty (if no match was found)
     * or has one of these objects in it (if a match was found). The Parser is designed in such a way that it does not
     * need to know if a particular argument is an option or a flag, instead the Option and Flag classes do the actual
     * reading work and return if an argument matches their requirements. Because of this, some arguments, like
     * `myapplication --config-file=/path/to/config` will return a Match but not require additional arguments, whereas
     * `myapplication --config-file /path/to/config` will return a Match where requiresValue() will return true and the
     * setValue() methods will actually modify the backend arguments list of the according Option object.
     *
     * Once a Match that requires a value has been provided one, requiresValue() will return false and the setValue()
     * methods will do nothing, similar to a Match that does not require additional arguments. A new Match object is
     * created for every returned value, but this is very cheap as a Match class is really just a wrapper around a
     * pointer. The copy constructor and assignment operators of this class have been deleted, as making copies of this
     * object does not respect the single Match class requirement, but the move constructor and assignment operators are
     * still fine.
     */
    class Match final
    {
        std::vector<string> *values;

    public:
        Match(Match const &other) = delete;
        Match(Match &&other)      = default;

        Match &operator=(Match const &other) = delete;
        Match &operator=(Match &&other)      = default;

        // We don't actually need this, but this is here to respect the rule of 5.
        ~Match() = default;

        /*!
         * \brief requiresValue
         * \return
         */
        bool requiresValue() const noexcept;

        /*!
         * \brief setValue
         * \param value
         */
        void setValue(string &&value) const;

        /*!
         * \brief setValue
         * \param value
         */
        void setValue(string_view value) const;

        /*!
         * \brief setValue
         * \param value
         */
        void setValue(char const *value) const;

    private:
        friend class Option;
    };

    /*!
     * \brief matches
     * \param toCheck
     * \throws std::bad_alloc If allocating space for the matched string in the values list fails.
     * \return A Maybe with a Match if the match succeeded, or an empty Maybe if it does not.
     */
    Maybe<Match> matches(string_view toCheck);
};

/*!
 * Object representing a flag in a particular Command.
 *
 * Flags allow passing a switchable on or off option to an application.
 */
class Flag final
{
    string_view flagNames;
    u32 flagMatches = 0;

public:
    /*!
     * Flag reader object constructor.
     *
     * This creates a new Flag object with the text strings passed in as arguments.
     * \param optionNames
     * \throws IllegalArgument If any of the single character options is a numeral or any argument contains an illegal
     * character.
     */
    Flag(string_view names);
};

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
struct Command
{
    Command(string_view commandName) noexcept;
};

class CommandFailed : public Command
{
};

class Parser final
{
    class Impl;
    UPtr<Impl> im;

public:
    /*!
     * Construct a new Parser object using argc and argv.
     *
     * This is the only constructor provided for this object, and this also deliberately makes a copy of the argc and
     * argv pointer values. This is because argc and argv are *not* modified themselves, instead, a copy of the
     * arguments is made internally and then that is modified. You can, therefore, see the raw input provided to the
     * application at any time without needing to coerce that information from this Arguments object.
     *
     * This constructor will perform the parsing of the command line arguments, however this is not guaranteed to work
     * correctly. Instead of throwing an exception (which is the worst-case scenario), instead you should check the
     * success() method to ensure the parsing succeeded. If it does not, the activeCommand() returned will always be
     * CommandFailed, the positionalArguments() will always be empty, and argc() and argv() will be unmodified.
     *
     * \param argc
     * \param argv
     */
    Parser(span<Command> commands, int argc, char **argv);

    /*!
     * \brief success
     * \return
     */
    bool success() const noexcept;

    /*!
     * \brief positionalArguments
     * \return
     */
    span<string_view> const &positionalArguments() const noexcept;

    /*!
     * \brief activeCommand
     * \return
     */
    Command const &activeCommand() const noexcept;

    /*!
     * \brief argc
     * \return
     */
    int argc() const noexcept;

    /*!
     * \brief argv
     * \return
     */
    char const **argv() const noexcept;
};

} // namespace KirHut::CLI
