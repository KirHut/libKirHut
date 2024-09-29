/***********************************************************************************************************************
** The KirHut Application Development Library
** base.cpp
** Copyright (C) 2024 KirHut Software Company
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
#include "kh/base.hpp"

#include "kh/ranges.hpp"

using namespace KirHut;

Invalid::Invalid(WhyInvalid why) noexcept : why_d(why)
{
    // No implementation.
}

Invalid::Invalid(WhyInvalid why, string &&message) noexcept : message_d(forward<string>(message)), why_d(why)
{
    // No implementation.
}

Invalid::Invalid(WhyInvalid why, char const *message) : Invalid(why, string_view(message))
{
    // No implementation.
}

Invalid::Invalid(WhyInvalid why, string_view message) : message_d(message), why_d(why)
{
    // No implementation.
}

Invalid::Invalid(Invalid &&other) noexcept : Invalid(other.why(), forward<string>(other.message_d))
{
    other.message_d.clear();
}

WhyInvalid Invalid::why() const noexcept
{
    return why_d;
}

string_view Invalid::message() const noexcept
{
    return message_d;
}

void Invalid::setMessage(string &&msg) noexcept
{
    if (message_d.empty())
    {
        overwriteMessage(forward<string>(msg));
    }
}

void Invalid::setMessage(string_view msg)
{
    if (message_d.empty())
    {
        overwriteMessage(msg);
    }
}

void Invalid::overwriteMessage(string &&msg) noexcept
{
    message_d = std::move(msg);
}

void Invalid::overwriteMessage(string_view msg)
{
    message_d = msg;
}

// Using a std::span here is more annoying than just using a contiguous_range because the QByteArray in one of the
// toStr() methods would need to be an lvalue.
template <typename StrType, R::contiguous_range RangeType>
inline StrType toStrImpl(RangeType rng)
{
    // The below code is shorter, but runs about 5-10% slower on most compilers. This is because memcpy is heavily
    // optimized.
    // return { R::begin(rng), R::end(rng) };

    StrType ret(R::size(rng), 0);
    memcpy(ret.data(), R::data(rng), R::size(rng));
    return ret;
}

#ifdef __cpp_char8_t
string KirHut::toStr(std::u8string_view in)
{
    return toStrImpl<string>(in);
}

std::u8string KirHut::toU8Str(string_view in)
{
    return toStrImpl<std::u8string>(in);
}
#endif // __cpp_char8_t

#if KH_USES_QT
# include <QString>

QString KirHut::toQStr(string_view in) noexcept
{
    return { QByteArray::fromRawData(in.data(), in.size()) };
}

string KirHut::toStr(QString const &in) noexcept
{
    // This used to do something else, but now it just calls a QString method. It is retained for source compatibility.
    return in.toStdString();
}

# ifdef __cpp_char8_t
QString KirHut::toQStr(std::u8string_view in) noexcept
{
    return { QByteArray::fromRawData(bit_cast<char const *>(in.data()), in.size()) };
}

std::u8string KirHut::toU8Str(QString const &in) noexcept
{
    return toStrImpl<std::u8string>(in.toUtf8());
}
# endif // __cpp_char8_t
#endif // KH_USES_QT
