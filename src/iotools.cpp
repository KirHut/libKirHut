/***********************************************************************************************************************
** The KirHut Application Development Library
** iotools.cpp
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
#include "kh/iotools.hpp"

#include "kh/exceptions.hpp"

using namespace KirHut::IO;
using namespace KirHut;

FixedBufferOutput::FixedBufferOutput(size_t initialBufferSize, bool inserting) :
    data(initialBufferSize, byte(0)),
    insertState(inserting)
{
    // No implementation.
}

size_t FixedBufferOutput::resize(size_t newBufferSize)
{
    data.resize(newBufferSize > data.max_size() ? data.max_size() : newBufferSize);
    if (cursorPosition > data.size())
    {
        cursorPosition = data.size();
    }

    return data.size();
}

bool FixedBufferOutput::isInserting() const noexcept
{
    return insertState;
}

bool FixedBufferOutput::setInserting(bool insert) noexcept
{
    bool last = insertState;
    return (insertState = insert) != last;
}

std::span<const byte> FixedBufferOutput::buffer() const noexcept
{
    return data;
}

std::span<byte> FixedBufferOutput::buffer() noexcept
{
    return data;
}

byte const *FixedBufferOutput::bufPtr() const noexcept
{
    return data.data();
}

byte *FixedBufferOutput::bufPtr() noexcept
{
    return data.data();
}

std::vector<byte> FixedBufferOutput::copyBuffer() const
{
    return data;
}

std::vector<byte> FixedBufferOutput::takeBuffer(size_t newSize)
{
    // Trust in NRVO, on all supported compilers this is only a single move operation.
    std::vector<byte> ret = std::move(data);

    if (newSize)
    {
        data.clear();
        data.resize(newSize == SAME_SIZE ? ret.size() : newSize);
    }

    return ret;
}

std::vector<std::byte> FixedBufferOutput::takeBuffer() noexcept
{
    return std::move(data);
}

size_t FixedBufferOutput::printsv(string_view str) noexcept
{
    if (isFull())
    {
        return 0;
    }

    SeekPos newPosition = cursorPosition + str.size();
    if (newPosition >= data.size())
    {
        newPosition = data.size();
    }
    else if (insertState)
    {
        size_t numShiftedBytes = data.size() - newPosition;
        auto shiftStartPoint = data.begin() + cursorPosition, shiftEndPoint = shiftStartPoint + numShiftedBytes;
        // Use copy_backward instead of memcpy to ensure no issues with overlapping.
        std::copy_backward(shiftStartPoint, shiftEndPoint, data.end());
    }

    size_t amount = newPosition - cursorPosition;
    memcpy(&data[cursorPosition], str.data(), amount);
    cursorPosition = newPosition;
    return amount;
}

bool FixedBufferOutput::failedOut() const noexcept
{
    // The FixedBufferOutput type never fails.
    return false;
}

bool FixedBufferOutput::isFull() const noexcept
{
    return cursorPosition >= data.size();
}

SeekPos FixedBufferOutput::tellOut() const noexcept
{
    return 0;
}

void FixedBufferOutput::seekOut(i64 offset, Relation from)
{
    if (from == BEGIN)
    {
        cursorPosition = 0;
    }
    else if (from == END)
    {
        cursorPosition = data.size();
    }

    if (offset < 0 && static_cast<SeekPos>(std::abs(offset)) > cursorPosition)
    {
        cursorPosition = 0;
    }
    else if ((cursorPosition += offset) > data.size())
    {
        cursorPosition = data.size();
    }
}

StringOutput::StringOutput(const char *start, SeekPos pos) : StringOutput(string_view{ start }, pos)
{
    // No implementation.
}

StringOutput::StringOutput(string_view start, SeekPos pos) : data(start), cursorPosition(std::min(pos, start.size()))
{
    // No implementation.
}

StringOutput::StringOutput(string &&start, SeekPos pos) noexcept :
    data(std::move(start)),
    cursorPosition(std::min(pos, data.size()))
{
    // No implementation.
}

StringOutput::StringOutput(const char *start, size_t bufSize, SeekPos pos) :
    StringOutput(string_view{ start, bufSize }, pos)
{
    // No implementation.
}

StringOutput::StringOutput(StringOutput &&other) noexcept :
    data(std::move(other.data)),
    cursorPosition(other.cursorPosition)
{
    other.cursorPosition = END_POS;
}

StringOutput &StringOutput::operator=(StringOutput &&other)
{
    data                 = std::move(other.data);
    cursorPosition       = other.cursorPosition;
    other.cursorPosition = END_POS;
    return *this;
}

string_view StringOutput::peekData() const noexcept
{
    return data;
}

string StringOutput::takeData() & noexcept
{
    // Trust in NRVO, this will only call a single move operation.
    string ret     = std::move(data);
    data           = {};
    cursorPosition = 0;
    return ret;
}

string StringOutput::takeData() && noexcept
{
    cursorPosition = END_POS;
    // Yes this is correct. data is a class member, so the RVO returned string must be move constructed from the member.
    return std::move(data);
}

string StringOutput::copyData() const
{
    return data;
}

void StringOutput::reserve(size_t capacity)
{
    data.reserve(capacity);
}

void StringOutput::setCapacity(size_t capacity)
{
    if (capacity < data.capacity())
    {
        data.shrink_to_fit();
    }

    if (capacity > data.size())
    {
        data.reserve(capacity);
    }
}

size_t StringOutput::dataCapacity() const noexcept
{
    return data.capacity();
}

size_t StringOutput::printsv(string_view str)
{
    if (cursorPosition + str.size() < cursorPosition or cursorPosition + str.size() > data.max_size())
    {
        throw IllegalArgument("The string being output to this StringOutput object is too large to fit in the "
                              "underlying string.\nThe size of the input: "s +=
                              std::to_string(str.size()) +
                              "\nThe current cursor position: " + std::to_string(cursorPosition) +
                              "\nThe maximum string size: " + std::to_string(data.max_size()));
    }

    if (cursorPosition == data.size())
    {
        data.append(str);
    }
    else
    {
        data.insert(cursorPosition, str);
    }

    cursorPosition += str.size();
    return str.size();
}

bool StringOutput::failedOut() const noexcept
{
    return cursorPosition == END_POS;
}
