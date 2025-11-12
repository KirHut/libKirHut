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

#include <catch2/catch_test_macros.hpp>
// clazy:excludeall=non-pod-global-static

using namespace KirHut;

constexpr auto tempDeleter = [](std::FILE *fp) {
    if (fp)
    {
        std::fclose(fp);
    }
};

using TempFile = std::unique_ptr<std::FILE, decltype(tempDeleter)>;

inline TempFile mkTempFile()
{
    std::FILE *tmpFilePtr = nullptr;
#if defined(KH_COMPILED_WITH_MSVC)
    if (tmpfile_s(&tmpFilePtr) == 0 and tmpFilePtr)
#else
    if (tmpFilePtr = std::tmpfile(); tmpFilePtr)
#endif
    {
        return TempFile{ tmpFilePtr };
    }

    throw std::runtime_error("Couldn't create temporary file.");
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
    TempFile tmp = mkTempFile();
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
    TempFile tmp = mkTempFile();
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
    TempFile tmp = mkTempFile();
    auto args    = FMT::make_format_args("Test");
    IO::vprintln(tmp.get(), "{}", args);
    std::fflush(tmp.get());
    std::rewind(tmp.get());
    char buf[16];
    std::fgets(buf, sizeof buf, tmp.get());
    REQUIRE(std::string(buf) == "Test\n");
}

TEST_CASE("IO::println() appends newline to FILE*", "[print][io_println]")
{
    TempFile tmp = mkTempFile();
    auto args    = "Test"sv;
    IO::println(tmp.get(), "{}", args);
    std::fflush(tmp.get());
    std::rewind(tmp.get());
    char buf[16];
    std::fgets(buf, sizeof buf, tmp.get());
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
