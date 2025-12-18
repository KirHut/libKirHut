/***********************************************************************************************************************
** The KirHut Application Development Library
** TestMd5Hash.cpp
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
#include "kh/md5hash.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
// clazy:excludeall=non-pod-global-static

#include <algorithm>
#include <random>
#include <numeric>

#include "Md51.hpp"
// #include "Md51.cpp"

using namespace KirHut;
using namespace KirHut::Hash;

string getHex(ReadableSpanOf<byte> auto biteSize)
{
    constexpr auto hexDigits = "0123456789abcdef"sv;
    string ret;
    ret.reserve(biteSize.size_bytes() * 2);
    for (byte b : biteSize)
    {
        ret.push_back(hexDigits[static_cast<char>(b >> 4)]);
        ret.push_back(hexDigits[static_cast<char>(b) & 0xf]);
    }

    return ret;
}

Md5Sum md5Reference(span<byte const> data)
{
    Md5Sum out;
    md5::md5_t(data.data(), static_cast<unsigned int>(data.size_bytes()), out.data());
    return out;
}

constexpr string_view SmallTestBlock =
    "b3b44391c0ace63a3b29c3638d29ccc187e6c4f7d42da9354c1e75db41a646d75708ed528c3cae01440067bb5e22a87def6543d2b519c6"
    "1dcbd34d2ef843aea154ffe0348fa11d3ff21cf3da089485f247c4eefec92bc41a531e3d8caae29a5aabc465d4b7af36b93125598613c9"
    "9d727e81168de769f6ed74d0928c33981c14dca26f23c8cab49375a0b29c91f5830b5762e12ba194c9d1e5c49bb11db065dba20240338c"
    "39fc460841e3723c6ca269cccc590f0616ac9239c9a30872d401c07bc2eb5b22605aa774b3c981417966b780ad1f1a8a5daf07bdc387b3"
    "5f5fa0635c905482bb3e8743129a32ee1336fbc3e410463588e68bce294de11a60331ddc869ff7b139eef056d4656e0338967f5817d70c"
    "945a6907d4315f27098a6b0db80455f7804cc9401e8032c9faf38b70f6f53c97a8ff887959dbf8477c3be627a8764beff1fed1aeea58a5"
    "6d7f4badc22f790383353adf48784702dd99b80e6cf6412baa84af3284a64ca9097d099d534d623996ded7ba200ee5b9e0547802cabcd7"
    "c8db352ac4c4a36408c8705615550abb754dfc5370c300ea7153c7fe159cb0a10802c15335bc8fe28cc55f39f0cbb91cf47424bc0066f2"
    "167c6189564508f70d3b981fd895a8d8a66fb4b739bf00f3fbda7ea45d0777d5dd8b13ccb52665d84a6407b0cfa0fc5b9d9aaec8786008"
    "05c9541de2277d6beaca0cdcab967e9e9642c4c407c3c8e86b897a049f089bb9886b3e5ed3dd1e8e3c1da86e03c044ab5e7b3c21eb6059"
    "e905127304102430fb8f44d7e62410898101774298c6898e7d2f2140e44433a4c2688d62b777f484772af225cab84cb0b86920c4074547"
    "9c0e8e17130ff96a30606d1f0b2b6e367463440eda75881acde76f8dd68253036971e051057254205d2d4f76ae32b0dfc36706ff043359"
    "6373dd0f8127611a33711466c9a02c4f902341ba65fa6ab75a87fc4e3be5e25b72e1700cc87c1099e94ae44028146393b97cf0f4bda108"
    "3180f17d5782124c10dde336f4beebf394b58aedc37c40a81b254dac9d8d6e6f0a35b61aed4696ebbb1f4bbedc84378cc30ba44b53b197"
    "01b739134c8e1be17d842c6cc9a25b84d0656a42f1b232c3e6a24ee8de84651b8cd0e799dd3a13c682f4ae3021eadd7c912782e6b0d3a7"
    "eb695c22a6708a2ee1d261a42616d685df82982e5787b18bb2897a87ca4b7c9a468ab81d342a5df7d81c38e43229986587912e595ff574"
    "e391e029f12cf4455a55b4f971876dd9fe47fbb8f52efe0bbbda0b65de421b35f8c0f9bb8f9e3ec754f1aeab42e526c4a434302724ef2d"
    "0e6b00aeed274d123c280f1f30ba875aa302b62e08d51ee5fe0e2420a1828e83c2ad8ab7614d56529898e43dd7715983059f94a943c61d"
    "17225778e765b732ee9fa7d3ed6f1959aa438e88196ce5358ba912c141f58895efbf";

TEST_CASE("Check the size for Md5Sum", "[md5hash][constant]")
{
    STATIC_REQUIRE(sizeof(Md5Sum) == md5SumSize);
    Md5Sum sum;
    REQUIRE(sum.size() == md5SumSize);
}

TEST_CASE("Empty Md5Hash Object Construction State", "[md5hash][constructor]")
{
    Md5 hasher;
    string emptyMd5Str = "d41d8cd98f00b204e9800998ecf8427e";
    Md5Sum sum         = hasher.getHash();
    REQUIRE(emptyMd5Str == getHex(span{ sum }));
}

TEST_CASE("RFC 1321 test vectors", "[md5hash][constructor][rfc]")
{
    struct Vec
    {
        string_view input;
        string_view expected;
    };

    constexpr Vec vectors[] = { { "", "d41d8cd98f00b204e9800998ecf8427e" },
                                { "a", "0cc175b9c0f1b6a831c399e269772661" },
                                { "abc", "900150983cd24fb0d6963f7d28e17f72" },
                                { "message digest", "f96b697d7cb7938d525a2f31aaf161d0" },
                                { "abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b" },
                                { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
                                  "d174ab98d277d9f5a5611c2c9f419d9f" },
                                { "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
                                  "57edf4a22be3c955ac49da2e2107b67a" } };

    for (auto const &v : vectors)
    {
        Md5 hasher(v.input);
        auto hash = hasher.getHash();
        REQUIRE(getHex(span{ hash }) == v.expected);
    }
}

TEST_CASE("Initialize Md5Hash With An Array of Bytes", "[md5hash][constructor]")
{
    Md5 hasher(SmallTestBlock);
    string blockMd5Str = "266c6efea54d63c04fb8083f40d441cc";
    Md5Sum sum         = hasher.getHash();
    REQUIRE(blockMd5Str == getHex(span{ sum }));
}

TEST_CASE("MD5 padding boundary conditions", "[md5hash][padding]")
{
    for (size_t size : { 55u, 56u, 57u, 63u, 64u, 65u })
    {
        INFO("Size = " << size);

        std::vector<u8> data(size);
        std::iota(data.begin(), data.end(), u8{ 0 });

        Md5 hasher(span{ data });
        REQUIRE(hasher.getHash() == md5Reference(std::as_bytes(span{ data })));
    }
}

TEST_CASE("Constructing With Different Types of Input Data", "[md5hash][constructor]")
{
    Md5 hasher(SmallTestBlock);
    array<byte, md5SumSize> md5OtherBuffer;
    md5::md5_t(SmallTestBlock.data(), SmallTestBlock.size(), md5OtherBuffer.data());
    REQUIRE(hasher.getHash() == md5OtherBuffer);
    array<u64, 512> randLongs;
    std::mt19937_64 rng(currentTicks());
    std::generate(randLongs.begin(), randLongs.end(), rng);
    Md5 hasher2(span<u64, 512>{ randLongs });
    md5::md5_t(randLongs.data(), randLongs.size() * sizeof(u64), md5OtherBuffer.data());
    REQUIRE(hasher2.getHash() == md5OtherBuffer);
}

TEST_CASE("Adding Bytes After Completing Md5Hash", "[md5hash]")
{
    Md5 hasher;
    string emptyMd5Str = "d41d8cd98f00b204e9800998ecf8427e";
    Md5Sum sum         = hasher.getHash();
    REQUIRE(emptyMd5Str == getHex(span{ sum }));
    hasher.provideInput(span{ "Other invalid data that won't affect anything." });
    Md5Sum sum2 = hasher.getHash();
    REQUIRE(emptyMd5Str == getHex(span{ sum2 }));
}

TEST_CASE("Providing Input of Different Types of Data", "[md5hash]")
{
    std::mt19937_64 rng(currentTicks());
    array<i64, 256> signedList;
    array<u32, 512> unsignedList;
    std::generate(signedList.begin(), signedList.end(), rng);
    std::generate(unsignedList.begin(), unsignedList.end(), [&rng] { return static_cast<u32>(rng()); });
    Md5 hasher, hasher2;
    array<byte, md5SumSize> md5OtherBuf;
    md5::md5_t(signedList.data(), signedList.size() * sizeof(i64), md5OtherBuf.data());
    hasher.provideInput(span{ signedList });
    REQUIRE(hasher.getHash() == md5OtherBuf);
    md5::md5_t(unsignedList.data(), unsignedList.size() * sizeof(u32), md5OtherBuf.data());
    hasher2.provideInput(span{ unsignedList });
    REQUIRE(hasher2.getHash() == md5OtherBuf);
}

TEST_CASE("Incremental input equals one-shot input", "[md5hash][streaming]")
{
    std::vector<byte> data(4096);
    std::mt19937 rng(12'345);
    std::generate(data.begin(), data.end(), [&] { return byte(rng()); });

    Md5 oneShot(span{ data });

    Md5 streamed;
    for (size_t offset = 0; offset < data.size();)
    {
        size_t chunk = std::min<size_t>((rng() % 37) + 1, data.size() - offset);
        streamed.provideInput(span{ data }.subspan(offset, chunk));
        offset += chunk;
    }

    REQUIRE(oneShot.getHash() == streamed.getHash());
}

TEST_CASE("MD5 output iterator variants", "[md5hash][output]")
{
    Md5 hasher("abc");

    std::vector<byte> out1;
    hasher.getHash(std::back_inserter(out1));

    std::vector<char> out2;
    hasher.getHash(std::back_inserter(out2));

    REQUIRE(out1.size() == md5SumSize);
    REQUIRE(std::equal(out1.begin(), out1.end(), out2.begin(), [](byte b, char c) { return b == byte(c); }));
}

TEST_CASE("fitHash copies partial hash correctly", "[md5hash][fitHash]")
{
    Md5 hasher("abc");
    array<byte, 8> small{};
    hasher.fitHash(span{ small });

    auto full = hasher.getHash();
    REQUIRE(std::equal(small.begin(), small.end(), full.begin()));
}

TEST_CASE("Using The provideInput() Method to Input Data", "[md5hash]")
{
    string blockMd5Str = "266c6efea54d63c04fb8083f40d441cc";

    size_t divAmount = GENERATE(range(1u, 33u));

    Md5 hasher;

    size_t amount    = SmallTestBlock.size() / divAmount;
    size_t remainder = SmallTestBlock.size() % divAmount;

    for (size_t i = 0; i < divAmount; ++i)
    {
        hasher.provideInput(amount, &SmallTestBlock[i * amount]);
    }

    if (remainder)
    {
        hasher.provideInput(remainder, &SmallTestBlock[SmallTestBlock.size() - remainder]);
    }

    Md5Sum sum = hasher.getHash();

    REQUIRE(blockMd5Str == getHex(span{ sum }));
}

TEST_CASE("Taking MD5 Result Using Spans", "[md5hash]")
{
    Md5 hasher;
    string emptyMd5Str = "d41d8cd98f00b204e9800998ecf8427e";
    array<byte, 16> buffer; // Set this below 16 to test that hasher.getMd5() fails to compile with too small a span.
    auto bSpan = span{ buffer };
    hasher.getHash(bSpan);
    REQUIRE(emptyMd5Str == getHex(bSpan));
}

TEST_CASE("Large input MD5", "[md5hash][stress]")
{
    constexpr size_t size = 1 << 20; // 1 MB
    std::vector<byte> data(size, byte{ 0xA5 });

    Md5 hasher(span{ data });
    REQUIRE(hasher.getHash() == md5Reference(span{ data }));
}

TEST_CASE("Repeated getHash calls are stable", "[md5hash][state]")
{
    Md5 hasher("abc");

    auto h1 = hasher.getHash();
    auto h2 = hasher.getHash();
    auto h3 = hasher.getHash();

    REQUIRE(h1 == h2);
    REQUIRE(h2 == h3);
}

TEST_CASE("Endian-sensitive data patterns", "[md5hash][endian]")
{
    array<u32, 4> words = { 0x11'22'33'44, 0x55'66'77'88, 0x99'AA'BB'CC, 0xDD'EE'FF'00 };

    Md5 hasher(span{ words });
    REQUIRE(hasher.getHash() == md5Reference(as_bytes(span{ words })));
}
