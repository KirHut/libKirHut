/***********************************************************************************************************************
** The KirHut Application Development Library
** TestPrint.cpp
** Copyright © KirHut Software Company
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** conditions found in the BSD 3-Clause License are met.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** You should have received a copy of the BSD 3-Clause license along with this program.  If not, see
** <https://opensource.org/license/bsd-3-clause>.
***********************************************************************************************************************/
#include "kh/print.hpp"

#include "kh/base.hpp"

#include <cstdio>

#if defined(KH_WINDOWS)
# include <windows.h>
# include <io.h>
# include <fcntl.h>
# include <fileapi.h>
#endif

#include <catch2/catch_test_macros.hpp>
// clazy:excludeall=non-pod-global-static

using namespace KirHut;

template <typename Holder_T>
concept FilePtrHolder = requires(Holder_T const &holder) {
    { holder.get() } -> std::same_as<std::FILE *>;
};

class EphemeralFile
{
    std::FILE *filePtr = nullptr;

public:
    inline EphemeralFile();
    inline EphemeralFile(EphemeralFile &&other) noexcept;
    inline EphemeralFile &operator=(EphemeralFile &&other) noexcept;
    inline ~EphemeralFile() noexcept;

    EphemeralFile(EphemeralFile const &)            = delete;
    EphemeralFile &operator=(EphemeralFile const &) = delete;

    inline std::FILE *get() const noexcept;
    inline std::FILE *release() noexcept;
};

EphemeralFile::EphemeralFile()
{
#if defined(KH_WINDOWS)
    // Oh my God, fuck you Microsoft...
    wchar_t tempPathBuffer[MAX_PATH];
    DWORD pathLen =
# if defined(KH_COMPILED_WITH_MSVC)
        GetTempPath2W(MAX_PATH, tempPathBuffer);
# else
        GetTempPathW(MAX_PATH, tempPathBuffer);
# endif

    if (pathLen == 0 or pathLen > MAX_PATH)
    {
        throw std::runtime_error("Call to GetTempPathW failed.");
    }

    wchar_t tempNameBuffer[MAX_PATH];
    if (not GetTempFileNameW(tempPathBuffer, L"kh", 0, tempNameBuffer))
    {
        throw std::runtime_error("Call to GetTempFileNameW failed.");
    }

    HANDLE tempHandle = CreateFileW(tempNameBuffer,
                                    GENERIC_READ | GENERIC_WRITE,
                                    0,
                                    nullptr,
                                    CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                                    nullptr);

    if (tempHandle == INVALID_HANDLE_VALUE)
    {
        DeleteFileW(tempNameBuffer);
        throw std::runtime_error("Call to CreateFileW failed.");
    }

    if (int fd = _open_osfhandle(reinterpret_cast<std::intptr_t>(tempHandle), _O_RDWR | _O_BINARY); fd != -1)
    {
        if (filePtr = _fdopen(fd, "w+b"); not filePtr)
        {
            _close(fd);
            throw std::runtime_error("Call to _fdopen() failed.");
        }
    }
    else
    {
        DeleteFileW(tempNameBuffer);
        throw std::runtime_error("Call to _open_osfhandle() failed.");
    }
#else
    // On everything that isn't Windows, tmpfile() just works like you'd expect, so this just does that.
    if (filePtr = std::tmpfile(); not filePtr)
    {
        throw std::runtime_error("Creating a temporary file failed.");
    }
#endif
}

EphemeralFile::EphemeralFile(EphemeralFile &&other) noexcept : filePtr(other.release())
{
    // No further implementation.
}

EphemeralFile &EphemeralFile::operator=(EphemeralFile &&other) noexcept
{
    if (this != &other)
    {
        if (filePtr)
        {
            std::fclose(filePtr);
        }

        filePtr = other.release();
    }

    return *this;
}

EphemeralFile::~EphemeralFile() noexcept
{
    if (filePtr)
    {
        std::fclose(filePtr);
    }
}

std::FILE *EphemeralFile::get() const noexcept
{
    return filePtr;
}

std::FILE *EphemeralFile::release() noexcept
{
    std::FILE *toReturn = filePtr;
    filePtr             = nullptr;
    return toReturn;
}

TEST_CASE("IO::vprint() writes to ostream", "[print][io_vprint]")
{
    std::ostringstream oss;
    auto args = FMT::make_format_args("World");
    IO::vprint(oss, "Hello, {}!", args);
    REQUIRE(oss.str() == "Hello, World!");
}

TEST_CASE("IO::print() writes to ostream", "[print][io_print]")
{
    std::ostringstream oss;
    string_view world = "World"sv;
    IO::print(oss, "Hello, {}!", world);
    REQUIRE(oss.str() == "Hello, World!");
}

TEST_CASE("IO::vprintln() appends newline to ostream", "[print][io_vprintln]")
{
    std::ostringstream oss;
    auto num  = 42;
    auto args = FMT::make_format_args(num);
    IO::vprintln(oss, "Answer: {}", args);
    REQUIRE(oss.str() == "Answer: 42\n");
}

TEST_CASE("IO::println() appends newline to ostream", "[print][io_println]")
{
    std::ostringstream oss;
    auto num = 42;
    IO::println(oss, "Answer: {}", num);
    REQUIRE(oss.str() == "Answer: 42\n");
}

TEST_CASE("IO::vprint() writes to FILE*", "[print][io_vprint]")
{
    EphemeralFile tmp;
    auto num     = 3.14;
    auto args    = FMT::make_format_args(num);
    IO::vprint(tmp.get(), "Pi={}", args);
    std::fflush(tmp.get());

    std::rewind(tmp.get());
    std::string result;
    char buf[128];
    while (std::fgets(buf, sizeof buf, tmp.get()))
    {
        result += buf;
    }
    REQUIRE(result == "Pi=3.14");
}

TEST_CASE("IO::print() writes to FILE*", "[print][io_print]")
{
    EphemeralFile tmp;
    auto num     = 3.14;
    IO::print(tmp.get(), "Pi={}", num);
    std::fflush(tmp.get());

    std::rewind(tmp.get());
    std::string result;
    char buf[128];
    while (std::fgets(buf, sizeof buf, tmp.get()))
    {
        result += buf;
    }
    REQUIRE(result == "Pi=3.14");
}

TEST_CASE("IO::vprintln() appends newline to FILE*", "[print][io_vprintln]")
{
    EphemeralFile tmp;
    auto args    = FMT::make_format_args("Test");
    IO::vprintln(tmp.get(), "{}", args);
    std::fflush(tmp.get());
    std::rewind(tmp.get());
    char buf[16];
    REQUIRE(std::fgets(buf, sizeof buf, tmp.get()));
    REQUIRE(std::string(buf) == "Test\n");
}

TEST_CASE("IO::println() appends newline to FILE*", "[print][io_println]")
{
    EphemeralFile tmp;
    auto args    = "Test"sv;
    IO::println(tmp.get(), "{}", args);
    std::fflush(tmp.get());
    std::rewind(tmp.get());
    char buf[16];
    REQUIRE(std::fgets(buf, sizeof buf, tmp.get()));
    REQUIRE(std::string(buf) == "Test\n");
}

TEST_CASE("UTF-8 text is printed intact", "[print][io_print]")
{
    std::ostringstream oss;
    IO::print(oss, "{}", "🌍");
    auto out = oss.str();
    REQUIRE(out == "🌍");
    REQUIRE(out.size() == 4);
}
