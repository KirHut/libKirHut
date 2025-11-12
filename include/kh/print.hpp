/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/print.hpp
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

#if defined(KH_INCLUDE_TERMINAL_PRINT) or defined(KH_PRIV_DOCS)

# include <ostream>

# include "kh/global.hpp"
# include "kh/priv/format.hpp"

# if defined(KH_USES_FMT)
#  include "fmt/ostream.h"
# elif defined(__cpp_lib_print)
#  include <print>
# endif

#endif

/*!
 * Namespace for IO operations provided by libKirHut.
 *
 * This namespace is currently only comprised of C++23-style print functions like KirHut::print() and
 * KirHut::vprintln(). These functions are all designed to mirror the C++23 versions of these functions, except unlike
 * the standard functions, all of the default outputting functions are "unicode aware" and only accept UTF-8 text.
 * KirHut applications use UTF-8 as the universal internal text representation on all platforms, so this library must
 * support outputting and formatting UTF-8 strings as the default in all cases.
 */
namespace KirHut::IO
{

/*!
 * Indication boolean for when the UTF-8 console printing system has been included in libKirHut.
 *
 * When you need to check if this library includes console print support using an if constexpr expression rather than
 * the preprocessor, you can use this to check if the library is built with console print support.
 *
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool hasConsolePrint = false
#if defined(KH_INCLUDE_TERMINAL_PRINT)
                                                  or true
#endif
    ;

/*!
 * \brief vprint
 *
 * Formats \p form using the passed in \p args, and then outputs that result to the passed in \p stream. If \p stream is
 * a nullptr, then this method will actually do nothing at all (including perform the formatting). This means that if
 * \p stream is nullptr, this method is guaranteed not to throw an exception, even if \p form is an invalid format
 * string. Much like std::vprint_nonunicode in C++23, this method will throw FMT::format_error if the \p form or \p args
 * are invalid for each other, will throw std::system_error if there is a failure to write to the underlying stream, and
 * will throw std::bad_alloc when there is an allocation failure.
 *
 * This function will always output the characters directly as they are to the underlying \p stream. It will **not**
 * perform any UTF-8 to native output encoding conversion like vprint(string_view,FMT::format_args) does. This is
 * because the functions that output to a std::FILE or std::ostream are assumed to be output to a file or network
 * destination, which would find precise control of the output to be preferable to automatic character encoding
 * conversions that outputting to a terminal would find useful.
 *
 * \warning The behavior of this method is undefined if the \p stream pointer is non-null and does not point to a valid
 * C output stream. A nullptr will do nothing, but this method cannot prevent passing invalid pointers as a std::FILE.
 *
 * \param stream
 * \param form
 * \param args
 * \throws FMT::format_error If there is an error with the format string passed as \p form or its \p args.
 * \throws std::system_error If writing to the underlying stream fails and this library is C++23 or later enabled.
 * \throws std::bad_alloc If there is a failure to allocate memory for the formatted string buffer.
 */
KH_EXPORT void vprint(std::FILE *stream, FMT::string_view form, FMT::format_args const &args);

/*!
 * \brief vprint
 * \param stream
 * \param form
 * \param args
 * \throws FMT::format_error If there is an error with the format string passed as \p form or its \p args.
 * \throws std::ios_base::failure If the \p stream throws this exception due to a text insertion.
 * \throws std::bad_alloc If there is a failure to allocate memory for the formatted string buffer.
 */
KH_EXPORT void vprint(std::ostream &stream, FMT::string_view form, FMT::format_args const &args);

/*!
 * \brief vprint
 * \param form
 * \param args
 */
KH_EXPORT void vprint(FMT::string_view form, FMT::format_args const &args);

/*!
 * \brief vprintln
 * \param stream
 * \param form
 * \param args
 */
KH_EXPORT void vprintln(std::FILE *stream, FMT::string_view form, FMT::format_args const &args);

/*!
 * \brief vprintln
 * \param stream
 * \param form
 * \param args
 */
KH_EXPORT void vprintln(std::ostream &stream, FMT::string_view form, FMT::format_args const &args);

/*!
 * \brief vprintln
 * \param form
 * \param args
 */
KH_EXPORT void vprintln(FMT::string_view form, FMT::format_args const &args);

/*!
 * \brief vprint
 * \param form
 * \param args
 */
KH_EXPORT void vreport(FMT::string_view form, FMT::format_args const &args);

/*!
 * \brief vprintln
 * \param form
 * \param args
 */
KH_EXPORT void vreportln(FMT::string_view form, FMT::format_args const &args);

#if defined(KH_INCLUDE_TERMINAL_PRINT) or defined(KH_PRIV_DOCS)
/*!
 * \brief print
 * \param stream
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void print(FILE *stream, FMT::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
# if defined(KH_USES_FMT) or defined(__cpp_lib_print)
    FMT::print(stream, formatString, std::forward<Arg_Ts>(args)...);
# else
    IO::vprint(stream, formatString.get(), std::make_format_args(std::forward<Arg_Ts>(args)...));
# endif
}

/*!
 * \brief print
 * \param stream
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void print(std::ostream &stream, FMT::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
# if defined(KH_USES_FMT) or defined(__cpp_lib_print)
    FMT::print(stream, formatString, std::forward<Arg_Ts>(args)...);
# else
    IO::vprint(stream, formatString.get(), std::make_format_args(args...));
# endif
}

/*!
 * \brief print
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void print(FMT::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
    // The UTF-8 output stream is only within the print.cpp TU, so we cannot acces it in the header. As such, all calls
    // to print must go through vprint(). This still benefits from compile time format checking.
    IO::vprint(formatString.get(), FMT::make_format_args(args...));
}

/*!
 * \brief println
 * \param stream
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void println(FILE *stream, FMT::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
# if defined(KH_USES_FMT) or defined(__cpp_lib_print)
    FMT::println(stream, formatString, std::forward<Arg_Ts>(args)...);
# else
    IO::vprintln(stream, formatString.get(), std::make_format_args(args...));
# endif
}

/*!
 * \brief println
 * \param stream
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void println(std::ostream &stream, FMT::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
# if defined(KH_USES_FMT) or defined(__cpp_lib_print)
    FMT::println(stream, formatString, std::forward<Arg_Ts>(args)...);
# else
    IO::vprintln(stream, formatString.get(), std::make_format_args(args...));
# endif
}

/*!
 * \brief println
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void println(FMT::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
    // The UTF-8 output stream is only within the print.cpp TU, so we cannot acces it in the header. As such, all calls
    // to println must go through vprintln(). This still benefits from compile time format checking.
    IO::vprintln(formatString.get(), FMT::make_format_args(args...));
}

/*!
 * \brief print
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void report(FMT::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
    // The UTF-8 output stream is only within the print.cpp TU, so we cannot acces it in the header. As such, all calls
    // to print must go through vprint(). This still benefits from compile time format checking.
    IO::vreport(formatString.get(), FMT::make_format_args(args...));
}

/*!
 * \brief println
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void reportln(FMT::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
    // The UTF-8 output stream is only within the print.cpp TU, so we cannot acces it in the header. As such, all calls
    // to println must go through vprintln(). This still benefits from compile time format checking.
    IO::vreportln(formatString.get(), FMT::make_format_args(args...));
}

#else
template <typename... Arg_Ts>
void print(FILE *stream, auto formatString, Arg_Ts &&...args)
{
    // No further implementation.
}

template <typename... Arg_Ts>
void print(std::ostream &stream, auto formatString, Arg_Ts &&...args)
{
    // No further implementation.
}

template <typename... Arg_Ts>
void print(auto formatString, Arg_Ts &&...args)
{
    // No further implementation.
}

template <typename... Arg_Ts>
void println(FILE *stream, auto formatString, Arg_Ts &&...args)
{
    // No further implementation.
}

template <typename... Arg_Ts>
void println(std::ostream &stream, auto formatString, Arg_Ts &&...args)
{
    // No further implementation.
}

template <typename... Arg_Ts>
void println(auto formatString, Arg_Ts &&...args)
{
    // No further implementation.
}

template <typename... Arg_Ts>
void report(auto formatString, Arg_Ts &&...args)
{
    // No further implementation.
}

template <typename... Arg_Ts>
void reportln(auto formatString, Arg_Ts &&...args)
{
    // No further implementation.
}
#endif

} // namespace KirHut::IO
