/***********************************************************************************************************************
** The KirHut Application Development Library
** md5hash.cpp
** Copyright (C) 2024 KirHut Software Company
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
#include "kh/md5hash.hpp"

#include <cstring>
#include <algorithm>

using namespace KirHut;

using std::fill;

constexpr byte PADDING_FRONT = static_cast<byte>(0b1 << (BYTE_BITS - 1));

// clang-format off
constexpr static u32 ACCUM_A_INIT = 0x67452301,
                     ACCUM_B_INIT = 0xefcdab89,
                     ACCUM_C_INIT = 0x98badcfe,
                     ACCUM_D_INIT = 0x10325476;

constexpr array<array<u32, 16>, 4> V{{
    {{ 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 }},
    {{ 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a }},
    {{ 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 }},
    {{ 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 }}
}};

constexpr array<array<u32, 16>, 4> S{{
    {{ 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22 }},
    {{ 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20 }},
    {{ 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23 }},
    {{ 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 }}
}};

constexpr array<array<u32, 16>, 4> X{{
    {{ 0, 1, 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15 }},
    {{ 1, 6, 11, 0,  5,  10, 15, 4,  9,  14, 3,  8,  13, 2,  7,  12 }},
    {{ 5, 8, 11, 14, 1,  4,  7,  10, 13, 0,  3,  6,  9,  12, 15, 2 }},
    {{ 0, 7, 14, 5,  12, 3,  10, 1,  8,  15, 6,  13, 4,  11, 2,  9 }}
}};
// clang-format on

class Md5Hash::Impl
{
public:
    template <ue32 uPos>
    constexpr static inline u32 cycleBits(u32 a, u32 b, u32 mod, u32 const *data, ue32 counter) noexcept
    {
        u32 value = a + mod;
        value += data[X[uPos][counter]];
        value += V[uPos][counter];
        value = (value << S[uPos][counter]) | (value >> (U32_BITS - S[uPos][counter]));
        return value + b;
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

    // Marking this function as KH_INLINE is essential for retaining maximum speed on MSVC.
    template <ue32 VAL, typename Func>
    constexpr static KH_INLINE void unitChunk(Func f, Md5Hash &self, u32 const *data) noexcept
    {
        self.A = f(self.A, self.B, self.C, self.D, data, (VAL * 4) + 0);
        self.D = f(self.D, self.A, self.B, self.C, data, (VAL * 4) + 1);
        self.C = f(self.C, self.D, self.A, self.B, data, (VAL * 4) + 2);
        self.B = f(self.B, self.C, self.D, self.A, data, (VAL * 4) + 3);
    }

    // Marking this function as KH_INLINE is essential for retaining maximum speed on MSVC.
    template <typename Func>
    constexpr static KH_INLINE void doUnit(Func f, Md5Hash &self, u32 const *data) noexcept
    {
        unitChunk<0>(f, self, data);
        unitChunk<1>(f, self, data);
        unitChunk<2>(f, self, data);
        unitChunk<3>(f, self, data);
    }

    static void processBlock(Md5Hash &self, byte const *blockLoc) noexcept
    {
        u32 A = self.A, B = self.B, C = self.C, D = self.D;

        u32 block[BLOCK_U32];
        memcpy(block, blockLoc, BLOCK_SIZE);

        // Here it is, this is where the hashing magic happens.
        // Each call to "doUnit" should be inlined and generates four "chunks" of MD5 hash cycling code. Each chunk is
        // a single iteration of the assignment loop:
        //  A = f(A, B, C, D, data, counter);
        //  D = f(D, A, B, C, data, counter);
        //  C = f(C, D, A, B, data, counter);
        //  B = f(B, C, D, A, data, counter);
        // The above pseudocode is described in RFC1321, which is recommended that you are familiar with:
        //  https://www.rfc-editor.org/rfc/rfc1321
        //
        // Each chunk has a different value VAL passed in that expands to the appropriate counter value 0 through 15.
        // This, along with another uPos value of each Unit type, determines the correct values in the constexpr arrays
        // holding the data for cycleBits(). This allows the compiler to optimize all of this down to directly using
        // the data value in the generated code, the same as hand writing it all in C. The "UnitF", G, H, and I lambdas
        // represent the different bit manipulation operations referred to in RFC1321 and found commonly as a
        // preprocessor define in C implementations of MD5. As constexpr lambdas, the compiler can omit the object
        // creation entirely and just inline these functions directly in the generated doUnit code.
        doUnit(UnitF, self, block);
        doUnit(UnitG, self, block);
        doUnit(UnitH, self, block);
        doUnit(UnitI, self, block);

        self.A += A;
        self.B += B;
        self.C += C;
        self.D += D;
    }

    static inline void processInput(Md5Hash &self, size_t length, byte const *input) noexcept
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
        size_t numFullBlocks = length / BLOCK_SIZE + 1;
        self.bufferPos       = length % BLOCK_SIZE;

        for (size_t fullBlockNum = 1; fullBlockNum < numFullBlocks; ++fullBlockNum)
        {
            processBlock(self, &input[fullBlockNum * BLOCK_SIZE]);
        }

        memcpy(self.buffer, &input[numFullBlocks * BLOCK_SIZE], self.bufferPos);
    }

    static inline void finish(Md5Hash &self) noexcept
    {
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

        u64 bitLength = self.totalInput * BYTE_BITS;

        memcpy(&self.buffer[SIZE_LOCATION], &bitLength, bytesNeededForBits(64));
        processBlock(self, self.buffer);

        self.finished = true;
    }

    constexpr static u32 BLOCK_U32        = BLOCK_SIZE / sizeof(u32);
    constexpr static size_t SIZE_LOCATION = BLOCK_SIZE - sizeof(u64);
};

Md5Hash::Md5Hash() noexcept : A(ACCUM_A_INIT), B(ACCUM_B_INIT), C(ACCUM_C_INIT), D(ACCUM_D_INIT)
{
}

Md5Hash::Md5Hash(size_t length, byte const *data) noexcept : Md5Hash()
{
    Impl::processInput(*this, length, data);
}

Md5Sum Md5Hash::getMd5() noexcept
{
    Md5Sum sum;
    getMd5(sum.data());
    return sum;
}

void Md5Hash::getMd5(byte *output) noexcept
{
    if (!finished)
    {
        Impl::finish(*this);
    }

    // This doesn't work on anything other than processors that have 8 bit bytes. I'd like to support arbitrary bit
    // width bytes, but I'm not even close to ready to do that.
    array sigNums{ A, B, C, D };
    memcpy(output, sigNums.data(), MD5SUM_RETURN_SIZE);
}

size_t Md5Hash::provideInput(size_t length, byte const *data) noexcept
{
    if (!finished)
    {
        Impl::processInput(*this, length, data);

        return length;
    }

    return 0;
}
