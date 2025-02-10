/***********************************************************************************************************************
** The KirHut Application Development Library
** output.cpp
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
#include "kh/output.hpp"

using namespace KirHut;
using namespace KirHut::IO;

bool Output::hasByteOut() const noexcept
{
    return false;
}

size_t Output::vprint(string_view fmt, std::format_args args)
{
    return printsv(std::vformat(fmt, args));
}

size_t Output::rawOutput([[maybe_unused]] byte const *toOutput, [[maybe_unused]] size_t amount)
{
    return 0;
}

size_t Output::println()
{
    return printsv("\n");
}

size_t Output::printsvln(string_view str)
{
    size_t ret = printsv(str);
    return ret + println();
}

size_t Output::vprintln(string_view fmt, std::format_args args)
{
    size_t ret = vprint(fmt, args);
    return ret + println();
}

void Output::flush()
{
    // No implementation.
}

bool Output::isFull() const noexcept
{
    return false;
}

bool Output::goodOut() const noexcept
{
    return !(isFull() || failedOut());
}
