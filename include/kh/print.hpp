/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/print.hpp
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
#pragma once

#include "kh/global.hpp"

#if defined(KH_INCLUDE_TERMINAL_PRINT) or defined(KH_PRIV_DOCS)

# include <ostream>

# include "kh/priv/format.hpp"

# if defined(KH_USES_FMT)
#  include "fmt/color.h"
# elif defined(__cpp_lib_print)
#  include <print>
# endif

#endif

/*!
 * Namespace for IO operations provided by libKirHut.
 *
 * This namespace is currently only comprised of C++23-style print functions like KirHut::IO::print() and
 * KirHut::IO::vprintln(). These functions are all designed to mirror the C++23 versions of these functions, except
 * unlike the standard functions, all of the default outputting functions are "unicode aware" and only accept UTF-8
 * text. KirHut applications use UTF-8 as the universal internal text representation on all platforms, so this library
 * must support outputting and formatting UTF-8 strings as the default in all cases.
 */
namespace KirHut
{

KH_INLINE_NAMESPACE_V1

namespace IO
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

#if defined(KH_INCLUDE_TERMINAL_PRINT) or defined(KH_PRIV_DOCS)
/*!
 * Formats \p form using the passed in \p args, and then outputs that result to the passed in \p stream.
 *
 * This function will attempt to perform UTF-8 transcoding to the appropriate output format for the terminal. Namely,
 * this method will detect if you pass in stdout or stderr as \p stream, and if you do, it will divert the text to an
 * appropriate UTF-8 transcoding output file. Otherwise, it will simply write the output bytes directly to the given
 * \p stream. The bytes may or may not be UTF-8 text, however that is generally assumed. This function makes no further
 * attempts beyond directly checking if \p stream is stdout or stderr, since this method is assumed to be output to a
 * file or network destination, and in those cases precise control of the output is preferable to automatic character
 * encoding conversions.
 *
 * If \p stream is a nullptr, then this method will actually do nothing at all (including perform the formatting). This
 * means that if \p stream is nullptr, this method is guaranteed not to throw an exception, even if \p form is an
 * invalid format string. Much like std::vprint_unicode in C++23, this method will throw FMT::format_error if the
 * \p form or \p args are invalid for each other, will throw std::system_error if there is a failure to write to the
 * underlying stream, and will throw std::bad_alloc when there is an allocation failure.
 *
 * If this library is compiled with {fmt} support, this method will always call the
 * fmt::vprint(std::FILE*,fmt::string_view,fmt::format_args) overload, otherwise it depends on if this library is
 * compiled with C++23 support. If it is, it uses std::vprint_unicode(std::FILE*,std::string_view,std::format_args), or
 * it will just use std::vformat(std::string_view,std::format_args) and fputs() the returned string to the \p stream.
 *
 * \warning The behavior of this method is undefined if the \p stream pointer is non-null and does not point to a valid
 * C output stream. A nullptr will do nothing, but this method cannot prevent passing invalid pointers as a std::FILE.
 *
 * \param stream A pointer to the std::FILE stream to output the formatted text to.
 * \param form A string_view of the format string which will be formatted with \p args and printed to \p stream.
 * \param args An object of FMT::format_args usually created from a set of template arguments.
 * \throws FMT::format_error If there is an error with the format string passed as \p form or its \p args.
 * \throws std::system_error If writing to the underlying stream fails and this library is C++23 or later enabled.
 * \throws std::bad_alloc If there is a failure to allocate memory for the formatted string buffer.
 */
KH_EXPORT void vprint(std::FILE *stream, FMT::string_view form, FMT::format_args const &args);

/*!
 * Formats \p form using the passed in \p args, and then outputs that result, and a newline, to the passed in \p stream.
 *
 * \copydetails KirHut::IO::vprint(std::FILE*,FMT::string_view,FMT::format_args const&)
 */
KH_EXPORT void vprintln(std::FILE *stream, FMT::string_view form, FMT::format_args const &args);

/*!
 * Formats \p form using the passed in \p args, and then outputs that result to the passed in \p stream.
 *
 * This function will attempt to perform UTF-8 transcoding to the appropriate output format for the terminal. Namely,
 * this method will detect if you pass in stdout or stderr as \p stream, and if you do, it will divert the text to an
 * appropriate UTF-8 transcoding output file. Otherwise, it will simply write the output bytes directly to the given
 * \p stream. The bytes may or may not be UTF-8 text, however that is generally assumed. This function makes no further
 * attempts beyond directly checking if \p stream is stdout or stderr, since this method is assumed to be output to a
 * file or network destination, and in those cases precise control of the output is preferable to automatic character
 * encoding conversions.
 *
 * If \p stream is a nullptr, then this method will actually do nothing at all (including perform the formatting). This
 * means that if \p stream is nullptr, this method is guaranteed not to throw an exception, even if \p form is an
 * invalid format string. Much like std::vprint_unicode in C++23, this method will throw FMT::format_error if the
 * \p form or \p args are invalid for each other, will throw std::system_error if there is a failure to write to the
 * underlying stream, and will throw std::bad_alloc when there is an allocation failure.
 *
 * If this library is compiled with {fmt} support, this method will always call the
 * fmt::vprint(std::FILE*,fmt::string_view,fmt::format_args) overload, otherwise it depends on if this library is
 * compiled with C++23 support. If it is, it uses std::vprint_unicode(std::FILE*,std::string_view,std::format_args), or
 * it will just use std::vformat(std::string_view,std::format_args) and fputs() the returned string to the \p stream.
 *
 * \warning The behavior of this method is undefined if the \p stream pointer is non-null and does not point to a valid
 * C output stream. A nullptr will do nothing, but this method cannot prevent passing invalid pointers as a std::FILE.
 *
 * \param stream A pointer to the std::FILE stream to output the formatted text to.
 * \param form A string_view of the format string which will be formatted with \p args and printed to \p stream.
 * \param args An object of FMT::format_args usually created from a set of template arguments.
 * \throws FMT::format_error If there is an error with the format string passed as \p form or its \p args.
 * \throws std::ios_base::failure If writing to the underlying stream fails and the stream is set to throw exceptions.
 * \throws std::bad_alloc If there is a failure to allocate memory for the formatted string buffer.
 */
KH_EXPORT void vprint(std::ostream &stream, FMT::string_view form, FMT::format_args const &args);

/*!
 * Formats \p form using the passed in \p args, and then outputs that result, and a newline, to the passed in \p stream.
 *
 * \copydetails KirHut::IO::vprint(std::ostream&,FMT::string_view,FMT::format_args const&)
 */
KH_EXPORT void vprintln(std::ostream &stream, FMT::string_view form, FMT::format_args const &args);

# if defined(KH_USES_FMT) or defined(KH_PRIV_DOCS)
/*!
 * \brief vprint
 * \param form
 * \param args
 */
KH_EXPORT void vprint(fmt::text_style style, fmt::string_view form, fmt::format_args const &args);

/*!
 * \brief vprintln
 * \param form
 * \param args
 */
KH_EXPORT void vprintln(fmt::text_style style, fmt::string_view form, fmt::format_args const &args);
# endif

/*!
 * \brief vprint
 * \param form
 * \param args
 */
KH_EXPORT void vprint(FMT::string_view form, FMT::format_args const &args);

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

/*!
 * \brief print
 * \param stream
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void print(FILE *stream, FMT::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
    IO::vprint(stream, formatString, FMT::make_format_args(args...));
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
    IO::vprintln(stream, formatString, FMT::make_format_args(args...));
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
    IO::vprint(stream, formatString, FMT::make_format_args(args...));
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
    IO::vprintln(stream, formatString, FMT::make_format_args(args...));
}

# if defined(KH_USES_FMT) or defined(KH_PRIV_DOCS)
/*!
 * \brief print
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void print(fmt::text_style style, fmt::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
    IO::vprint(style, formatString, fmt::make_format_args(args...));
}

/*!
 * \brief print
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void println(fmt::text_style style, fmt::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
    IO::vprint(style, formatString, fmt::make_format_args(args...));
}
# endif

/*!
 * \brief print
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void print(FMT::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
    IO::vprint(formatString.get(), FMT::make_format_args(args...));
}

/*!
 * \brief println
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void println(FMT::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
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
    IO::vreportln(formatString.get(), FMT::make_format_args(args...));
}
#endif

} // namespace IO

KH_END_INLINE_NAMESPACE

} // namespace KirHut
