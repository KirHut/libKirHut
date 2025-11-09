/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/base.hpp
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

/*!
 * \file kh/base.hpp
 *
 * The KirHut Standard (or "Base") include file.
 *
 * This file provides all of the basic types and build information provided by kh/global.hpp, all the platform
 * information provided by kh/platform.hpp, and also includes several basic standard library types in the KirHut
 * namespace. This file also provides some string conversion functions and the Invalid and MaybeInv types. These are
 * used universally throughout KirHut software to prevent Exceptions where they are not needed.
 *
 * \see KirHut
 */

#include "kh/global.hpp" // IWYU pragma: export
#include "kh/platform.hpp" // IWYU pragma: export

#include <string>
#include <concepts>
#include <string_view>
#include <memory>
#include <array>
#include <bitset>
#include <span>
#include <utility>
#include <variant>
#include <optional>

#if not defined(KH_WINDOWS)
# include <cstring>
#endif

/*!
 * The primary namespace for all KirHut software, including libraries, applications, and plugins.
 *
 * This namespace is used everywhere in KirHut C++ software, and ensures that no name conflicts should ever occur when
 * mixing KirHut software with any other software, including software not within a namespace (Like Qt software). Within
 * this namespace there should be additional namespaces, primarily the application namespaces (which by convention are
 * simple abbreviations of the software name, like "PSM" for the KirHut Passwords and Secrets Manager or "DVT" for the
 * KirHut Data Verification Tool). There are other sub namespaces where functionality is provided, and they are
 * documented individually within this namespace.
 *
 * The KirHut namespace also import a selection of C++ standard library names. These names are usually aliased, but are
 * also frequently not, so it will depend on the situation and they are also individually documented. The ideal way to
 * use the KirHut namespace in code is to directly use the entire namespace, a 'la `using namespace KirHut`. The KirHut
 * libraries avoid the hazards of `using namespace std` because this does not include the entire standard library, and
 * name conflicts are far less likely when the standard names are aliased and limited to a curated subset of the
 * functionality.
 *
 * All KirHut software written in C++ requires C++20 as a base. The following standard identifiers are imported into the
 * KirHut namespace implicitly:
 * - std::byte
 * - std::array
 * - std::bit_cast
 * - std::get
 * - std::make_shared
 * - std::make_unique
 * - std::pair
 * - std::span
 * - std::string
 * - std::string_view
 * - std::tuple
 * - The entire std::literals::string_literals namespace
 * - The entire std::literals::string_view_literals namespace
 *
 * Ensure none of your identifiers conflict with the above standard identifiers when using the KirHut namespace.
 */
namespace KirHut
{

//! \cond
using std::array;
using std::bit_cast;
using std::bitset;
using std::get;
using std::make_shared;
using std::make_unique;
using std::pair;
using std::span;
using std::string;
using std::string_view;
using std::tuple;

using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;
//! \endcond

/*!
 * Alias name for std::unique_ptr.
 *
 * The smart pointer names are a bit long, so this makes it easier to type out and have in method signatures.
 */
template <typename T, typename Deleter_T = std::default_delete<T>>
using UPtr = std::unique_ptr<T, Deleter_T>;

/*!
 * Alias name for std::shared_ptr.
 *
 * \copydetails KirHut::UPtr<T,Deleter>
 */
template <typename T>
using SPtr = std::shared_ptr<T>;

/*!
 * Alias name for std::variant.
 *
 * This is used purely to shorten the name since this is common in method signatures.
 */
template <typename... Ts>
using Var = std::variant<Ts...>;

/*!
 * Alias name for std::optional.
 *
 * Slightly shorter name that is more descriptive of intent then "optional."
 */
template <typename T>
using Maybe = std::optional<T>;

/*!
 * Alias name for a std::span of std::string_views that are both const.
 *
 * This is a frequently used return value for a lot of objects so declaring this here makes it more convenient to use.
 */
using StringViews = span<string_view const> const;

/*!
 * Matching object name for Maybe to return an empty Maybe.
 *
 * This just is std::nullopt, which is the default return value for empty std::optional values.
 */
[[maybe_unused]] constexpr auto maybeNot = std::nullopt;

/*!
 * Single value enumeration type for allowing copying overloads.
 *
 * If you need to have two separate method or constructor overloads that are meant to either take a reference to one of
 * the passed argument types or alternatively copy the argument, this type can be used as an argument type for the
 * copying overload to signify to copy the value instead of just the reference to it.
 *
 * This is a distinct concept from Emplace or std::in_place as those type flags signify that a type is supposed to be
 * created "in place," which is different from making a copy of data.
 */
enum class CopyFlag
{
    copy = 1 //!< Value indicating that a function should use the copying overload.
};

/*!
 * \brief The RuntimeFlag enum
 */
enum class RuntimeFlag
{
    runtime = 1 //!< Value indicating that a check or action should be done at runtime rather than compile time.
};

/*!
 * Alias name for the std::in_place_t type.
 *
 * This allows using a more natural type name for passing the make flag than "std::in_place_t". An "emplace flag" is
 * really just std::in_place, which is a simple one-byte type that just provides an easy way to select a particular
 * method or function override, just like std::in_place_t does.
 */
using EmplaceFlag = std::in_place_t;

/*!
 * Alias name for the std::in_place_type_t type.
 *
 * This allows using a more natural type name for passing the make flag than "std::in_place_type_t". A "type make flag"
 * is really just std::in_place_type<T>, which is a simple one-byte type that just provides an easy way to select a
 * particular template method or function override, just like std::in_place_type_t does.
 */
template <typename T>
using TypedEmplaceFlag = std::in_place_type_t<T>;

namespace Flags
{

/*!
 * A flag type that is used to select the "copy" override of a particular method, usually a constructor.
 *
 * What a "copy override" means, and when this is appropriate to use for your methods or constructors, is dependent on
 * what you are trying to do. If you want to allow a class to construct a type internally or take a reference as an
 * argument, you could use this type to signify that it should create a new instance instead of taking on the reference
 * itself. For example, you could also use it to signify between directly using a string_view or copying the string
 * data.
 *
 * This is just CopyFlag::copy, in a more intuitive location.
 */
[[maybe_unused]] constexpr CopyFlag copy = CopyFlag::copy;

/*!
 * \brief runtime
 */
[[maybe_unused]] constexpr RuntimeFlag runtime = RuntimeFlag::runtime;

/*!
 * A flag type that is used to select the "emplace" override of a particular method, usually a constructor.
 *
 * What an "emplace override" means, and when this is appropriate to use for your methods or constructors, is dependent
 * on what you are trying to do. If you want to allow a template class to construct a type internally, you could use
 * this type to signify that it should create a new internal type "in-place" rather than requiring a copy or move
 * construction (for example). You could also use this to signify that a collection of types should be built "in-place"
 * rather than copy constructed.
 *
 * This is just an alias for std::in_place, which takes on the same role in the standard library as it does in
 * libKirHut. This name is mostly for convenience sake, to have a type to use under the KirHut namespace. You could also
 * simply pass std::in_place anywhere that a KirHut::EmplaceFlag is accepted, or pass emplace to any standard method
 * that takes a std::in_place_t.
 */
[[maybe_unused]] constexpr EmplaceFlag emplace = std::in_place;

/*!
 * A flag type that is used to select the "typed make" override of a particular method, usually a constructor.
 *
 * What a "typed make override" means, and when this is appropriate to use for your methods or constructors, is
 * dependent on what you are trying to do. If you want to allow a template class to contain any given type internally or
 * to take custom types in given methods, you could use this type to signify that it should create a new instance
 * internally instead of requiring a move construction (for example). You could also use this to signify that a
 * collection of types should be built "in-place" rather than copy constructed.
 *
 * This is just an alias for std::in_place_type<T>, which takes on the same role in the standard library as it does in
 * libKirHut. This name is mostly for convenience sake, to have a type to use under the KirHut namespace. You could also
 * simply pass std::in_place_type<T> anywhere that a KirHut::TypedEmplaceFlag<T> is accepted, or pass typeEmplace to
 * any standard method that takes a std::in_place_type_t<T>.
 */
template <typename T>
[[maybe_unused]] constexpr TypedEmplaceFlag<T> typeEmplace = std::in_place_type<T>;

} // namespace Flags

/*!
 * Constant expression of the number of bytes needed to hold a given number of bits.
 *
 * For basically all processors supported by KirHut, the value is just the number passed in divided by 8, plus one if
 * there are any additional bits. This is because nearly all processors use an 8 bit byte. There is the extremely vague
 * possibility of using something with a differently sized number of bytes, so this function protects against that,
 * while also labeling the purpose of some given magic number.
 *
 * \param numBits The number of bits that need to fit in the number of returned bytes.
 * \return The number of bytes that will completely contain the number of bits passed as \p numBits.
 */
[[nodiscard]] constexpr ue32 bytesNeededForBits(ue32 numBits) noexcept
{
    return (numBits + Platform::bitsInByte - 1) / Platform::bitsInByte;
}

/*!
 * Convert any type into a std::span view of the underlying byte data.
 *
 * This allow for viewing the underlying data of any \p object passed to this function. This can be useful for
 * serialization, generic identification, or other purposes needing the data of an \p object itself.
 *
 * This is just a simple wrapper around the std::as_bytes or as_writable_bytes functions. As such, it is impossible to
 * implement these functions as constexpr.
 *
 * \param object Any value or object type whatsoever.
 * \return A std::span viewing the underlying byte data of the \p object.
 */
template <typename T>
inline span<byte const, sizeof(T)> asBytes(T const &object) noexcept
{
    return std::as_bytes(span<T const, 1>{ &object, &object + 1 });
}

/*!
 * \copydoc asBytes(T const&)
 */
template <typename T>
inline span<byte, sizeof(T)> asWritableBytes(T &object) noexcept
{
    return std::as_writable_bytes(span<T, 1>{ &object, &object + 1 });
}

/*!
 * Concept representing a "numeric" type, which means either an integer literal or a floating point value.
 *
 * This matches with any type that returns true from std::is_arithmetic_v<T>, with the exception of the bool type, as
 * bools are not considered "numbers" for these purposes. Bool types tend to be special cases in a lot of contexts so it
 * is better to remove them.
 */
template <typename T>
concept Numeric = std::is_arithmetic_v<T> and not std::is_same_v<T, bool>;

/*!
 * Concept to identify if a type is one of a set of distinct types.
 *
 * There are times when you just want a concept to a specific fixed set of types you want to accept, but don't want to
 * just make separate overloads of that function for each type. This concept simplifies taking those arguments, and you
 * can easily define other concepts in terms of this concept. You can also check the return types of expressions to
 * ensure that they return one of a distinct set of types.
 */
template <typename Tested_T, typename First_T, typename... Rest_Ts>
concept OneOf = std::same_as<Tested_T, First_T> or (std::same_as<Tested_T, Rest_Ts> or ...);

/*!
 * Concept to identify the different types that may bypass strict aliasing rules in C++.
 *
 * Some types are not undefined behavior to dereference from a different type. Those three types are considered "byte
 * types" in KirHut software. Obviously, those three types are `char`, `unsigned char`, and std::byte.
 */
template <typename Byte_T>
concept ByteType = OneOf<std::remove_cv_t<Byte_T>, char, byte, unsigned char, std::byte>;

/*!
 * A constexpr version of the std::abs function that is free of UB.
 *
 * For some inexplicable reason, the std::abs() function is not constexpr until C++23, and this library needs to provide
 * support for C++20. As such, a constexpr abs function is provided by this library directly, but unlike the C++
 * std::abs() function, if the result of the absolute transformation cannot be represented by the return value, the
 * original value passed in as \p number is returned instead. This gives at least some kind of defined behavior in that
 * case. In short, this means if \p number is Limits<decltype(number)>::min(), it will not convert to the positive
 * value, but only in that case.
 *
 * This version of abs should not accept passing an unsigned integer type as \p number at all, unlike the std::abs()
 * function overloads, which will perform integer promotions in that case. The failure to compile behavior of this
 * template function is considered desireable and is retained.
 *
 * \param number A signed integer to get the absolute value of (or to return as-is if this is impossible).
 * \return The absolute value of \p number, unless this is impossible to represent, then \p number.
 */
[[nodiscard]] constexpr auto abs(std::signed_integral auto number) noexcept -> decltype(number)
{
    return number < 0 and number != Limits<decltype(number)>::min() ? -number : number;
}

/*!
 * A version of abs that returns an unsigned value instead of a signed value, and is free of UB.
 *
 * For some inexplicable reason, the std::abs() function returns a signed integer, even though by all logical accounts,
 * the abs() function always returns an unsigned value. This makes it so the edge case of Limits<int>::min() is not
 * converted at all, so you end up needing to check an edge case when you really just wanted an unsigned value in the
 * first place. This is guaranteed to always work, even with min(), and always works correctly on all standard C++
 * compilers and environments.
 *
 * Because this library supports the standard syntax, KirHut::abs() also returns a signed integer, and will simply not
 * convert the value when Limits<int>::min() is passed. If you would prefer to always get an unsigned value regardless
 * of what signed value is passed, use this function instead. This function should almost always be preferred over abs()
 * unless you have some genuine requirement to get a signed integer value.
 *
 * Unlike abs(), this function is designed to support unsigned integers as well, simply returning them as passed. This
 * function is designed to prevent integer promotions in that case. KirHut::uabs() is what std::abs() should have been.
 *
 * \param number An integer to get the unsigned absolute value of.
 * \return The absolute value of \p number, as an unsigned value.
 */
[[nodiscard]] constexpr auto uabs(std::signed_integral auto number) noexcept -> std::make_unsigned_t<decltype(number)>
{
    using Ret_T = std::make_unsigned_t<decltype(number)>;
    return number < 0 ? static_cast<Ret_T>(KirHut::abs(number + 1)) + 1u : static_cast<Ret_T>(number);
}

/*!
 * \copydoc KirHut::uabs(std::signed_integral auto)noexcept
 */
[[nodiscard]] constexpr auto uabs(std::unsigned_integral auto number) noexcept -> decltype(number)
{
    return number;
}

/*!
 * Perform a bit shift of a given mask to a given bit location, but without UB risk.
 *
 * In C and C++, if you perform a bitwise left shift to a bit location beyond the size in bits of the integer you are
 * modifying, or you use a negative value for the shift amount, the behavior is undefined. This is fine if you are very
 * conscious of what values are passed to the bit shift operators, but in most cases it can be much safer to use
 * something that has guaranteed defined behavior when a less-than expected value is passed. The maskAt function is just
 * such a function: If \p location is greater than the number of bits in \p mask, than the "bit shift operation" simply
 * results in 0. If \p location is negative, it shifts to the right instead and uses the same "zeroing" technique, but
 * otherwise behaves as a positive \p location.
 *
 * "Oh, so it's like a shitty std::rotl?"
 *
 * Well, kind of, but no. The std::rotl() function *rotates* a set of bits, whereas this function **only shifts** the
 * bits, it does not rotate them. Sometimes this is preferred over a rotation, when you want bits to fall off instead of
 * appearing in the lower bits.
 *
 * This function is constexpr like std::rotl() so that it may be used at compile time.
 *
 * \param mask The bitmask to apply the bit shift operation to.
 * \param location The number of bits to shift left (or right if negative).
 * \return The \p mask shifted to the desired \p location.
 */
[[nodiscard]] constexpr auto maskAt(std::unsigned_integral auto mask, int location) noexcept -> decltype(mask)
{
    if (auto shift = uabs(location); shift < sizeof(mask) * Platform::bitsInByte)
    {
        return location < 0 ? mask >> shift : mask << shift;
    }

    return 0;
}

/*!
 * Constant expression byte swapping function that should always do the most efficient thing.
 *
 * The byteSwap function in libKirHut is meant to work like std::byteswap in C++23 for applications still only using
 * C++20. When this library is compiled by a C++23 or newer compiler, this function is just a wrapper for std::byteswap.
 * Otherwise, it uses one of the builtin byte swap functions in your according compiler, or if it lacks a builtin, it
 * uses a fallback implementation that is constexpr safe. The fallback implementation is deliberately designed to allow
 * the compiler to generate hardware instructions to perform a byte swap if the hardware has it, even if the compiler
 * lacks a builtin for it.
 *
 * \param bytes An unsigned 16 bit integer you want to have the bytes swapped in.
 * \return The same 16 bit integer passed as \p bytes, but with the bytes swapped.
 */
[[nodiscard]] constexpr u16 byteSwap(u16 bytes)
{
#if defined(__cpp_lib_byteswap)
    return std::byteswap(bytes);
#endif

    if (not std::is_constant_evaluated())
    {
#if defined(KH_COMPILED_WITH_MSVC)
        return _byteswap_ushort(bytes);
#elif __has_builtin(__builtin_bswap16)
        return __builtin_bswap16(bytes);
#endif
    }

    constexpr int shiftAmount = Platform::bitsInByte;
    return (bytes >> shiftAmount) | (bytes << shiftAmount);
}

/*!
 * Constant expression byte swapping function that should always do the most efficient thing.
 *
 * The byteSwap function in libKirHut is meant to work like std::byteswap in C++23 for applications still only using
 * C++20. When this library is compiled by a C++23 or newer compiler, this function is just a wrapper for std::byteswap.
 * Otherwise, it uses one of the builtin byte swap functions in your according compiler, or if it lacks a builtin, it
 * uses a fallback implementation that is constexpr safe. The fallback implementation is deliberately designed to ensure
 * that the compiler generates hardware instructions to perform a byte swap if the hardware has it, even if the compiler
 * lacks a builtin for it.
 *
 * \param bytes An unsigned 32 bit integer you want to have the bytes swapped in.
 * \return The same 32 bit integer passed as \p bytes, but with the bytes swapped.
 */
[[nodiscard]] constexpr u32 byteSwap(u32 bytes)
{
#if defined(__cpp_lib_byteswap)
    return std::byteswap(bytes);
#endif

    if (not std::is_constant_evaluated())
    {
#if defined(KH_COMPILED_WITH_MSVC)
        return _byteswap_ulong(bytes);
#elif __has_builtin(__builtin_bswap32)
        return __builtin_bswap32(bytes);
#endif
    }

    constexpr int outerShift = Platform::bitsInByte * 3;
    constexpr int innerShift = Platform::bitsInByte * 1;
    constexpr u32 bitMask    = 0xFF;

    // clang-format off
    return ((bytes & maskAt(bitMask, Platform::bitsInByte * 0)) << outerShift) |
           ((bytes & maskAt(bitMask, Platform::bitsInByte * 1)) << innerShift) |
           ((bytes & maskAt(bitMask, Platform::bitsInByte * 2)) >> innerShift) |
           ((bytes & maskAt(bitMask, Platform::bitsInByte * 3)) >> outerShift);
    // clang-format on
}

/*!
 * Constant expression byte swapping function that should always do the most efficient thing.
 *
 * The byteSwap function in libKirHut is meant to work like std::byteswap in C++23 for applications still only using
 * C++20. When this library is compiled for C++23 or newer, this function is just a wrapper for std::byteswap.
 * Otherwise, it uses one of the builtin byte swap functions in your respective compiler, or if it lacks a builtin, it
 * uses a fallback implementation that is constexpr safe. The fallback implementation is deliberately designed to ensure
 * that the compiler generates hardware instructions to perform a byte swap if the hardware has it, even if the compiler
 * lacks a builtin for it.
 *
 * \param bytes An unsigned 64 bit integer you want to have the bytes swapped in.
 * \return The same 64 bit integer passed as \p bytes, but with the bytes swapped.
 */
[[nodiscard]] constexpr u64 byteSwap(u64 bytes)
{
#if defined(__cpp_lib_byteswap)
    return std::byteswap(bytes);
#endif

    if (not std::is_constant_evaluated())
    {
#if defined(KH_COMPILED_WITH_MSVC)
        return _byteswap_uint64(bytes);
#elif __has_builtin(__builtin_bswap64)
        return __builtin_bswap64(bytes);
#endif
    }

    constexpr int outerShift    = Platform::bitsInByte * 7;
    constexpr int midOuterShift = Platform::bitsInByte * 5;
    constexpr int midInnerShift = Platform::bitsInByte * 3;
    constexpr int innerShift    = Platform::bitsInByte * 1;
    constexpr u64 bitMask       = 0xFF;

    // clang-format off
    return ((bytes & maskAt(bitMask, Platform::bitsInByte * 0)) << outerShift)    |
           ((bytes & maskAt(bitMask, Platform::bitsInByte * 1)) << midOuterShift) |
           ((bytes & maskAt(bitMask, Platform::bitsInByte * 2)) << midInnerShift) |
           ((bytes & maskAt(bitMask, Platform::bitsInByte * 3)) << innerShift)    |
           ((bytes & maskAt(bitMask, Platform::bitsInByte * 4)) >> innerShift)    |
           ((bytes & maskAt(bitMask, Platform::bitsInByte * 5)) >> midInnerShift) |
           ((bytes & maskAt(bitMask, Platform::bitsInByte * 6)) >> midOuterShift) |
           ((bytes & maskAt(bitMask, Platform::bitsInByte * 7)) >> outerShift);
    // clang-format on
}

#if KH_USE_128BIT_TYPES or KH_PRIV_DOCS
/*!
 * Constant expression byte swapping function that supports 128 bit types.
 *
 * The byteSwap function in libKirHut is meant to work like std::byteswap in C++23 for applications still only using
 * C++20. When this library is compiled by a C++23 or newer compiler, this function is just a wrapper for std::byteswap.
 * Otherwise, it attempts to use a builtin byte swap functions in GCC or Clang, or if it lacks a builtin, it uses a
 * fallback implementation that is constexpr safe. The fallback implementation just splits the u128 into two u64s and
 * performs byteSwap on them, and returning those u64s swapped.
 *
 * \param bytes An unsigned 128 bit integer you want to have the bytes swapped in.
 * \return The same 128 bit integer passed as \p bytes, but with the bytes swapped.
 */
[[nodiscard]] constexpr u128 byteSwap(u128 bytes)
{
# if defined(__cpp_lib_byteswap)
    // Extra if constexpr to ensure that the library byteswap supports 128-bit integers. It should on both GCC and
    // Clang. Check the return type to make sure it isn't just doing a narrowing conversion.
    if constexpr (requires {
                      { std::byteswap(bytes) } -> std::same_as<u128>;
                  })
    {
        return std::byteswap(bytes);
    }
# endif

    if (not std::is_constant_evaluated())
    {
# if __has_builtin(__builtin_bswap128)
        return __builtin_bswap128(bytes);
# endif
    }

    auto [firstHalf, lastHalf] = pair{ static_cast<u64>(bytes >> Platform::bitsInU64), static_cast<u64>(bytes) };
    return (static_cast<u128>(byteSwap(lastHalf)) << Platform::bitsInU64) | byteSwap(firstHalf);
}
#endif

namespace Detail
{

/*!
 * \internal
 *
 * Default wrong type base template for InstanceOfValue.
 *
 * Credit to Lawrence Murray for this InstanceOfValue implementation to determine if a type is a given template type.
 * I just did not possess the profound galaxy brain necessary to concoct this monstrosity. Thank you!
 * https://indii.org/blog/is-type-instantiation-of-template/
 *
 * I extend this idea to other template identifiers, but I did not invent this at all.
 */
template <typename Wrong_T, template <typename...> typename Template_T>
constexpr bool InstanceOfValue = false;

/*!
 * \internal
 *
 * Correct type template specialization for InstanceOfValue.
 *
 * Credit to Lawrence Murray for this InstanceOfValue implementation to determine if a type is a given template type.
 * I just did not possess the profound galaxy brain necessary to concoct this monstrosity. Thank you!
 * https://indii.org/blog/is-type-instantiation-of-template/
 *
 * I extend this idea to other template identifiers, but I did not invent this at all.
 */
template <template <typename...> typename Template_T, typename... Arg_Ts>
constexpr bool InstanceOfValue<Template_T<Arg_Ts...>, Template_T> = true;

/*!
 * \internal
 *
 * InstanceOf implementation concept.
 *
 * This is just a concept wrapper for the InstanceOfValue template trait.
 */
template <typename Test_T, template <typename...> typename Template>
concept InstanceOf = InstanceOfValue<std::remove_cvref_t<Test_T>, Template>;

/*!
 * \internal
 *
 * Default wrong span type (or wrong type in general) base template for SpanTypeValue.
 *
 * This models the same methodology used for InstanceOfValue, except the specialization is used specifically to find
 * span types.
 */
template <typename Wrong_T>
constexpr bool SpanTypeValue = false;

/*!
 * \internal
 *
 * Correct span type template specialization for SpanTypeValue.
 *
 * This models the same methodology used for InstanceOfValue, except the specialization is used specifically to find
 * span types. Note that this does NOT accept cv-qualified spans nor span references. This is deliberate, since it is
 * always possible to apply these qualifiers to a templated type afterward and there is value in the user being able to
 * distinguish these when desired.
 */
template <typename Element_T, size_t extent>
constexpr bool SpanTypeValue<span<Element_T, extent>> = true;

/*!
 * \internal
 *
 * SpanType implementation concept.
 *
 * This is just a concept wrapper for the SpanTypeValue template trait.
 */
template <typename Test_T>
concept SpanType = SpanTypeValue<Test_T>;

/*!
 * \internal
 *
 * A dummy class used by the EmptyClass concept to compare with another object with an EBO parent class.
 *
 * If the EboChild object is of equal size to this object after inheriting from a possibly EBO class, it proves that the
 * candidate EBO class is actually empty (and not merely containing a single byte like a char or a bool member).
 */
struct NoChild final
{
    int val = 0; //!< A single integer value to give NoChild a predictable size.
};

/*!
 * \internal
 *
 * A dummy class used by the EmptyClass concept to compare with another object that has no parent class.
 *
 * If the NoChild object is of equal size to this object after inheriting from a possibly EBO class, it proves that the
 * candidate EBO class is actually empty (and not merely containing a single byte like a char or a bool member).
 */
template <typename EBO>
struct EboChild final : public EBO
{
    int val = 0; //!< A single integer value to give EboChild a predictable size.
};

/*!
 * \internal
 *
 * \brief The EboFinalChild class
 */
template <typename EBO>
struct EboFinalChild final
{
    int val = 0;
    [[KH_ATTR_NO_UNIQUE_ADDRESS]] EBO ebo;
};

/*!
 * \internal
 *
 * \brief emptyTest
 * \return
 */
template <typename Class_T>
consteval bool emptyTest()
{
    using Normalized = std::remove_cv_t<Class_T>;
    if constexpr (std::is_final_v<Normalized>)
    {
        return sizeof(Detail::NoChild) == sizeof(Detail::EboFinalChild<Normalized>);
    }
    else
    {
        return sizeof(Detail::NoChild) == sizeof(Detail::EboChild<Normalized>);
    }
}

/*!
 * \internal
 *
 * Actual implementation function for all the fromBigEndian/fromLittleEndian public functions.
 *
 * This function has two separate behavior branches for constant evaluation vs non-constant evaluation, so it must be
 * tested under both conditions.
 *
 * \param source
 * \tparam Num_T
 * \tparam sourceEndianness
 * \return
 */
template <Numeric Num_T, std::endian sourceEndianness>
[[nodiscard]] constexpr Num_T fromEndian(ByteType auto const *source) noexcept
{
    if constexpr (sizeof(Num_T) == sizeof(byte))
    {
        return std::bit_cast<Num_T>(*source);
    }

    ExactUIntOf<Num_T> ret = 0;
    if (std::is_constant_evaluated())
    {
        constexpr bool fromBig = sourceEndianness == std::endian::big;
        for (size_t i = 0; i < sizeof(Num_T); ++i)
        {
            // We do a bit_cast to unsigned first because the bytes in source may be signed, so static_cast could change
            // the binary representation if we don't first bit_cast to u8.
            auto temp = std::bit_cast<u8>(fromBig ? source[sizeof(Num_T) - 1 - i] : source[i]);
            ret |= static_cast<ExactUIntOf<Num_T>>(temp) << (i * Platform::bitsInByte);
        }
    }
    else
    {
        memcpy(&ret, source, sizeof(Num_T));

        if constexpr (sourceEndianness != Platform::Endianness)
        {
            ret = byteSwap(ret);
        }
    }

    return std::bit_cast<Num_T>(ret);
}

/*!
 * \internal
 *
 * \brief toEndian
 * \param dest
 * \return
 */
template <std::endian destEndianness>
constexpr auto toEndian(Numeric auto value, ByteType auto *dest) noexcept -> decltype(dest)
{
    using Byte_T = std::remove_pointer_t<decltype(dest)>;
    static_assert(not std::is_const_v<Byte_T>, "toEndian needs a non-const array to write to.");

    constexpr auto returnTypeSize = sizeof(decltype(value));

    if constexpr (returnTypeSize == sizeof(byte))
    {
        *dest = std::bit_cast<byte>(value);
        return ++dest;
    }

    auto bits = std::bit_cast<ExactUInt<returnTypeSize>>(value);
    if (std::is_constant_evaluated())
    {
        constexpr bool toBig = destEndianness == std::endian::big;
        for (size_t i = 0; i < returnTypeSize; ++i)
        {
            auto temp = static_cast<u8>(bits >> (i * Platform::bitsInByte));

            dest[toBig ? returnTypeSize - 1 - i : i] = std::bit_cast<Byte_T>(temp);
        }
    }
    else
    {
        if constexpr (destEndianness != Platform::Endianness)
        {
            bits = byteSwap(bits);
        }

        memcpy(dest, &bits, returnTypeSize);
    }

    return dest + returnTypeSize;
}

/*!
 * \internal
 *
 * \brief throwTooSmallSpan
 * \param message
 */
[[noreturn]] KH_EXPORT void throwTooSmallSpan(string_view message);

/*!
 * \internal
 *
 * \brief convertTest
 * \param args
 * \return
 */
template <typename Tested_T, typename... Arg_Ts>
constexpr Tested_T convertTest(Arg_Ts &&...args)
{
    return { std::forward<Arg_Ts>(args)... };
}

/*!
 * \internal
 *
 * \brief varIndexLoop
 * \return
 */
template <typename Var_T, typename T, size_t indexPos>
[[nodiscard]] consteval size_t varIndexLoop()
{
    using UnqualVar = std::remove_cvref_t<Var_T>;

    if constexpr (indexPos >= std::variant_size_v<UnqualVar>)
    {
        return indexPos;
    }
    else
    {
        if constexpr (std::is_same_v<T, std::variant_alternative_t<indexPos, UnqualVar>>)
        {
            return indexPos;
        }
        else
        {
            return varIndexLoop<Var_T, T, indexPos + 1>();
        }
    }
}

/*!
 * \internal
 *
 * \brief varIndex
 * \return
 */
template <typename Var_T, typename T>
[[nodiscard]] consteval size_t varIndex()
{
    static_assert(InstanceOf<Var_T, Var> or InstanceOf<Var_T, std::variant>);
    return varIndexLoop<Var_T, T, 0>();
}

/*!
 * \internal
 *
 * HasTypeOption implementation concept.
 *
 * Remember that this implementation is using Detail::InstanceOf, not KirHut::InstanceOf. As such, this cannot use
 * `InstanceOf<Var_T, Var, std::variant>` because Detail::InstanceOf does not check for multiple separate template
 * template types.
 */
template <typename Var_T, typename Contain_T>
concept HasTypeOption = (InstanceOf<Var_T, Var> or InstanceOf<Var_T, std::variant>) and
                        varIndex<Var_T, Contain_T>() < std::variant_size_v<std::remove_reference_t<Var_T>>;

} // namespace Detail

/*!
 * Concept to identify if a type can be converted to one of a set of distinct types.
 *
 * This is just the std::convertible_to concept applied to multiple types instead of one, so you are not forced to make
 * a long chain of std::convertible_to concepts or std::is_convertible_to_v, and can instead just use this concept to
 * get the same power.
 */
template <typename Tested_T, typename First_T, typename... Rest_Ts>
concept ConvertsTo = std::convertible_to<Tested_T, First_T> or (std::convertible_to<Tested_T, Rest_Ts> or ...);

/*!
 * Concept to identify if a type is one of a set of distinct template types.
 *
 * This is similar to the IsOneOf concept with non-templated types, but matches with any given template type. This
 * allows matching against a given set of template types, without needing a bunch of template specializations or
 * overloads. This concept simplifies taking template arguments, and you can easily define other concepts in terms of
 * this concept.
 *
 * \note Important: This concept cannot be used to identify a template type that has any non-type template parameters!
 * As such, you cannot use this concept to identify template types like std::span or std::array, and must instead use
 * alternative concepts in this library for that.
 */
template <typename Tested_T, template <typename...> typename Template_T, template <typename...> typename... Template_Ts>
concept InstanceOf = Detail::InstanceOf<Tested_T, Template_T> or (Detail::InstanceOf<Tested_T, Template_Ts> or ...);

/*!
 * Concept to identify types that are std::span of a given set of Element_Ts, or a writable span if no arguments are
 * given.
 *
 * This is similar to InstanceOf in that it is for identifying a std::span template type, however it only works for
 * std::span and InstanceOf cannot work because it only works with types that have a set of typenames they take as
 * parameters, concrete parameters like size_of break InstanceOf. This is the workaround, but unlike InstanceOf, this
 * concept accepts no elements and will match with any std::span whatsoever. This is useful to remove the need to
 * explicitly accept an Extent parameter in abbreviated template functions and methods.
 */
template <typename Tested_T, typename... Element_Ts>
concept SpanOf = Detail::SpanType<Tested_T> and (not std::is_reference_v<Element_Ts> and ...) and
                 ((sizeof...(Element_Ts) == 0 and not std::is_const_v<typename Tested_T::element_type>) or
                  (OneOf<typename Tested_T::element_type, Element_Ts> or ...));

/*!
 * Concept to identify types that are a std::span of a given set of Element_Ts, ignoring cv-qualifications.
 */
template <typename Tested_T, typename... Element_Ts>
concept ReadableSpanOf =
    Detail::SpanType<Tested_T> and (std::same_as<Element_Ts, std::remove_cvref_t<Element_Ts>> and ...) and
    (sizeof...(Element_Ts) == 0 or (OneOf<std::remove_cv_t<typename Tested_T::element_type>, Element_Ts> or ...));

/*!
 * Concept to identify a span of some kind of ByteType.
 *
 * The ByteType concept is useful for detecting particular types, but when you are specifically trying to get a span to
 * one of those types, this concept is a better fit for that purpose. Like SpanOf, this works despite the fact that
 * InstanceOf does not work for types that contain non-type template parameters, namely size_t.
 */
template <typename Span_T>
concept ByteSpan = SpanOf<Span_T, char, byte, unsigned char, std::byte>;

/*!
 * Concept to idenfiy a span of some kind of constant ByteType.
 *
 * The ByteType concept is useful for detecting particular types, but when you are specifically trying to get a span to
 * one of those types, this concept is a better fit for that purpose. Like SpanOf, this works despite the fact that
 * InstanceOf does not work for types that contain non-type template parameters, namely size_t.
 */
template <typename Span_T>
concept ReadableByteSpan = ReadableSpanOf<Span_T, char, byte, unsigned char, std::byte>;

/*!
 * Concept to identify "character" types, as specified by the C++ standard.
 *
 * There are five character types in C++: `char`, `wchar_t`, `char8_t`, `char16_t`, and `char32_t`. The `signed char`
 * and `unsigned char` types are actually integer types instead of character types, and are distinct from `char`. This
 * concept is useful when you are trying to ensure you get something that is specifically a character.
 */
template <typename Char_T>
concept CharType = OneOf<std::remove_cv_t<Char_T>, char, wchar_t, char8_t, char16_t, char32_t>;

/*!
 * Concept to identify a type that can be converted to some kind of standard string_view type.
 *
 * There are a lot of times when this concept is extremely helpful to get certain things to build, namely when you are
 * taking a std::basic_string_view template as an argument and you want to accept types like char const * and
 * std::string without having an explicit constructor.
 */
template <typename String_T>
concept StringLike = ConvertsTo<String_T,
                                std::string_view,
                                std::wstring_view,
                                std::u8string_view,
                                std::u16string_view,
                                std::u32string_view>;

namespace Detail
{

/*!
 * \internal
 *
 * \brief The StringLikeTraits class
 */
template <StringLike String_T>
struct StringLikeTraits
{
    /*!
     * \internal
     */
    using type = typename String_T::value_type;
};

/*!
 * \internal
 *
 * \brief The StringLikeTraits class
 */
template <CharType Char_T>
struct StringLikeTraits<Char_T *>
{
    /*!
     * \internal
     */
    using type = std::remove_cv_t<Char_T>;
};

} // namespace Detail

/*!
 * Template alias of the character type used in a type that matches the StringLike concept.
 *
 * This extracts the type correctly from both string-like objects (with a value_type type alias member) and from
 * character arrays equally well. This allows for you to extract the character type data in your concepts or deduction
 * guides easily.
 */
template <StringLike String_T>
using StringLikeType = typename Detail::StringLikeTraits<String_T>::type;

/*!
 * Concept that identifies some Var or std::variant object that contains one of the given HasTypes.
 *
 * This matches with types of std::variant (or the Var alias) that contain a given HasType as one of its type options.
 * This has a separated HasType and HasTypes types in the template because in abbreviated template method signatures or
 * when applying this concept to requires clauses, having an empty set of HasTypes should fail to compile, and the
 * separate HasType ensures that happens.
 */
template <typename Var_T, typename Has_T, typename... Has_Ts>
concept HasTypeOption = Detail::HasTypeOption<Var_T, Has_T> or (Detail::HasTypeOption<Var_T, Has_Ts> or ...);

/*!
 * Concept that identifies an "empty" class, or a type that, when used as a parent class, will not increase the size of
 * the object.
 *
 * This basically allows easy identification of types that can have the Empty Base Optimization applied to them or used
 * in a [[KH_ATTR_NO_UNIQUE_ADDRESS]] context.
 */
template <typename Class_T>
concept EmptyClass = not std::is_reference_v<Class_T> and sizeof(Class_T) == 1 and Detail::emptyTest<Class_T>();

/*!
 * Concept representing a type Has_T that can be contained in a given Var_T (Var or std::variant).
 *
 * This matches with types that are accepted by the std::variant (or the Var alias) given as type Var_T as one of its
 * type options. This will be true if Has_T is a type option of Var_T.
 */
template <typename Has_T, typename Var_T>
concept TypeOptionOf = Detail::HasTypeOption<Var_T, Has_T>;

/*!
 * \brief varIndex
 * \param var
 * \return
 */
template <typename Var_T>
[[nodiscard]] consteval size_t varIndex(InstanceOf<Var, std::variant> auto const &var)
{
    return Detail::varIndex<decltype(var), Var_T>();
}

/*!
 * Read the data pointed at by \p source and return it as the given T integer type for the native platform.
 *
 * This method should be used any time you need to read a big endian integer value from a source of bytes, such as from
 * an ethernet frame or a data file. On both big endian and little endian systems, this function will return the correct
 * value that is represented by the data pointed to by \p source, read as a big endian integer.
 *
 * Using this function is as simple as designating the desired Numeric return type, and then providing a pointer to a
 * buffer of bytes. The buffer of bytes will be interpreted as a big endian value of type T and returned to you.
 *
 * ~~~
 * auto result = fromBigEndian<i32>(buffer);
 * static_assert(std::is_same_v<decltype(result), i32>);
 * ~~~
 *
 * \warning It is undefined behavior to provide a pointer to a byte buffer that is not at least sizeof(T) bytes large.
 * This will cause a read to occur in invalid memory and so you should always check that there are sufficient bytes in
 * the buffer area for conversion.
 *
 * \param source A pointer to a buffer of bytes at least sizeof(T) large.
 * \return The requested Numeric T type.
 */
template <Numeric T>
[[nodiscard]] constexpr T fromBigEndian(ByteType auto const *source) noexcept
{
    return Detail::fromEndian<T, std::endian::big>(source);
}

/*!
 * Read the data in the first sizeof(T) bytes contained in the \p source span and return it as the given T integer type
 * for the native platform.
 *
 * This method should be used any time you need to read a big endian integer value from a given \p source buffer. This
 * version of fromBigEndian should be favored over the pointer version as this version is guaranteed never to result in
 * undefined behavior. In the case of a buffer being too small at runtime, this function will throw an exception.
 *
 * Using this function is as simple as designating the desired Numeric return type, and then providing a span to a
 * buffer of bytes. The first sizeof(T) bytes in the buffer will be interpreted as a big endian value of type T and
 * returned to you.
 *
 * ~~~
 * auto result = fromBigEndian<double>(bufferSpan);
 * static_assert(std::is_same_v<decltype(result), double>);
 * ~~~
 *
 * \param source A span to a buffer of bytes at least sizeof(T) large.
 * \return The requested Numeric T type.
 */
template <Numeric T, ByteType Byte_T, size_t size>
[[nodiscard]] constexpr T fromBigEndian(span<Byte_T const, size> source) noexcept(size != std::dynamic_extent)
    requires(size >= sizeof(T))
{
    if constexpr (size == std::dynamic_extent)
    {
        if (source.size() < sizeof(T))
        {
            Detail::throwTooSmallSpan("Source span is too small.");
        }
    }

    return Detail::fromEndian<T, std::endian::big>(source.data());
}

/*!
 * \copydoc fromBigEndian(span<Byte_T const,size>)
 */
template <Numeric T, ByteType Byte_T, size_t fixedSize>
[[nodiscard]] constexpr T fromBigEndian(span<Byte_T, fixedSize> source) noexcept(fixedSize != std::dynamic_extent)
    requires(fixedSize >= sizeof(T))
{
    return fromBigEndian<T>(span<Byte_T const, fixedSize>{ source });
}

/*!
 * \brief fromLittleEndian
 * \param source
 * \return
 */
template <Numeric T>
[[nodiscard]] constexpr T fromLittleEndian(ByteType auto const *source) noexcept
{
    return Detail::fromEndian<T, std::endian::little>(source);
}

/*!
 * \brief fromLittleEndian
 * \param source
 * \return
 */
template <Numeric T, ByteType Byte_T, size_t fixedSize>
[[nodiscard]] constexpr T fromLittleEndian(span<Byte_T const, fixedSize> source)
    noexcept(fixedSize != std::dynamic_extent) requires(fixedSize >= sizeof(T))
{
    if constexpr (fixedSize == std::dynamic_extent)
    {
        if (source.size() < sizeof(T))
        {
            Detail::throwTooSmallSpan("Source span is too small.");
        }
    }

    return Detail::fromEndian<T, std::endian::little>(source.data());
}

template <Numeric T, ByteType Byte_T, size_t fixedSize>
[[nodiscard]] constexpr T fromLittleEndian(span<Byte_T, fixedSize> source) noexcept(fixedSize != std::dynamic_extent)
    requires(fixedSize >= sizeof(T))
{
    return fromLittleEndian<T>(span<Byte_T const, fixedSize>{ source });
}

/*!
 * Accept a Numeric \p value of any kind, and write it to the provided \p dest byte buffer as a big endian value.
 *
 * \param value The numeric value (a signed or unsigned integer, or a floating point type) to write as big endian.
 * \param dest The buffer to write the big endian data for \p value.
 * \return A pointer to the first byte in \p dest that was **not** written to as a result of this call.
 */
constexpr auto toBigEndian(Numeric auto value, ByteType auto *dest) noexcept -> decltype(dest)
{
    static_assert(not std::is_const_v<decltype(*dest)>, "toBigEndian needs a non-const array to write to.");

    return Detail::toEndian<std::endian::big>(value, dest);
}

/*!
 * Accept a Numeric \p value of any kind, and return a std::array of \p value stored as a big endian value.
 *
 * \param value The numeric value (a signed or unsigned integer, or a floating point type) to return as big endian.
 * \return A std::array of bytes for \p value in big endian order.
 */
template <Numeric Num_T>
[[nodiscard]] constexpr array<byte, sizeof(Num_T)> toBigEndian(Num_T value) noexcept
{
    array<byte, sizeof(Num_T)> ret;
    Detail::toEndian<std::endian::big>(value, ret.data());
    return ret;
}

/*!
 * Accept a Numeric \p value of any kind, and write it to the provided \p dest span buffer as a big endian value.
 *
 * \param value The numeric value (a signed or unsigned integer, or a floating point type) to write as big endian.
 * \param dest The buffer to write the big endian data for \p value.
 * \throws IllegalArgument If the span passed to this method has std::dynamic_extent but size() < sizeof(value).
 */
template <ByteType Byte_T, size_t fixedSize>
constexpr void toBigEndian(Numeric auto value, span<Byte_T, fixedSize> dest) noexcept(fixedSize != std::dynamic_extent)
    requires(fixedSize >= sizeof(decltype(value)))
{
    static_assert(not std::is_const_v<Byte_T>, "toBigEndian needs a non-const span to write to.");

    if constexpr (fixedSize == std::dynamic_extent)
    {
        if (dest.size() < sizeof(decltype(value)))
        {
            Detail::throwTooSmallSpan("Destination span is too small for toBigEndian.");
        }
    }

    Detail::toEndian<std::endian::big>(value, dest.data());
}

/*!
 * \brief toLittleEndian
 * \param value
 * \param dest
 * \return A pointer to the first byte in \p dest that was **not** written to as a result of this call.
 */
constexpr auto toLittleEndian(Numeric auto value, ByteType auto *dest) noexcept -> decltype(dest)
{
    static_assert(not std::is_const_v<decltype(*dest)>, "toLittleEndian needs a non-const array to write to.");

    return Detail::toEndian<std::endian::little>(value, dest);
}

/*!
 * \brief toLittleEndian
 * \return
 */
template <Numeric Num_T>
[[nodiscard]] constexpr array<byte, sizeof(Num_T)> toLittleEndian(Num_T value) noexcept
{
    array<byte, sizeof(Num_T)> ret{};
    Detail::toEndian<std::endian::little>(value, ret.data());
    return ret;
}

/*!
 * \brief toLittleEndian
 * \param dest
 */
template <ByteType Byte_T, size_t fixedSize>
constexpr void toLittleEndian(Numeric auto value, span<Byte_T, fixedSize> dest)
    noexcept(fixedSize != std::dynamic_extent) requires(fixedSize >= sizeof(decltype(value)))
{
    static_assert(not std::is_const_v<Byte_T>, "toLittleEndian needs a non-const span to write to.");

    if constexpr (fixedSize == std::dynamic_extent)
    {
        if (dest.size() < sizeof(decltype(value)))
        {
            Detail::throwTooSmallSpan("Destination span is too small for toLittleEndian.");
        }
    }

    Detail::toEndian<std::endian::little>(value, dest.data());
}

/*!
 * Constexpr function that simply returns the compile-time size of the passed-in array.
 *
 * This function isn't useful in most cases with an array, but sometimes you take an array as an argument through an
 * abbreviated template or some other circumstance where you do not get the size of the array, but you still need to
 * have the size of the array as a compile-time value. This function provides that, since nothing exists in the standard
 * for some reason.
 *
 * This function will simply return std::dynamic_extent if the array does not have a compile-time defined size.
 *
 * \param ar The array to get the size of.
 * \return The second template argument to the array, as a size_t value.
 */
template <typename T, size_t size>
[[nodiscard]] consteval size_t arraySize([[maybe_unused]] array<T, size> &ar)
{
    return size;
}

/*!
 * Get the current time as a count of "ticks," or the number of units in the smallest measure available on the system.
 *
 * The returned value isn't guaranteed to mean much more than that it will be greater than the values returned in
 * previous calls, and that it will continue to change through successive calls of the method on the same system. There
 * is little way to know what unit was actually returned. It *should* never loop, given how large u64 is, but even this
 * is something that this method cannot actually guarantee. This could be useful as input to a non-secure random number
 * seed, or to provide rough time estimates for how long some event took to occur.
 *
 * All this method does is call `std::chrono::steady_clock::now()` and return `now.time_since_epoch().count()`.
 *
 * \return A number of "ticks" since the beginning of some system dependent time period.
 */
[[nodiscard]] KH_EXPORT u64 currentTicks() noexcept;

} // namespace KirHut
