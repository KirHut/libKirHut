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

#include "Md51.hpp"
// #include "Md51.cpp"

using namespace KirHut;

template <size_t SIZE = std::dynamic_extent>
string getHex(span<byte, SIZE> biteSize)
{
    constexpr auto tform = [](char hexDigit) { return hexDigit < 10 ? hexDigit += '0' : hexDigit += 'a' - 10; };
    size_t bufSize = biteSize.size_bytes() * 2;
    string ret(bufSize, '\0');
    char *dsti = ret.data();
    for (byte b : biteSize)
    {
        *dsti++ = tform(static_cast<char>(b >> 4));
        *dsti++ = tform(static_cast<char>(b) & 0x0F);
    }

    return ret;
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
    STATIC_REQUIRE(sizeof(Md5Sum) == MD5SUM_RETURN_SIZE);
    Md5Sum sum;
    REQUIRE(sum.size() == MD5SUM_RETURN_SIZE);
}

TEST_CASE("Empty Md5Hash Object Construction State", "[md5hash][constructor]")
{
    Md5Hash hasher;
    string emptyMd5Str = "d41d8cd98f00b204e9800998ecf8427e";
    Md5Sum sum         = hasher.getMd5();
    REQUIRE(emptyMd5Str == getHex(span{ sum }));
}

TEST_CASE("Initialize Md5Hash With An Array of Bytes", "[md5hash][constructor]")
{
    Md5Hash hasher(SmallTestBlock);
    string blockMd5Str = "266c6efea54d63c04fb8083f40d441cc";
    Md5Sum sum         = hasher.getMd5();
    REQUIRE(blockMd5Str == getHex(span{ sum }));
}

TEST_CASE("Constructing With Different Types of Input Data", "[md5hash][constructor]")
{
    Md5Hash hasher(SmallTestBlock);
    array<byte, MD5SUM_RETURN_SIZE> md5OtherBuffer;
    md5::md5_t(SmallTestBlock.data(), SmallTestBlock.size(), md5OtherBuffer.data());
    REQUIRE(hasher.getMd5() == md5OtherBuffer);
    array<u64, 512> randLongs;
    std::mt19937_64 rng(currentTicks());
    std::generate(randLongs.begin(), randLongs.end(), rng);
    Md5Hash hasher2(span<u64, 512>{ randLongs });
    md5::md5_t(randLongs.data(), randLongs.size() * sizeof(u64), md5OtherBuffer.data());
    REQUIRE(hasher2.getMd5() == md5OtherBuffer);
}

TEST_CASE("Adding Bytes After Completing Md5Hash", "[md5hash]")
{
    Md5Hash hasher;
    string emptyMd5Str = "d41d8cd98f00b204e9800998ecf8427e";
    Md5Sum sum         = hasher.getMd5();
    REQUIRE(emptyMd5Str == getHex(span{ sum }));
    hasher.provideInput(span{ "Other invalid data that won't affect anything." });
    Md5Sum sum2 = hasher.getMd5();
    REQUIRE(emptyMd5Str == getHex(span{ sum2 }));
}

TEST_CASE("Providing Input of Different Types of Data", "[md5hash]")
{
    std::mt19937_64 rng(currentTicks());
    array<i64, 256> signedList;
    array<u32, 512> unsignedList;
    std::generate(signedList.begin(), signedList.end(), rng);
    std::generate(unsignedList.begin(), unsignedList.end(), [&rng] { return static_cast<u32>(rng()); });
    Md5Hash hasher, hasher2;
    array<byte, MD5SUM_RETURN_SIZE> md5OtherBuf;
    md5::md5_t(signedList.data(), signedList.size() * sizeof(i64), md5OtherBuf.data());
    hasher.provideInput(span{ signedList });
    REQUIRE(hasher.getMd5() == md5OtherBuf);
    md5::md5_t(unsignedList.data(), unsignedList.size() * sizeof(u32), md5OtherBuf.data());
    hasher2.provideInput(span{ unsignedList });
    REQUIRE(hasher2.getMd5() == md5OtherBuf);
}

TEST_CASE("Using The provideInput() Method to Input Data", "[md5hash]")
{
    std::string blockMd5Str = "266c6efea54d63c04fb8083f40d441cc";

    size_t divAmount = GENERATE(range(1u, 33u));

    Md5Hash hasher;

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

    Md5Sum sum = hasher.getMd5();

    REQUIRE(blockMd5Str == getHex(span{ sum }));
}

TEST_CASE("Taking MD5 Result Using Spans", "[md5hash]")
{
    Md5Hash hasher;
    string emptyMd5Str = "d41d8cd98f00b204e9800998ecf8427e";
    array<byte, 16> buffer; // Set this below 16 to test that hasher.getMd5() fails to compile with too small a span.
    auto bSpan = span{ buffer };
    hasher.getMd5(bSpan);
    REQUIRE(emptyMd5Str == getHex(bSpan));
}
