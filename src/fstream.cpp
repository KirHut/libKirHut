/***********************************************************************************************************************
** The KirHut Application Development Library
** fstream.cpp
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
#include "kh/fstream.hpp"
#include "kh/base.hpp"
#include "kh/exceptions.hpp"

#include <fstream>

using namespace KirHut;
using namespace KirHut::IO;

namespace KirHut::IO::Priv
{

constexpr int EXIST_FLAG    = 0;
constexpr int TRUNCATE_FLAG = 1;
constexpr int APPEND_FLAG   = 2;
constexpr int AT_END_FLAG   = 3;

std::ios::openmode toOpenFlags(FileFactoryFlags const &flags)
{
    std::ios::openmode ret = 0;
    if (flags.test(TRUNCATE_FLAG))
    {
        ret |= std::ios::trunc;
    }

    if (flags.test(APPEND_FLAG))
    {
        ret |= std::ios::app;
    }

    if (flags.test(AT_END_FLAG))
    {
        ret |= std::ios::ate;
    }

    return ret;
}

} // namespace KirHut::IO::Priv

std::ios_base::seekdir toStdDir(Relation dir)
{
    switch (dir)
    {
    case Relation::BEGIN: return std::ios_base::beg;
    case Relation::END: return std::ios_base::end;

    default: return std::ios_base::cur;
    }
}

template <IsOneOf<FileOutput, FileReader, FileStream> OpenerType>
MaybeInv<OpenerType> IO::Priv::openFileFactoryImpl(FS::path const &filePath, FileFactoryFlags flags) noexcept
{
    bool mustExist = flags.test(0);
    if constexpr (std::same_as<OpenerType, FileReader>)
    {
        mustExist = true;
    }

    try
    {
        try
        {
            if (mustExist)
            {
                if (!FS::exists(filePath))
                {
                    return { WhyInvalid::NotFound,
                             std::move("The file path provided does not exist: "s += filePath.string()) };
                }
                else if (!FS::is_regular_file(filePath))
                {
                    return { WhyInvalid::BadArgument,
                             std::move("The file path provided is not a regular data file: "s += filePath.string()) };
                }
            }
            else if (auto ppath = filePath.parent_path(); !FS::exists(ppath))
            {
                // In the long run, it should be possible to create the parent path folders up until the final object
                // in the filesystem. However, I'm too lazy to do that now, so it just fails.
                return { WhyInvalid::CouldntOpen,
                         std::move("The parent directory of the requested file does not exist: "s += ppath.string()) };
            }

            if (UPtr<typename OpenerType::Impl> im =
                    make_unique<typename OpenerType::Impl>(filePath, toOpenFlags(flags));
                !im)
            {
                return { WhyInvalid::BadAllocation,
                         "The file opening factory could not allocate sufficient data for the underlying "
                         "implementation object: "s += std::to_string(sizeof(typename OpenerType::Impl)) + "\n" };
            }
            else if (im->stream.good())
            {
                // There are two options here: This either moves an unique pointer twice, or the File opening types
                // require a public constructor. Currently, since moving unique pointers is cheap, this just does that.
                // If this becomes an efficiency issue, it can be changed.
                return OpenerType{ std::move(im) };
            }
            else
            {
                return { WhyInvalid::CorruptData, "The internal stream was not opened in a good state."sv };
            }
        }
        catch (Exception &ex)
        {
            return { WhyInvalid::CouldntOpen, std::move("An Exception was thrown by the constructor: "s += ex.info()) };
        }
        catch (std::bad_alloc &ex)
        {
            throw;
        }
        catch (std::exception &ex)
        {
            return { WhyInvalid::CouldntOpen,
                     std::move("A standard exception was thrown by the constructor: "s += ex.what()) };
        }
        catch (...)
        {
            return { WhyInvalid::UnknownReason,
                     "An unknown exception was thrown by the internal stream constructor"sv };
        }
    }
    catch (std::bad_alloc &ex)
    {
        return { WhyInvalid::BadAllocation, "An attempt to allocate memory while opening a file failed."sv };
    }
}

struct FileOutput::Impl
{
    typedef std::ofstream StreamType;
    StreamType stream;

    Impl(FS::path const &filePath, std::ios::openmode addFlags) :
        stream(filePath, std::ios::binary | std::ios::out | addFlags)
    {
        // We never want to throw an exception, set fail state instead.
        stream.exceptions(0);
    }
};

FileOutput::FileOutput(UPtr<Impl> &&im) noexcept : im(std::move(im))
{
    // No implementation.
}

size_t FileOutput::printsv(string_view str)
{
    if (!im)
    {
        throw BadState("The internal state of this FileOutput object has been removed (likely because of a previous "
                       "move operation)."sv);
    }

    size_t change = im->stream.rdbuf()->sputn(str.data(), str.size());
    if (change != str.size())
    {
        im->stream.setstate(im->stream.rdstate() | std::ios_base::badbit);
    }

    return change;
}

bool FileOutput::failedOut() const noexcept
{
    return !im or im->stream.bad() or im->stream.fail();
}

SeekPos FileOutput::tellOut() const noexcept
{
    return im ? static_cast<SeekPos>(im->stream.tellp()) : IO::INVALID_POS;
}

void FileOutput::seekOut(i64 offset, Relation from)
{
    if (!im)
    {
        throw BadState("The internal state of this FileOutput object has been removed (likely because of a previous "
                       "move operation)."sv);
    }

    im->stream.seekp(offset, toStdDir(from));
}

MaybeInv<FileOutput> IO::openFileForOutput(FS::path filePath) noexcept
{
    return IO::Priv::openFileFactoryImpl<FileOutput>(filePath, false);
}

struct FileReader::Impl
{
    typedef std::ifstream StreamType;
    StreamType stream;
    size_t totalBytes;

    Impl(FS::path const &filePath, std::ios::openmode addFlags) :
        stream(filePath, std::ios::binary | std::ios::in | addFlags)
    {
        stream.seekg(0, StreamType::end);
        totalBytes = stream.tellg();
        stream.seekg(0, StreamType::beg);
        totalBytes -= stream.tellg();
        // We never want to throw an exception, set fail state instead.
        stream.exceptions(0);
    }
};

FileReader::FileReader(UPtr<Impl> &&im) noexcept : im(std::move(im))
{
    // No implementation.
}

bool FileReader::moreContent() const noexcept
{
    if (!im or !im->stream.rdbuf() or im->stream.eof())
    {
        return false;
    }
    else if (auto avail = im->stream.rdbuf()->in_avail(); avail == -1)
    {
        return false;
    }
    else if (avail == 0)
    {
        return im->stream.rdbuf()->sgetc() != Impl::StreamType::traits_type::eof();
    }

    // If avail is any value other than 0 or -1, there must be more content.
    return true;
}

bool FileReader::failedIn() const noexcept
{
    return !im or im->stream.bad() or im->stream.fail();
}

template <typename ReturnType, typename ImplType>
string readUntilImpl(ImplType &im, Input::TestObj const &to, bool include)
{
    using IntType      = ImplType::StreamType::int_type;
    using BType        = ReturnType::value_type;
    constexpr auto eof = ImplType::StreamType::traits_type::eof();

    ReturnType ret;
    for (IntType c; (c = im.stream.get()) != eof;)
    {
        char test = static_cast<BType>(c);
        if (to(test))
        {
            ret.push_back(test);
        }
        else
        {
            if (include)
            {
                ret.push_back(test);
            }
            else
            {
                im.stream.unget();
            }

            break;
        }
    }

    return ret;
}

string FileReader::readUntil(TestObj const &to, bool include)
{
    return readUntilImpl<string>(*im, to, include);
}

size_t FileReader::fillContent(byte *buffer, size_t amount)
{
    if (!im->stream.read(bit_cast<char *>(buffer), amount).good())
    {
        return im->stream.gcount();
    }

    return amount;
}

size_t FileReader::fillContent(char *buffer, size_t amount)
{
    if (!im->stream.read(buffer, amount).good())
    {
        return im->stream.gcount();
    }

    return amount;
}

SeekPos FileReader::tellIn() const noexcept
{
    return im->stream.tellg();
}

void FileReader::seekIn(i64 off, Relation from)
{
    im->stream.seekg(off, toStdDir(from));
}

size_t FileReader::bytesReady() const noexcept
{
    return im->stream.rdbuf()->in_avail();
}

size_t FileReader::bytesRemaining() const noexcept
{
    return im->totalBytes - im->stream.tellg();
}

template <typename ReturnType, IsOneOf<FileReader, FileStream> InputType>
ReturnType readDataImpl(InputType &me, typename ReturnType::value_type to, bool include)
{
    ReturnType ret = me.readLine(to);
    if (include)
    {
        ret.push_back(to);
    }

    return ret;
}

string FileReader::readData(char c, bool include)
{
    return readDataImpl<string>(*this, c, include);
}

std::vector<byte> FileReader::readData(byte b, bool include)
{
    return readDataImpl<std::vector<byte>>(*this, b, include);
}

string FileReader::readLine(char c)
{
    string ret;
    std::getline(im->stream, ret, c);
    return ret;
}

std::vector<byte> FileReader::readLine(byte b)
{
    string copyFrom = readLine(bit_cast<char>(b));
    std::vector<byte> ret;
    ret.reserve(copyFrom.size() + 1);
    ret.resize(copyFrom.size());
    memcpy(ret.data(), copyFrom.data(), copyFrom.size());
    return ret;
}

MaybeInv<FileReader> IO::openFileForInput(FS::path filePath) noexcept
{
    return IO::Priv::openFileFactoryImpl<FileReader>(filePath, true);
}

struct FileStream::Impl
{
    typedef std::fstream StreamType;
    StreamType stream;

    Impl(FS::path const &filePath, std::ios::openmode addFlags) :
        stream(filePath, std::ios::binary | std::ios::in | std::ios::out | addFlags)
    {
        // We never want to throw an exception, set fail state instead.
        stream.exceptions(0);
    }
};

FileStream::FileStream(UPtr<Impl> &&im) noexcept : im(std::move(im))
{
    // No implementation.
}

size_t FileStream::printsv(string_view str)
{
    return 0;
}

bool FileStream::failedOut() const noexcept
{
    return false;
}

SeekPos FileStream::tellOut() const noexcept
{
    return 0;
}

void FileStream::seekOut(i64 offset, Relation from)
{
}

bool FileStream::moreContent() const noexcept
{
    return false;
}

bool FileStream::failedIn() const noexcept
{
    return false;
}

string FileStream::readUntil(TestObj const &to, bool include)
{
    return readUntilImpl<string>(*im, to, include);
}

size_t FileStream::fillContent(byte *buffer, size_t amount)
{
    return 0;
}

SeekPos FileStream::tellIn() const noexcept
{
    return 0;
}

void FileStream::seekIn(i64 off, Relation from)
{
}

MaybeInv<FileStream> IO::openFileForIO(FS::path filePath, bool mustExist) noexcept
{
    return IO::Priv::openFileFactoryImpl<FileStream>(filePath, mustExist);
}
