/***********************************************************************************************************************
** The KirHut Application Development Library
** print.cpp
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
#include "kh/print.hpp"

#include "kh/base.hpp"

#include <iostream>

#if defined(KH_USES_QT)
# include <QTextStream>
#endif

#if defined(KH_WINDOWS) and (defined(KH_USES_FMT) or not defined(__cpp_lib_print))
# include "nowide/iostream.hpp"
auto &cout = nowide::cout;
auto &cerr = nowide::cerr;
#else
auto &cout = std::cout;
auto &cerr = std::cerr;
#endif

namespace KirHut::IO
{

void vprint(std::FILE *stream, FMT::string_view form, FMT::format_args const &args)
{
#if defined(__cpp_lib_print) and not defined(KH_USES_FMT)
    std::vprint_unicode(stream, form, args);
#else
# if defined(KH_WINDOWS)
    // This function should just "magically" know if the passed in stream goes to stdout or stderr, and if it does, it
    // should output UTF-8 correctly. I have no useful way to do that, so we just fake it 'till we make it by checking
    // if the address of stream is equal to stdout or stderr.
    if (stream == stdout)
    {
        IO::vprint(nowide::cout, form, args);
    }
    else if (stream == stderr)
    {
        IO::vprint(nowide::cerr, form, args);
    }
    else
    {
# endif
# if defined(KH_USES_FMT)
        fmt::vprint(stream, form, args);
# else
    std::fputs(std::vformat(form, args).c_str(), stream);
# endif
# if defined(KH_WINDOWS)
    }
# endif
#endif
}

void vprint(std::ostream &stream, FMT::string_view form, FMT::format_args const &args)
{
#if defined(__cpp_lib_print) and not defined(KH_USES_FMT)
    std::vprint_unicode(stream, form, args);
#else
    // This function should just "magically" know if the passed in stream goes to std::cout or std::cerr, and if it
    // does, it should output using Unicode. I have no useful way to do that, so we just fake it 'till we make it by
    // checking if the address of stream is equal to std::cout or std::cerr.
    std::ostream *out = &stream;

# if defined(KH_WINDOWS)
    if (out == &std::cout)
    {
        out = &nowide::cout;
    }
    else if (out == &std::cerr)
    {
        out = &nowide::cerr;
    }
# endif // defined(KH_WINDOWS)

# if defined(KH_USES_FMT)
    fmt::vprint(*out, form, args);
# else
    *out << std::vformat(form, args);
# endif
#endif
}

#if defined(KH_USES_QT)
void vprint(QIODevice &stream, FMT::string_view form, FMT::format_args const &args)
{
    if (stream.isWritable())
    {
        string formatted = FMT::vformat(form, args);
        QTextStream(&stream) << QByteArray::fromRawData(formatted.data(), formatted.size() + 1);
    }
}
#endif

#if defined(KH_USES_FMT)
void vprint(fmt::text_style style, fmt::string_view form, fmt::format_args const &args)
{
    // The {fmt} lib's vprint() function internally uses detail::vprint_to to output the formatted string to a buffer,
    // then prints that, but we cannot use names in the detail namespace. We're forced to use {fmt}'s vformat function,
    // then print the returned string using fmt::print().
    fmt::print(cout, fmt::string_view{ "{}" }, fmt::vformat(style, form, args));
}
#endif

void vprint(FMT::string_view form, FMT::format_args const &args)
{
    IO::vprint(cout, form, args);
}

void vprintln(std::FILE *stream, FMT::string_view form, FMT::format_args const &args)
{
    IO::vprint(stream, form, args);
    std::fputc('\n', stream);
}

void vprintln(std::ostream &stream, FMT::string_view form, FMT::format_args const &args)
{
    IO::vprint(stream, form, args);
    stream << '\n';
}

void vprintln(FMT::string_view form, FMT::format_args const &args)
{
    IO::vprintln(cout, form, args);
}

void vreport(FMT::string_view form, FMT::format_args const &args)
{
    IO::vprint(cerr, form, args);
}

void vreportln(FMT::string_view form, FMT::format_args const &args)
{
    IO::vprintln(cerr, form, args);
}

} // namespace KirHut::IO
