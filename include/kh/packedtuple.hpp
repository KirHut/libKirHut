/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/packedtuple.hpp
** Copyright © KirHut Software Company
**
** Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
** the License. You may obtain a copy of the License at
**
**   http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
** an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
** specific language governing permissions and limitations under the License.
***********************************************************************************************************************/
#pragma once

#include "kh/base.hpp"

namespace KirHut
{

KH_INLINE_NAMESPACE_V1

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
 * \tparam bitSize The number of binary digits (or "bits") to represent the given \p Int_T, including the sign bit.
 */
template <std::integral Int_T, unsigned int bitSize>
struct BitField
{
    /*!
     * Typedef of the \p Int_T type template parameter of this class.
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
    consteval static Int min() noexcept;

    /*!
     * Highest integer value that can be placed into this BitField.
     *
     * An interesting characteristic of the max value is that it is also a mask value for all of the bits that are
     * allowed to be distinct from each other in this BitField. With two's complement representation, an integer of 0
     * has all zero bits and an integer of -1 has all 1 bits. The lower order bits are flipped as we go in either
     * direction, with the rest of all the upper order bits remaining the same. This allows us to intuitively discern if
     * a given integer can fit in a particular BitField, using something like this:
     *
     * ~~~
     * return (integer < 0 ? ~integer : integer) bitand ~BitField<Field>::max() == 0;
     * ~~~
     *
     * A value that returns true from the previous expression will never throw an exception from a call to
     * BasicPackedTuple::get().
     *
     * \return The highest value that can be placed in this BitField.
     */
    consteval static Int max() noexcept;

    /*!
     * Returns the number of bits used as \p bitSize.
     *
     * This method simply returns \p bitSize.
     *
     * \return The \p bitSize non-type parameter of this template.
     */
    consteval static unsigned int bits() noexcept;

    /*!
     * Returns the number of digits available in this BitField.
     *
     * This method returns \p bitSize when the given \p Int_T is unsigned, or one less than that for signed integers.
     *
     * \return The number of binary integer digits used in the given BitField.
     */
    consteval static unsigned int digits() noexcept;

    static_assert(not std::is_same_v<Int, bool> or bits() == 1u, "A boolean BitField is always one bit in size.");
    static_assert(bits() <= bitsOf<Int>(), "The given bitSize is too large.");
    static_assert(bits() > (std::is_signed_v<Int> ? 1u : 0u), "The given bitSize is too small.");
};

template <std::integral Int_T, unsigned int bitSize>
consteval Int_T BitField<Int_T, bitSize>::min() noexcept
{
    if constexpr (std::is_same_v<bool, Int> or std::is_unsigned_v<Int> or bits() == bitsOf<Int>())
    {
        return Limits<Int>::min();
    }
    else
    {
        return static_cast<Int>(-1) & ~max();
    }
}

template <std::integral Int_T, unsigned int bitSize>
consteval Int_T BitField<Int_T, bitSize>::max() noexcept
{
    if constexpr (std::is_same_v<bool, Int> or bits() == bitsOf<Int>())
    {
        return Limits<Int>::max();
    }
    else
    {
        return (static_cast<Int>(1) << digits()) - 1;
    }
}

template <std::integral Int_T, unsigned int bitSize>
consteval unsigned int BitField<Int_T, bitSize>::bits() noexcept
{
    return bitSize;
}

template <std::integral Int_T, unsigned int bitSize>
consteval unsigned int BitField<Int_T, bitSize>::digits() noexcept
{
    return bits() - (std::is_signed_v<Int> ? 1u : 0u);
}

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
template <unsigned int bits>
using I8 = BitField<i8, bits>;

/*!
 * BitField alias for the u8 unsigned integer type.
 *
 * This type may be a maximum of 8 bits in length.
 *
 * \tparam bits The number of bits to use to represent the u8 value.
 */
template <unsigned int bits>
using U8 = BitField<u8, bits>;

/*!
 * BitField alias for the i16 signed integer type.
 *
 * This type may be a maximum of 16 bits in length. The given \p bits includes the sign bit.
 *
 * \tparam bits The number of bits to use to represent the i16 value.
 */
template <unsigned int bits>
using I16 = BitField<i16, bits>;

/*!
 * BitField alias for the u16 unsigned integer type.
 *
 * This type may be a maximum of 16 bits in length.
 *
 * \tparam bits The number of bits to use to represent the u16 value.
 */
template <unsigned int bits>
using U16 = BitField<u16, bits>;

/*!
 * BitField alias for the i32 signed integer type.
 *
 * This type may be a maximum of 32 bits in length. The given \p bits includes the sign bit.
 *
 * \tparam bits The number of bits to use to represent the i32 value.
 */
template <unsigned int bits>
using I32 = BitField<i32, bits>;

/*!
 * BitField alias for the u32 unsigned integer type.
 *
 * This type may be a maximum of 32 bits in length.
 *
 * \tparam bits The number of bits to use to represent the u32 value.
 */
template <unsigned int bits>
using U32 = BitField<u32, bits>;

/*!
 * BitField alias for the i64 signed integer type.
 *
 * This type may be a maximum of 64 bits in length. The given \p bits includes the sign bit.
 *
 * \tparam bits The number of bits to use to represent the i64 value.
 */
template <unsigned int bits>
using I64 = BitField<i64, bits>;

/*!
 * BitField alias for the u64 unsigned integer type.
 *
 * This type may be a maximum of 64 bits in length.
 *
 * \tparam bits The number of bits to use to represent the u64 value.
 */
template <unsigned int bits>
using U64 = BitField<u64, bits>;

#if defined(KH_USE_128BIT_TYPES)
/*!
 * BitField alias for the i128 signed integer type.
 *
 * This type may be a maximum of 128 bits in length. The given \p bits includes the sign bit.
 *
 * \tparam bits The number of bits to use to represent the i64 value.
 */
template <unsigned int bits>
using I128 = BitField<i128, bits>;

/*!
 * BitField alias for the u128 unsigned integer type.
 *
 * This type may be a maximum of 128 bits in length.
 *
 * \tparam bits The number of bits to use to represent the u64 value.
 */
template <unsigned int bits>
using U128 = BitField<u128, bits>;
#endif

/*!
 * BitField alias for the standard signed integer type.
 *
 * This type may be a maximum of bitsOf<int>() bits in length. The given \p bits includes the sign bit.
 *
 * \tparam bits The number of bits to use to represent the int value.
 */
template <unsigned int bits>
using Int = BitField<int, bits>;

/*!
 * BitField alias for the standard unsigned integer type.
 *
 * This type may be a maximum of bitsOf<unsigned int>() bits in length.
 *
 * \tparam bits The number of bits to use to represent the unsigned int value.
 */
template <unsigned int bits>
using UInt = BitField<unsigned int, bits>;

} // namespace BF

namespace Detail
{

/*!
 * \internal
 *
 * Default non-specialized version of BitField type check.
 *
 * Any type that doesn't match the template specialization is obviously not a Bit Field type.
 *
 * \tparam Wrong_T A type that is clearly not a BitField type.
 */
template <typename Wrong_T>
constexpr bool isBitField = false;

/*!
 * \internal
 *
 * Template specialization that matches any KirHut::BitField template type.
 *
 * If a type is a BitField type, than this obviously is true.
 *
 * \tparam Int_T The integral type used in the BitField type.
 * \tparam bits The number of bits for the passed BitField type.
 */
template <std::integral Int_T, unsigned int bits>
constexpr bool isBitField<BitField<Int_T, bits>> = true;

/*!
 * \internal
 *
 * Concept that matches the isBitField template state.
 *
 * This allows using isBitField as a concept rather than just a template bool.
 *
 * \tparam BitField_T A type that may or may not be a BitField type.
 */
template <typename BitField_T>
concept BitFieldType = isBitField<BitField_T>;

/*!
 * \internal
 *
 * A Basic traits class for a particluar index in a list of fields given as [Field_T, Field_Ts...].
 *
 * The Fields as described here are some kind of BitField type that this object is then providing information about for
 * that BitField in a list. There is some information that a BitField itself cannot know, like where it is located in
 * bits for a given list of fields,
 *
 * \tparam index The index of the field found in \p Field_T and \p Field_Ts...
 * \tparam Field_T The first field in the list of fields (so as to require at least one field).
 * \tparam Field_Ts The rest of the fields in the list of fields (which shouldn't be zero, or else this should use the
 * template specialization).
 */
template <size_t index, BitFieldType Field_T, BitFieldType... Field_Ts>
class FieldTraits
{
    /*!
     * \internal
     *
     * Alias to a std::tuple type what matches the FieldTraits given BitFields.
     *
     * This is useful for certain useful metadata in the FieldTratis class, namely getting the appropriate BitFieldType
     * based on index using std::tuple_element_t, and the number of fields using std::tuple_size_v. Since this is all
     * done at compile time, it has no impact on the size or efficiency of the BasicPackedTuple class itself.
     */
    using Tuple_T = std::tuple<Field_T, Field_Ts...>;

public:
    /*!
     * \internal
     *
     * Typedef for the field type found at the given \p index.
     */
    using type = typename std::tuple_element_t<index, Tuple_T>::Int;

    /*!
     * \internal
     *
     * Returns the bit location of a given \p index for the given set of fields.
     *
     * \return The bit location of the given \p index.
     */
    consteval static size_t bitLocation() noexcept;

    /*!
     * \internal
     *
     * Returns the number of bits assigned to the given \p index in the set of fields.
     *
     * \return The size in bits of the given \p index.
     */
    consteval static unsigned int bits() noexcept;

    /*!
     * \internal
     *
     * Returns the number of digits assigned to the given \p index in the set of fields.
     *
     * This is similar to std::numeric_limits<T>::digits(), in that it returns a base 2 number of digits rather than a
     * base 10 number of digits. It will also correctly ignore the sign bit on signed integer types.
     *
     * \return The number of numeric "digits" of the given \p index.
     */
    consteval static unsigned int digits() noexcept;

    /*!
     * \internal
     *
     * Returns the maximum value that can be assigned to a given \p index in the set of fields.
     *
     * \return The maximum value allowed in the given \p index.
     */
    consteval static type max() noexcept;

    /*!
     * \internal
     *
     * Returns the minimum value that can be assigned to a given \p index in the set of fields.
     *
     * For signed integers, this is a negative value, and for unsigned values, it is always 0.
     *
     * \return The minimum value allowed in the given \p index.
     */
    consteval static type min() noexcept;

    /*!
     * \internal
     *
     * Returns whether or not the passed \p toTest value can be stored in the given \p index BitField without losing any
     * information.
     *
     * This checks if the value of \p toTest is within the bounds of [min(), max()] for the given \p index. If it is
     * not, this returns false, otherwise it returns true.
     *
     * This is done by using a direct bit count check, which involves normalizing the bits for negative values (if a
     * negative value is passed as \p toTest) and checking if there are any 1 bits above the maximum number of storable
     * digits() for the BitField at the given \p index. The check is performed this way to ensure that this check is a
     * fast, branch free check on the majority of hardware devices that libKirHut targets.
     *
     * \note A bool type always returns true, so you may not even need to check it in that case.
     *
     * \param toTest A value of the type for the given \p index we're checking is small enough to fit in the BitField.
     * \return Whether or not the \p toTest value can fit in the BitField at \p index.
     */
    constexpr static bool canFit(std::same_as<type> auto toTest) noexcept;

    static_assert(index < std::tuple_size_v<Tuple_T>, "The selected index for FieldTraits is too large.");
};

template <size_t index, BitFieldType Field_T, BitFieldType... Field_Ts>
consteval size_t FieldTraits<index, Field_T, Field_Ts...>::bitLocation() noexcept
{
    return []<size_t... indexes>([[maybe_unused]] std::index_sequence<indexes...>) {
        return (static_cast<size_t>(std::tuple_element_t<indexes, Tuple_T>::bits()) + ... + size_t(0));
    }(std::make_index_sequence<index>{});
}

template <size_t index, BitFieldType Field_T, BitFieldType... Field_Ts>
consteval unsigned int FieldTraits<index, Field_T, Field_Ts...>::bits() noexcept
{
    return std::tuple_element_t<index, Tuple_T>::bits();
}

template <size_t index, BitFieldType Field_T, BitFieldType... Field_Ts>
consteval FieldTraits<index, Field_T, Field_Ts...>::type FieldTraits<index, Field_T, Field_Ts...>::max() noexcept
{
    return std::tuple_element_t<index, Tuple_T>::max();
}

template <size_t index, BitFieldType Field_T, BitFieldType... Field_Ts>
consteval FieldTraits<index, Field_T, Field_Ts...>::type FieldTraits<index, Field_T, Field_Ts...>::min() noexcept
{
    return std::tuple_element_t<index, Tuple_T>::min();
}

template <size_t index, BitFieldType Field_T, BitFieldType... Field_Ts>
consteval unsigned int FieldTraits<index, Field_T, Field_Ts...>::digits() noexcept
{
    return std::tuple_element_t<index, Tuple_T>::digits();
}

template <size_t index, BitFieldType Field_T, BitFieldType... Field_Ts>
constexpr bool FieldTraits<index, Field_T, Field_Ts...>::canFit(std::same_as<type> auto toTest) noexcept
{
    // canFit has to be a BS template to satisfy MSVC otherwise it tries to instantiate make_unsigned_t<bool>.
    if constexpr (not std::same_as<decltype(toTest), bool>)
    {
        // This just suppresses a warning in Clang for a signed/unsigned comparison.
        constexpr auto zero = static_cast<type>(0);
        auto bitsToTest =
            std::bit_cast<std::make_unsigned_t<type>>(static_cast<type>(toTest < zero ? ~toTest : toTest));
        return static_cast<unsigned int>(std::bit_width(bitsToTest)) <= digits();
    }

    return true;
}

} // namespace Detail

/*!
 * A concept that identifies if a given type \p BitField_T is a BitField template type.
 *
 * This is primarily used in the template parameters of the BasicPackedTuple class,
 */
template <typename BitField_T>
concept BitFieldType = Detail::BitFieldType<BitField_T>;

/*!
 * A Bit Packing tuple type to contain any given integer types within a series of blocks and according to a clearly
 * defined specification.
 *
 * This BasicPackedTuple type can contain any number of boolean or integer types as needed by the user in a bit-packed
 * way, allowing you to select the maximum number of bits that a given BitField can contain, then get and set that
 * BitField using natural methods. It will only use the precise number of bits necessary for each given BitField type,
 * which can be very useful in Data Oriented Design to reduce the number of actual bytes necessary to store a given
 * object or data structure's information. The interface is deliberately intended to resemble that of the std::tuple,
 * but this type does not support using types that are not boolean or integer as contained types within the tuple. This
 * is because it is impossible to know how a user wishes to "bit pack" a given object type, and floating point types are
 * not easy to represent naturally in a bit-packed way.
 *
 * This type uses a precise data layout specification for how each field in the tuple is bit packed, with the field at
 * index 0 occupying the bits of the underlying representation from [0, sizeof(BitField0)), the field at index 1
 * occupying the bits from [sizeof(BitField0), sizeof(BitField0) + sizeof(BitField1)), etc. The data is stored in a
 * series of blocks, where all the blocks are always normalized to a Little-endian representation. This means that,
 * regardless of what platform this object is used on, the underlying data will always be exactly the same, and can be
 * bit_cast between different Block sizes directly without needing to rebuild the object. The low-order bits of every
 * value always start at the first byte, and each subsequent byte has the higher-order bits of the same BitFields or
 * the bits, going from low to high, of subsequent BitFields beyond the fist one.
 *
 * A consequence of this is that this is legal, has defined behavior, and works on all platforms:
 *
 * ~~~
 * using PT8 = PackedTuple8<BF::Bool, BF::U64<63>>;
 * using PT64 = PackedTuple64<BF::Bool, BF::U64<63>>;
 *
 * PT8 start{ true, 12345ull };
 * PT64 the_same = std::bit_cast<PT64>(start);
 * ~~~
 *
 * This only works when the sizeof() the two PackedTuple types are equal to each other, otherwise the bit_cast will
 * completely fail. The contained types must also be the same or you will run into unpredictable behavior. Finally, when
 * converting from a larger Block size PackedTuple to a smaller one, you need to ensure that the alignof() of the
 * smaller PackedTuple type is equal to the block size of the larger one, to prevent memory alignment issues on certain
 * processor architectures.
 *
 * \internal
 * \note The memory alignment issues described above may not be completely accurate. Thorough testing on ARM and RISC-V
 * processors needs to be performed with bit_casts between different Block sizes to verify that this is an issue or that
 * the library can instead remove this incorrect documentation. Currently, matching alignment does not have any real
 * drawbacks, so the documentation recommends doing so, but it is not verified that this is actually the case.
 * \endinternal
 *
 * The BasicPackedTuple completely supports use in a constexpr context, so can be directly manipulated like any other
 * type at compile time, then stored after performing arbitrary compile-time transformations for runtime use without any
 * cost to runtime performance. This class only supports storing signed values that are represented using two's
 * complement underneath, though that is not a real limitation in the modern era as all hardware uses this
 * representation for signed integer types.
 *
 * The class provides easy access to the underlying data blocks to allow for any system to serialize and deserialize
 * this class easily. The method of transmitting the serialized data is irrelevant, and any platform you send the
 * serialized data to will be able to deserialize the data and read it without need to change or process the internal
 * data structure at all. This can be very useful for transmitting a large number of structs or simple data objects that
 * use the BasicPackedTuple to contain a variety of different sized values over a network connection very efficiently.
 *
 * This class follows the rule of 0, having no explicit copy/move constructors or assignment operators, nor destructor.
 * All of the default versions of these methods will be synthesized by the compiler and work as expected for all
 * BasicPackedTuple types with the same \p Block_T, \p Field_T, and \p Field_Ts. All methods and constructors in this
 * class are marked noexcept, under the assumptions made about the underlying platform and types.
 *
 * \tparam Block_T An unsigned integer type used as the "Block" type for the underlying data array.
 * \tparam Field_T The first BitField of the BasicPackedTuple, separated to ensure at least one field exists.
 * \tparam Field_Ts The rest of the BitFields of the BasicPackedTuple.
 */
template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
class BasicPackedTuple
{
    /*!
     * \internal
     *
     * The number of blocks required to hold the given set of BitFields in this BasicPackedTuple.
     *
     * This is initialized using a lambda, but this is all done at compile time so there should be zero runtime effect.
     */
    consteval static size_t blocksNeeded() noexcept;

    /*!
     * \internal
     *
     * The actual data storage array for this BasicPackedTuple.
     *
     * Here is where the data really lives! It should be initialized to all zeros upon construction of this class. This
     * is a requirement to avoid UB while using this class.
     */
    array<Block_T, blocksNeeded()> data{};

    /*!
     * \internal
     *
     * A FieldTraits type alias for easier access to the Detail::FieldTraits type.
     */
    template <size_t index>
    using FieldTraits = Detail::FieldTraits<index, Field_T, Field_Ts...>;

public:
    /*!
     * An alias for the type of a BitField at \p index.
     *
     * You can use this as a way to initialize a value returned by get() or as a value to be passed to the set()
     * functions before actually calling those methods. Generally speaking, a packed tuple type should be given its own
     * alias name in your code, then you just use that to access the FieldType, like so:
     *
     * \snippet PackedTuple_FieldTraits.cpp FieldType Example
     *
     * This allows choosing a type based on the index.
     *
     * \tparam index The index of the field type that this resolves to.
     */
    template <size_t index>
    requires(index <= sizeof...(Field_Ts))
    using FieldType = FieldTraits<index>::type;

    /*!
     * Useful alias of the current BasicPackedTuple's block type.
     *
     * This is mostly useful when using a BasicPackedTuple under an alias, and you need to get the block type of that
     * alias. An example of use would be something like this:
     *
     * ~~~
     * using MyPT = PackedTuple<BF::Bool, BF::U64<63>>;
     * MyPT pt{ true, 12345ull };
     * MyPT::Block firstBlock = pt.peekInternalData()[0];
     * ~~~
     *
     * Generally speaking, however, you're unlikely to need to refer to the blocks used for the internal data
     * representation itself unless you are doing something non-trivial with serializing this type.
     */
    using Block = Block_T;

    /*!
     * A useful type alias for the underlying data type that BasicPackedTuple uses to contain the tuple values.
     *
     * This is useful for serialization and deserialization of this class. You can use it like this in your own code:
     *
     * ~~~
     * using MyPT = PackedTuple<BF::Bool, BF::U64<63>>;
     * MyPT pt{ true, 12345ull };
     * MyPT::DataBlocks data = pt.peekInternalData();
     * ~~~
     *
     * From there, you would directly serialize each block in your according serialization system, or send it over the
     * network, or whatever it is you wish to do with the underlying data of this tuple type.
     */
    using DataBlocks = array<Block_T, blocksNeeded()>;

    /*!
     * Construct a BasicPackedTuple with all fields set to 0.
     *
     * This constructor will initialize all tuple members with a value of 0. If you want to initialize the values
     * contained in this tuple, you should use one of the tuple initializing constructors instead of this one.
     */
    constexpr BasicPackedTuple() noexcept;

    /*!
     * Construct a BasicPackedTuple setting all of the fields in the tuple.
     *
     * This constructor sets each field to the passed-in values respectively to each index value in the current
     * BasicPackedTuple. You must pass a value for each member of the tuple, or this will fail to compile.
     *
     * If this constructor is provided a value that is too large to fit in a given field, it will simply set the value
     * at the greatest number of bits allowed for that field type. It does this by using the setSaturate() function
     * instead of the set() function. Please refer to the documentation for setSaturate() for more information on how
     * this will specifically set the value for an integer value that is too large to fit.
     *
     * \param first The first of the fields in the BasicPackedTuple to initialize.
     * \param rest The rest of the fields in the BasicPackedTuple to initialize.
     */
    constexpr explicit(sizeof...(Field_Ts) == 0)
        BasicPackedTuple(typename Field_T::Int first, typename Field_Ts::Int... rest) noexcept;

    /*!
     * Construct a BasicPackedTuple setting all of the fields using a std::tuple.
     *
     * This constructor sets each field to the passed-in tuple's values respectively at each index value in the current
     * BasicPackedTuple. The passed std::tuple must have an integer type of each field in the BasicPackedTuple, and the
     * integer types must match the types used in each BitField for this BasicPackedTuple.
     *
     * If this constructor is provided a value that is too large to fit in a given field, it will simply set the value
     * at the greatest number of bits allowed for that field type. It does this by using the setSaturate() function
     * instead of the set() function. Please refer to the documentation for setSaturate() for more information on how
     * this will specifically set the value for an integer value that is too large to fit.
     *
     * \param beginState A tuple with all of the same fields as this BasicPackedTuple to initialize from.
     */
    constexpr explicit BasicPackedTuple(
        std::tuple<typename Field_T::Int, typename Field_Ts::Int...> const &beginState) noexcept;

    /*!
     * Serialized state initializing BasicPackedTuple constructor.
     *
     * This should only be provided a state that has been serialized from another BasicPackedTuple class with the same
     * BitField parameters as this BasicPackedTuple class. You may use a different Block type because the way the data
     * is formatted in this type is completely little-endian, so the different Block types of BasicPackedTuple may be
     * directly `std::bit_cast`ed into each other without issue.
     *
     * While this class has a perfectly functional default copy constructor, this method also allows creating similar
     * copies by using `MyTuple copy{ original.peekInternalData() };` at roughly the same efficiency as the default copy
     * constructor.
     *
     * \param beginState A std::array of the underlying Block type with bits laid out according to this object's data
     * layout specification.
     */
    constexpr explicit BasicPackedTuple(DataBlocks const &beginState) noexcept;

    /*!
     * Equality comparison operator for BasicPackedTuple.
     *
     * This is implemented using `= default` so that the underlying array type is directly compared instead of needing
     * to extract each value and compare them in series. This is usually much more efficient to do, at least when you
     * are using a BasicPackedTuple with a Block size equal to the processor's word size.
     *
     * The != operator should be synthesized from this operator, so both should work equally well and equally
     * efficiently.
     *
     * \param other Another BasicPackedTuple with the same BitField types to compare with this one.
     * \return Whether or not the passed BasicPackedTuple type is equal to this one.
     */
    [[nodiscard]] constexpr bool operator==(BasicPackedTuple const &other) const noexcept = default;

    /*!
     * Spaceship comparison operator for BasicPackedTuple.
     *
     * This comparison will extract each field from this BasicPackedTuple as well as the \p other one, starting from the
     * 0th indexed field, and compare them using <=>. If they are equal, it will continue to compare each field until a
     * field is found where they are not equal, or all fields have been found to be equal. If any field is found to not
     * be std::strong_ordering::equal, this method will return the difference (either std::strong_ordering::less or
     * std::strong_ordering::greater).
     *
     * This comparison operator works equivalently well to the std::tuple version, where it will "short circuit" upon
     * finding the first field that is non-equal, and immediately return that result instead of performing an extraction
     * and comparison of the fields after that.
     *
     * All comparison operators, such as operator< and operator>, should be synthesized from this operator.
     *
     * \param other Another BasicPackedTuple with the same BitField types to compare with this one.
     * \return A std::strong_ordering value of the first BitField found to be non-equal, or std::strong_ordering::equal.
     */
    [[nodiscard]] constexpr std::strong_ordering operator<=>(BasicPackedTuple const &other) const noexcept;

    /*!
     * Extracts a field value from its bit-packed representation.
     *
     * This method retrieves the bits associated with the field at \p index, "stitches" them together if they span
     * multiple underlying blocks, and restores the original logical type. For stored values that are negative, this
     * method will also perform appropriate sign extension so that the returned value is correctly representative of the
     * value that was originally stored.
     *
     * \tparam index The zero-based index of the field to retrieve.
     * \return The field value, correctly sign-extended if the FieldType is signed.
     */
    template <size_t index>
    requires(index <= sizeof...(Field_Ts))
    [[nodiscard]] constexpr FieldType<index> get() const noexcept;

    /*!
     * Checks if the \p newVal will fit in the BitField at \p index, and if so, writes it, otherwise it returns false.
     *
     * This method is a checked set method that will not write a given BitField \p index if the value passed is larger
     * than can fit within the bit bounds of the BitField. If it can fit, this method will do so and return true to
     * indicate that it was successful and setting the value, otherwise the BasicPackedTuple will remain unchanged and
     * this method will instead return false.
     *
     * If you want to ensure that the field is written to regardless of if the input value is too large, please use the
     * approprate setTruncate() or setSaturate() method according to how you would prefer to handle the bits that are
     * beyond the size of the \p index BitField.
     *
     * \tparam index The zero-based index of the field to write to.
     * \param newVal The value with which to overwrite the \p index BitField with.
     * \return Whether or not this method could successfully fit the given \p newVal into the \p index BitField.
     */
    template <size_t index>
    requires(index <= sizeof...(Field_Ts))
    constexpr bool set(FieldType<index> newVal) noexcept;

    /*!
     * Sets the given \p index field to \p newVal, truncating any bits beyond the bit field's maximum limit.
     *
     * This will always set the \p index field regardless of what is passed as \p newVal, so if you have a field like
     * BF::I32<4>, where the number of non-sign digits is 3, if \p newVal has non-sign bits set to 1 that are above the
     * third bit, the rest of those bits will be truncated. The sign bit will always be retained on signed bit fields,
     * so the maximum value can be larger on unsigned bit fields of the same bit width. It is usually more useful to use
     * setSaturate() or the checking set() function than this one, but for the cases where truncation is preferred, this
     * method is available to use.
     *
     * \tparam index The zero-based index of the field to write to.
     * \param newVal The value with which to overwrite the \p index BitField with.
     */
    template <size_t index>
    requires(index <= sizeof...(Field_Ts))
    constexpr void setTruncate(FieldType<index> newVal) noexcept;

    /*!
     * Sets the given \p index field to \p newVal, clamping the result for any values too large to fit.
     *
     * This will always set the \p index field regardless of what is passed as \p newVal, so if you have a field like
     * BF::I32<4>, where the maximum value is 7 and the minimum value is -8, if \p newVal is above 7, the written value
     * will be 7, and if \p newVal is below -8, the written value will be -8. This is usually more desirable than what
     * setTruncate() does, however there are times when setTruncate() is more useful, for example if the field value is
     * meant to represent a set of bit flags rather than an actual integer value.
     *
     * \tparam index The zero-based index of the field to write to.
     * \param newVal The value with which to overwrite the \p index BitField with.
     */
    template <size_t index>
    requires(index <= sizeof...(Field_Ts))
    constexpr void setSaturate(FieldType<index> newVal) noexcept;

    /*!
     * See the raw underlying data array used by this BasicPackedTuple to contain all of the data members.
     *
     * This function is very useful for performing serialization of this class. The according constructor that takes a
     * DataBlocks can be used to quickly deserialize this class from a given std::array, and with this method, this
     * provides a complete way to get the underlying data representation of this class and perform any data transmission
     * or storage that can be done with any array of unsigned integer values. The underlying data representation is
     * always the same for all platforms and processors that libKirHut builds on, so regardless of the source machine
     * that stores, transmits, or otherwise provides the data of a BasicPackedTuple, another device with any other
     * hardware or endianness can read that data into a BasicPackedTuple and completely restore its state.
     *
     * The returned data blocks are always stored in memory as little-endian values, regardless of the endianness of the
     * platform. This allows for all of the bit_cast tricks and cross-platform serializability that this class is
     * capable of.
     *
     * \return The underlying data blocks of this class as a const reference.
     */
    [[nodiscard]] constexpr DataBlocks const &peekInternalData() const noexcept;

private:
    /*!
     * \internal
     *
     * A data struct that contains information for a given BitField within this BasicPackedTuple.
     *
     * This object is only used in constexpr contexts, so should only ever be built and used by the compiler. The only
     * method that creates this object is the getLocation() method. The object is always otherwise used as a constant
     * value, so no other methods will modify its values or create the object at runtime.
     */
    struct FieldLocation;

    /*!
     * \internal
     *
     * Creates a FieldLocation at compile-time for the given \p index in this BasicPackedTuple.
     *
     * See the FieldLocation object for more information on what this method returns.
     *
     * \tparam index The zero-based index of the field to get the location information for.
     * \return A FieldLocation struct with metadata information about the given \p index field.
     */
    template <size_t index>
    requires(index <= sizeof...(Field_Ts))
    consteval static FieldLocation getLocation() noexcept;

    /*!
     * \internal
     *
     * Initialize a BasicPackedTuple using a std::tuple.
     *
     * This allows for construction of a BasicPackedTuple using a set of values or a tuple assignment, rather than by
     * repeatedly calling set() for each index. The caller of this function should always pass 0 as the index, and no
     * other value, unless you want initialization to start after the first value in the tuple!
     *
     * \tparam index Should always be 0, since it is an implementation detail for recursive calls.
     * \param initTuple The tuple used to initialize this PackedTuple with as starting values.
     * \throws IllegalArgument If one of the fields in the passed tuple is too large to fit in it's according bit field.
     */
    constexpr void tupleInit(std::tuple<typename Field_T::Int, typename Field_Ts::Int...> const &initTuple) noexcept;

    /*!
     * \internal
     *
     * Returns the number of bits that the underlying block type consumes.
     *
     * This just returns `sizeof(Block_T) * Constant::bitsInByte`.
     */
    consteval static size_t blockBits() noexcept;

    /*!
     * \internal
     *
     * Little-endian Block receiving function that reads a block and converts it to the platform's representation.
     *
     * On Little-endian systems, this function simply returns the Block found at the given \p index from the underlying
     * data array. On Big-endian systems, this will get the Block at the given \p index, then perform a byte swap, and
     * return the result of that byte swap.
     *
     * \param index The index in the underlying data array to retrieve the Block from.
     * \return A system-endian normalized value of the underlying Little-endian Block value.
     */
    constexpr Block_T blockAt(size_t index) const noexcept;

    /*!
     * \internal
     *
     * Little-endian Block setting function that converts the block to Little-endian and writes it to the underlying
     * data array.
     *
     * On Little-endian systems, this function simply writes the passed \p newBlock to the given \p index in the
     * underlying data array. On Big-endian systems, this will perform a byte swap on \p newBlock, then write that
     * result to the \p index.
     *
     * \param index The index in the underlying data array to write \p newBlock to.
     * \param newBlock A system-endian value that will be converted to Little-endian then written to \p index.
     */
    constexpr void setBlockAt(size_t index, Block_T newBlock) noexcept;
};

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
struct BasicPackedTuple<Block_T, Field_T, Field_Ts...>::FieldLocation
{
    /*!
     * \internal
     *
     * The index of the block where the given BitField begins.
     *
     * This does not contain any information about which bit in the given block the BitField begins, instead it is the
     * block index where the BitField starts in the underlying DataBlocks.
     */
    size_t blockIndex;

    /*!
     * \internal
     *
     * The bit location at the blockIndex where the BitField actually starts.
     *
     * This should never equal or exceed sizeof(Block) since this contains the specific bit location in a given Block
     * for where the BitField starts.
     */
    unsigned int blockStart;

    /*!
     * \internal
     *
     * The number of blocks after the first block that are completely made up of the given BitField.
     *
     * Under the current supported configuration, this should never exceed 15 since even with 128-bit types and a single
     * byte Block, a BitField is at max 16 bytes long, and one of the bytes will always be divided between the first and
     * last block.
     */
    unsigned int fullBlockCount;

    /*!
     * \internal
     *
     * A bitmask of the BasicPackedTuple's Block type with all 1 bits set for the given BitField in the first block.
     *
     * This could be derived from the blockStart value and the number of bits(), but this makes the information much
     * easier to directly refer to.
     */
    Block_T frontMask;

    /*!
     * \internal
     *
     * A bitmask of the BasicPackedTuple's Block type with all 1 bits set for the given BitField in the last block.
     *
     * This value may be all zeros if there are no bits in the last block that are used. This includes when the
     * fullBlockCount value is non-zero, so these must be both checked independently. Like frontMask, this could be
     * derived by using the blockStart position along with the bits(), but this makes it easier to refer to.
     */
    Block_T backMask;
};

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
constexpr BasicPackedTuple<Block_T, Field_T, Field_Ts...>::BasicPackedTuple() noexcept = default;

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
constexpr BasicPackedTuple<Block_T, Field_T, Field_Ts...>::BasicPackedTuple(typename Field_T::Int first,
                                                                            typename Field_Ts::Int... rest) noexcept
{
    tupleInit(std::tuple{ first, rest... });
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
constexpr BasicPackedTuple<Block_T, Field_T, Field_Ts...>::BasicPackedTuple(
    std::tuple<typename Field_T::Int, typename Field_Ts::Int...> const &beginState) noexcept
{
    tupleInit(beginState);
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
constexpr BasicPackedTuple<Block_T, Field_T, Field_Ts...>::BasicPackedTuple(
    BasicPackedTuple<Block_T, Field_T, Field_Ts...>::DataBlocks const &beginState) noexcept :
    data(beginState)
{
    // No further implementation.
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
constexpr std::strong_ordering BasicPackedTuple<Block_T, Field_T, Field_Ts...>::operator<=>(
    BasicPackedTuple<Block_T, Field_T, Field_Ts...> const &other) const noexcept
{
    std::strong_ordering retVal = std::strong_ordering::equal;

    [&]<size_t... indexes>([[maybe_unused]] std::index_sequence<indexes...>) {
        // Use assignment here to silence warnings from certain compilers.
        auto _ = ((retVal = get<indexes>() <=> other.get<indexes>(), retVal != std::strong_ordering::equal) or ...);
    }(std::index_sequence_for<Field_T, Field_Ts...>{});

    return retVal;
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
template <size_t index>
requires(index <= sizeof...(Field_Ts))
constexpr BasicPackedTuple<Block_T, Field_T, Field_Ts...>::FieldType<index>
BasicPackedTuple<Block_T, Field_T, Field_Ts...>::get() const noexcept
{
    constexpr FieldLocation fieldInfo = getLocation<index>();
    if constexpr (std::is_same_v<FieldType<index>, bool>)
    {
        return static_cast<bool>(blockAt(fieldInfo.blockIndex) >> fieldInfo.blockStart bitand 1);
    }
    else
    {
        using Unsigned_T = std::make_unsigned_t<FieldType<index>>;
        auto retVal =
            static_cast<Unsigned_T>((blockAt(fieldInfo.blockIndex) bitand fieldInfo.frontMask) >> fieldInfo.blockStart);
        size_t shiftAmount = blockBits() - fieldInfo.blockStart;

        for (unsigned int readCount = 1; readCount <= fieldInfo.fullBlockCount; ++readCount)
        {
            retVal |= static_cast<Unsigned_T>(blockAt(fieldInfo.blockIndex + readCount)) << shiftAmount;
            shiftAmount += blockBits();
        }

        if constexpr (fieldInfo.backMask)
        {
            constexpr auto backIndex = fieldInfo.blockIndex + fieldInfo.fullBlockCount + 1;
            retVal |= static_cast<Unsigned_T>(blockAt(backIndex) bitand fieldInfo.backMask) << shiftAmount;
        }

        if constexpr (std::is_signed_v<FieldType<index>>)
        {
            constexpr auto signBit = shl(static_cast<Unsigned_T>(1), FieldTraits<index>::bits() - 1);
            if (retVal bitand signBit)
            {
                retVal or_eq shl(Limits<Unsigned_T>::max(), FieldTraits<index>::bits());
            }

            return std::bit_cast<FieldType<index>>(retVal);
        }
        else
        {
            return retVal;
        }
    }
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
template <size_t index>
requires(index <= sizeof...(Field_Ts))
constexpr bool BasicPackedTuple<Block_T, Field_T, Field_Ts...>::set(FieldType<index> newVal) noexcept
{
    constexpr FieldLocation fieldInfo = getLocation<index>();

    Block_T frontBlock = blockAt(fieldInfo.blockIndex);
    if constexpr (std::same_as<FieldType<index>, bool>)
    {
        constexpr Block_T newBitMask = ~(static_cast<Block_T>(1) << fieldInfo.blockStart);

        Block_T newBit = static_cast<Block_T>(newVal) << fieldInfo.blockStart;
        setBlockAt(fieldInfo.blockIndex, (frontBlock bitand newBitMask) bitor newBit);
    }
    else
    {
        if (not FieldTraits<index>::canFit(newVal))
        {
            return false;
        }

        auto input = std::bit_cast<std::make_unsigned_t<FieldType<index>>>(newVal);
        setBlockAt(fieldInfo.blockIndex,
                   (frontBlock bitand ~fieldInfo.frontMask) bitor
                       (static_cast<Block_T>(input) << fieldInfo.blockStart bitand fieldInfo.frontMask));
        input = shr(input, blockBits() - fieldInfo.blockStart);

        for (unsigned int writeCount = 1; writeCount <= fieldInfo.fullBlockCount; ++writeCount)
        {
            setBlockAt(fieldInfo.blockIndex + writeCount, static_cast<Block_T>(input));
            input = shr(input, blockBits());
        }

        if constexpr (fieldInfo.backMask)
        {
            constexpr auto backIndex = fieldInfo.blockIndex + fieldInfo.fullBlockCount + 1;
            setBlockAt(backIndex,
                       (blockAt(backIndex) bitand ~fieldInfo.backMask) bitor
                           (static_cast<Block_T>(input) bitand fieldInfo.backMask));
        }
    }

    return true;
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
template <size_t index>
requires(index <= sizeof...(Field_Ts))
constexpr void BasicPackedTuple<Block_T, Field_T, Field_Ts...>::setTruncate(FieldType<index> newVal) noexcept
{
    if constexpr (not std::same_as<FieldType<index>, bool>)
    {
        newVal = newVal < 0 ? newVal bitor ~FieldTraits<index>::max() : newVal bitand FieldTraits<index>::max();
    }

    set<index>(newVal);
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
template <size_t index>
requires(index <= sizeof...(Field_Ts))
constexpr void BasicPackedTuple<Block_T, Field_T, Field_Ts...>::setSaturate(FieldType<index> newVal) noexcept
{
    if (newVal > FieldTraits<index>::max())
    {
        newVal = FieldTraits<index>::max();
    }

    // We do *not* put this in an else block to make this function branch free on x86 and ARM.
    if constexpr (std::is_signed_v<FieldType<index>>)
    {
        if (newVal < FieldTraits<index>::min())
        {
            newVal = FieldTraits<index>::min();
        }
    }

    set<index>(newVal);
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
constexpr BasicPackedTuple<Block_T, Field_T, Field_Ts...>::DataBlocks const &
BasicPackedTuple<Block_T, Field_T, Field_Ts...>::peekInternalData() const noexcept
{
    return data;
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
constexpr void BasicPackedTuple<Block_T, Field_T, Field_Ts...>::tupleInit(
    std::tuple<typename Field_T::Int, typename Field_Ts::Int...> const &initTuple) noexcept
{
    [this, &initTuple]<size_t... indexes>([[maybe_unused]] std::index_sequence<indexes...>) {
        ((this->setSaturate<indexes>(std::get<indexes>(initTuple))), ...);
    }(std::index_sequence_for<Field_T, Field_Ts...>{});
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
consteval size_t BasicPackedTuple<Block_T, Field_T, Field_Ts...>::blocksNeeded() noexcept
{
    auto const blockSize = sizeof(Block_T) * Constant::bitsInByte;
    return (Field_T::bits() + (Field_Ts::bits() + ... + blockSize) - 1) / blockSize;
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
consteval size_t BasicPackedTuple<Block_T, Field_T, Field_Ts...>::blockBits() noexcept
{
    return bitsOf<Block_T>();
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
template <size_t index>
requires(index <= sizeof...(Field_Ts))
consteval BasicPackedTuple<Block_T, Field_T, Field_Ts...>::FieldLocation
BasicPackedTuple<Block_T, Field_T, Field_Ts...>::getLocation() noexcept
{
    size_t const beginBit         = FieldTraits<index>::bitLocation();
    size_t remainBits             = FieldTraits<index>::bits();
    size_t const blockIndex       = beginBit / blockBits();
    unsigned int const blockStart = beginBit % blockBits();
    size_t const frontBits        = std::min(remainBits, blockBits() - blockStart);
    unsigned int fullBlockCount   = 0;

    remainBits -= frontBits;
    while (remainBits >= blockBits())
    {
        ++fullBlockCount;
        remainBits -= blockBits();
    }

    Block_T const frontMask = shl(shr(Limits<Block_T>::max(), blockBits() - frontBits), blockStart);
    Block_T const backMask  = shr(Limits<Block_T>::max(), blockBits() - remainBits);

    return { .blockIndex     = blockIndex,
             .blockStart     = blockStart,
             .fullBlockCount = fullBlockCount,
             .frontMask      = frontMask,
             .backMask       = backMask };
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
constexpr Block_T BasicPackedTuple<Block_T, Field_T, Field_Ts...>::blockAt(size_t index) const noexcept
{
    Block_T retVal = data[index];

    if constexpr (Platform::BigEndian)
    {
        retVal = byteSwap(retVal);
    }

    return retVal;
}

template <UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
constexpr void BasicPackedTuple<Block_T, Field_T, Field_Ts...>::setBlockAt(size_t index, Block_T newBlock) noexcept
{
    if constexpr (Platform::BigEndian)
    {
        newBlock = byteSwap(newBlock);
    }

    data[index] = newBlock;
}

/*!
 * An independent get function for BasicPackedTuple types within the KirHut namespace.
 *
 * This is useful for using BasicPackedTuple in generic templated algorithms that use ADL-based calls to the get()
 * function without considering if the object has a get() method within it. This function simply calls the get() method
 * within the passed \p pt BasicPackedTuple.
 *
 * \tparam index The index of the BitField that this method should extract. You must specify this.
 * \tparam Block_T The Block type of the passed in \p pt BasicPackedTuple. This should be inferred.
 * \tparam Field_T The first BitField type of the passed in \p pt BasicPackedTuple. This should be inferred.
 * \tparam Field_Ts The remaining BitField types of the passed in \p pt BasicPackedTuple. This should be inferred.
 * \param pt The BasicPackedTuple to get a given \p index from.
 */
template <size_t index, UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
constexpr auto get(BasicPackedTuple<Block_T, Field_T, Field_Ts...> &pt) noexcept
{
    return pt.template get<index>();
}

/*!
 * An independent get function for BasicPackedTuple types within the KirHut namespace.
 *
 * This is useful for using BasicPackedTuple in generic templated algorithms that use ADL-based calls to the get()
 * function without considering if the object has a get() method within it. This function simply calls the get() method
 * within the passed \p pt BasicPackedTuple.
 *
 * \tparam index The index of the BitField that this method should extract. You must specify this.
 * \tparam Block_T The Block type of the passed in \p pt BasicPackedTuple. This should be inferred.
 * \tparam Field_T The first BitField type of the passed in \p pt BasicPackedTuple. This should be inferred.
 * \tparam Field_Ts The remaining BitField types of the passed in \p pt BasicPackedTuple. This should be inferred.
 * \param pt The BasicPackedTuple to get a given \p index from.
 */
template <size_t index, UIntegral Block_T, BitFieldType Field_T, BitFieldType... Field_Ts>
constexpr auto get(BasicPackedTuple<Block_T, Field_T, Field_Ts...> const &pt) noexcept
{
    return pt.template get<index>();
}

/*!
 * The 8-bit BasicPackedTuple type alias.
 *
 * This is the most space-efficient version of the PackedTuple, the PackedTuple8 is useful when you need your packed
 * tuple to fit precisely in the number of bytes that it requires and you are unlikely to need to bit_cast this type to
 * a larger PackedTuple32 or PackedTuple64 representation. In most cases, you are probably better off using a
 * PackedTuple32 or PackedTuple64 dependent on what your given processor's word size is.
 */
template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple8 = BasicPackedTuple<u8, Field_T, Field_Ts...>;

/*!
 * The 16-bit BasicPackedTuple type alias.
 *
 * The use-case of this BasicPackedTuple alias is pretty niche, since it has many of the raw speed disadvantages of the
 * PackedTuple8 alias while also not being the most compact version of the PackedTuple. This type remains in the library
 * in case you do find a use-case for it, since keeping this alias is extremely cheap and it matches the already
 * existing pattern in this library with the more useful PackedTuple8, PackedTuple32, and PackedTuple64 aliases.
 */
template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple16 = BasicPackedTuple<u16, Field_T, Field_Ts...>;

/*!
 * The 32-bit BasicPackedTuple type alias.
 *
 * This is an excellent BasicPackedTuple alias for use on 32-bit hardware platforms, of which are still extremely common
 * in mobile and embedded systems. Unless you specifically need to use a 32-bit BasicPackedTuple type, you are probably
 * better served just using the PackedTuple alias in most use-cases.
 */
template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple32 = BasicPackedTuple<u32, Field_T, Field_Ts...>;

/*!
 * The 64-bit BasicPackedTuple type alias.
 *
 * This is an excellent BasicPackedTuple alias for use on 64-bit hardware platforms, of which are still extremely common
 * in mobile and embedded systems. Unless you specifically need to use a 64-bit BasicPackedTuple type, you are probably
 * better served just using the PackedTuple alias in most use-cases.
 */
template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple64 = BasicPackedTuple<u64, Field_T, Field_Ts...>;

/*!
 * The default BasicPackedTuple type alias.
 *
 * This alias on 32-bit systems points to the PackedTuple32 and on all other systems points to the PackedTuple64, as
 * this should be the most processor efficient version of the PackedTuple for the platform you are on. If you are trying
 * to get the most compact version of a BasicPackedTuple, you should use the CompactTuple (which is just an alias for a
 * PackedTuple8). The CompactTuple is better for space savings, but could require a longer amount of time to set and
 * get values within it.
 */
template <BitFieldType Field_T, BitFieldType... Field_Ts>
using PackedTuple =
#if defined(KH_32BIT)
    PackedTuple32<Field_T, Field_Ts...>;
#else
    PackedTuple64<Field_T, Field_Ts...>;
#endif

/*!
 * The most compact BasicPackedTuple type alias.
 *
 * This alias always points to the PackedTuple8, which is the most space-efficient version of the PackedTuple. If you
 * are using a PackedTuple that is likely to require a multiple of 4 or 8 bytes, you are probably better off using a
 * PackedTuple32 or PackedTuple64 respectively, since this will not be any more space efficient than those would in that
 * situation. This is best for situations where you are trying to squeeze as many small objects as you can into an
 * array, and your object needs something like 2 bytes, than this version of the PackedTuple can save you considerable
 * amounts of space in memory and improve cache locality beyond the already existing cache locality savings inherent to
 * this type.
 */
template <BitFieldType Field_T, BitFieldType... Field_Ts>
using CompactTuple = PackedTuple8<Field_T, Field_Ts...>;

KH_END_INLINE_NAMESPACE

} // namespace KirHut

//! \cond
// This area is for structured binding support.
namespace std
{

template <KirHut::UIntegral Block_T, KirHut::BitFieldType Field_T, KirHut::BitFieldType... Field_Ts>
struct tuple_size<KirHut::BasicPackedTuple<Block_T, Field_T, Field_Ts...>> :
    integral_constant<size_t, sizeof...(Field_Ts) + 1>
{
    // No further implementation.
};

template <size_t index, KirHut::UIntegral Block_T, KirHut::BitFieldType Field_T, KirHut::BitFieldType... Field_Ts>
struct tuple_element<index, KirHut::BasicPackedTuple<Block_T, Field_T, Field_Ts...>>
{
    using type = KirHut::BasicPackedTuple<Block_T, Field_T, Field_Ts...>::template FieldType<index>;
};

} // namespace std
//! \endcond
