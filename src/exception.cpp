/***********************************************************************************************************************
** The KirHut Application Development Library
** exception.cpp
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
    static Exception const *getOwner(Exception const *ex) noexcept
    {
        while (ex->prev)
        {
            ex = ex->prev;
        }

        return ex;
    }

    static Exception const *getLast(Exception const *ex) noexcept
    {
        while (ex->next)
        {
            ex = ex->next;
        }

        return ex;
    }

    static void clean(Exception &me) noexcept
    {
        if (!me.infoView.data())
        {
            if (me.prev)
            {
                me.prev->next = me.next;
            }
            else if (me.next)
            {
                me.next->data = std::move(me.data);
            }

            if (me.next)
            {
                me.next->prev = me.prev;
            }
        }

        me.data     = {};
        me.infoView = {};
        me.prev     = nullptr;
        me.next     = nullptr;
    }
};

Exception::Exception(string_view sv) noexcept : infoView(sv), prev(nullptr), next(nullptr)
{
    // No implementation.
}

Exception::Exception(char const *ptr) noexcept : Exception(string_view(ptr))
{
    // No implementation.
}

Exception::Exception(string_view sv, bool copy) : Exception(sv)
{
    if (copy)
    {
        data     = sv;
        infoView = {};
    }
}

#if __cpp_char8_t
using std::u8string_view;

Exception::Exception(u8string_view sv) noexcept : Exception(string_view{ toCharPtr(sv.data()), sv.size() })
{
    // No implementation.
}

Exception::Exception(char8_t const *ptr) noexcept : Exception(u8string_view(ptr))
{
    // No implementation.
}

Exception::Exception(u8string_view sv, bool copy) : Exception(string_view{ toCharPtr(sv.data()), sv.size() }, copy)
{
    // No implementation.
}
#endif // __cpp_char8_t

Exception::Exception(string &&other) noexcept : data(std::move(other)), prev(nullptr), next(nullptr)
{
    // No implementation.
}

Exception::Exception(Exception const &other) noexcept :
    infoView(other.infoView),
    prev(other.next ? Impl::getLast(other.next) : &other),
    next(nullptr)
{
    if (infoView.data())
    {
        prev = nullptr;
    }
    else
    {
        prev->next = this;
    }
}

Exception::Exception(Exception &&other) noexcept :
    data(std::move(other.data)),
    infoView(other.infoView),
    prev(other.prev),
    next(other.next)
{
    if (prev)
    {
        prev->next = this;
    }

    if (next)
    {
        next->prev = this;
    }
}

Exception::~Exception()
{
    Impl::clean(*this);
}

Exception &Exception::operator=(Exception const &other) noexcept
{
    Impl::clean(*this);
    if (infoView = other.infoView; !infoView.data())
    {
        prev       = other.next ? Impl::getLast(other.next) : &other;
        prev->next = this;
    }

    return *this;
}

Exception &Exception::operator=(Exception &&other) noexcept
{
    Impl::clean(*this);
    if (other.data.empty())
    {
        infoView = other.infoView;

        if (other.prev)
        {
            prev       = other.prev;
            prev->next = this;

            // other.next is handled after the top if block.
        }
    }
    else
    {
        data = std::move(other.data);
    }

    if (other.next)
    {
        next       = other.next;
        next->prev = this;
    }

    Impl::clean(other);
    return *this;
}

string_view Exception::info() const noexcept
{
    if (infoView.data())
    {
        return infoView;
    }

    return Impl::getOwner(this)->data;
}

char const *Exception::what() const noexcept
{
    return info().data();
}
