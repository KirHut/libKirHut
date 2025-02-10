/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/iostream.hpp
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
 * \file
 *
 * Header providing system input and output functionality using stdin, stdout, and the others.
 */

#include "kh/base.hpp"
#include "kh/input.hpp"
#include "kh/output.hpp"

namespace KirHut::IO
{

/*!
 * Initialize the user input to UTF-8 on all platforms.
 *
 * This should not be done when using Qt, as Qt will correctly normalize command line input and there is other things
 * that Qt does. If you are not using Qt, however, this method will create a nowide::args object statically and set the
 * user input appropriately to UTF-8.
 *
 * \param[out] argc The number of arguments as provided by the main() function.
 * \param[out] argv The array of string arguments as provided by the main() function.
 * \throws std::bad_alloc If the application fails to allocate sufficient memory for the arguments.
 * \throws std::runtime_error If there is an error when running any of the system calls necessary in Windows.
 */
KH_EXPORT void initArgs(int &argc, char **&argv);

/*!
 * Get an Output reference to the standard output stream for this platform, UTF-8 aware.
 *
 * The Output stream always expects UTF-8 character output, so just use that. This is the case even on Windows, and is
 * implemented using Boost::nowide underneath.
 *
 * Unlike the standard output streams, this output stream is not synchronized with stdout. As such, the buffer needs to
 * be manually flushed to output to the console.
 *
 * \return The Output stream associated with stdout.
 */
KH_EXPORT Output &out() noexcept;

/*!
 * Get an Input reference to the standard input stream for this platform, UTF-8 aware.
 *
 * The Input stream always provide input as UTF-8 character data, so expect to parse that. This is the case even on
 * Windows, and is implemented using Boost::nowide underneath.
 *
 * This stream **is** tied to the output stream, so requesting input will automatically flush all remaining output to
 * the console or other character destination.
 *
 * \return The Input stream associated with stdin.
 */
KH_EXPORT Input &in() noexcept;

/*!
 * Get an Output reference to the standard error stream for this platform, UTF-8 aware.
 *
 * The Output stream always expects UTF-8 character output, so just use that. This is the case even on Windows, and is
 * implemented using Boost::nowide underneath.
 *
 * Unlike the standard output streams, this output stream is not synchronized with stderr. As such, the buffer needs to
 * be manually flushed to output to the error stream.
 *
 * \return The Output stream associated with stderr.
 */
KH_EXPORT Output &err() noexcept;

/*!
 * Attempt automatic recovery of the stdin stream.
 *
 * This calls nowide::cin.clear() in order to attempt to get the input state into a good state again. Since the Input
 * object never performs formatted input, this should not be necessary in most cases, however it may be possible for the
 * stdin stream to be caught on something that would require a recovery of some kind. This method attempts to make that
 * recovery if it is possible. This method then returns in().goodIn() to check if the recovery worked.
 *
 * \return If the stream is now in a "good" state or not.
 */
KH_EXPORT bool recoverIn() noexcept;

/*!
 * Shortcut method to simply call Output::println() on the stream returned by out().
 *
 * This method simply adds a new line to the output location. This function, unlike the C++ standard stream operation
 * std::endl, does *not* flush the output by default. You can flush the output by calling `out().flush()`.
 *
 * \throws std::bad_alloc If there is a memory allocation failure.
 * \return The actual number of bytes output to the underlying stream, which may be less than 1.
 */
inline size_t println()
{
    return out().println();
}

/*!
 * Shortcut method to simply call Output::print(std::format_string<Args...>,Args&&...) on the stream returned by out().
 *
 * C++20 introduced a much better system for output, and recently all supported platforms for KirHut software began
 * supporting std::format. As such, this library has moved on to using this superior output system, and provides
 * this output class as a means to completely replace std::ostream based output.
 *
 * This method just gets the stream from out() and calls Output::print(std::format_string<Args...>,Args&&...) from it.
 * There is nothing else this does.
 *
 * \param fmt The format string to format the passed arguments into.
 * \param args The arguments that are input into the format string by std::format().
 * \throws std::bad_alloc If there is a memory allocation failure.
 * \return The actual number of bytes output to the underlying stream, which may be less than fmt.size().
 */
template <typename... Args>
inline size_t print(std::format_string<Args...> fmt, Args &&...args)
{
    return out().print(fmt, forward<Args>(args)...);
}

/*!
 * Shortcut method to simply call Output::vprint(string_view,std::format_args) on the stream returned by out().
 *
 * This method is the runtime equivalent of print(std::format_string<Args...>,Args&&...), and is useful when you
 * need to format a dynamically created string rather than a constant string. This loses the compile-time type
 * checking, in exchange for throwing std::format_error, so the passed in string_view had better work with the
 * std::format_args provided! If you are using this method to simply get around a compile error with the print()
 * method, you are probably doing it wrong.
 *
 * This method by default simply calls printsv(string_view) with the std::string returned from std::vformat(). This
 * does mean that, unless overridden by a subclass, this method may also throw std::bad_alloc in addition to that
 * which may be thrown by printsv(string_view).
 *
 * \param fmt The format string to format the passed std::format_args into.
 * \param args The arguments that are input into the format string by std::vformat().
 * \throws std::bad_alloc If there is a memory allocation failure.
 * \throws std::format_error If there is a formatting issue thrown by std::vformat(string_view,std::format_args).
 * \throws any Exception thrown by the subclass printsv(string_view) method.
 * \return The actual number of bytes output to the underlying stream, which may be less than fmt.size().
 */
KH_EXPORT size_t vprint(string_view fmt, std::format_args args);

/*!
 * Shortcut method to simply call Output::println(std::format_string<Args...>,Args&&...) on the stream returned by
 * out().
 *
 * C++20 introduced a much better system for output, and recently all supported platforms for KirHut software began
 * supporting std::format. As such, this library has moved on to using this superior output system, and provides
 * this output class as a means to completely replace std::ostream based output. This method also outputs a newline
 * character or characters at the end of the formatted bytes.
 *
 * This method just gets the stream from out() and calls Output::println(std::format_string<Args...>,Args&&...) from it.
 * There is nothing else this does.
 *
 * \param fmt The format string to format the passed arguments into.
 * \param args The arguments that are input into the format string by std::format().
 * \throws std::bad_alloc If there is a memory allocation failure.
 * \return The actual number of bytes output to the underlying stream, which may be less than fmt.size().
 */
template <typename... Args>
inline size_t println(std::format_string<Args...> fmt, Args &&...args)
{
    return out().println(fmt, forward<Args>(args)...);
}

/*!
 * Shortcut method to simply call Output::vprintln(string_view,std::format_args) on the stream returned by out().
 *
 * This method is the runtime equivalent of println(std::format_string<Args...>,Args&&...), and is useful when you
 * need to format a dynamically created string rather than a constant string. This loses the compile-time type
 * checking, in exchange for throwing std::format_error, so the passed in string_view had better work with the
 * std::format_args provided! If you are using this method to simply get around a compile error with the print()
 * method, you are probably doing it wrong. This method also outputs a newline character or characters at the end of
 * the formatted bytes.
 *
 * This method just gets the stream from out() and calls Output::vprintln(string_view,std::format_args) from it. There
 * is nothing else this does.
 *
 * \param fmt The format string to format the passed std::format_args into.
 * \param args The arguments that are input into the format string by std::vformat().
 * \throws std::bad_alloc If there is a memory allocation failure.
 * \throws std::format_error If there is a formatting issue thrown by std::vformat(string_view,std::format_args).
 * \return The actual number of bytes output to the underlying stream, which may be less than fmt.size().
 */
KH_EXPORT size_t vprintln(string_view fmt, std::format_args args);

} // namespace KirHut::IO
