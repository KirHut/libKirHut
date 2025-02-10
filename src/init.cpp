/***********************************************************************************************************************
** The KirHut Application Development Library
** init.cpp
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
#include "kh/init.hpp"

#include "priv/ioinit.hpp"

#include "nowide/args.hpp"

using namespace KirHut;
using namespace KirHut::Init;

struct InitTag::Impl
{
    Impl(InitFlags flags)
    {
        if (flags.test(1))
        {
            ioinit.emplace();
        }
    }

    Impl(int argc, char **argv, InitFlags flags) : Impl(flags)
    {
        if (flags.test(0))
        {
            a.emplace(argc, argv);
        }

        args.reserve(argc);
        for (int i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }

        commandLineConfigs();
    }

    void commandLineConfigs()
    {
        // Does nothing currently.
    }

    std::vector<char *> args;
    Maybe<nowide::args> a;
    Maybe<IO::Priv::Init> ioinit;
};

InitTag::InitTag(InitFlags flags) : im(make_unique<Impl>(flags))
{
    // No implementation.
}

InitTag::InitTag(int argc, char **argv, InitFlags flags) : im(make_unique<Impl>(argc, argv, flags))
{
    // No implementation.
}

InitTag::~InitTag()
{
    // No implementation
}

size_t InitTag::argc() const noexcept
{
    return im->args.size();
}

char const *InitTag::argv(size_t arg) const noexcept
{
    if (arg < argc())
    {
        return im->args[arg];
    }

    return nullptr;
}
