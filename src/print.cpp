/***********************************************************************************************************************
** The KirHut Application Development Library
** print.cpp
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
#include "kh/print.hpp"

#include "kh/base.hpp"

#include <iostream>
#include "nowide/iostream.hpp"

#include "fmt/ostream.h"

namespace KirHut
{

using namespace v1::IO;

void IO::vprint(std::FILE *stream, FMT::string_view form, FMT::format_args const &args)
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

void IO::vprintln(std::FILE *stream, FMT::string_view form, FMT::format_args const &args)
{
    IO::print(stream, "{}\n", FMT::vformat(form, args));
}

void IO::vprint(std::ostream &stream, FMT::string_view form, FMT::format_args const &args)
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

void IO::vprintln(std::ostream &stream, FMT::string_view form, FMT::format_args const &args)
{
    IO::print(stream, "{}\n", FMT::vformat(form, args));
}

#if defined(KH_USES_FMT)
void IO::vprint(fmt::text_style style, fmt::string_view form, fmt::format_args const &args)
{
    // The {fmt} lib's vprint() function internally uses detail::vprint_to to output the formatted string to a buffer,
    // then prints that, but we cannot use names in the detail namespace. We're forced to use {fmt}'s vformat function,
    // then print the returned string using fmt::print().
    fmt::print(nowide::cout, fmt::string_view{ "{}" }, fmt::vformat(style, form, args));
}

void IO::vprintln(fmt::text_style style, fmt::string_view form, fmt::format_args const &args)
{
    // The {fmt} lib's vprint() function internally uses detail::vprint_to to output the formatted string to a buffer,
    // then prints that, but we cannot use names in the detail namespace. We're forced to use {fmt}'s vformat function,
    // then print the returned string using fmt::print().
    fmt::print(nowide::cout, fmt::string_view{ "{}\n" }, fmt::vformat(style, form, args));
}
#endif

void IO::vprint(FMT::string_view form, FMT::format_args const &args)
{
    IO::vprint(nowide::cout, form, args);
}

void IO::vprintln(FMT::string_view form, FMT::format_args const &args)
{
    IO::vprintln(nowide::cout, form, args);
}

void IO::vreport(FMT::string_view form, FMT::format_args const &args)
{
    IO::vprint(nowide::cerr, form, args);
}

void IO::vreportln(FMT::string_view form, FMT::format_args const &args)
{
    IO::vprintln(nowide::cerr, form, args);
}

} // namespace KirHut
