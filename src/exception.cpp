/***********************************************************************************************************************
** The KirHut Library for the Public Benefit
** exception.cpp
** Copyright (C) 2024 KirHut Security Company
**
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
** later version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
** warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
** details.
**
** You should have received a copy of the GNU Affero General Public License along with this program.  If not, see
** <http://www.gnu.org/licenses/>.
***********************************************************************************************************************/
#include "kh/exception.hpp"

namespace KirHut
{

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

        me.data = {};
        me.infoView = {};
        me.prev = nullptr;
        me.next = nullptr;
    }
};

Exception::Exception(StringView sv) noexcept :
    infoView(sv),
    prev(nullptr),
    next(nullptr)
{
    // No implementation.
}

Exception::Exception(StringView sv, bool copy) :
    Exception(sv)
{
    if (copy)
    {
        data = sv;
        infoView = {};
    }
}

Exception::Exception(String &&other) noexcept :
    data(std::move(other)),
    prev(nullptr),
    next(nullptr)
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
        prev = other.next ? Impl::getLast(other.next) : &other;
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
            prev = other.prev;
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
        next = other.next;
        next->prev = this;
    }

    return *this;
}

StringView Exception::info() const noexcept
{
    if (infoView.data())
    {
        return infoView;
    }

    return Impl::getOwner(this)->data;
}

char const *Exception::what() const noexcept
{
    return reinterpret_cast<char const *>(info().data());
}

} // namespace KirHut
