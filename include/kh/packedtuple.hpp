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

#include "kh/base.hpp"

namespace KirHut
{

/*!
 * Declarative Field Template for any std::integral type of a requested \p bitSize.
 *
 * This is used by the BasicPackedTuple class to declare a requested bit field. The class does nothing but provide the
 * information given in its template arguments, with the first template argument \p Int_T being used to determine the
 * return value of getting this BitField from a given BasicPackedTuple, and \p bitSize is used to report how many bits
 * you wish to use to represent the given BitField. The \p Int_T must be a signed or unsigned integer type, and the
 * \p bitSize must be at least 1 for unsigned values, and at least 2 for signed values, and may not exceed the number of
 * bits that the according \p Int_T type has itself.
 *
 * The given bitSize includes the signed bit for signed integers, so the bitSize values can be visually added up where
 * they are used to easily verify the PackedTuple should be no bigger than the expected number of bits.
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
     * Lowest integer value that can be placed into this BitField.
     *
     * For unsigned values, this just returns 0. Signed values return the lowest negative value that can be placed in
     * the given BitField.
     *
     * \return The lowest value that can be contained in this BitField.
     */
    consteval static Int_T min() noexcept
    {
        if constexpr (std::is_same_v<bool, Int_T> or std::is_unsigned_v<Int_T> or
                      bitSize == sizeof(Int_T) * Platform::bitsInByte)
        {
            return Limits<Int_T>::min();
        }
        else
        {
            return static_cast<Int_T>(-1) & ~max();
        }
    }

    /*!
     * Highest integer value that can be placed into this BitField.
     *
     * \return The highest value that can be placed in this BitField.
     */
    consteval static Int_T max() noexcept
    {
        if constexpr (std::is_same_v<bool, Int_T> or bitSize == sizeof(Int_T) * Platform::bitsInByte)
        {
            return Limits<Int_T>::max();
        }
        else
        {
            return (static_cast<Int_T>(1) << digits()) - 1;
        }
    }

    /*!
     * Returns the number of bits used as \p bitSize.
     *
     * This method simply returns \p bitSize.
     *
     * \return The \p bitSize parameter of this template type.
     */
    consteval static size_t bits() noexcept
    {
        return bitSize;
    }

    /*!
     * Returns the number of digits available in this BitField.
     *
     * This method returns \p bitSize when the given \p Int_T is unsigned, or one less than that for signed integers.
     *
     * \return The number of integer digits used in the given BitField.
     */
    consteval static size_t digits() noexcept
    {
        return bitSize - (std::is_signed_v<Int_T> ? 1 : 0);
    }

    static_assert(bitSize <= sizeof(Int_T) * Platform::bitsInByte, "The given bitSize is too large.");
    static_assert(bitSize > (std::is_signed_v<Int_T> ? 1 : 0), "The given bitSize is too small.");
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

/*!
 * BitField alias for the bool type.
 *
 * Bools always only take one bit to contain, so there is no reason to use more.
 */
using Bool = BitField<bool, 1>;

/*!
 * BitField alias for the i8 signed integer type.
 *
 * This type may be a maximum of 8 bits in length. The given \p bits includes the sign bit.
 *
 * \tparam bits The number of bits to use to represent the i8 value.
 */
template <size_t bits>
using I8 = BitField<i8, bits>;

/*!
 * BitField alias for the u8 signed integer type.
 *
 * This type may be a maximum of 8 bits in length.
 *
 * \tparam bits The number of bits to use to represent the u8 value.
 */
template <size_t bits>
using U8 = BitField<u8, bits>;

/*!
 * BitField alias for the i16 signed integer type.
 *
 * This type may be a maximum of 16 bits in length. The given \p bits includes the sign bit.
 *
 * \tparam bits The number of bits to use to represent the i16 value.
 */
template <size_t bits>
using I16 = BitField<i16, bits>;

/*!
 * BitField alias for the u16 signed integer type.
 *
 * This type may be a maximum of 16 bits in length.
 *
 * \tparam bits The number of bits to use to represent the u16 value.
 */
template <size_t bits>
using U16 = BitField<u16, bits>;

/*!
 * BitField alias for the i32 signed integer type.
 *
 * This type may be a maximum of 32 bits in length. The given \p bits includes the sign bit.
 *
 * \tparam bits The number of bits to use to represent the i32 value.
 */
template <size_t bits>
using I32 = BitField<i32, bits>;

/*!
 * BitField alias for the u32 signed integer type.
 *
 * This type may be a maximum of 32 bits in length.
 *
 * \tparam bits The number of bits to use to represent the u32 value.
 */
template <size_t bits>
using U32 = BitField<u32, bits>;

/*!
 * BitField alias for the i64 signed integer type.
 *
 * This type may be a maximum of 64 bits in length. The given \p bits includes the sign bit.
 *
 * \tparam bits The number of bits to use to represent the i64 value.
 */
template <size_t bits>
using I64 = BitField<i64, bits>;

/*!
 * BitField alias for the u64 signed integer type.
 *
 * This type may be a maximum of 64 bits in length.
 *
 * \tparam bits The number of bits to use to represent the u64 value.
 */
template <size_t bits>
using U64 = BitField<u64, bits>;

} // namespace BF

/*!
 *
 */
template <typename BitField_T>
concept BitFieldType = requires {
    typename BitField_T::Int;
    { BitField_T::bits() } noexcept -> std::same_as<size_t>;
} and BitField_T::bits() > 0 and std::is_integral_v<typename BitField_T::Int>;

/*!
 * \brief The BasicPackedTuple class
 */
template <std::unsigned_integral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
class BasicPackedTuple
{
    /*!
     * \internal
     *
     * The number of blocks required to hold the given set of BitFields in this BasicPackedTuple.
     *
     * This is initialized using a lambda, but this is all done at compile time so there should be zero runtime effect.
     */
    constexpr static size_t blocksNeeded = []() consteval {
        auto byteAmount = bytesNeededForBits(Field_T::bits() + (Field_Ts::bits() + ...));
        return byteAmount / sizeof(Block_T) + (byteAmount % sizeof(Block_T) ? 1 : 0);
    }();

    /*!
     * \internal
     *
     * The actual data storage array for this BasicPackedTuple.
     *
     * Here is where the data really lives! It should be initialized to all zeros upon construction of this class. This
     * is a requirement to avoid UB while using this class.
     */
    std::array<Block_T, blocksNeeded> data{};

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
            constexpr Block_T newBitMask = ~(static_cast<Block_T>(1) << fieldInfo.front);

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
        constexpr size_t maskShift = blockBits() - FieldTraits<index>::bits();
        constexpr size_t blockLoc  = loc % blockBits();

        constexpr auto temp = static_cast<int>(blockLoc + FieldTraits<index>::bits()) -
                              static_cast<int>(sizeof(Block_T) * Platform::bitsInByte);
        constexpr size_t backMaskWidth = temp < 0 ? 0 : temp;

        return { .blockIndex = loc / blockBits(),
                 .front      = blockLoc,
                 .back       = endLoc % blockBits(),
                 .frontMask  = static_cast<Block_T>(Limits<Block_T>::max() >> maskShift << blockLoc),
                 .backMask   = static_cast<Block_T>(
                     backMaskWidth ? Limits<Block_T>::max() >> (blockBits() - backMaskWidth) : 0) };
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
            return index == 0 ? CurField_T::digits() : _FieldTraits<index - 1, RestField_Ts...>::digits();
        }

        constexpr static bool canFit(std::same_as<type> auto toTest) noexcept
        {
            // canFit has to be a BS template to satisfy MSVC otherwise it tries to instantiate make_unsigned_t<bool>.
            if constexpr (not std::same_as<decltype(toTest), bool>)
            {
                auto bitsToTest =
                    std::bit_cast<std::make_unsigned_t<type>>(static_cast<type>(toTest < 0 ? ~toTest : toTest));
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
            return CurField_T::digits();
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

    /*!
     * \internal
     *
     * Returns the number of bits that the underlying block type consumes.
     *
     * This just returns `sizeof(Block_T) * Platform::bitsInByte`.
     */
    consteval static size_t blockBits() noexcept
    {
        return sizeof(Block_T) * Platform::bitsInByte;
    }
};

/*!
 *
 */
template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple8 = BasicPackedTuple<u8, Field_T, Field_Ts...>;

/*!
 *
 */
template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple16 = BasicPackedTuple<u16, Field_T, Field_Ts...>;

/*!
 *
 */
template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple32 = BasicPackedTuple<u32, Field_T, Field_Ts...>;

/*!
 *
 */
template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple64 = BasicPackedTuple<u64, Field_T, Field_Ts...>;

/*!
 * The default BasicPackedTuple type alias.
 *
 * The PackedTuple64 is the default choice of block type for the PackedTuple since it is the most useful version of the
 * four (as it can contain any non-128-bit type) and there is usually no reason to worry about data packing in 8-byte
 * increments. However, if you wish to avoid packing in 8-byte increments, you should select one of the other
 * BasicPackedTuple aliases. Keep in mind that the types that may be contained in a BasicPackedTuple are limited to the
 * size of the underlying data block used, to ensure this type's efficiency in use.
 */
template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple = PackedTuple64<Field_T, Field_Ts...>;

} // namespace KirHut
