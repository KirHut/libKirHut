/***********************************************************************************************************************
** The KirHut Application Development Library
** md5hash.cpp
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

#include "kh/ranges.hpp"

#include <cstring>
#include <algorithm>

// Just don't build if there aren't the supported data types.
static_assert(::KirHut::Platform::u3264Are3264Bits,
              "The libKirHut MD5 Hash implementation requires unsigned 32, and 64 bit integers.\n"
              "The target platform does not support the MD5 implementation, if your software does "
              "not require MD5, simply set the KH_INCLUDE_MD5HASH CMake option to off in your build.");

namespace KirHut
{

namespace R = Ranges;

struct v1::Hash::Md5::Impl
{
    constexpr static u32 BLOCK_U32        = BLOCK_SIZE / sizeof(u32);
    constexpr static size_t SIZE_LOCATION = BLOCK_SIZE - sizeof(u64);

    using ByteBlockSpan = span<byte const, BLOCK_SIZE>;
    using UIntBlockSpan = span<u32 const, BLOCK_U32>;

    constexpr static u32 coreMd5Operation(u32 a, u32 b, u32 mod, UIntBlockSpan sourceSpan, int index) noexcept;
    template <int rowIndex>
    constexpr static void rowOfOperations(Md5 &self, auto f, UIntBlockSpan sourceSpan) noexcept;
    template <int roundIndex>
    constexpr static KH_FORCEINLINE void roundOfOperations(Md5 &self, auto f, UIntBlockSpan sourceSpan) noexcept;

    constexpr static void processBlock(Md5 &self, ByteBlockSpan blockSpan) noexcept;
    constexpr static void processInput(Md5 &self, span<byte const> input) noexcept;

    constexpr static void finish(Md5 &self) noexcept;
    constexpr static size_t provideInput(Md5 &self, span<byte const> input) noexcept;
    constexpr static void getMd5(Md5 &self, Md5SumSpan output) noexcept;
};

// clang-format off

// Per RFC1321:
// This step uses a 64-element table T[1 ... 64] constructed from the sine function. Let T[i] denote the i-th element of
// the table, which is equal to the integer part of 4294967296 times abs(sin(i)), where i is in radians.
//
// Each block of values in the table below is identical to those given in the Appendix A reference implementation of the
// MD5 hash algorithm found in RFC1321. This uses u32 to guarantee that the value is 32 and only 32 bits in length.
constexpr array<u32, 64> sineFunctionTableT =
{
    // Sine function values used in Round 1 (F)
    0xd7'6a'a4'78, 0xe8'c7'b7'56, 0x24'20'70'db, 0xc1'bd'ce'ee,
    0xf5'7c'0f'af, 0x47'87'c6'2a, 0xa8'30'46'13, 0xfd'46'95'01,
    0x69'80'98'd8, 0x8b'44'f7'af, 0xff'ff'5b'b1, 0x89'5c'd7'be,
    0x6b'90'11'22, 0xfd'98'71'93, 0xa6'79'43'8e, 0x49'b4'08'21,

    // Sine function values used in Round 2 (G)
    0xf6'1e'25'62, 0xc0'40'b3'40, 0x26'5e'5a'51, 0xe9'b6'c7'aa,
    0xd6'2f'10'5d, 0x02'44'14'53, 0xd8'a1'e6'81, 0xe7'd3'fb'c8,
    0x21'e1'cd'e6, 0xc3'37'07'd6, 0xf4'd5'0d'87, 0x45'5a'14'ed,
    0xa9'e3'e9'05, 0xfc'ef'a3'f8, 0x67'6f'02'd9, 0x8d'2a'4c'8a,

    // Sine function values used in Round 3 (H)
    0xff'fa'39'42, 0x87'71'f6'81, 0x6d'9d'61'22, 0xfd'e5'38'0c,
    0xa4'be'ea'44, 0x4b'de'cf'a9, 0xf6'bb'4b'60, 0xbe'bf'bc'70,
    0x28'9b'7e'c6, 0xea'a1'27'fa, 0xd4'ef'30'85, 0x04'88'1d'05,
    0xd9'd4'd0'39, 0xe6'db'99'e5, 0x1f'a2'7c'f8, 0xc4'ac'56'65,

    // Sine function values used in Round 4 (I)
    0xf4'29'22'44, 0x43'2a'ff'97, 0xab'94'23'a7, 0xfc'93'a0'39,
    0x65'5b'59'c3, 0x8f'0c'cc'92, 0xff'ef'f4'7d, 0x85'84'5d'd1,
    0x6f'a8'7e'4f, 0xfe'2c'e6'e0, 0xa3'01'43'14, 0x4e'08'11'a1,
    0xf7'53'7e'82, 0xbd'3a'f2'35, 0x2a'd7'd2'bb, 0xeb'86'd3'91,
};

// This is a table of rotations that reuses the index value of T above to derive the left rotation value for the core
// MD5 operation. This uses the same four fixed ascending values four times in a row per round, with a different set of
// four shift amounts per round. The values here are provided directly by RFC1321, and appear to otherwise be arbitrary.
// This is a constant array rather than a function for the sake of simplicity.
constexpr array<int, 64> rotationsTable =
{
    // Bitwise left rotation values used in Round 1 (F)
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,

    // Bitwise left rotation values used in Round 2 (G)
    5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,

    // Bitwise left rotation values used in Round 3 (H)
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,

    // Bitwise left rotation values used in Round 4 (I)
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
};

// This is a table of k values, which are the integers [0..16) in a fixed rotation that changes in each round. This
// table is also reusing the index value of T above to derive the appropriate k value. Each k value should appear once
// and only once in each round, and is used as a source index of the u32 value found in a given block being processed by
// the MD5 Hash function when the input block is read as a sequence of 16 u32 integers. This is a constant array rather
// than a function for the sake of simplicity.
constexpr array<int, 64> kValueTable =
{
    // k values used in Round 1 (F)
    0, 1, 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,

    // k values used in Round 2 (G)
    1, 6, 11, 0,  5,  10, 15, 4,  9,  14, 3,  8,  13, 2,  7,  12,

    // k values used in Round 3 (H)
    5, 8, 11, 14, 1,  4,  7,  10, 13, 0,  3,  6,  9,  12, 15, 2,

    // k values used in Round 4 (I)
    0, 7, 14, 5,  12, 3,  10, 1,  8,  15, 6,  13, 4,  11, 2,  9,
};
// clang-format on

// Per RFC1321:
// Let [abcd k s i] denote the operation `a = b + ((a + [FGHI](b,c,d) + X[k] + T[i]) <<< s)`.
//
// The value of mod is the result of F, G, H, or I applied to (b,c,d). This function accept the arguments in the
// function above like this: `return b + ((a + mod + sourceSpan[k] + T[index]) <<< s`.
// The values for k are found in kValueTable, values for s are in rotationsTable, and T is the sineFunctionTableT
// constant array.
constexpr u32 v1::Hash::Md5::Impl::coreMd5Operation(u32 a, u32 b, u32 mod, UIntBlockSpan sourceSpan, int index) noexcept
{
    a += mod;
    a += sourceSpan[kValueTable[index]];
    a += sineFunctionTableT[index];
    a = std::rotl(a, rotationsTable[index]);
    return a + b;
}

// This function represents a single row of operations as found in the MD5 Hash algorithm definition.
//
// [ABCD  k  s  i]  [DABC  k  s  i]  [CDAB  k  s  i]  [BCDA  k  s  i]
//
// Where k and s are fixed values given in the kValueTable and rotationsTable above, and i is the index value of the
// operation going [0..64). This function gets A, B, C, and D from self, and the provided auxiliaryFunc is one of the
// auxiliary functions F, G, H, or I.
template <int rowIndex>
constexpr void v1::Hash::Md5::Impl::rowOfOperations(Md5 &self, auto auxiliaryFunc, UIntBlockSpan sourceSpan) noexcept
{
    constexpr int opIndexMod = rowIndex * 4;
    self.A = coreMd5Operation(self.A, self.B, auxiliaryFunc(self.B, self.C, self.D), sourceSpan, opIndexMod + 0);
    self.D = coreMd5Operation(self.D, self.A, auxiliaryFunc(self.A, self.B, self.C), sourceSpan, opIndexMod + 1);
    self.C = coreMd5Operation(self.C, self.D, auxiliaryFunc(self.D, self.A, self.B), sourceSpan, opIndexMod + 2);
    self.B = coreMd5Operation(self.B, self.C, auxiliaryFunc(self.C, self.D, self.A), sourceSpan, opIndexMod + 3);
}

// This function represents a full round of 16 operations, so four full rows of operations. As an example, this is the
// first round of operations per RFC1321:
//
// Let [abcd k s i] denote the operation
//   a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s).
// Do the following 16 operations.
// [ABCD  0  7  1]  [DABC  1 12  2]  [CDAB  2 17  3]  [BCDA  3 22  4]
// [ABCD  4  7  5]  [DABC  5 12  6]  [CDAB  6 17  7]  [BCDA  7 22  8]
// [ABCD  8  7  9]  [DABC  9 12 10]  [CDAB 10 17 11]  [BCDA 11 22 12]
// [ABCD 12  7 13]  [DABC 13 12 14]  [CDAB 14 17 15]  [BCDA 15 22 16]
//
// Four of these rounds are performed for each block that is processed for the MD5 sum. Each block is provided a
// different roundIndex (0, 1, 2, or 3) and auxiliaryFunc (F, G, H, or I), and should all get the same self and
// sourceSpan. This function is declared with forced inline and has the flatten attribute, as it is the core operation
// where all of the efficiency is required.
template <int roundIndex>
[[KH_ATTR_FLATTEN]] constexpr KH_FORCEINLINE void
v1::Hash::Md5::Impl::roundOfOperations(Md5 &self, auto auxiliaryFunc, UIntBlockSpan sourceSpan) noexcept
{
    constexpr int rowIndexMod = roundIndex * 4;
    rowOfOperations<rowIndexMod + 0>(self, auxiliaryFunc, sourceSpan);
    rowOfOperations<rowIndexMod + 1>(self, auxiliaryFunc, sourceSpan);
    rowOfOperations<rowIndexMod + 2>(self, auxiliaryFunc, sourceSpan);
    rowOfOperations<rowIndexMod + 3>(self, auxiliaryFunc, sourceSpan);
}

constexpr void v1::Hash::Md5::Impl::processBlock(Md5 &self, ByteBlockSpan blockSpan) noexcept
{
    // Per RFC1321:
    // We first define four auxiliary functions that each take as input
    // three 32-bit words and produce as output one 32-bit word.
    //
    //   F(X,Y,Z) = XY v not(X) Z
    //   G(X,Y,Z) = XZ v Y not(Z)
    //   H(X,Y,Z) = X xor Y xor Z
    //   I(X,Y,Z) = Y xor (X v not(Z))
    constexpr auto auxiliaryFunctionF = [](u32 b, u32 c, u32 d) constexpr -> u32 { return (b & c) | (~b & d); };
    constexpr auto auxiliaryFunctionG = [](u32 b, u32 c, u32 d) constexpr -> u32 { return (b & d) | (c & ~d); };
    constexpr auto auxiliaryFunctionH = [](u32 b, u32 c, u32 d) constexpr -> u32 { return b ^ c ^ d; };
    constexpr auto auxiliaryFunctionI = [](u32 b, u32 c, u32 d) constexpr -> u32 { return c ^ (b | ~d); };

    // Per RFC1321:
    // Save A as AA, B as BB, C as CC, and D as DD.
    u32 const AA = self.A, BB = self.B, CC = self.C, DD = self.D;

    array<u32, BLOCK_U32> u32Block;
    for (u32 i = 0; i < BLOCK_U32; ++i)
    {
        u32Block[i] = fromLittleEndian<u32>(blockSpan.subspan(i * sizeof(u32), sizeof(u32)));
    }

    auto sourceBlockSpan = UIntBlockSpan{ u32Block };
    roundOfOperations<0>(self, auxiliaryFunctionF, sourceBlockSpan);
    roundOfOperations<1>(self, auxiliaryFunctionG, sourceBlockSpan);
    roundOfOperations<2>(self, auxiliaryFunctionH, sourceBlockSpan);
    roundOfOperations<3>(self, auxiliaryFunctionI, sourceBlockSpan);

    self.A += AA;
    self.B += BB;
    self.C += CC;
    self.D += DD;
}

// This function takes arbitrary amounts of input bytes, marshalls the input into a buffer until enough data is passed,
// then processes the block. For large input, it will continue to process blocks directly in input without buffering
// until there are no more full blocks of input, then will buffer any remaining bytes until the next processInput call
// or the Md5Hash is finished. If input is empty, this function will do nothing.
constexpr void v1::Hash::Md5::Impl::processInput(Md5 &self, span<byte const> input) noexcept
{
    self.totalInput += input.size_bytes();

    if (self.bufferPos + input.size_bytes() >= BLOCK_SIZE)
    {
        size_t const frontChop = BLOCK_SIZE - self.bufferPos;
        // std::ranges::copy is substituted with memcpy() when it is possible to do so, meaning this should be as
        // efficient as a memcpy() would be in non-constexpr contexts.
        R::copy(input.subspan(0, frontChop), R::next(self.buffer.begin(), self.bufferPos));
        processBlock(self, self.buffer);
        input          = input.subspan(frontChop);
        self.bufferPos = 0;

        auto const numFullBlocks = input.size_bytes() / BLOCK_SIZE;
        for (size_t fullBlockNum = 0; fullBlockNum < numFullBlocks; ++fullBlockNum)
        {
            // Don't use subspan since we need a fixed-size span for this operation.
            processBlock(self, ByteBlockSpan{ R::next(input.data(), fullBlockNum * BLOCK_SIZE), BLOCK_SIZE });
        }

        input = input.subspan(numFullBlocks * BLOCK_SIZE);
    }

    self.bufferPos = R::copy(input, self.buffer.begin() + self.bufferPos).out - self.buffer.begin();
}

constexpr void v1::Hash::Md5::Impl::finish(Md5 &self) noexcept
{
    auto bufIter = R::next(self.buffer.begin(), self.bufferPos);
    auto sizeLoc = R::prev(self.buffer.end(), sizeof(u64));

    // Regardless of how many bytes are in the buffer, the MD5 specification requires adding at least one buffer bit as
    // a 1, which effectively means adding at least one byte as a padding byte. Per RFC1321:
    // The message is "padded" (extended) so that its length (in bits) is congruent to 448, modulo 512. That is, the
    // message is extended so that it is just 64 bits shy of being a multiple of 512 bits long. Padding is always
    // performed, even if the length of the message is already congruent to 448, modulo 512.
    //
    // Padding is performed as follows: a single "1" bit is appended to the message, and then "0" bits are appended so
    // that the length in bits of the padded message becomes congruent to 448, modulo 512. In all, at least one bit and
    // at most 512 bits are appended.
    constexpr byte PADDING_FRONT = static_cast<byte>(0x80);
    *bufIter++                   = PADDING_FRONT;

    if (bufIter > sizeLoc)
    {
        R::fill(bufIter, self.buffer.end(), byte(0));
        processBlock(self, self.buffer);
        bufIter = self.buffer.begin();
    }

    R::fill(bufIter, sizeLoc, byte(0));

    // This is correct even if bitLength overflows. Per RFC1321:
    // A 64-bit representation of b (the length of the message before the padding bits were added) is appended to the
    // result of the previous step. In the unlikely event that b is greater than 2^64, then only the low-order 64 bits
    // of b are used. (These bits are appended as two 32-bit words and appended low-order word first in accordance with
    // the previous conventions.)
    u64 const bitLength = self.totalInput * Platform::bitsInByte;

    toLittleEndian(bitLength, sizeLoc);
    processBlock(self, self.buffer);

    self.finished = true;
}

void v1::Hash::Md5::finish() noexcept
{
    if (not finished)
    {
        Impl::finish(*this);
    }
}

// clang-format off
constexpr static u32 accumInitA = 0x67452301,
                     accumInitB = 0xefcdab89,
                     accumInitC = 0x98badcfe,
                     accumInitD = 0x10325476;
// clang-format on

// The accumulators are initialized here instead of the class object definition to keep this information out of the
// header. This ensures that the implementation remains in the source rather than bleeding into the header document.
v1::Hash::Md5::Md5() noexcept : A(accumInitA), B(accumInitB), C(accumInitC), D(accumInitD)
{
    // No further implementation.
}

v1::Hash::Md5::Md5(span<byte const> input) noexcept : Md5()
{
    if (input.size() > 0)
    {
        Impl::processInput(*this, input);
    }
}

constexpr void v1::Hash::Md5::Impl::getMd5(Md5 &self, Md5SumSpan output) noexcept
{
    if (not self.finished)
    {
        Impl::finish(self);
    }

    size_t outLoc = 0;
    for (u32 out : array{ self.A, self.B, self.C, self.D })
    {
        toLittleEndian(out, output.subspan(outLoc++ * sizeof(u32), sizeof(u32)));
    }
}

void v1::Hash::Md5::getHash(Md5SumSpan output) noexcept
{
    return Impl::getMd5(*this, output);
}

constexpr size_t v1::Hash::Md5::Impl::provideInput(Md5 &self, span<byte const> input) noexcept
{
    // Every part of the Md5Hash object is designed to use this method to provide data to the class. This method,
    // however, just delegates the job to a separate method after some basic sanity checks.

    if (not self.finished and input.size_bytes() != 0)
    {
        processInput(self, input);

        return input.size_bytes();
    }

    return 0;
}

size_t v1::Hash::Md5::provideInput(span<byte const> input) noexcept
{
    return Impl::provideInput(*this, input);
}

// Compile-time testing:
consteval bool rotationsCheck() noexcept
{
    // clang-format off
    constexpr auto expectedRotationValues = array {
        array { 7, 12, 17, 22, },
        array { 5, 9, 14, 20, },
        array { 4, 11, 16, 23, },
        array { 6, 10, 15, 21, },
    };
    // clang-format on

    auto testFunction = [&expectedRotationValues](int index) -> bool {
        return rotationsTable[index] == expectedRotationValues[(index & 0b110000) >> 4][index & 0b11];
    };

    return R::all_of(Views::iota(0, 64), testFunction);
}

static_assert(rotationsCheck(), "The rotation values in MD5 Hash implementation are incorrect.");

consteval bool kValuesCheck() noexcept
{
    std::array<bool, 16> foundKs{};
    for (int i = 0; i < 4; ++i)
    {
        for (int pos = 0; pos < 16; ++pos)
        {
            if (int k = kValueTable[i * 16 + pos]; k >= 0 and k < 16 and not foundKs[k])
            {
                foundKs[k] = true;
            }
            else
            {
                return false;
            }
        }

        R::fill(foundKs, false);
    }

    return true;
}

static_assert(kValuesCheck(), "The k values in MD5 Hash implementation are incorrect.");

} // namespace KirHut
