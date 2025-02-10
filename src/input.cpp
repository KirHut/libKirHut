/***********************************************************************************************************************
** The KirHut Application Development Library
** input.cpp
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
#include "kh/input.hpp"

using namespace KirHut;
using namespace KirHut::IO;

bool Input::TestObj::operator()(char c) const noexcept
{
    return (*this)(bit_cast<byte>(c));
}

bool Input::TestObj::operator()(byte b) const noexcept
{
    return (*this)(bit_cast<char>(b));
}

size_t Input::fillContent(byte *buffer, size_t amount)
{
    if (!buffer || !amount)
    {
        return 0;
    }

    string toCopy = readUntil(TestIm{ [&]([[maybe_unused]] char c) { return !(amount--); } }, false);
    memcpy(buffer, toCopy.data(), toCopy.size());
    return toCopy.size();
}

bool Input::ready() const noexcept
{
    return bytesReady();
}

size_t Input::bytesReady() const noexcept
{
    return bytesRemaining();
}

size_t Input::bytesRemaining() const noexcept
{
    return UNKNOWN_BYTES;
}

bool Input::goodIn() const noexcept
{
    return !failedIn() && moreContent();
}

string Input::nextToken()
{
    ignoreUntil(TestIm{ [](unsigned char c) { return !std::isspace(c); } }, false);
    return readUntil(TestIm{ [](unsigned char c) { return std::isspace(c); } }, false);
}

string Input::readData(char b, bool include)
{
    return readUntil(TestIm{ [b](char c) { return c == b; } }, include);
}

inline std::vector<byte> readByteDataIm(string &&str)
{
    std::vector<byte> ret(str.size());
    memcpy(ret.data(), str.data(), str.size());
    return ret;
}

std::vector<byte> Input::readData(byte b, bool include)
{
    // readUntil unconditionally returns a std::string, regardless of if the tested type is a byte or a char. As such,
    // this method has no other choice but to copy the string data into a returned std::vector<byte>. Unfortunately,
    // there is no method to take the data from a std::string without some serious convolution.
    return readByteDataIm(readUntil(TestIm{ [b](byte c) { return c == b; } }, include));
}

string Input::readReadyUntil(TestObj const &to, bool include)
{
    if (!ready())
    {
        return {};
    }

    size_t pos = bytesReady();
    if (pos)
    {
        --pos;
    }

    return readUntil(TestIm{ [&](byte b) { return !pos or (--pos, to(b)); } }, include);
}

string Input::readReadyData(char b, bool include)
{
    return readReadyUntil(TestIm{ [b](char c) { return c == b; } }, include);
}

std::vector<byte> Input::readReadyData(byte b, bool include)
{
    // readReadyUntil unconditionally returns a std::string, regardless of if the tested type is a byte or a char. As
    // such, this method has no other choice but to copy the string data into a returned std::vector<byte>.
    // Unfortunately, there is no method to take the data from a std::string without some serious convolution.
    return readByteDataIm(readReadyUntil(TestIm{ [b](byte c) { return c == b; } }, include));
}

// TODO: Combine these templates.
template <typename T, ByteType B>
inline T readLineIm(Input &in, B b)
{
    T ret = in.readData(b, true);
    ret.pop_back();
    return ret;
}

template <typename T, ByteType B>
inline T readReadyLineIm(Input &in, B b)
{
    T ret = in.readReadyData(b, true);
    ret.pop_back();
    return ret;
}

string Input::readLine(char b)
{
    return readLineIm<string, char>(*this, b);
}

std::vector<byte> Input::readLine(byte b)
{
    return readLineIm<std::vector<byte>, byte>(*this, b);
}

string Input::readReadyLine(char b)
{
    return readReadyLineIm<string, char>(*this, b);
}

std::vector<byte> Input::readReadyLine(byte b)
{
    return readReadyLineIm<std::vector<byte>, byte>(*this, b);
}

size_t Input::ignoreData(char b, bool include)
{
    return readData(b, include).size();
}

size_t Input::ignoreData(byte b, bool include)
{
    return readData(b, include).size();
}

size_t Input::ignoreData(size_t amount)
{
    return getContent(amount).size();
}

size_t Input::ignoreUntil(const TestObj &to, bool include)
{
    return readUntil(to, include).size();
}

size_t Input::ignoreReadyData(char b, bool include)
{
    return readReadyData(b, include).size();
}

size_t Input::ignoreReadyData(byte b, bool include)
{
    return readReadyData(b, include).size();
}

size_t Input::ignoreReadyData(size_t amount)
{
    return getReadyContent(amount).size();
}

size_t Input::ignoreReadyUntil(const TestObj &to, bool include)
{
    return readReadyUntil(to, include).size();
}

string Input::getContent(size_t amount)
{
    string ret(amount, '\0');
    fillContent(ret.data(), amount);
    return ret;
}

string Input::getReadyContent(size_t amount)
{
    string ret(amount, '\0');
    ret.resize(fillReadyContent(ret.data(), amount));
    return ret;
}

size_t Input::fillContent(char *buffer, size_t amount)
{
    return fillContent(bit_cast<byte *>(buffer), amount);
}

size_t Input::fillReadyContent(byte *buffer, size_t amount)
{
    if (!ready())
    {
        return 0;
    }

    return fillContent(buffer, std::min(amount, bytesReady()));
}

size_t Input::fillReadyContent(char *buffer, size_t amount)
{
    return fillReadyContent(bit_cast<byte *>(buffer), amount);
}

bool SeekableInput::isFloating() const noexcept
{
    return true;
}

bool SeekableInput::setFloating([[maybe_unused]] bool shouldFloat) noexcept
{
    return true;
}
