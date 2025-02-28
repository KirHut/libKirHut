/***********************************************************************************************************************
** The KirHut Application Development Library
** iostream.cpp
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
#include "kh/iostream.hpp"

#include "nowide/args.hpp"
#include "nowide/iostream.hpp"

namespace KirHut::IO
{

void initArgs(int &argc, char **&argv)
{
    static nowide::args _(argc, argv);
}

typedef decltype(nowide::cout) OutStream;

struct StdOutput final : public Output
{
    OutStream &out;

    StdOutput(OutStream &outStream) : out(outStream)
    {
        // No implementation.
    }

    size_t printsv(string_view str) override
    {
        out << str;
        return str.size();
    }

    void flush() override
    {
        out.flush();
    }

    bool failedOut() const noexcept override
    {
        return out.fail();
    }
};

struct StdInput final : public Input
{
    bool moreContent() const noexcept override
    {
        return !failedIn();
    }

    bool failedIn() const noexcept override
    {
        return nowide::cin.fail();
    }

    size_t bytesReady() const noexcept override
    {
        return 0;
    }

    string readUntil(TestObj const &to, bool include) override
    {
        constexpr int BUF_SIZE = 128;
        array<char, BUF_SIZE> buffer;
        string retStr;
        int ctr = 0;
        for (char c; nowide::cin.get(c).good(); ++ctr)
        {
            if (ctr == BUF_SIZE)
            {
                retStr.append(buffer.data(), BUF_SIZE);
                ctr = 0;
            }

            if (to(c))
            {
                if (include)
                {
                    buffer[ctr++] = c;
                }
                else
                {
                    // Input streams are guaranteed to take back at least 1 character.
                    nowide::cin.unget();
                }

                break;
            }

            buffer[ctr] = c;
        }

        retStr.append(buffer.data(), ctr);
        return retStr;
    }

    string readLine(char b) override
    {
        string ret;
        std::getline(nowide::cin, ret, b);
        return ret;
    }

    size_t ignoreData(char b, bool include) override
    {
        size_t ret = nowide::cin.ignore(UNKNOWN_BYTES, b).gcount();
        if (ret && !include)
        {
            // Input streams are guaranteed to take back at least 1 character.
            nowide::cin.unget();
            --ret;
        }

        return ret;
    }

    size_t fillContent(byte *buffer, size_t amount) override
    {
        return StdInput::fillContent(bit_cast<char *>(buffer), amount);
    }

    inline size_t fillContent(char *buffer, size_t amount) override
    {
        if (nowide::cin.read(buffer, amount).good())
        {
            return amount;
        }

        return nowide::cin.gcount();
    }
};

Output &out() noexcept
{
    static StdOutput o(nowide::cout);
    return o;
}

Input &in() noexcept
{
    static StdInput i;
    return i;
}

Output &err() noexcept
{
    static StdOutput o(nowide::cerr);
    return o;
}

bool recoverIn() noexcept
{
    nowide::cin.clear();
    return nowide::cin.good();
}

size_t vprint(string_view fmt, std::format_args args)
{
    return out().vprint(fmt, args);
}

size_t vprintln(string_view fmt, std::format_args args)
{
    return out().vprintln(fmt, args);
}

} // namespace KirHut::IO
