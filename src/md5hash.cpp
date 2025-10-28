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

#include <cstring>
#include <algorithm>

namespace KirHut
{

constexpr byte PADDING_FRONT = static_cast<byte>(0b1 << (Platform::bitsInByte - 1));

// clang-format off
constexpr static u32 ACCUM_A_INIT = 0x67452301,
                     ACCUM_B_INIT = 0xefcdab89,
                     ACCUM_C_INIT = 0x98badcfe,
                     ACCUM_D_INIT = 0x10325476;

constexpr array<u32, 64> V =
{
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

constexpr array<int, 64> S =
{
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

constexpr array<ue32, 64> X =
{
    0, 1, 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
    1, 6, 11, 0,  5,  10, 15, 4,  9,  14, 3,  8,  13, 2,  7,  12,
    5, 8, 11, 14, 1,  4,  7,  10, 13, 0,  3,  6,  9,  12, 15, 2,
    0, 7, 14, 5,  12, 3,  10, 1,  8,  15, 6,  13, 4,  11, 2,  9
};
// clang-format on

// Just don't build if there aren't the supported data types.
static_assert(Platform::u3264Are3264Bits,
              "The libKirHut MD5 Hash implementation requires unsigned 32, and 64 bit integers.\n"
              "The target platform does not support the MD5 implementation, if your software does "
              "not require MD5, simply remove the KH_MD5_SUPPORT definition in your build.");

struct Md5Hash::Impl
{
    template <ue32 uPos>
    constexpr static u32 cycleBits(u32 a, u32 b, u32 mod, u32 const *data, ue32 counter) noexcept
    {
        const ue32 pos = uPos * 16 + counter;
        u32 value = a + mod;
        value += data[X[pos]];
        value += V[pos];
        return std::rotl(value, S[pos]) + b;
    }

    constexpr static auto UnitF = [](u32 a, u32 b, u32 c, u32 d, u32 const *data, ue32 counter) noexcept {
        return cycleBits<0>(a, b, (b & c) | (~b & d), data, counter);
    };

    constexpr static auto UnitG = [](u32 a, u32 b, u32 c, u32 d, u32 const *data, ue32 counter) noexcept {
        return cycleBits<1>(a, b, (b & d) | (c & ~d), data, counter);
    };

    constexpr static auto UnitH = [](u32 a, u32 b, u32 c, u32 d, u32 const *data, ue32 counter) noexcept {
        return cycleBits<2>(a, b, b ^ c ^ d, data, counter);
    };

    constexpr static auto UnitI = [](u32 a, u32 b, u32 c, u32 d, u32 const *data, ue32 counter) noexcept {
        return cycleBits<3>(a, b, c ^ (b | ~d), data, counter);
    };

    template <ue32 VAL>
    constexpr static void unitChunk(auto f, Md5Hash &self, u32 const *data) noexcept
    {
        self.A = f(self.A, self.B, self.C, self.D, data, (VAL * 4) + 0);
        self.D = f(self.D, self.A, self.B, self.C, data, (VAL * 4) + 1);
        self.C = f(self.C, self.D, self.A, self.B, data, (VAL * 4) + 2);
        self.B = f(self.B, self.C, self.D, self.A, data, (VAL * 4) + 3);
    }

    // Marking this function as KH_FORCEINLINE is essential for retaining maximum speed on MSVC.
    [[KH_ATTR_FLATTEN]] constexpr static KH_FORCEINLINE void unitRound(auto f, Md5Hash &self, u32 const *data) noexcept
    {
        unitChunk<0>(f, self, data);
        unitChunk<1>(f, self, data);
        unitChunk<2>(f, self, data);
        unitChunk<3>(f, self, data);
    }

    inline static void processBlock(Md5Hash &self, byte const *blockLoc) noexcept
    {
        u32 A = self.A, B = self.B, C = self.C, D = self.D;

        u32 block[BLOCK_U32];
        for (u32 i = 0; i < BLOCK_U32; ++i)
        {
            block[i] = fromLittleEndian<u32>(&blockLoc[i * sizeof(u32)]);
        }

        unitRound(UnitF, self, block);
        unitRound(UnitG, self, block);
        unitRound(UnitH, self, block);
        unitRound(UnitI, self, block);

        self.A += A;
        self.B += B;
        self.C += C;
        self.D += D;
    }

    inline static void processInput(Md5Hash &self, size_t length, byte const *input) noexcept
    {
        self.totalInput += length;

        if (self.bufferPos + length < BLOCK_SIZE)
        {
            memcpy(&self.buffer[self.bufferPos], input, length);
            self.bufferPos += length;
            return;
        }

        size_t frontChop = BLOCK_SIZE - self.bufferPos;

        memcpy(&self.buffer[self.bufferPos], input, frontChop);
        processBlock(self, self.buffer);
        length -= frontChop;

        // Given the length, it is possible to precalculate the number of blocks needed to process, and to know when to
        // copy data into the buffer as opposed to simply passing a location from the user input.
        size_t numFullBlocks = length / BLOCK_SIZE;
        self.bufferPos       = length % BLOCK_SIZE;

        for (size_t fullBlockNum = 0; fullBlockNum < numFullBlocks; ++fullBlockNum)
        {
            processBlock(self, &input[frontChop + fullBlockNum * BLOCK_SIZE]);
        }

        memcpy(self.buffer, &input[frontChop + numFullBlocks * BLOCK_SIZE], self.bufferPos);
    }

    inline static void finish(Md5Hash &self) noexcept
    {
        using std::fill;

        self.buffer[self.bufferPos++] = PADDING_FRONT;

        // This does include 56 because you are required to add at least one bit, so if the bytes remaining has exactly
        // enough space for an 8 byte block then you still need to use a second full buffer. The MD5 specification
        // requires that at least 1 bit is appended to the end of the input (effectively at least one byte), and an
        // additional number of bits necessary to fill at least 64 bits of additional data up to a multiple of 512.
        if (self.bufferPos > SIZE_LOCATION)
        {
            fill(&self.buffer[self.bufferPos], &self.buffer[BLOCK_SIZE], byte(0));
            processBlock(self, self.buffer);
            fill(self.buffer, &self.buffer[SIZE_LOCATION], byte(0));
        }
        else
        {
            fill(&self.buffer[self.bufferPos], &self.buffer[SIZE_LOCATION], byte(0));
        }

        u64 bitLength = self.totalInput * Platform::bitsInByte;

        toLittleEndian(bitLength, &self.buffer[SIZE_LOCATION]);
        processBlock(self, self.buffer);

        self.finished = true;
    }

    constexpr static u32 BLOCK_U32        = BLOCK_SIZE / sizeof(u32);
    constexpr static size_t SIZE_LOCATION = BLOCK_SIZE - sizeof(u64);
};

// The accumulators are initialized here instead of the class object definition to keep this information out of the
// header. This ensures that the implementation remains in the source rather than bleeding into the header document.
Md5Hash::Md5Hash() noexcept : A(ACCUM_A_INIT), B(ACCUM_B_INIT), C(ACCUM_C_INIT), D(ACCUM_D_INIT)
{
    // No further implementation.
}

Md5Hash::Md5Hash(size_t length, byte const *data) noexcept : Md5Hash()
{
    if (length != 0 and data != nullptr)
    {
        Impl::processInput(*this, length, data);
    }
}

Md5Sum Md5Hash::getMd5() noexcept
{
    Md5Sum sum;
    getMd5(sum.data());
    return sum;
}

void Md5Hash::getMd5(byte *output) noexcept
{
    if (output == nullptr)
    {
        return;
    }

    if (not finished)
    {
        Impl::finish(*this);
    }

    size_t outLoc = 0;
    for (u32 out : array{ A, B, C, D })
    {
        toLittleEndian(out, &output[outLoc++ * sizeof(u32)]);
    }
}

size_t Md5Hash::provideInput(size_t length, byte const *data) noexcept
{
    // Every part of the Md5Hash object is designed to use this method to provide data to the class. This method,
    // however, just delegates the job to the implementation object after some basic sanity checks.

    if (not finished and length != 0 and data != nullptr)
    {
        Impl::processInput(*this, length, data);

        return length;
    }

    return 0;
}

} // namespace KirHut
