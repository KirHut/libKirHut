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

#if defined(__cpp_lib_byteswap)
# include <bit>
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
 * \headerfile base.hpp "kh/base.hpp"
 *
 * This matches with any type that returns true from std::is_arithmetic_v<T>, with the exception of the bool type, as
 * bools are not considered "numbers" for these purposes. Bool types tend to be special cases in a lot of contexts so it
 * is better to remove them.
 */
template <typename T>
concept Numeric = std::is_arithmetic_v<T> and not std::is_same_v<T, bool>;

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
constexpr u16 byteSwap(u16 bytes)
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
constexpr u32 byteSwap(u32 bytes)
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

    // clang-format off
    return ((bytes & 0x0000'00FF) << outerShift) |
           ((bytes & 0x0000'FF00) << innerShift) |
           ((bytes & 0x00FF'0000) >> innerShift) |
           ((bytes & 0xFF00'0000) >> outerShift);
    // clang-format on
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
 * \param bytes An unsigned 64 bit integer you want to have the bytes swapped in.
 * \return The same 64 bit integer passed as \p bytes, but with the bytes swapped.
 */
constexpr u64 byteSwap(u64 bytes)
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

    // clang-format off
    return ((bytes & 0x0000'0000'0000'00FFull) << outerShift)    |
           ((bytes & 0x0000'0000'0000'FF00ull) << midOuterShift) |
           ((bytes & 0x0000'0000'00FF'0000ull) << midInnerShift) |
           ((bytes & 0x0000'0000'FF00'0000ull) << innerShift)    |
           ((bytes & 0x0000'00FF'0000'0000ull) >> innerShift)    |
           ((bytes & 0x0000'FF00'0000'0000ull) >> midInnerShift) |
           ((bytes & 0x00FF'0000'0000'0000ull) >> midOuterShift) |
           ((bytes & 0xFF00'0000'0000'0000ull) >> outerShift);
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
constexpr u128 byteSwap(u128 bytes)
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
 * I just do not possess the profound galaxy brain necessary to concoct this monstrosity. Thank you!
 * https://indii.org/blog/is-type-instantiation-of-template/
 */
template <typename Wrong_T, template <typename...> typename Template_T>
constexpr bool InstanceOfValue = false;

/*!
 * \internal
 *
 * Correct type template specialization for InstanceOfValue.
 *
 * Credit to Lawrence Murray for this InstanceOfValue implementation to determine if a type is a given template type.
 * I just do not possess the profound galaxy brain necessary to concoct this monstrosity. Thank you!
 * https://indii.org/blog/is-type-instantiation-of-template/
 */
template <template <typename...> typename Template_T, typename... Arg_Ts>
constexpr bool InstanceOfValue<Template_T<Arg_Ts...>, Template_T> = true;

/*!
 * \internal
 *
 * Concept wrapper for the InstanceOfValue templates.
 *
 * This makes InstanceOfValue usable as a concept.
 */
template <typename Test_T, template <typename...> typename Template>
concept InstanceOf = InstanceOfValue<Test_T, Template>;

/*!
 * \internal
 *
 * \brief SpanOfValue
 */
template <typename Wrong_T, typename Element_T>
constexpr bool SpanOfValue = false;

/*!
 * \internal
 *
 * \brief SpanOfValue
 */
template <typename Element_T, size_t Extent>
constexpr bool SpanOfValue<span<Element_T, Extent>, Element_T> = true;

/*!
 * \internal
 *
 *
 */
template <typename Test_T, typename Element_T>
concept SpanOf = SpanOfValue<Test_T, std::remove_cv_t<Element_T>>;

/*!
 * \internal
 *
 *
 */
template <typename Var_T, typename Contain_T>
concept HasTypeOption =
    (InstanceOf<Var_T, Var> or InstanceOf<Var_T, std::variant>) and requires(Var_T v) { std::get<Contain_T>(v); };

/*!
 * \internal
 *
 * \brief The NoChild class
 */
struct NoChild final
{
    int val = 0; //!< A single integer value to give NoChild a predictable size.
};

/*!
 * \internal
 *
 * \brief The EboChild class
 */
template <typename EBO>
struct EboChild final : public EBO
{
    int val = 0; //!< A single integer value to give EboChild a predictable size.
};

/*!
 * \internal
 *
 * \brief fromEndian
 * \param source
 * \return
 */
template <Numeric Num_T, std::endian sourceEndianness>
constexpr Num_T fromEndian(byte const *source) noexcept
{
    if constexpr (sizeof(Num_T) == sizeof(byte))
    {
        return std::bit_cast<Num_T>(*source);
    }

    UIntOf<Num_T> ret = 0;
    if (std::is_constant_evaluated())
    {
        constexpr bool fromBig = sourceEndianness == std::endian::big;
        for (size_t i = 0; i < sizeof(Num_T); ++i)
        {
            ret |= static_cast<UIntOf<Num_T>>(fromBig ? source[sizeof(Num_T) - 1 - i] : source[i])
                   << (i * Platform::bitsInByte);
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
constexpr byte *toEndian(Numeric auto value, byte *dest) noexcept
{
    constexpr auto typeSize = sizeof(decltype(value));
    if constexpr (typeSize == sizeof(byte))
    {
        *dest = std::bit_cast<byte>(value);
        return ++dest;
    }

    auto bits = std::bit_cast<UInt<typeSize>>(value);
    if (std::is_constant_evaluated())
    {
        constexpr bool toBig = destEndianness == std::endian::big;
        for (size_t i = 0; i < typeSize; ++i)
        {
            dest[toBig ? typeSize - 1 - i : i] = static_cast<byte>(bits >> (i * Platform::bitsInByte));
        }
    }
    else
    {
        if constexpr (Platform::Endianness != destEndianness)
        {
            bits = byteSwap(bits);
        }

        memcpy(dest, &bits, typeSize);
    }

    return dest + typeSize;
}

/*!
 * \internal
 *
 * \brief throwTooSmallSpan
 * \param message
 */
[[noreturn]] void throwTooSmallSpan(string_view message);

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

} // namespace Detail

/*!
 * Concept to identify if a type is one of a set of distinct types.
 *
 * There are times when you just want a concept to be some fixed set of types you want to accept, but don't want to just
 * make separate overloads of that function for each type. This concept simplifies taking those arguments, and you can
 * easily define other concepts in terms of this concept. You can also check the return types of expressions to ensure
 * that they return one of a distinct set of types.
 */
template <typename Tested_T, typename First_T, typename... Rest_Ts>
concept OneOf = std::same_as<Tested_T, First_T> or (std::same_as<Tested_T, Rest_Ts> or ...);

/*!
 * Concept to identify if a type can be converted to one of a set of distinct types.
 *
 * This is just the std::convertible_to concept applied to multiple types instead of one, so you are not forced to make
 * a long chain of std::convertible_to concepts or std::is_convertible_to_v templates, and can instead just use this
 * concept to get the same power.
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
 */
template <typename Tested, template <typename...> typename Template, template <typename...> typename... Templates>
concept InstanceOf = Detail::InstanceOf<Tested, Template> or (Detail::InstanceOf<Tested, Templates> or ...);

/*!
 * Concept to identify types that are std::span of a given set of ElementTypes, or simply a span if no ElementTypes are
 * given.
 *
 * This is similar to InstanceOf in that it is for identifying a std::span template type, however it only works for
 * std::span and InstanceOf cannot work because it only works with types that have a set of typenames they take as
 * parameters, concrete parameters like size_of break InstanceOf. This is the workaround, but unlike InstanceOf, this
 * concept accepts no elements and will match with any std::span whatsoever. This is useful to remove the need to
 * explicitly accept an Extent parameter in abbreviated template functions and methods.
 */
template <typename Tested, typename... ElementTypes>
concept SpanOf = requires {
    typename Tested::element_type;
    requires Detail::SpanOf<Tested, typename Tested::element_type>;
    ((sizeof...(ElementTypes) == 0) or ... or Detail::SpanOf<Tested, ElementTypes>);
};

/*!
 * Concept to identify the different types that may bypass strict aliasing rules in C++.
 *
 * Some types are not undefined behavior to dereference from a different type. Those three types are considered "byte
 * types" in KirHut software. Obviously, those three types are `char`, `unsigned char`, and std::byte.
 */
template <typename Tested>
concept ByteType = OneOf<Tested, char, byte, unsigned char, std::byte>;

/*!
 * Concept to identify a span of some kind of ByteType.
 *
 * The ByteType concept is useful for detecting particular types, but when you are specifically trying to get a span to
 * one of those types, this concept is a better fit for that purpose. Like SpanOf, this works despite the fact that
 * InstanceOf does not work for types that contain non-type template parameters, namely size_t.
 */
template <typename Tested>
concept ByteSpan = SpanOf<Tested, char, byte, unsigned char, std::byte>;

/*!
 * Concept to idenfiy a span of some kind of constant ByteType.
 *
 * The ByteType concept is useful for detecting particular types, but when you are specifically trying to get a span to
 * one of those types, this concept is a better fit for that purpose. Like SpanOf, this works despite the fact that
 * InstanceOf does not work for types that contain non-type template parameters, namely size_t.
 */
template <typename Tested>
concept ConstByteSpan = SpanOf<Tested, char const, byte const, unsigned char const, std::byte const>;

/*!
 * Concept that identifies some Var or std::variant object that contains one of the given HasTypes.
 *
 * This matches with types of std::variant (or the Var alias) that contain a given HasType as one of its type options.
 * This has a separated HasType and HasTypes types in the template because in abbreviated template method signatures or
 * when applying this concept to requires clauses, having an empty set of HasTypes should fail to compile, and the
 * separate HasType ensures that happens.
 */
template <typename VarType, typename HasType, typename... HasTypes>
concept HasTypeOption = Detail::HasTypeOption<VarType, HasType> or (Detail::HasTypeOption<VarType, HasTypes> or ...);

/*!
 * Concept that identifies an "empty" class, or a type that, when used as a parent class, will not increase the size of
 * the object.
 *
 * This basically allows easy identification of types that can have the Empty Base Optimization applied to them or used
 * in a [[KH_NO_UNIQUE_ADDRESS]] context.
 */
template <typename EBO>
concept EmptyClass = sizeof(EBO) == 1 and sizeof(Detail::NoChild) == sizeof(Detail::EboChild<EBO>);

/*!
 * Concept representing a type T that can be contained in a given Var or std::variant.
 *
 * This matches with types that are accepted by the std::variant (or the Var alias) given as type V as one of its type
 * options.
 */
template <typename HasType, typename VarType>
concept TypeOptionOf = Detail::HasTypeOption<VarType, HasType>;

/*!
 * Concept to identify types that are implicitly convertible to other types.
 *
 * The C++ standard library has std::convertible_to, which is great for checking if a type can be implicitly converted
 * to another type, but if you need to determine if it is convertible using explicit conversion, this concept can be
 * used. This library has two conversion concepts, that follow this truth table:
 *
 * | Object Conversion | std::convertible_to | ExplicitlyConvertible | ImplicitlyConvertible |
 * |-------------------|---------------------|-----------------------|-----------------------|
 * | Implicit          | true                | false                 | true                  |
 * | Explicit          | true                | true                  | false                 |
 * | Not Convertible   | false               | false                 | false                 |
 *
 * Using the two concepts defined in this library, you can determine the implicit and explicit conversion status of any
 * given type in C++.
 */
template <typename To, typename First, typename... From>
concept ExplicitlyConstructible = std::constructible_from<To, First, From...> and not requires {
    Detail::convertTest<To>(std::declval<First>(), std::declval<From>()...);
};

/*!
 * Concept to identify types that are (only) explicitly convertible to other types.
 *
 * The C++ standard library has std::convertible_to, which is great for checking if a type can be implicitly converted
 * to another type, but if you need to determine if it is convertible using explicit conversion, this concept can be
 * used.
 *
 * This is slightly different from ExplicitlyConstructible<To, From> because that concept allows for *any* arguments
 * that can be used to construct an object, including when using an rvalue reference or forwarding reference (which will
 * modify the converted value). This concept rejects those possible arguments, but if you wish to accept them in your
 * template, you should use ExplicitlyConstructible instead.
 *
 * Using the two concepts defined in this library, you can determine the implicit and explicit conversion status of any
 * given type in C++.
 */
template <typename From, typename To>
concept ExplicitlyConvertible = not std::convertible_to<From, To> and requires(From f) { static_cast<To>(f); };

/*!
 * Read the data pointed at by \p loc and return it as the given \p T integer type for the native platform.
 *
 * This method should be used any time you need to read a big endian integer value from a source of bytes, such as from
 * an ethernet frame or a data file. On both big endian and little endian systems, this function will return the correct
 * value that is represented by the data pointed to by \p loc, read as a big endian integer.
 *
 * The number of bytes read is equal to the size in bytes of \p T, so you need to ensure that there are sufficient
 * bytes to read from at \p loc or this function will invoke undefined behavior.
 *
 * \param source
 * \return
 */
template <Numeric T>
constexpr T fromBigEndian(byte const *source) noexcept
{
    return Detail::fromEndian<T, std::endian::big>(source);
}

/*!
 * \brief fromBigEndian
 * \param source
 * \return
 */
template <Numeric T, size_t SZ>
constexpr T fromBigEndian(span<byte const, SZ> source) noexcept(SZ != std::dynamic_extent) requires(SZ >= sizeof(T))
{
    if constexpr (SZ == std::dynamic_extent)
    {
        if (source.size() < sizeof(T))
        {
            Detail::throwTooSmallSpan("Source span is too small.");
        }
    }

    return Detail::fromEndian<T, std::endian::big>(source.data());
}

/*!
 * \brief fromBigEndian
 * \param source
 * \return
 */
template <Numeric T, size_t SZ>
constexpr T fromBigEndian(span<byte, SZ> source) noexcept(SZ != std::dynamic_extent) requires(SZ >= sizeof(T))
{
    return Detail::fromEndian<T>(span<byte const, SZ>{ source });
}

/*!
 * \brief fromLittleEndian
 * \param source
 * \return
 */
template <Numeric T>
constexpr T fromLittleEndian(byte const *source) noexcept
{
    return Detail::fromEndian<T, std::endian::little>(source);
}

/*!
 * \brief fromLittleEndian
 * \param source
 * \return
 */
template <Numeric T, size_t SZ>
constexpr T fromLittleEndian(span<byte const, SZ> source) noexcept(SZ != std::dynamic_extent) requires(SZ >= sizeof(T))
{
    if constexpr (SZ == std::dynamic_extent)
    {
        if (source.size() < sizeof(T))
        {
            Detail::throwTooSmallSpan("Source span is too small.");
        }
    }

    return Detail::fromEndian<T, std::endian::little>(source.data());
}

/*!
 * \brief fromLittleEndian
 * \param source
 * \return
 */
template <Numeric T, size_t SZ>
constexpr T fromLittleEndian(span<byte, SZ> source) noexcept(SZ != std::dynamic_extent) requires(SZ >= sizeof(T))
{
    return Detail::fromEndian<T>(span<byte const, SZ>{ source });
}

/*!
 * \brief toBigEndian
 * \param value
 * \param dest
 */
constexpr void toBigEndian(Numeric auto value, byte *dest) noexcept
{
    Detail::toEndian<std::endian::big>(value, dest);
}

/*!
 * \brief toBigEndian
 * \return
 */
constexpr auto toBigEndian(Numeric auto value) noexcept -> array<byte, sizeof(value)>
{
    array<byte, sizeof(value)> ret;
    Detail::toEndian<std::endian::big>(value, ret.data());
    return ret;
}

/*!
 * \brief toBigEndian
 * \param dest
 * \throws IllegalArgument If the span passed to this method has std::dynamic_extent but size() < sizeof(value).
 */
template <size_t SZ>
constexpr void toBigEndian(Numeric auto value, span<byte, SZ> dest) noexcept(SZ != std::dynamic_extent)
    requires(SZ >= sizeof(decltype(value)))
{
    if constexpr (SZ == std::dynamic_extent)
    {
        if (dest.size() < sizeof(decltype(value)))
        {
            Detail::throwTooSmallSpan("Destination span is too small.");
        }
    }

    Detail::toEndian<std::endian::big>(value, dest);
}

/*!
 * \brief toLittleEndian
 * \param value
 * \param dest
 */
constexpr void toLittleEndian(Numeric auto value, byte *dest) noexcept
{
    Detail::toEndian<std::endian::little>(value, dest);
}

/*!
 * \brief toLittleEndian
 * \return
 */
constexpr auto toLittleEndian(Numeric auto value) noexcept -> array<byte, sizeof(value)>
{
    array<byte, sizeof(value)> ret;
    Detail::toEndian<std::endian::little>(value, ret.data());
    return ret;
}

/*!
 * \brief toLittleEndian
 * \param dest
 */
template <size_t SZ>
constexpr void toLittleEndian(Numeric auto value, span<byte, SZ> dest) noexcept(SZ != std::dynamic_extent)
    requires(SZ >= sizeof(decltype(value)))
{
    if constexpr (SZ == std::dynamic_extent)
    {
        if (dest.size() < sizeof(decltype(value)))
        {
            Detail::throwTooSmallSpan("Destination span is too small.");
        }
    }

    Detail::toEndian<std::endian::little>(value, dest);
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
template <typename T, size_t SIZE>
constexpr size_t arraySize([[maybe_unused]] array<T, SIZE> &ar)
{
    return SIZE;
}

/*!
 * Convert an array of constant char data that ends with '\0' to a string.
 *
 * This simply constructs a std::string around the data and does nothing else. Since this copies the data this function
 * is not marked `noexcept`, and it is only here for completeness with the other versions of toStr(). There is otherwise
 * no reason to use this function.
 *
 * \param data A pointer to an array of chars that ends with '\0'. The behavior is undefined if this is not the case.
 * \return A string of the data passed to this function.
 */
[[nodiscard]] KH_EXPORT string toStr(char const *data);

#if KH_PRIV_DOCS or KH_USES_QT
/*!
 * Conversion function for String to QString.
 *
 * This creates a QByteArray with the data pointed to by \p in and the length, ensuring no copies are made and the
 * QString ctor does not need to get the length of the data. This function still runs at **O(n)** because it must still
 * make at least one copy of the string data to the underlying UTF-16 QString data block.
 *
 * \param in A std::string_view to convert to a QString.
 * \return A QString of the contents in the passed String.
 */
[[nodiscard]] KH_EXPORT QString toQStr(string_view in) noexcept;

/*!
 * Conversion function for QString to String.
 *
 * This uses QString::toUtf8() to create a UTF-8 QByteArray of the QString data, then must perform a second copy of that
 * UTF-8 data to the returned string objects data buffer because there is no way to initialize a std::string with a
 * char array that does not make a copy. It will then dispose of the QByteArray. This function still runs at **O(n)**
 * time efficiency, despite the dual copies.
 *
 * \param in A QString to convert to a std::string.
 * \return A String copy of the contents in the passed QString.
 */
[[nodiscard]] KH_EXPORT string toStr(QString const &in) noexcept;
#endif // KH_USES_QT

/*!
 * Get the current time as a count of "ticks," or the number of units in the smallest measure available on the system.
 *
 * The returned value isn't guaranteed to mean much more than that it will be greater than the values returned in
 * previous calls, and that it will continue to change through successive calls of the method on the same system. There
 * is little way to know what unit was actually returned. It *should* never loop, given how large u64 is, but even this
 * is something that this method cannot actually guarantee. This should be useful as input to a non-secure random number
 * seed.
 *
 * All this method does is call `std::chrono::high_resolution_clock::now().time_since_epoch().count()`.
 *
 * \return A number of "ticks" since the beginning of some system dependent time period.
 */
[[nodiscard]] KH_EXPORT u64 currentTicks() noexcept;

} // namespace KirHut
