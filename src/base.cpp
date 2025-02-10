/***********************************************************************************************************************
** The KirHut Application Development Library
** base.cpp
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
#include "kh/base.hpp"

#include "kh/ranges.hpp"
#include "kh/exceptions.hpp"

#include <chrono>
#include <cassert>

#if KH_USES_QT
# include <QString>
#endif

namespace KirHut
{

void Priv::throwNoValidData(Invalid const &inv)
{
    throw NoValidData(inv);
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

string toStr(const char *from)
{
    return string{ from };
}

#ifdef __cpp_char8_t
string toStr(std::u8string_view in)
{
    return toStrImpl<string>(in);
}

std::u8string toU8Str(string_view in)
{
    return toStrImpl<std::u8string>(in);
}
#endif // __cpp_char8_t

#if KH_USES_QT
QString toQStr(string_view in) noexcept
{
    return { QByteArray::fromRawData(in.data(), in.size()) };
}

string toStr(QString const &in) noexcept
{
    // This used to do something else, but now it just calls a QString method. It is retained for source compatibility.
    return in.toStdString();
}

# ifdef __cpp_char8_t
QString toQStr(std::u8string_view in) noexcept
{
    return { QByteArray::fromRawData(bit_cast<char const *>(in.data()), in.size()) };
}

std::u8string toU8Str(QString const &in) noexcept
{
    return toStrImpl<std::u8string>(in.toUtf8());
}
# endif // __cpp_char8_t
#endif // KH_USES_QT

u64 currentTicks() noexcept
{
    return static_cast<u64>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

template <typename T>
inline void khAssertImpl(bool condition, T &&message)
{
    if constexpr (Build::release)
    {
        if (!condition)
        {
            throw KirHutSucksAtProgramming(std::forward<T>(message));
        }
    }

    assert(condition);
}

void khAssert(bool condition, string &&message)
{
    khAssertImpl(condition, std::move(message));
}

void khAssert(bool condition, string_view message)
{
    khAssertImpl(condition, message);
}

void khAssert(bool condition, char const *message)
{
    khAssertImpl(condition, message);
}

void khAssert([[maybe_unused]] bool condition)
{
    throw KirHutSucksAtProgramming("Invalid khAssert function called.");
}

} // namespace KirHut
