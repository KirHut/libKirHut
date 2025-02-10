/***********************************************************************************************************************
** The KirHut Application Development Library
** exception.cpp
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
#include "kh/exception.hpp"

using namespace KirHut;

// "Why not just use a simpler shared data implementation like std::shared_ptr to keep the string data?"
//
// This is because the Exception object is required to not allocate any memory *unless the user explicitly requests it
// to do so.* Notice that the string rvalue reference constructor is marked noexcept. You are REQUIRED to allocate
// memory on the heap when you use std::shared_ptr, so there is no way to not allocate memory when you use that
// constructor other than having another string to use it as a data source for already allocated memory. While it is
// possible to move construct the contained string's data, the actual data holding the std::string itself in
// std::shared_ptr must be allocated.

struct Exception::Impl
{
    template <TypeOptionOf<Data> T>
    constexpr static bool has(Data const &data) noexcept
    {
        return std::holds_alternative<T>(data);
    }

    static Exception const *getOwner(Exception const *ex) noexcept
    {
        if (ex)
        {
            while (has<CopyState>(ex->data))
            {
                ex = get<CopyState>(ex->data).prev;
            }
        }

        return ex;
    }

    static Exception const *getLast(Exception const *ex) noexcept
    {
        if (ex)
        {
            while (has<CopyState>(ex->data))
            {
                if (auto next = get<CopyState>(ex->data).next)
                {
                    ex = next;
                }
                else
                {
                    break;
                }
            }
        }

        return ex;
    }

    static void commonCopy(Exception &me)
    {
        if (Exception const &prev = *get<CopyState>(me.data).prev; Impl::has<RootState>(prev.data))
        {
            get<RootState>(prev.data).next = &me;
        }
        else if (Impl::has<CopyState>(prev.data))
        {
            get<CopyState>(prev.data).next = &me;
        }
    }

    static void commonMove(Exception &me, Exception &other)
    {
        if (Impl::has<RootState>(me.data))
        {
            if (Exception const *next = get<RootState>(me.data).next)
            {
                get<CopyState>(next->data).prev = &me;
            }
        }
        else if (Impl::has<CopyState>(me.data))
        {
            if (Exception const &prev = *get<CopyState>(me.data).prev; Impl::has<RootState>(prev.data))
            {
                get<RootState>(prev.data).next = &me;
            }
            else
            {
                get<CopyState>(prev.data).next = &me;
            }

            if (Exception const *next = get<CopyState>(me.data).next)
            {
                get<CopyState>(next->data).prev = &me;
            }
        }

        other.data = StringState{ "", WhyInvalid::DataRemoved };
    }

    static void clean(Exception &me) noexcept
    {
        if (has<RootState>(me.data))
        {
            if (Exception const *next = get<RootState>(me.data).next)
            {
                get<RootState>(me.data).next = get<CopyState>(next->data).next;
                next->data                   = std::move(get<RootState>(me.data));
            }
        }
        else if (has<CopyState>(me.data))
        {
            Exception const *prev = get<CopyState>(me.data).prev, *next = get<CopyState>(me.data).next;
            if (has<RootState>(prev->data))
            {
                get<RootState>(prev->data).next = next;
            }
            else
            {
                get<CopyState>(prev->data).next = next;
            }

            if (next)
            {
                get<CopyState>(next->data).prev = prev;
            }
        }

        me.data = StringState{ "", WhyInvalid::DataRemoved };
    }

    static Data getCorrectCopyData(Exception &me, Exception const &other)
    {
        if (Impl::has<StringState>(other.data))
        {
            return Data{ inpSS, other.info(), other.why() };
        }
        else
        {
            return Data{ inpCS, getLast(&other), nullptr };
        }
    }

    constexpr static auto inpSS = std::in_place_type<StringState>;
    constexpr static auto inpRS = std::in_place_type<RootState>;
    constexpr static auto inpCS = std::in_place_type<CopyState>;
};

Exception::Exception(string_view info) noexcept : Exception(WhyInvalid::UnknownReason, info)
{
    // No implementation.
}

Exception::Exception(char const *info) noexcept : Exception(WhyInvalid::UnknownReason, string_view(info))
{
    // No implementation.
}

Exception::Exception(MakeFlag const &flag, string_view info) : Exception(flag, WhyInvalid::UnknownReason, info)
{
    // No implementation.
}

// BASE CONSTRUCTOR:
Exception::Exception(WhyInvalid why, string_view info) noexcept : data(Impl::inpSS, info, why)
{
    // No implementation.
}

Exception::Exception(WhyInvalid why, char const *info) noexcept : Exception(why, string_view(info))
{
    // No implementation.
}

Exception::Exception([[maybe_unused]] MakeFlag const &flag, WhyInvalid why, string_view info) :
    Exception(Invalid(why, info))
{
    // No implementation.
}

#if __cpp_char8_t
using std::u8string_view;

Exception::Exception(u8string_view sv) noexcept :
    Exception(WhyInvalid::UnknownReason, string_view(std::bit_cast<char const *>(sv.data()), sv.size()))
{
    // No implementation.
}

Exception::Exception(char8_t const *ptr) noexcept : Exception(u8string_view(ptr))
{
    // No implementation.
}

Exception::Exception(MakeFlag const &flag, u8string_view sv) :
    Exception(flag, WhyInvalid::UnknownReason, string_view(std::bit_cast<char const *>(sv.data()), sv.size()))
{
    // No implementation.
}

Exception::Exception(WhyInvalid why, u8string_view sv) noexcept :
    Exception(why, string_view(std::bit_cast<char const *>(sv.data()), sv.size()))
{
    // No implementation.
}

Exception::Exception(WhyInvalid why, char8_t const *ptr) noexcept : Exception(why, u8string_view(ptr))
{
    // No implementation.
}

Exception::Exception(MakeFlag const &flag, WhyInvalid why, u8string_view sv) :
    Exception(flag, why, string_view(std::bit_cast<char const *>(sv.data()), sv.size()))
{
    // No implementation.
}
#endif // __cpp_char8_t

Exception::Exception(string &&other) noexcept : Exception(WhyInvalid::UnknownReason, std::move(other))
{
    // No implementation.
}

Exception::Exception(WhyInvalid why, string &&other) noexcept : Exception(Invalid(why, std::move(other)))
{
    // No implementation.
}

// BASE CONSTRUCTOR:
Exception::Exception(Invalid &&invalid) noexcept : data(Impl::inpRS, std::move(invalid), nullptr)
{
    // No implementation.
}

Exception::Exception(Invalid const &invalid) : Exception(Invalid(invalid))
{
    // No implementation.
}

Exception::Exception(Exception const &other) noexcept : data(Impl::getCorrectCopyData(*this, other))
{
    if (Impl::has<CopyState>(data))
    {
        Impl::commonCopy(*this);
    }
}

Exception::Exception(Exception &&other) noexcept : data(std::move(other.data))
{
    Impl::commonMove(*this, other);
}

Exception::~Exception() noexcept
{
    Impl::clean(*this);
}

Exception &Exception::operator=(Exception const &other) noexcept
{
    Impl::clean(*this);
    data.emplace<CopyState>(Impl::getLast(&other), nullptr);
    Impl::commonCopy(*this);
    return *this;
}

Exception &Exception::operator=(Exception &&other) noexcept
{
    Impl::clean(*this);
    data = std::move(other.data);
    Impl::commonMove(*this, other);
    return *this;
}

WhyInvalid Exception::why() const noexcept
{
    if (Impl::has<StringState>(data))
    {
        return get<StringState>(data).why;
    }

    return get<RootState>(Impl::getOwner(this)->data).inv.why();
}

string_view Exception::info() const noexcept
{
    if (Impl::has<StringState>(data))
    {
        return get<StringState>(data).str;
    }

    return get<RootState>(Impl::getOwner(this)->data).inv.info();
}

char const *Exception::what() const noexcept
{
    return info().data();
}
