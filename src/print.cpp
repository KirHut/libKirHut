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

#if defined(_WIN32) and not defined(__cpp_lib_print)
# include "nowide/iostream.hpp"
auto &cout = nowide::cout;
auto &cerr = nowide::cerr;
#else
# include <iostream>
auto &cout = std::cout;
auto &cerr = std::cerr;
#endif

namespace KirHut::IO
{

void vprint(std::FILE *stream, FMT::string_view form, FMT::format_args const &args)
{
#if defined(KH_USES_FMT)
    fmt::vprint(stream, form, args);
#elif defined(__cpp_lib_print)
    vprint_nonunicode(stream, form, args);
#else
    std::fputs(std::vformat(form, args).c_str(), stream);
#endif
}

void vprint(std::ostream &stream, FMT::string_view form, FMT::format_args const &args)
{
#if defined(KH_USES_FMT)
    fmt::vprint(stream, form, args);
#elif defined(__cpp_lib_print)
    vprint_nonunicode(stream, form, args);
#else
    stream << std::vformat(form, args);
#endif
}

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
