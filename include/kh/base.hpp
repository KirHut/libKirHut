/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/base.hpp
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

#if defined(__cpp_lib_unreachable)
# include <utility>
#elif not defined(KH_COMPILED_WITH_MSVC)
# if defined(__has_builtin)
#  if not __has_builtin(__builtin_unreachable)
#   include <cstdlib>
#  endif
# else
#  include <cstdlib>
# endif
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
 * - std::bitset
 * - std::make_shared
 * - std::make_unique
 * - std::rotl
 * - std::rotr
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
using std::bitset;
using std::make_shared;
using std::make_unique;
using std::rotl;
using std::rotr;
using std::span;
using std::string;
using std::string_view;
using std::tuple;

using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;
//! \endcond

KH_INLINE_NAMESPACE_V1

/*!
 * Alias name for std::unique_ptr.
 *
 * The smart pointer names are a bit long, so this makes it easier to type out and have in method signatures.
 */
template <typename Object_T, typename Deleter_T = std::default_delete<Object_T>>
using UPtr = std::unique_ptr<Object_T, Deleter_T>;

/*!
 * Alias name for std::shared_ptr.
 *
 * \copydetails KirHut::UPtr<Object_T,Deleter_T>
 */
template <typename Object_T>
using SPtr = std::shared_ptr<Object_T>;

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
 * Single value enumeration type for copying constructor or method overloads.
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
 * Single value enumeration type for signifying that something should be done at runtime instead of compile-time.
 *
 *
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
 * This allows using a more natural type name for passing the make flag than "std::in_place_type_t". A "typed emplace
 * flag" is really just std::in_place_type<T>, which is a simple one-byte type that just provides an easy way to select
 * a particular template method or function override, just like std::in_place_type_t does.
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
 * A flag type that is used to select the "runtime" override of a particular method, usually a constructor.
 *
 * What a "runtime override" means, and when this is appropriate to use for your methods or constructors, is dependent
 * on what you are trying to do. If you want to allow a class that performs compile-time checking to ignore that
 * compile-time checking and instead check some value at runtime instead of compile-time, you would use this type to
 * signify that to the constructor/method. This library uses this type to omit compile-time checking of strings passed
 * to a checked object constructor.
 *
 * This is just RuntimeFlag::runtime, in a more intuitive location.
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
 * Returns the number of bytes needed to hold a given number of bits.
 *
 * For basically all processors supported by KirHut, the value is just the number passed in divided by 8, plus one if
 * there are any additional bits. This is because nearly all processors use an 8 bit byte. This library is designed to
 * potentially support building on very alternative hardware configurations, so this function does corretly calculate
 * the number of bytes for a given number of bits, so long as Platform::bitsInByte is correctly updated to this value
 * (it should be on all conforming compilers).
 *
 * \param numBits The number of bits that need to fit in the number of returned bytes.
 * \return The number of bytes that will completely contain the number of bits passed as \p numBits.
 */
[[nodiscard]] constexpr unsigned int bytesNeededForBits(unsigned int numBits) noexcept
{
    return (numBits + Constant::bitsInByte - 1) / Constant::bitsInByte;
}

/*!
 * Returns the number of bits that a given amount of bytes will contain.
 *
 * For basically all processors supported by KirHut, the value is just the number passed in multiplied by 8. This is
 * because nearly all processors use an 8 bit byte. This library is designed to potentially support building on very
 * alternative hardware configurations, so this function does correctly calculate the number of bits for a given number
 * of bytes, so long as Constant::bitsInByte is correctly updated to this value (it should be on all conforming
 * compilers).
 *
 * \note If the number of bytes passed has an amount of bits that exceeds the value that can fit into an unsigned int on
 * your target platform, this function will truncate the top bits to fit in the return value, losing those binary
 * digits. You should not pass in a value that exceeds Limits<unsigned int>::max() / Constant::bitsInByte if you want to
 * have an accurate return result, though this will not invoke undefined behavior.
 *
 * \param numBytes The number of bytes to multiply by Constant::bitsInByte.
 * \return The number of bits that are available in the given \p numBytes.
 */
[[nodiscard]] constexpr unsigned int numBitsInBytes(unsigned int numBytes) noexcept
{
    return numBytes * Constant::bitsInByte;
}

/*!
 * Returns the number of bits in the passed template type T.
 *
 * This is preferred over using sizeof() and multiplying by Platform::bitsInByte because it is both more concise and
 * more explanatory than the mathematical expression. The version of this method where you pass a value as an argument
 * is a better option if you can do that, since it more closely matches the interface of sizeof(). Unfortunately, since
 * this is a function and not a language operator, you cannot pass a type as a value to bitsOf, and it must instead be
 * passed as a template argument.
 *
 * \tparam T Any valid C++ type. This must be explicitly specified.
 * \return The number of bits it takes on the compiled for platform to represent type T.
 */
template <typename T>
[[nodiscard]] consteval unsigned int bitsOf()
{
    return numBitsInBytes(sizeof(T));
}

/*!
 * Returns the number of bits in the passed \p object type.
 *
 * This is preferred over using sizeof() and multiplying by Platform::bitsInByte because it is both more concise and
 * more explanatory than the mathematical expression. You should prefer using this version as the type of \p object is
 * inferred, which more closely matches the sizeof() operator. Unfortunately, since this is a function and not a
 * language operator, you cannot pass a type as a value directly to bitsOf, and it must instead be passed as a template
 * argument.
 *
 * \param object Any valid C++ value at all.
 * \return The number of bits it takes on the compiled for platform to represent \p object.
 */
[[nodiscard]] consteval unsigned int bitsOf(auto &object)
{
    return numBitsInBytes(sizeof(object));
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
 * \tparam T Literally any type that a reference to is passed in as \p object. This should be inferred.
 * \param object Any value or object type whatsoever.
 * \return A std::span viewing the underlying byte data of the \p object.
 */
template <typename T>
inline span<byte const, sizeof(T)> asBytes(T const &object) noexcept
{
    return std::as_bytes(span<T const, 1>{ &object, 1 });
}

/*!
 * \copydoc asBytes(T const&)
 */
template <typename T>
inline span<byte, sizeof(T)> asWritableBytes(T &object) noexcept
{
    return std::as_writable_bytes(span<T, 1>{ &object, 1 });
}

/*!
 * Concept representing a "numeric" type, which means either an integer literal or a floating point value.
 *
 * This matches with any type that returns true from std::is_arithmetic_v<T>, with the exception of the bool type, as
 * bools are not considered "numbers" for these purposes. Bool types tend to be special cases in a lot of contexts so it
 * is frequently better to remove them.
 */
template <typename T>
concept Numeric = std::is_arithmetic_v<T> and not std::is_same_v<T, bool>;

/*!
 * Concept representing an "integer" type, which means a signed or unsigned integer that is not a bool.
 *
 * This matches with any type that returns true from std::is_integral_v<T>, with the exception of the bool type, as
 * bools are not considered "integers" for these purposes. Bool types tend to be special cases in a lot of contexts so
 * it is frequently better to remove them.
 */
template <typename T>
concept Integral = (std::is_integral_v<T>
#if defined(KH_PRIV_DOCS) or defined(KH_USE_128BIT_TYPES)
                    or std::is_same_v<T, __int128_t> or std::is_same_v<T, __uint128_t>
#endif
                    ) and
                   not std::is_same_v<T, bool>;

/*!
 * Concept to identify if a type is one of a set of distinct types.
 *
 * There are times when you just want a concept to a specific fixed set of types you want to accept, but don't want to
 * just make separate overloads of that function for each type. This concept simplifies taking those arguments, and you
 * can easily define other concepts in terms of this concept. You can also check the return types of expressions to
 * ensure that they return one of a distinct set of types.
 */
template <typename Tested_T, typename Accepted_T, typename... Accepted_Ts>
concept OneOf = std::same_as<Tested_T, Accepted_T> or (std::same_as<Tested_T, Accepted_Ts> or ...);

/*!
 * Concept to identify the different types that may bypass strict aliasing rules in C++.
 *
 * Some types are not undefined behavior to dereference from a different type. Those three types are considered "byte
 * types" in KirHut software. Obviously, those three types are `char`, `unsigned char`, and std::byte.
 */
template <typename Byte_T>
concept ByteType = OneOf<std::remove_cv_t<Byte_T>, char, byte, unsigned char, std::byte>;

/*!
 * A constexpr version of the std::abs() function that is free of UB.
 *
 * For some inexplicable reason, the std::abs() function is not constexpr until C++23, and this library needs to provide
 * support for C++20. As such, a constexpr abs function is provided by this library directly, but unlike the C++
 * std::abs() function, if the result of the absolute transformation cannot be represented by the return value, the
 * original value passed in as \p number is returned instead. This gives at least some kind of defined behavior in that
 * case. In short, this means if \p number is Limits<decltype(number)>::min(), it will not convert to the positive
 * value, but only in that case.
 *
 * You should prefer the uabs() function over this one in the majority of cases, since you likely do not need a signed
 * integer anyway. If you must use a signed integer or your calculation simply expects everything to remain signed, and
 * you know will never have or can handle Limits<decltype(number)>::min(), this function remains available.
 *
 * This version of abs should not accept passing an unsigned integer type as \p number at all, unlike the std::abs()
 * function overloads, which will perform integer promotions in that case. The failure to compile behavior of this
 * template function is considered desireable and is retained.
 *
 * \param number A signed integer to get the absolute value of (or to return as-is if this is impossible).
 * \return The absolute value of \p number, unless this is impossible to represent, then \p number.
 */
template <std::signed_integral Int_T>
[[nodiscard]] constexpr Int_T abs(Int_T number) noexcept
{
    using UInt_T         = std::make_unsigned_t<Int_T>;
    UInt_T const negBits = ~std::bit_cast<UInt_T>(number) + 1u;
    return number < 0 ? std::bit_cast<Int_T>(negBits) : number;
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
 * function is designed to prevent integer promotions in that case.
 *
 * KirHut::uabs() is what std::abs() should have been.
 *
 * \param number An integer to get the unsigned absolute value of.
 * \return The absolute value of \p number, as an unsigned value.
 */
[[nodiscard]] constexpr auto uabs(std::signed_integral auto number) noexcept -> std::make_unsigned_t<decltype(number)>
{
    using UInt_T               = std::make_unsigned_t<decltype(number)>;
    UInt_T const uCastedNumber = std::bit_cast<UInt_T>(number);
    UInt_T const negAbs        = ~uCastedNumber + 1u;
    return number < 0 ? negAbs : uCastedNumber;
}

/*!
 * \copydoc KirHut::uabs(std::signed_integral auto)noexcept
 */
[[nodiscard]] constexpr auto uabs(std::unsigned_integral auto number) noexcept -> decltype(number)
{
    return number;
}

/*!
 * Perform a bitwise logical left shift of a given integer, but without UB risk.
 *
 * In C and C++, if you perform a bitwise left shift to a bit location beyond the size in bits of the integer you are
 * modifying the behavior is undefined. This is fine if you are very careful what values are passed to the bit shift
 * operators, but in most cases it can be much safer to use something that has guaranteed defined behavior when a
 * less-than expected value is passed. The shl function is just such a function: If \p amount is greater than the number
 * of bits in \p value, than the "bit shift operation" simply results in 0. The \p amount is an unsigned integer, so
 * signed integers will be automatically converted, including negative values, which will inevitably result in shl()
 * returning 0.
 *
 * This library's shl() and shr() functions are based off of the WG21 proposal P3793R0: Better Shifting.
 * https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p3793r0.html
 *
 * The only real difference is that these methods take an unsigned int as the shift \p amount rather than a signed
 * integer. This is because the recommended implementation of shl() and shr() both already cast the signed integer into
 * an unsigned one, so this function simply front-loads that conversion to the interface, making it clear to a
 * programmer using this function that the value passed is read as an unsigned integer. This does not use the compile
 * failure trick of abs() because silent conversion from signed integer literals is desireable.
 *
 * This function is constexpr like std::rotl() so that it may be used at compile time.
 *
 * \param value The signed or unsigned integer to apply the bit shift operation to.
 * \param amount The number of bits to shift left.
 * \return The \p value left-shifted by \p amount bits.
 */
[[nodiscard]] constexpr auto shl(Integral auto value, unsigned int amount) noexcept -> decltype(value)
{
    return amount < sizeof(value) * Constant::bitsInByte ? value << amount : 0;
}

/*!
 * Perform a bitwise arithmetic right shift of a given integer, but without UB risk.
 *
 * In C and C++, if you perform a bitwise right shift to a bit location beyond the size in bits of the integer you are
 * modifying the behavior is undefined. This is fine if you are very careful what values are passed to the bit shift
 * operators, but in most cases it can be much safer to use something that has guaranteed defined behavior when a
 * less-than expected value is passed. The shr function is just such a function: If \p amount is greater than the number
 * of bits in \p value, than the "bit shift operation" simply results in the same thing if two consecutive shifts were
 * performed, so 0 for unsigned \p value and non-negative signed \p value, and -1 for negative signed \p value. The \p
 * amount is an unsigned integer, so signed integers will be automatically converted, including negative values, which
 * will inevitably result in shr() returning 0 or -1.
 *
 * This library's shl() and shr() functions are based off of the WG21 proposal P3793R0: Better Shifting.
 * https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p3793r0.html
 *
 * The only real difference is that these methods take an unsigned int as the shift \p amount rather than a signed
 * integer. This is because the recommended implementation of shl() and shr() both already cast the signed integer into
 * an unsigned one, so this function simply front-loads that conversion to the interface, making it clear to a
 * programmer using this function that the value passed is read as an unsigned integer. This does not use the compile
 * failure trick of abs() because silent conversion from signed integer literals is desireable.
 *
 * This function is constexpr like std::rotr() so that it may be used at compile time.
 *
 * \param value The signed or unsigned integer to apply the bit shift operation to.
 * \param amount The number of bits to shift right.
 * \return The \p value right-shifted by \p amount bits.
 */
[[nodiscard]] constexpr auto shr(Integral auto value, unsigned int amount) noexcept -> decltype(value)
{
    auto const bottom = static_cast<decltype(value)>(value < 0 ? -1 : 0);
    return amount < sizeof(value) * Constant::bitsInByte ? value >> amount : bottom;
}

/*!
 * Detect if summing the two passed integers \p left and \p right would result in a signed integer overflow.
 *
 * This function is implemented by performing the addition of the two values after casting them to an unsigned integer,
 * which supports integer overflow safely, then detects if the results of that addition would have caused an overflow.
 * The logic used by this function is branchless, so it should be safe to use in a tight loop without efficiency issues.
 *
 * \param left The signed integer value at the left hand side of a sum computation.
 * \param right The signed integer value at the right hand side of a sum computation.
 * \return Whether or not the sum would result in undefined behavior (namely an overflow).
 */
template <std::signed_integral Int_T>
[[nodiscard]] constexpr bool wouldSumCauseUB(Int_T left, Int_T right) noexcept
{
    if constexpr (sizeof(Int_T) < sizeof(int))
    {
        return false;
    }

#if defined(__has_builtin)
# if __has_builtin(__builtin_add_overflow)
    return __builtin_add_overflow_p(left, right, left);
# endif
#endif

    using UInt_T       = std::make_unsigned_t<Int_T>;
    Int_T const result = static_cast<Int_T>(static_cast<UInt_T>(left) + static_cast<UInt_T>(right));
    return (left ^ result) & (right ^ result) & Limits<Int_T>::min();
}

/*!
 * Detect if the difference of the two passed integers \p left and \p right would result in a signed integer overflow.
 *
 * This function is implemented by performing the subtraction of the two values after casting them to an unsigned
 * integer, which supports integer overflow safely, then detects if the results of that difference would have caused an
 * overflow. The logic used by this function is branchless, so it should be safe to use in a tight loop without
 * efficiency issues.
 *
 * \param left The signed integer value at the left hand side of a difference computation.
 * \param right The signed integer value at the right hand side of a difference computation.
 * \return Whether or not the difference would result in undefined behavior (namely an overflow).
 */
template <std::signed_integral Int_T>
[[nodiscard]] constexpr bool wouldDifferenceCauseUB(Int_T left, Int_T right) noexcept
{
    if constexpr (sizeof(Int_T) < sizeof(int))
    {
        return false;
    }

#if defined(__has_builtin)
# if __has_builtin(__builtin_sub_overflow)
    return __builtin_sub_overflow_p(left, right, left);
# endif
#endif

    using UInt_T       = std::make_unsigned_t<Int_T>;
    Int_T const result = static_cast<Int_T>(static_cast<UInt_T>(left) - static_cast<UInt_T>(right));
    return (left ^ result) & (right ^ result) & Limits<Int_T>::min();
}

/*!
 * Detect if the product of the two passed integers \p left and \p right would result in a signed integer overflow.
 *
 * This function is implemented as a series of if statements (meaning it is branchy) that test certain conditions, and
 * possibly an integer division and comparison to check if the multiplication of \p left and \p right will result in an
 * integer overflow.
 *
 * \param left The signed integer value at the left hand side of a product computation.
 * \param right The signed integer value at the right hand side of a product computation.
 * \return Whether or not the product would result in undefined behavior (namely an overflow).
 */
template <std::signed_integral Int_T>
[[nodiscard]] constexpr bool wouldProductCauseUB(Int_T left, Int_T right) noexcept
{
#if defined(__has_builtin)
# if __has_builtin(__builtin_mul_overflow)
    return __builtin_mul_overflow_p(left, right, left);
# endif
#endif

    // Ensure right is the smaller of the two values, ignoring sign.
    if (uabs(left) < uabs(right))
    {
        using std::swap;
        swap(left, right);
    }

    if ((right & ~1) == 0)
    {
        return false;
    }

    if (right == -1)
    {
        return left == Limits<Int_T>::min();
    }

    if (left > 0)
    {
        if (right > 0)
        {
            return left > Limits<Int_T>::max() / right;
        }
        else
        {
            return right < Limits<Int_T>::min() / left;
        }
    }
    else
    {
        if (right > 0)
        {
            return left < Limits<Int_T>::min() / right;
        }
        else
        {
            return left < Limits<Int_T>::max() / right;
        }
    }
}

/*!
 * Detect if the quotient of the two passed integers \p left and \p right would result in any undefined behavior.
 *
 * This function is implemented as just two basic checks: checking that \p right is not zero and that, if \p right is
 * -1, than that \p left is not Limits<Int_T>::min(). These are the only two conditions in which integer division
 * results in undefined behavior, so there is no need to make the check any more complicated than that.
 *
 * \param left The signed integer value at the left hand side of a quotient computation.
 * \param right The signed integer value at the right hand side of a quotient computation.
 * \return Whether or not the quotient would result in undefined behavior.
 */
template <std::signed_integral Int_T>
[[nodiscard]] constexpr bool wouldQuotientCauseUB(Int_T left, Int_T right) noexcept
{
    return right == 0 or (left == Limits<Int_T>::min() and right == -1);
}

/*!
 * Constant expression byte swapping function that should always do the most efficient thing.
 *
 * The byteSwap function in libKirHut is meant to work like std::byteswap in C++23 for applications still only using
 * C++20. Unlike all of the other byteSwap functions, since there is no "swap" functionality for a single byte integer,
 * this does nothing at all and simply returns the value passed as \p bytes.
 *
 * \param bytes An unsigned 8 bit integer you want to have the bytes swapped in.
 * \return The same 8 bit integer passed as \p bytes, entirely unchanged.
 */
[[nodiscard]] constexpr u8 byteSwap(u8 bytes)
{
    return bytes;
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

    return shl(bytes, Constant::bitsInByte) bitor shr(bytes, Constant::bitsInByte);
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
#elif defined(__has_builtin)
# if __has_builtin(__builtin_bswap32)
        return __builtin_bswap32(bytes);
# endif
#endif
    }

    constexpr unsigned int outerShift = Constant::bitsInByte * 3;
    constexpr unsigned int innerShift = Constant::bitsInByte * 1;
    constexpr u32 bitMask             = 0xFF;

    // clang-format off
    return shl(bytes bitand shl(bitMask, Constant::bitsInByte * 0), outerShift) bitor
           shl(bytes bitand shl(bitMask, Constant::bitsInByte * 1), innerShift) bitor
           shr(bytes bitand shl(bitMask, Constant::bitsInByte * 2), innerShift) bitor
           shr(bytes bitand shl(bitMask, Constant::bitsInByte * 3), outerShift);
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
#elif defined(__has_builtin)
# if __has_builtin(__builtin_bswap64)
        return __builtin_bswap64(bytes);
# endif
#endif
    }

    constexpr unsigned int outerShift    = Constant::bitsInByte * 7;
    constexpr unsigned int midOuterShift = Constant::bitsInByte * 5;
    constexpr unsigned int midInnerShift = Constant::bitsInByte * 3;
    constexpr unsigned int innerShift    = Constant::bitsInByte * 1;
    constexpr u64 bitMask                = 0xFF;

    // clang-format off
    return shl(bytes bitand shl(bitMask, Constant::bitsInByte * 0), outerShift)    bitor
           shl(bytes bitand shl(bitMask, Constant::bitsInByte * 1), midOuterShift) bitor
           shl(bytes bitand shl(bitMask, Constant::bitsInByte * 2), midInnerShift) bitor
           shl(bytes bitand shl(bitMask, Constant::bitsInByte * 3), innerShift)    bitor
           shr(bytes bitand shl(bitMask, Constant::bitsInByte * 4), innerShift)    bitor
           shr(bytes bitand shl(bitMask, Constant::bitsInByte * 5), midInnerShift) bitor
           shr(bytes bitand shl(bitMask, Constant::bitsInByte * 6), midOuterShift) bitor
           shr(bytes bitand shl(bitMask, Constant::bitsInByte * 7), outerShift);
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
# if defined(__has_builtin)
#  if __has_builtin(__builtin_bswap128)
        return __builtin_bswap128(bytes);
#  endif
# endif
    }

    auto firstHalf = static_cast<u64>(bytes >> Platform::bitsInU64);
    auto lastHalf  = static_cast<u64>(bytes);
    return (static_cast<u128>(byteSwap(lastHalf)) << Platform::bitsInU64) bitor byteSwap(firstHalf);
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
 * A class that contains an \p EBO object that is placed under no_unique_address, to see if it uses any memory.
 *
 * If a class possesses data members (IE it is not an empty object) then this object will inevitably be larger than the
 * NoChild object,
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
consteval bool emptyTest() noexcept
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
 * This function has two primary behavior branches: The runtime pointer implementation, and the backup universal
 * implementation. The runtime pointer implementation uses std::memcpy to copy the data from \p source to the returned
 * Numeric value if \p source is a pointer and this function is not constant evaluated. Otherwise, this function will
 * manually iterate through the \p source contiguous iterator. In all cases, the length is compile-time predictable,
 * and every modern compiler this has been tested on will emit only a single copy for the returned Num_T value and a
 * bswap operation if necessary to convert from the source endianness. This seems to include when \p source is not a
 * pointer type, since std::contiguous_iterator guarantees contiguous memory.
 *
 * This implementation **does not** use std::copy because std::copy is under the <algorithm> header, and KirHut's
 * base.hpp header may not include <algorithm>.
 *
 * \tparam Num_T The Numeric type to return from this operation. The user usually selects this.
 * \tparam sourceEndianness The endianness of the source data as a std::endian type.
 * \param source A std::contiguous_iterator to the source data. The iterator must be over a ByteType.
 * \return The bytes pointed at by \p source interpreted as a \p Num_T, using whichever chosen \p sourceEndianness.
 */
template <Numeric Num_T, std::endian sourceEndianness>
[[nodiscard]] constexpr Num_T fromEndian(std::contiguous_iterator auto source) noexcept
    requires(ByteType<std::iter_value_t<decltype(source)>>)
{
    if constexpr (sizeof(Num_T) == sizeof(byte))
    {
        return std::bit_cast<Num_T>(*source);
    }

    ExactUIntOf<Num_T> ret = 0;
    if (std::is_pointer_v<decltype(source)> and not std::is_constant_evaluated())
    {
        // Keep memcpy nested in if constexpr to ensure compiler doesn't try to manifest memcpy with a non-pointer
        // contiguous_iterator.
        if constexpr (std::is_pointer_v<decltype(source)>)
        {
            memcpy(&ret, source, sizeof(Num_T));
        }
    }
    else
    {
        for (size_t i = 0; i < sizeof(Num_T); ++i)
        {
            // We do a bit_cast to unsigned first because the bytes in source may be signed, so static_cast could change
            // the binary representation if we don't first bit_cast to u8.
            auto temp = std::bit_cast<u8>(*source++);
            ret |= static_cast<ExactUIntOf<Num_T>>(temp) << (i * Constant::bitsInByte);
        }
    }

    if constexpr (sourceEndianness != Platform::Endianness)
    {
        ret = byteSwap(ret);
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
constexpr auto toEndian(Numeric auto value, std::contiguous_iterator auto dest) noexcept -> decltype(dest)
    requires(ByteType<std::iter_value_t<decltype(dest)>> and not std::is_const_v<std::iter_value_t<decltype(dest)>>)
{
    using Byte_T = std::iter_value_t<decltype(dest)>;

    constexpr auto returnTypeSize = sizeof(decltype(value));
    if constexpr (returnTypeSize == sizeof(byte))
    {
        *dest++ = std::bit_cast<Byte_T>(value);
        return dest;
    }

    auto bits = std::bit_cast<ExactUInt<returnTypeSize>>(value);
    if constexpr (destEndianness != Platform::Endianness)
    {
        bits = byteSwap(bits);
    }

    if constexpr (std::is_pointer_v<decltype(dest)>)
    {
        if (not std::is_constant_evaluated())
        {
            memcpy(dest, &bits, returnTypeSize);
            return std::next(dest, returnTypeSize);
        }
    }

    for (size_t i = 0; i < returnTypeSize; ++i)
    {
        u8 temp = static_cast<u8>(bits >> (i * Constant::bitsInByte));
        *dest++ = std::bit_cast<Byte_T>(temp);
    }

    return dest;
}

/*!
 * \internal
 *
 * Throw an IllegalArgument exception because the destination span of a given write operation is too small.
 *
 * This simply takes the message passed as a string_view and throws an IllegalArgument exception with that message.
 *
 * \param message The message to pass to the IllegalArgument exception.
 */
[[noreturn]] KH_EXPORT void throwTooSmallSpan(string_view message);

/*!
 * \internal
 *
 * Constant function test to check if a given \p Tested_T can be implicitly constructed from the given \p Arg_Ts.
 *
 * This "function" is only useful in the context of a requires clause, otherwise you'd just use the \p Tested_T object
 * constructor directly. This allows you to check in a requires clause if the given arguments in \p args can be
 * implicitly converted into the given \p Tested_T. If it can be, this will compile successfully, and if not, this will
 * fail to compile, which is useful in the context of a requires clause.
 *
 * \tparam Tested_T The type we're checking can be constructed from \p args. Must be explicitly given.
 * \tparam Arg_Ts The types of the \p args passed. This should be inferred.
 * \param args The actual arguments, usually not a runtime value since this is only done in requires clauses.
 * \return Nothing, ideally, since this is only used in requires clauses.
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
        return Constant::amountUnknown;
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
 * Get the index number of a particular type in the passed in Var or std::variant.
 *
 * For some reason, there's no way to get a constant index value of a given type in a given std::variant in the C++
 * standard library. I haven't the faintest idea why, but this function overcomes that limitation by providing said
 * required functionality when you are taking a std::variant as a template argument and may not know which index a given
 * type is in.
 *
 * Further, you can use this to check if a given std::variant even has a particular type by checking if this returns
 * Constant::amountUnknown. If it does, then the given type was not found in the Var or std::variant and your template
 * should respond accordingly.
 *
 * This function is marked consteval since it should always be performed at compile time.
 *
 * \tparam T The type you are looking for in the Var or std::variant.
 * \param var The variant to check for which
 * \return
 */
template <typename T>
[[nodiscard]] consteval size_t varIndex(InstanceOf<Var, std::variant> auto const &var)
{
    return Detail::varIndex<decltype(var), T>();
}

/*!
 * Read the data pointed at by \p source and return it as the given \p Num_T integer type for the native platform.
 *
 * This method should be used any time you need to read a big endian integer value from a source of bytes, such as from
 * an ethernet frame or a data file. On both big endian and little endian systems, this function will return the correct
 * value that is represented by the data pointed to by \p source, read as a big endian integer.
 *
 * Using this function is as simple as designating the desired Numeric return type, and then providing a pointer to a
 * buffer of bytes. The buffer of bytes will be interpreted as a big endian value of type \p Num_T and returned to you.
 *
 * ~~~
 * auto result = fromBigEndian<i32>(buffer);
 * static_assert(std::is_same_v<decltype(result), i32>);
 * ~~~
 *
 * If this function is provided a nullptr for \p source, the returned value will be 0 of whichever type you have
 * designated this function to return. If a non-nullptr is provided as \p source, it must point to valid memory for this
 * application.
 *
 * \warning It is undefined behavior to provide a pointer to a byte buffer that is not at least sizeof(T) bytes large.
 * This will cause a read to occur in invalid memory and so you should always check that there are sufficient bytes in
 * the buffer area for conversion.
 *
 * \tparam Num_T An integer or floating point type you wish to interpret the \p source as the big endian value of.
 * \param source A std::contiguous_iterator to a buffer of bytes at least sizeof(T) large.
 * \return The bytes under \p source interpreted as a big endian \p Num_T type.
 */
template <Numeric Num_T>
[[nodiscard]] constexpr Num_T fromBigEndian(std::contiguous_iterator auto source) noexcept
    requires(ByteType<std::iter_value_t<decltype(source)>>)
{
    if constexpr (std::is_pointer_v<decltype(source)>)
    {
        // FIXME: Reimplemented to keep branch free functionality, however this may be unnecessary. Need to check with
        // more compilers!
        return source ? Detail::fromEndian<Num_T, std::endian::big>(source) : Num_T{};
    }

    return Detail::fromEndian<Num_T, std::endian::big>(source);
}

/*!
 * Read the data in the first sizeof(Num_T) bytes contained in the \p source span and return it as the given \p Num_T
 * type.
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
 * \tparam Num_T An integer or floating point type you wish to interpret the \p source as the big endian value of.
 * \param source A span to a buffer of bytes at least sizeof(T) large.
 * \return The requested Numeric T type.
 */
template <Numeric Num_T, ByteType Byte_T, size_t fixedSize>
[[nodiscard]] constexpr Num_T fromBigEndian(span<Byte_T const, fixedSize> source)
    noexcept(fixedSize != std::dynamic_extent) requires(fixedSize >= sizeof(Num_T))
{
    if constexpr (fixedSize == std::dynamic_extent)
    {
        if (source.size() < sizeof(Num_T))
        {
            Detail::throwTooSmallSpan("Source span is too small.");
        }
    }

    return Detail::fromEndian<Num_T, std::endian::big>(source.data());
}

/*!
 * \copydoc fromBigEndian(span<Byte_T const,fixedSize>)
 */
template <Numeric Num_T, ByteType Byte_T, size_t fixedSize>
[[nodiscard]] constexpr Num_T fromBigEndian(span<Byte_T, fixedSize> source) noexcept(fixedSize != std::dynamic_extent)
    requires(fixedSize >= sizeof(Num_T))
{
    return fromBigEndian<Num_T>(span<Byte_T const, fixedSize>{ source });
}

/*!
 * \brief fromLittleEndian
 * \param source
 * \return
 */
template <Numeric Num_T>
[[nodiscard]] constexpr Num_T fromLittleEndian(std::contiguous_iterator auto source) noexcept
    requires(ByteType<std::iter_value_t<decltype(source)>>)
{
    if constexpr (std::is_pointer_v<decltype(source)>)
    {
        // FIXME: Reimplemented to keep branch free functionality, however this may be unnecessary. Need to check with
        // more compilers!
        return source ? Detail::fromEndian<Num_T, std::endian::little>(source) : Num_T{};
    }

    return Detail::fromEndian<Num_T, std::endian::little>(source);
}

/*!
 * \brief fromLittleEndian
 * \param source
 * \return
 */
template <Numeric Num_T, ByteType Byte_T, size_t fixedSize>
[[nodiscard]] constexpr Num_T fromLittleEndian(span<Byte_T const, fixedSize> source)
    noexcept(fixedSize != std::dynamic_extent) requires(fixedSize >= sizeof(Num_T))
{
    if constexpr (fixedSize == std::dynamic_extent)
    {
        if (source.size() < sizeof(Num_T))
        {
            Detail::throwTooSmallSpan("Source span is too small.");
        }
    }

    return Detail::fromEndian<Num_T, std::endian::little>(source.data());
}

/*!
 * \brief fromLittleEndian
 * \param source
 * \return
 */
template <Numeric Num_T, ByteType Byte_T, size_t fixedSize>
[[nodiscard]] constexpr Num_T fromLittleEndian(span<Byte_T, fixedSize> source)
    noexcept(fixedSize != std::dynamic_extent) requires(fixedSize >= sizeof(Num_T))
{
    return fromLittleEndian<Num_T>(span<Byte_T const, fixedSize>{ source });
}

/*!
 * Accept a Numeric \p value of any kind, and write it to the provided \p dest byte buffer as a big endian value.
 *
 * \param value The Numeric value (a signed or unsigned integer, or a floating point type) to write as big endian.
 * \param dest The buffer to write the big endian data for \p value.
 * \return A pointer to the first byte in \p dest that was **not** written to as a result of this call.
 */
constexpr auto toBigEndian(Numeric auto value, std::contiguous_iterator auto dest) noexcept -> decltype(dest)
    requires(ByteType<std::iter_value_t<decltype(dest)>> and not std::is_const_v<std::iter_value_t<decltype(dest)>>)
{
    if constexpr (std::is_pointer_v<decltype(dest)>)
    {
        // FIXME: Reimplemented to keep branch free functionality, however this may be unnecessary. Need to check with
        // more compilers!
        return dest ? Detail::toEndian<std::endian::big>(value, dest) : dest;
    }

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
    requires(fixedSize >= sizeof(value))
{
    static_assert(not std::is_const_v<Byte_T>, "toBigEndian needs a non-const span to write to.");

    if constexpr (fixedSize == std::dynamic_extent)
    {
        if (dest.size() < sizeof(value))
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
constexpr auto toLittleEndian(Numeric auto value, std::contiguous_iterator auto dest) noexcept -> decltype(dest)
    requires(ByteType<std::iter_value_t<decltype(dest)>> and not std::is_const_v<std::iter_value_t<decltype(dest)>>)
{
    if constexpr (std::is_pointer_v<decltype(dest)>)
    {
        // FIXME: Reimplemented to keep branch free functionality, however this may be unnecessary. Need to check with
        // more compilers!
        return dest ? Detail::toEndian<std::endian::little>(value, dest) : dest;
    }

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
 * \def KH_UNREACHABLE
 *
 * A macro that calls the underlying compiler's appropriate "unreachable" functionality, or std::unreachable().
 *
 * This should be used to signify to the compiler that a particular branch of code is completely unreachable, and it
 * should optimize away any code that would be generated to apply to that particular code branch. This can be
 * particularly useful in switch statements, but there are many other situations this is useful. You are required to
 * follow this macro up with a semicolon, so using it in code should look like this:
 *
 * ~~~
 * switch (something) {
 *   case 1: return 1;
 *   case 2: return 2;
 *   default: KH_UNREACHABLE();
 * }
 * ~~~
 *
 * This macro is guaranteed to be defined. However, there is the possibility of a compiler not having support for any
 * of the different "kinds" of unreachable functions. In that case, it expands to std::abort() to communicate to the
 * compiler that this is should never validly return control flow from that branch.
 */

#if defined(__cpp_lib_unreachable)
# define KH_UNREACHABLE() std::unreachable()
#elif defined(KH_COMPILED_WITH_MSVC) and not defined(__clang__)
# define KH_UNREACHABLE() __assume(false)
#elif defined(__has_builtin)
# if __has_builtin(__builtin_unreachable)
#  define KH_UNREACHABLE() __builtin_unreachable()
# else
#  define KH_UNREACHABLE() std::abort()
# endif
#else
# define KH_UNREACHABLE() std::abort()
#endif

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

KH_END_INLINE_NAMESPACE

} // namespace KirHut
