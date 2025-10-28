/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/print.hpp
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
#pragma once

#include "kh/platform.hpp"

namespace KirHut
{

/*!
 * Declarative Field Template for any std::integral type of a requested \p bitSize.
 *
 * This is used by the BasicPackedTuple class to declare a requested bit field. The class does nothing but provide the
 * information given in its template arguments, with the first template argument \p Int_T being used
 *
 * \tparam Int_T A std::integral type that the underlying \p bitSize bits will be used to represent.
 * \tparam bitSize The number of binary digits (or "bits") available to represent the given \p Int_T.
 */
template <std::integral Int_T, size_t bitSize>
struct BitField
{
    /*!
     * Typedef of the \p Int_T type in this template type.
     */
    using Int = Int_T;

    /*!
     * Returns the number of bits used as \p bitSize.
     *
     * This is a consteval static method, guaranteeing that it is run at compile time. It simply returns \p bitSize.
     *
     * \return The \p bitSize parameter of this template type.
     */
    consteval static size_t bits() noexcept
    {
        return bitSize;
    }
};

/*!
 * A namespace containing convenience typedefs for different BitField types.
 *
 * Bit fields will usually be some kind of integer type, or a boolean, and this namespace contains basically every kind
 * of BitField type you would ever need to make. This library does not support float or double type bit fields yet, so
 * those are not included here either.
 */
namespace BF
{

using Bool = BitField<bool, 1>;

template <size_t bits>
using I8 = BitField<i8, bits>;

template <size_t bits>
using U8 = BitField<u8, bits>;

template <size_t bits>
using I16 = BitField<i16, bits>;

template <size_t bits>
using U16 = BitField<u16, bits>;

template <size_t bits>
using I32 = BitField<i32, bits>;

template <size_t bits>
using U32 = BitField<u32, bits>;

template <size_t bits>
using I64 = BitField<i64, bits>;

template <size_t bits>
using U64 = BitField<u64, bits>;

} // namespace BF

template <typename BitField_T>
concept BitFieldType = requires {
    typename BitField_T::Int;
    { BitField_T::bits() } noexcept -> std::same_as<size_t>;
} and BitField_T::bits() > 0 and std::is_integral_v<typename BitField_T::Int>;

template <BitFieldType BitField_T>
consteval int fieldDigits() noexcept
{
    if constexpr (std::same_as<typename BitField_T::Int, bool>)
    {
        return 1;
    }
    else
    {
        return BitField_T::bits() - (std::is_signed_v<typename BitField_T::Int> ? 1 : 0);
    }
}

template <std::unsigned_integral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
class BasicPackedTuple
{
    constexpr static size_t blocksNeeded = []() consteval {
        auto byteAmount = bytesNeededForBits(Field_T::bits() + (Field_Ts::bits() + ...));
        return byteAmount / sizeof(Block_T) + (byteAmount % sizeof(Block_T) ? 1 : 0);
    }();

    std::array<Block_T, blocksNeeded> data;

    template <size_t index, BitFieldType CurField_T, BitFieldType... RestField_Ts>
    struct _FieldTraits;

    struct FieldLocation
    {
        size_t blockIndex;
        size_t front;
        size_t back;
        Block_T frontMask;
        Block_T backMask;
    };

public:
    static_assert(not std::same_as<Block_T, bool>);
    static_assert(sizeof(Block_T) >= sizeof(typename Field_T::Int) and
                  ((sizeof(Block_T) >= sizeof(typename Field_Ts::Int)) and ...));

    template <size_t index>
    using FieldTraits = _FieldTraits<index, Field_T, Field_Ts...>;

    template <size_t index>
    using FieldType = FieldTraits<index>::type;

    template <size_t index>
    requires(index <= sizeof...(Field_Ts))
    constexpr FieldType<index> get() const noexcept
    {
        constexpr FieldLocation fieldInfo = getLocation<index>();

        Block_T frontBlock = (data.at(fieldInfo.blockIndex) & fieldInfo.frontMask) >> fieldInfo.front;
        if constexpr (fieldInfo.backMask)
        {
            frontBlock |= (fieldInfo.backMask & data.at(fieldInfo.blockIndex + 1)) << (blockBits() - fieldInfo.front);
        }

        if constexpr (std::is_signed_v<FieldType<index>>)
        {
            if constexpr (FieldTraits<index>::bits() == blockBits())
            {
                return std::bit_cast<FieldType<index>>(frontBlock);
            }

            constexpr Block_T signBit = static_cast<Block_T>(1) << (FieldTraits<index>::bits() - 1);
            if (frontBlock & signBit)
            {
                frontBlock |= std::numeric_limits<Block_T>::max() << FieldTraits<index>::bits();
            }

            return static_cast<FieldType<index>>(std::bit_cast<std::make_signed_t<Block_T>>(frontBlock));
        }
        else
        {
            return static_cast<FieldType<index>>(frontBlock);
        }
    }

    template <size_t index>
    requires(index <= sizeof...(Field_Ts))
    constexpr void set(FieldType<index> newVal)
    {
        constexpr FieldLocation fieldInfo = getLocation<index>();

        if (not FieldTraits<index>::canFit(newVal))
        {
            throw newVal;
        }

        Block_T &block = data.at(fieldInfo.blockIndex);
        if constexpr (std::same_as<decltype(newVal), bool>)
        {
            constexpr Block_T newBitMask = ~(1 << fieldInfo.front);

            Block_T newBit = static_cast<Block_T>(newVal) << fieldInfo.front;
            block          = (block & newBitMask) | newBit;
        }
        else
        {
            Block_T input = static_cast<Block_T>(std::bit_cast<std::make_unsigned_t<FieldType<index>>>(newVal));

            if constexpr (fieldInfo.backMask)
            {
                Block_T &backBlock = data.at(fieldInfo.blockIndex + 1);
                backBlock          = (backBlock & ~fieldInfo.backMask) | (input >> (blockBits() - fieldInfo.front));
            }

            block = (block & ~fieldInfo.frontMask) | ((input << fieldInfo.front) & fieldInfo.frontMask);
        }
    }

private:
    template <size_t index>
    consteval static FieldLocation getLocation() noexcept
    {
        constexpr size_t loc       = FieldTraits<index>::bitLocation();
        constexpr size_t endLoc    = loc + FieldTraits<index>::bits();
        constexpr size_t maskShift = sizeof(Block_T) * Platform::bitsInByte - FieldTraits<index>::bits();

        constexpr auto temp = static_cast<int>(loc % blockBits() + FieldTraits<index>::bits()) -
                              static_cast<int>(sizeof(Block_T) * Platform::bitsInByte);
        constexpr size_t backMaskWidth = temp < 0 ? 0 : temp;

        return { loc / blockBits(),
                 loc % blockBits(),
                 endLoc % blockBits(),
                 std::numeric_limits<Block_T>::max() >> maskShift << (loc % blockBits()),
                 backMaskWidth ? std::numeric_limits<Block_T>::max() >> (blockBits() - backMaskWidth) : 0 };
    }

    template <size_t index, BitFieldType CurField_T, BitFieldType... RestField_Ts>
    struct _FieldTraits
    {
        using type = std::conditional_t<index == 0,
                                        typename CurField_T::Int,
                                        typename _FieldTraits<index - 1, RestField_Ts...>::type>;

        consteval static size_t bitLocation() noexcept
        {
            return index == 0 ? 0 : CurField_T::bits() + _FieldTraits<index - 1, RestField_Ts...>::bitLocation();
        }

        consteval static size_t bits() noexcept
        {
            return index == 0 ? CurField_T::bits() : _FieldTraits<index - 1, RestField_Ts...>::bits();
        }

        consteval static int digits() noexcept
        {
            return index == 0 ? fieldDigits<CurField_T>() : _FieldTraits<index - 1, RestField_Ts...>::digits();
        }

        constexpr static bool canFit(std::same_as<type> auto toTest) noexcept
        {
            // canFit has to be a BS template to satisfy MSVC otherwise it tries to instantiate make_unsigned_t<bool>.
            if constexpr (not std::same_as<decltype(toTest), bool>)
            {
                auto bitsToTest = std::bit_cast<std::make_unsigned_t<type>>(toTest < 0 ? ~toTest : toTest);
                return std::bit_width(bitsToTest) <= digits();
            }

            return true;
        }
    };

    template <size_t index, BitFieldType CurField_T>
    struct _FieldTraits<index, CurField_T>
    {
        using type = CurField_T::Int;

        consteval static size_t bitLocation() noexcept
        {
            return 0;
        }

        consteval static size_t bits() noexcept
        {
            return CurField_T::bits();
        }

        consteval static size_t digits() noexcept
        {
            return fieldDigits<CurField_T>();
        }

        constexpr static bool canFit(std::same_as<type> auto toTest) noexcept
        {
            // canFit has to be a BS template to satisfy MSVC otherwise it tries to instantiate make_unsigned_t<bool>.
            if constexpr (not std::same_as<decltype(toTest), bool>)
            {
                auto bitsToTest = std::bit_cast<std::make_unsigned_t<type>>(toTest < 0 ? ~toTest : toTest);
                return std::bit_width(bitsToTest) <= digits();
            }

            return true;
        }
    };

    consteval static size_t blockBits() noexcept
    {
        return sizeof(Block_T) * Platform::bitsInByte;
    }
};

template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple8 = BasicPackedTuple<u8, Field_T, Field_Ts...>;

template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple16 = BasicPackedTuple<u16, Field_T, Field_Ts...>;

template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple32 = BasicPackedTuple<u32, Field_T, Field_Ts...>;

template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple64 = BasicPackedTuple<u64, Field_T, Field_Ts...>;

template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple = PackedTuple64<Field_T, Field_Ts...>;

} // namespace KirHut
