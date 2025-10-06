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

#include <ostream>

#include "kh/base.hpp"

#if KH_USES_FMT
# include "fmt/ostream.h"
# define FMT fmt
#else
# include <format>
# if defined(__cpp_lib_print)
#  include <print>
# endif
# define FMT std
#endif

/*!
 * Namespace for IO operations provided by libKirHut.
 *
 * This namespace is currently only comprised of C++23-style print functions like KirHut::print() and
 * KirHut::vprintln(). These functions are all designed to mirror the C++23 versions of these functions, except unlike
 * the standard functions, all of these functions are "unicode aware" and only accept UTF-8 text. KirHut applications
 * use UTF-8 as the universal internal text representation on all platforms, so this library must support outputting and
 * formatting UTF-8 strings as the default in all cases.
 */
namespace KirHut::IO
{

/*!
 * \brief vprint
 * \param stream
 * \param form
 * \param args
 */
void vprint(std::FILE *stream, string_view form, FMT::format_args args);

/*!
 * \brief vprint
 * \param stream
 * \param form
 * \param args
 */
void vprint(std::ostream &stream, string_view form, FMT::format_args args);

/*!
 * \brief vprint
 * \param form
 * \param args
 */
void vprint(string_view form, FMT::format_args args);

/*!
 * \brief vprintln
 * \param stream
 * \param form
 * \param args
 */
void vprintln(std::FILE *stream, string_view form, FMT::format_args args);

/*!
 * \brief vprintln
 * \param stream
 * \param form
 * \param args
 */
void vprintln(std::ostream &stream, string_view form, FMT::format_args args);

/*!
 * \brief vprintln
 * \param form
 * \param args
 */
void vprintln(string_view form, FMT::format_args args);

/*!
 * \brief print
 * \param stream
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void print(ConvertsTo<FILE *, std::ostream &> auto stream, FMT::format_string<Arg_Ts...> formatString, Arg_Ts &&...args)
{
#if defined(KH_USES_FMT) or defined(__cpp_lib_print)
    FMT::print(stream, formatString, std::forward<Arg_Ts>(args)...);
#else
    vprint(stream, formatString.get(), std::make_format_args(std::forward<Arg_Ts>(args)...));
#endif
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
    vprint(formatString.get(), FMT::make_format_args(std::forward<Arg_Ts>(args)...));
}

/*!
 * \brief println
 * \param stream
 * \param formatString
 * \param args
 */
template <typename... Arg_Ts>
void println(ConvertsTo<FILE *, std::ostream &> auto stream,
             FMT::format_string<Arg_Ts...> formatString,
             Arg_Ts &&...args)
{
#if defined(KH_USES_FMT) or defined(__cpp_lib_print)
    FMT::println(stream, formatString, std::forward<Arg_Ts>(args)...);
#else
    vprintln(stream, formatString.get(), FMT::make_format_args(std::forward<Arg_Ts>(args)...));
#endif
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
    vprintln(formatString.get(), FMT::make_format_args(std::forward<Arg_Ts>(args)...));
}

} // namespace KirHut::IO
