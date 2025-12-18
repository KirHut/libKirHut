/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/platform.hpp
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

#include <limits>
#include <bit>

#include "kh/global.hpp"

namespace KirHut
{

KH_INLINE_NAMESPACE_V1

namespace Detail
{

/*!
 * \internal
 *
 * Function that checks if KH_USE_128BIT_TYPES is defined.
 *
 * It is trivial to check KH_USE_128BIT_TYPES with the preprocessor, and this method isn't meant to replace that,
 * however there are many situations where you want to access if we are building with 128-bit types, both at runtime and
 * at compile time in constexpr contexts. This function makes accessing that information as trivial as it is on the
 * preprocessor.
 *
 * \return whether or not this library was built with 128-bit integer support.
 */
[[nodiscard]] consteval bool has128bit()
{
#if defined(KH_USE_128BIT_TYPES)
    return true;
#else
    return false;
#endif
}

/*!
 * \internal
 *
 * The LimitsFinder class
 *
 * This is required to be a struct and not an alias template because there are specializations of LimitsFinder that are
 * specifically for 128-bit types when using libc++. This is, otherwise, simply an alias over std::numeric_limits.
 *
 * \tparam Num_T The number type to use for std::numeric_limits as limits, if such a specialization exists.
 */
template <typename Num_T>
struct LimitsFinder
{
    /*!
     * \internal
     *
     * The actual limits type being used by the \p Num_T type.
     *
     * In nearly every condition, LimitsFinder<Num_T>::limits is just std::numeric_limits<Num_T>. The only time this
     * isn't the case is on Clang on 64-bit systems, because Clang does not implement std::numeric_limits<__int128_t>,
     * so this library has to provide its own custom implementation. That is the reason all of this exists in the first
     * place.
     */
    using limits = std::numeric_limits<Num_T>;

    static_assert(limits::is_specialized,
                  "There must exist a specialization of std::numeric_limits for the given Num_T.");
};

#if defined(KH_USE_128BIT_TYPES) or defined(KH_PRIV_DOCS)
/*!
 * \internal
 *
 * Custom implementation of the std::numeric_limits<__uint128_t> object.
 *
 * This is usually used in Clang compilers and derivatives that rely on libc++, as libc++ does not come with a 128-bit
 * specialization of std::numeric_limits. In that case, we need to provide a custom implementation, and that is done
 * using this type. The values in this type are mostly undocumented since they are identical to what would be expected
 * from a std::numeric_limits implementation.
 */
struct LimitsOfUInt128Impl
{
    constexpr static bool is_specialized                = true;
    constexpr static bool is_signed                     = false;
    constexpr static bool is_integer                    = true;
    constexpr static bool is_exact                      = true;
    constexpr static bool has_infinity                  = false;
    constexpr static bool has_quiet_NaN                 = false;
    constexpr static bool has_signalign_NaN             = false;
    constexpr static std::float_denorm_style has_denorm = std::denorm_absent;
    constexpr static bool has_denorm_loss               = false;
    constexpr static std::float_round_style round_style = std::round_toward_zero;
    constexpr static bool is_iec559                     = false;
    constexpr static bool is_bounded                    = true;
    constexpr static bool is_modulo                     = true;

    constexpr static int digits           = std::numeric_limits<unsigned char>::digits * sizeof(__int128_t);
    constexpr static int digits10         = static_cast<int>(static_cast<double>(digits) * Detail::log10_2);
    constexpr static int max_digits10     = 0;
    constexpr static int radix            = 2;
    constexpr static int min_exponent     = 0;
    constexpr static int min_exponent10   = 0;
    constexpr static int max_exponent     = 0;
    constexpr static int max_exponent10   = 0;
    constexpr static bool traps           = true;
    constexpr static bool tinyness_before = false;

    [[nodiscard]] consteval static __uint128_t min() noexcept
    {
        return {};
    }

    [[nodiscard]] consteval static __uint128_t lowest() noexcept
    {
        return min();
    }

    [[nodiscard]] consteval static __uint128_t max() noexcept
    {
        return ~__uint128_t{};
    }

    [[nodiscard]] consteval static __uint128_t epsilon() noexcept
    {
        return {};
    }

    [[nodiscard]] consteval static __uint128_t round_error() noexcept
    {
        return {};
    }

    [[nodiscard]] consteval static __uint128_t infinity() noexcept
    {
        return {};
    }

    [[nodiscard]] consteval static __uint128_t quiet_NaN() noexcept
    {
        return {};
    }

    [[nodiscard]] consteval static __uint128_t signaling_NaN() noexcept
    {
        return {};
    }

    [[nodiscard]] consteval static __uint128_t denorm_min() noexcept
    {
        return {};
    }
};

/*!
 * \internal
 *
 * Custom implementation of the std::numeric_limits<__int128_t> object.
 *
 * This is usually used in Clang compilers and derivatives that rely on libc++, as libc++ does not come with a 128-bit
 * specialization of std::numeric_limits. In that case, we need to provide a custom implementation, and that is done
 * using this type. The values in this type are mostly undocumented since they are identical to what would be expected
 * from a std::numeric_limits implementation.
 */
struct LimitsOfInt128Impl
{
    constexpr static bool is_specialized                = true;
    constexpr static bool is_signed                     = true;
    constexpr static bool is_integer                    = true;
    constexpr static bool is_exact                      = true;
    constexpr static bool has_infinity                  = false;
    constexpr static bool has_quiet_NaN                 = false;
    constexpr static bool has_signalign_NaN             = false;
    constexpr static std::float_denorm_style has_denorm = std::denorm_absent;
    constexpr static bool has_denorm_loss               = false;
    constexpr static std::float_round_style round_style = std::round_toward_zero;
    constexpr static bool is_iec559                     = false;
    constexpr static bool is_bounded                    = true;
    constexpr static bool is_modulo                     = std::numeric_limits<std::int_least64_t>::is_modulo;

    constexpr static int digits           = std::numeric_limits<unsigned char>::digits * sizeof(__int128_t) - 1;
    constexpr static int digits10         = static_cast<int>(static_cast<double>(digits) * Detail::log10_2);
    constexpr static int max_digits10     = 0;
    constexpr static int radix            = 2;
    constexpr static int min_exponent     = 0;
    constexpr static int min_exponent10   = 0;
    constexpr static int max_exponent     = 0;
    constexpr static int max_exponent10   = 0;
    constexpr static bool traps           = true;
    constexpr static bool tinyness_before = false;

    /*!
     * Returns the minimum (or lowest) signed 128-bit integer value.
     *
     * This library assumes two's complement integers, so what it actually returns is an __int128_t with the MSB set to
     * 1, which is always the lowest negative value in two's complement (and one's complement) representation. In
     * sign-magnitude systems, this just returns -0. This library only supports two's complement representation, so do
     * not use this library except with that integer representation.
     *
     * \return The minimum signed 128-bit integer value.
     */
    [[nodiscard]] consteval static __int128_t min() noexcept
    {
        return std::bit_cast<__int128_t>(static_cast<__uint128_t>(1) << digits);
    }

    [[nodiscard]] consteval static __int128_t lowest() noexcept
    {
        return min();
    }

    [[nodiscard]] consteval static __int128_t max() noexcept
    {
        return std::bit_cast<__int128_t>(LimitsOfUInt128Impl::max() >> 1);
    }

    [[nodiscard]] consteval static __int128_t epsilon() noexcept
    {
        return {};
    }

    [[nodiscard]] consteval static __int128_t round_error() noexcept
    {
        return {};
    }

    [[nodiscard]] consteval static __int128_t infinity() noexcept
    {
        return {};
    }

    [[nodiscard]] consteval static __int128_t quiet_NaN() noexcept
    {
        return {};
    }

    [[nodiscard]] consteval static __int128_t signaling_NaN() noexcept
    {
        return {};
    }

    [[nodiscard]] consteval static __int128_t denorm_min() noexcept
    {
        return {};
    }
};

/*!
 * \internal
 *
 * Specialization of LimitsFinder for unsigned 128-bit integer types.
 */
template <>
struct LimitsFinder<__uint128_t>
{
    /*!
     * \internal
     *
     * Specialized limits value for 128-bit unsigned integer types.
     *
     * This is always used when building for 128-bit systems, however there are many implementations of the standard
     * library that include a 128-bit integer specializations of std::numeric_limits, and if that is the case, we should
     * just use that. If it isn't, only then do we fall back on the backup implementation in this library.
     */
    using limits = std::conditional_t<std::numeric_limits<__uint128_t>::is_specialized,
                                      std::numeric_limits<__uint128_t>,
                                      LimitsOfUInt128Impl>;
};

/*!
 * \internal
 *
 * Specialization of LimitsFinder for signed 128-bit integer types.
 */
template <>
struct LimitsFinder<__int128_t>
{
    /*!
     * \internal
     *
     * Specialized limits value for 128-bit signed integer types.
     *
     * This is always used when building for 128-bit systems, however there are many implementations of the standard
     * library that include a 128-bit integer specializations of std::numeric_limits, and if that is the case, we should
     * just use that. If it isn't, only then do we fall back on the backup implementation in this library.
     */
    using limits = std::conditional_t<std::numeric_limits<__int128_t>::is_specialized,
                                      std::numeric_limits<__int128_t>,
                                      LimitsOfInt128Impl>;
};
#endif

} // namespace Detail

/*!
 * Alias for std::numeric_limits.
 *
 * It is very useful to get to std::numeric_limits frequently, but the name is cumbersome, so this makes it less so.
 *
 * This actually has the ability to use a custom implementation of Limits when the type is 128 bit on Clang. This is
 * because GCC supports std::numeric_limits<__int128_t> but Clang does not, so on Clang and derived compilers this
 * library needs to use a backup implementation.
 */
template <class T>
using Limits = Detail::LimitsFinder<T>::limits;

/*!
 * An Unknown or Invalid number of bytes to use.
 *
 * There is no actual buffer of bytes or set of data of Limits<size_t>::max(), so this value is used instead to identify
 * an amount that is invalid or otherwise unknown or unknowable. This is the case when a connection has not finished
 * sending data or the data comes from a user source or separate application.
 */
[[maybe_unused]] constexpr size_t amountUnknown = Limits<size_t>::max();

/*!
 * Namespace for types that distinguish the platform at compile time. These are useful for if constexpr and other more
 * modern C++ capabilities that are not dependent on preprocessor defines.
 *
 * This namespace includes platform information such as the operating system, the processor ISA being used, the types of
 * data available on the platform, and more. Generally something within this namespace can be used to decide how you
 * should approach a set of code best for that target platform.
 */
namespace Platform
{

/*!
 * The number of bits in a byte.
 *
 * Some platforms may have differently sized bytes, and this can be used when that matters. The type used by
 * std::numeric_limits<T> cannot be std::byte because of a limitation where std::numeric_limits<std::byte> is not
 * defined so cannot be used in constexpr expressions.
 */
[[maybe_unused]] constexpr int bitsInByte = Limits<unsigned char>::digits;

/*!
 * A byte where all of the bits are set to 1.
 *
 * This is useful when you want a byte that is representative of a "full" byte, or a byte with all of the bits set to 1
 * (or 'on'). This can be both shorter and more descriptive than the alternative used in the initializer.
 *
 * The type used by std::numeric_limits<T> cannot be std::byte because of a limitation where
 * std::numeric_limits<std::byte> is not defined so cannot be used in constexpr expressions.
 */
[[maybe_unused]] constexpr byte fullByte = std::bit_cast<byte>(Limits<unsigned char>::max());

/*!
 * A byte where all of the bits are set to 0.
 *
 * This is useful when you want a byte that is representative of an "empty" byte, or a byte with none of the bits set to
 * 1 (or 'on'). This is a bit more descriptive than simply using `byte{0}`.
 */
[[maybe_unused]] constexpr byte emptyByte = byte{ 0 };

/*!
 * The lowest (minimum) value of an i8 integer.
 *
 * The i8 type is guaranteed to be **at least** 8 bits long, not precisely 8 bits long. Since all currently
 * supported platforms require an 8 bit byte, it is unlikely this will not be 8 bits, but the header supports it.
 */
[[maybe_unused]] constexpr i8 i8Min = Limits<i8>::min();

/*!
 * The lowest (minimum) value of an i16 integer.
 *
 * The i16 type is guaranteed to be **at least** 16 bits long, not precisely 16 bits long. It is unlikely the
 * platform does not support the short int type, however, so this is probably 16 bits on all supported platforms.
 */
[[maybe_unused]] constexpr i16 i16Min = Limits<i16>::min();

/*!
 * The lowest (minimum) value of an i32 integer.
 *
 * The i32 type is guaranteed to be **at least** 32 bits long, not precisely 32 bits long. Since all currently
 * supported platforms require a 32 bit type, it is unlikely this will not be 32 bits, but the header supports it.
 */
[[maybe_unused]] constexpr i32 i32Min = Limits<i32>::min();

/*!
 * The lowest (minimum) value of an i64 integer.
 *
 * The i64 type is guaranteed to be **at least** 64 bits long, not precisely 64 bits long. Since all currently
 * supported platforms require a 64 bit type, it is unlikely this will not be 64 bits, but the header supports it.
 */
[[maybe_unused]] constexpr i64 i64Min = Limits<i64>::min();

/*!
 * The lowest (minimum) value of an i128 integer.
 *
 * This type is only functional on GCC and Clang versions that have support for 128 bit types. As such, it is usually
 * not a good idea to use these types except when inside of an if constexpr block, wrapping the code with KH_HAS_128
 * blocks, or if the application should only support building with GCC or Clang.
 *
 * The types and values used with 128 bit types are deliberately always defined, even when there is no support, because
 * this allows using them in if constexpr blocks, as the code must remain legal even if the if constexpr block is never
 * evaluated under those contexts.
 */
[[maybe_unused]] constexpr i128 i128Min = Limits<i128>::min();

/*!
 * The lowest (minimum) value of the widest signed integer.
 *
 * The iWidest type is guaranteed to be the largest integer type supported on the platform, and should be at
 * least 64 bits on all supported platforms.
 */
[[maybe_unused]] constexpr iWidest iWidestMin = Limits<iWidest>::min();

/*!
 * The highest (maximum) value of an i8 integer.
 *
 * The i8 type is guaranteed to be **at least** 8 bits long, not precisely 8 bits long. Since all currently
 * supported platforms require an 8 bit byte, it is unlikely this will not be 8 bits, but the header supports it.
 */
[[maybe_unused]] constexpr i8 i8Max = Limits<i8>::max();

/*!
 * The highest (maximum) value of an i16 integer.
 *
 * The i16 type is guaranteed to be **at least** 16 bits long, not precisely 16 bits long. It is unlikely the
 * platform does not support the short int type, however, so this is probably 16 bits on all supported platforms.
 */
[[maybe_unused]] constexpr i16 i16Max = Limits<i16>::max();

/*!
 * The highest (maximum) value of an i32 integer.
 *
 * The i32 type is guaranteed to be **at least** 32 bits long, not precisely 32 bits long. Since all currently
 * supported platforms require a 32 bit type, it is unlikely this will not be 32 bits, but the header supports it.
 */
[[maybe_unused]] constexpr i32 i32Max = Limits<i32>::max();

/*!
 * The highest (maximum) value of an i64 integer.
 *
 * The i64 type is guaranteed to be **at least** 64 bits long, not precisely 64 bits long. Since all currently
 * supported platforms require a 64 bit type, it is unlikely this will not be 64 bits, but the header supports it.
 */
[[maybe_unused]] constexpr i64 i64Max = Limits<i64>::max();

/*!
 * The highest (maximum) value of an i128 integer.
 *
 * This type is only supported on GCC and Clang versions that have support for 128 bit types. As such, it is usually not
 * a good idea to use these types except when wrapping the code with `#if defined(KH_HAS_128)` blocks or if the
 * application should only support building with GCC or Clang.
 */
[[maybe_unused]] constexpr i128 i128Max = Limits<i128>::max();

/*!
 * The highest (maximum) value of the widest signed integer.
 *
 * The iWidest type is guaranteed to be the largest integer type supported on the platform, and should be at
 * least 64 bits on all supported platforms.
 */
[[maybe_unused]] constexpr iWidest iWidestMax = Limits<iWidest>::max();

/*!
 * The number of numeric bits found in the i8 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 8 bits long, this should be 7.
 */
[[maybe_unused]] constexpr int bitsInI8 = Limits<i8>::digits;

/*!
 * The number of numeric bits found in the i16 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 16 bits long, this should be 15.
 */
[[maybe_unused]] constexpr int bitsInI16 = Limits<i16>::digits;

/*!
 * The number of numeric bits found in the i32 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 32 bits long, this should be 31.
 */
[[maybe_unused]] constexpr int bitsInI32 = Limits<i32>::digits;

/*!
 * The number of numeric bits found in the i64 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 64 bits long, this should be 63.
 */
[[maybe_unused]] constexpr int bitsInI64 = Limits<i64>::digits;

/*!
 * The number of numeric bits found in the i128 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 128 bits long, this should be 127. If the
 * platform does not support using 128 bit types, this value will instead be 63, similar to an i64 type that the i128
 * type aliases. It could also be a different size, depending on the platform, but that is very unlikely.
 */
[[maybe_unused]] constexpr int bitsInI128 = Limits<i128>::digits;

/*!
 * The number of numeric bits found in the widest signed integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 64 bits long, this should be 63.
 */
[[maybe_unused]] constexpr int bitsInIWidest = Limits<iWidest>::digits;

/*!
 * The highest (maximum) value of an 8 bit unsigned integer.
 *
 * The u8 type is guaranteed to be **at least** 8 bits long, not precisely 8 bits long. Since all platforms
 * supported by Qt are dependent on having 8 bit byte types, it is unlikely this will not be 8 bits, but the header
 * supports it.
 */
[[maybe_unused]] constexpr u8 u8Max = Limits<u8>::max();

/*!
 * The highest (maximum) value of a 16 bit unsigned integer.
 *
 * The u16 type is guaranteed to be **at least** 16 bits long, not precisely 16 bits long. It is unlikely the
 * platform does not support the short int type, however, so this is probably 16 bits on all supported platforms.
 */
[[maybe_unused]] constexpr u16 u16Max = Limits<u16>::max();

/*!
 * The highest (maximum) value of a 32 bit unsigned integer.
 *
 * The u32 type is guaranteed to be **at least** 32 bits long, not precisely 32 bits long. Since all platforms
 * supported by Qt require 32 bit types, it is unlikely this will not be 32 bits, but the header supports it.
 */
[[maybe_unused]] constexpr u32 u32Max = Limits<u32>::max();

/*!
 * The highest (maximum) value of a 64 bit unsigned integer.
 *
 * The u64 type is guaranteed to be **at least** 64 bits long, not precisely 64 bits long. Since all platforms
 * supported by Qt require 64 bit types, it is unlikely this will not be 64 bits, but the header supports it.
 */
[[maybe_unused]] constexpr u64 u64Max = Limits<u64>::max();

/*!
 * The highest (maximum) value of a 128 bit unsigned integer.
 *
 * This type is only supported on GCC and Clang versions that have support for 128 bit types. As such, it is usually not
 * a good idea to use these types except when wrapping the code with `#if defined(KH_HAS_128)` blocks or if the
 * application should only support building with GCC or Clang.
 */
[[maybe_unused]] constexpr u128 u128Max = Limits<u128>::max();

/*!
 * The highest (maximum) value of the widest unsigned integer.
 *
 * The uWidest type is guaranteed to be the largest integer type supported on the platform, and should be at
 * least 64 bits on all supported platforms.
 */
[[maybe_unused]] constexpr uWidest uWidestMax = Limits<uWidest>::max();

/*!
 * The number of numeric bits found in the u8 integer.
 *
 * On platforms with 8-bit std::uint_least8_t integers, this should be 8.
 */
[[maybe_unused]] constexpr int bitsInU8 = Limits<u8>::digits;

/*!
 * The number of numeric bits found in the u16 integer.
 *
 * On platforms with 16-bit std::uint_least16_t integers, this should be 16.
 */
[[maybe_unused]] constexpr int bitsInU16 = Limits<u16>::digits;

/*!
 * The number of numeric bits found in the u32 integer.
 *
 * On platforms with 32-bit std::uint_least32_t integers, this should be 32.
 */
[[maybe_unused]] constexpr int bitsInU32 = Limits<u32>::digits;

/*!
 * The number of numeric bits found in the u64 integer.
 *
 * On platforms with 64-bit std::uint_least64_t integers, this should be 64.
 */
[[maybe_unused]] constexpr int bitsInU64 = Limits<u64>::digits;

/*!
 * The number of numeric bits found in the u128 integer.
 *
 * On most 64-bit platforms compiled with Clang or GCC, this should be 128.
 */
[[maybe_unused]] constexpr int bitsInU128 = Limits<u128>::digits;

/*!
 * The number of numeric bits found in the widest unsigned integer.
 *
 * On platforms with 64-bit integers and without 128-bit integer support, this should be 64.
 */
[[maybe_unused]] constexpr int bitsInUWidest = Limits<uWidest>::digits;

/*!
 * The lowest (minimum) value of an efficient 8 bit signed integer.
 *
 * The e8 type is guaranteed to be **at least** 8 bits long, not precisely 8 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr e8 e8Min = Limits<e8>::min();

/*!
 * The lowest (minimum) value of an efficient 16 bit signed integer.
 *
 * The e16 type is guaranteed to be **at least** 16 bits long, not precisely 16 bits long. Always assume these
 * types can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr e16 e16Min = Limits<e16>::min();

/*!
 * The lowest (minimum) value of an efficient 32 bit signed integer.
 *
 * The e32 type is guaranteed to be **at least** 32 bits long, not precisely 32 bits long. Always assume these
 * types can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr e32 e32Min = Limits<e32>::min();

/*!
 * The lowest (minimum) value of an efficient 64 bit signed integer.
 *
 * The e64 type is guaranteed to be **at least** 64 bits long, not precisely 64 bits long. All supported
 * platforms have a 64 bit integer type, so this is likely to be 64 bits long.
 */
[[maybe_unused]] constexpr e64 e64Min = Limits<e64>::min();

/*!
 * The number of numeric bits found in the e8 integer.
 *
 * This does not include the sign bit, but you should never assume that it will be 7
 */
[[maybe_unused]] constexpr int bitsInE8 = Limits<e8>::digits;

/*!
 * The number of numeric bits found in the e16 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 16 bits long, this should be 15.
 */
[[maybe_unused]] constexpr int bitsInE16 = Limits<e16>::digits;

/*!
 * The number of numeric bits found in the e32 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 32 bits long, this should be 31.
 */
[[maybe_unused]] constexpr int bitsInE32 = Limits<e32>::digits;

/*!
 * The number of numeric bits found in the e64 integer.
 *
 * This does not include the sign bit on most platforms, so if the type is 64 bits long, this should be 63.
 */
[[maybe_unused]] constexpr int bitsInE64 = Limits<e64>::digits;

/*!
 * The highest (maximum) value of an efficient 8 bit signed integer.
 *
 * The e8 type is guaranteed to be **at least** 8 bits long, not precisely 8 bits long. Always assume these types
 * can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr e8 e8Max = Limits<e8>::max();

/*!
 * The highest (maximum) value of an efficient 16 bit signed integer.
 *
 * The e16 type is guaranteed to be **at least** 16 bits long, not precisely 16 bits long. Always assume these
 * types can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr e16 e16Max = Limits<e16>::max();

/*!
 * The highest (maximum) value of an efficient 32 bit signed integer.
 *
 * The e32 type is guaranteed to be **at least** 32 bits long, not precisely 32 bits long. Always assume these
 * types can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr e32 e32Max = Limits<e32>::max();

/*!
 * The highest (maximum) value of an efficient 64 bit signed integer.
 *
 * The e64 type is guaranteed to be **at least** 64 bits long, not precisely 64 bits long. All supported
 * platforms have a 64 bit integer type, so this is likely to be 64 bits long.
 */
[[maybe_unused]] constexpr e64 e64Max = Limits<e64>::max();

/*!
 * The highest (maximum) value of an efficient 8 bit unsigned integer.
 *
 * The ue8 type is guaranteed to be **at least** 8 bits long, not precisely 8 bits long. Always assume these
 * types can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr ue8 ue8Max = Limits<ue8>::max();

/*!
 * The highest (maximum) value of an efficient 16 bit unsigned integer.
 *
 * The ue16 type is guaranteed to be **at least** 16 bits long, not precisely 16 bits long. Always assume these
 * types can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr ue16 ue16Max = Limits<ue16>::max();

/*!
 * The highest (maximum) value of an efficient 32 bit unsigned integer.
 *
 * The ue32 type is guaranteed to be **at least** 32 bits long, not precisely 32 bits long. Always assume these
 * types can be larger than the intended size, however so long as you do not rely on integer rollover and want efficient
 * computations, this is the preferred type to use.
 */
[[maybe_unused]] constexpr ue32 ue32Max = Limits<ue32>::max();

/*!
 * The highest (maximum) value of an efficient 64 bit unsigned integer.
 *
 * The ue64 type is guaranteed to be **at least** 64 bits long, not precisely 64 bits long. All supported
 * platforms have a 64 bit integer type, so this is likely to be 64 bits long.
 */
[[maybe_unused]] constexpr ue64 ue64Max = Limits<ue64>::max();

/*!
 * The number of numeric bits found in the ue8 integer.
 *
 * On platforms with 8-bit std::uint_fast8_t integers, this should be 8.
 */
[[maybe_unused]] constexpr int bitsInUe8 = Limits<ue8>::digits;

/*!
 * The number of numeric bits found in the ue16 integer.
 *
 * On platforms with 16-bit std::uint_fast16_t integers, this should be 16.
 */
[[maybe_unused]] constexpr int bitsInUe16 = Limits<ue16>::digits;

/*!
 * The number of numeric bits found in the ue32 integer.
 *
 * On platforms with 32-bit std::uint_fast32_t integers, this should be 32.
 */
[[maybe_unused]] constexpr int bitsInUe32 = Limits<ue32>::digits;

/*!
 * The number of numeric bits found in the ue64 integer.
 *
 * On platforms with 64-bit std::uint_fast64_t integers, this should be 64.
 */
[[maybe_unused]] constexpr int bitsInUe64 = Limits<ue64>::digits;

/*!
 * Non Preprocessor equivalent to #KH_WINDOWS.
 *
 * This is useful for static_assert, if constexpr, and other functionality that requires non macro types.
 *
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool windows = false
// God I wish there was something better than this unholy construction. Unfortunately this really is the shortest way I
// can come to in order to express something that is true or false based on a preprocessor define.
#if defined(KH_WINDOWS)
                                          or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_LINUX.
 *
 * \copydetails KirHut::Platform::windows
 *
 * This cannot simply be named "linux" because of an existing C preprocessor define.
 *
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool gnulinux = false
#if defined(KH_LINUX)
                                           or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_BSD.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool bsd = false
#if defined(KH_BSD)
                                      or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_APPLE.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool apple = false
#if defined(KH_APPLE)
                                        or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_MACOS.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool macOS = false
#if defined(KH_MACOS)
                                        or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_WASM.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool wasm = false
#if defined(KH_WASM)
                                       or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_IPHONE.
 *
 * \copydetails KirHut::Platform::windows
 *
 * \note This includes iPadOS! As of right now there is no need to distinguish them, but this may change in the future.
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool iPhone = false
#if defined(KH_IPHONE)
                                         or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_ANDROID.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool android = false
#if defined(KH_ANDROID)
                                          or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_MOBILE.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool mobile = false
#if defined(KH_MOBILE)
                                         or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_DESKTOP.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool desktop = false
#if defined(KH_DESKTOP)
                                          or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_X64.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool x64 = false
#if defined(KH_X64)
                                      or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_X32.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool x32 = false
#if defined(KH_X32)
                                      or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_X86.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool x86 = false
#if defined(KH_X86)
                                      or true
#endif
    ;

/*!
 * Check if the current platform being built for is a big endian platform.
 *
 * This library does not support platforms that do not either define this or the inverse to be true. Simply put, if this
 * is true, than LittleEndian must be false, and if this is false, than LittleEndian must be true. You can also get the
 * current platforms endianness using the Platform::Endianness value.
 */
[[maybe_unused]] constexpr bool BigEndian = std::endian::native == std::endian::big;

/*!
 * Check if the current platform being built for is a little endian platform.
 *
 * This library does not support platforms that do not either define this or the inverse to be true. Simply put, if this
 * is true, than BigEndian must be false, and if this is false, than BigEndian must be true. You can also get the
 * current platforms endianness using the Platform::Endianness value.
 */
[[maybe_unused]] constexpr bool LittleEndian = std::endian::native == std::endian::little;

/*!
 * Provide the current endianness of the platform as a std::endian value.
 *
 * C++20 introduced std::endian, which allowed identifying the endianness of the current platform very easily. This is
 * basically another name for std::endian::native, but within the KirHut::Platform namespace.
 */
[[maybe_unused]] constexpr std::endian Endianness = std::endian::native;

/*!
 * Check if the current build environment supports 128-bit signed integers or not.
 */
[[maybe_unused]] constexpr bool hasI128 = Detail::has128bit();

/*!
 * Check if the current build environment supports 128-bit unsigned integers or not.
 */
[[maybe_unused]] constexpr bool hasU128 = Detail::has128bit();

/*!
 * Check if the current platform i8 integer bit length is specifically 7 bits long.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must an 8-bit signed integer, instead
 * this value informs you if the platform does have 8-bit signed integers or not. This value is therefore always there,
 * even on platforms where there are not 8 bit integers.
 *
 * This library does not support any hardware device with bytes that are larger than 8 bits, and the C++ standard
 * requires that a byte can contain at least as much as 8 bits of data, so this should effectively always be true in all
 * cases this library supports. You should never need to check this, but it is here for pedantic completeness.
 */
[[maybe_unused]] constexpr bool i8Is8Bits = bitsInI8 == 7;

/*!
 * Check if the current platform i16 integer bit length is specifically 15 bits long.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must a 16-bit signed integer, instead
 * this value informs you if the platform does have 16-bit signed integers or not. This value is therefore always there,
 * even on platforms where there are not 16 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool i16Is16Bits = bitsInI16 == 15;

/*!
 * Check if the current platform i32 integer bit length is specifically 31 bits long.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must a 32-bit signed integer, instead
 * this value informs you if the platform does have 32-bit signed integers or not. This value is therefore always there,
 * even on platforms where there are not 32 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool i32Is32Bits = bitsInI32 == 31;

/*!
 * Check if the current platform i64 integer bit length is specifically 63 bits long.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must a 64-bit signed integer, instead
 * this value informs you if the platform does have 64-bit signed integers or not. This value is therefore always there,
 * even on platforms where there are not 64 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool i64Is64Bits = bitsInI64 == 63;

/*!
 * Check if the current platform i128 integer bit length is specifically 127 bits long.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must a 128-bit signed integer, instead
 * this value informs you if the platform does have 128-bit signed integers or not. This value is therefore always
 * there, even on platforms where there are not 128 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool i128Is128Bits = bitsInI128 == 127;

/*!
 * Check if the current platform u8 integer bit length is specifically 8 bits long.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must an 8-bit unsigned integer, instead
 * this value informs you if the platform does have 8-bit unsigned integers or not. This value is therefore always
 * there, even on platforms where there are not 8 bit integers.
 *
 * This library does not support any hardware device with bytes that are larger than 8 bits, and the C++ standard
 * requires that a byte can contain at least as much as 8 bits of data, so this should effectively always be true in all
 * cases this library supports. You should never need to check this, but it is here for pedantic completeness.
 */
[[maybe_unused]] constexpr bool u8Is8Bits = bitsInU8 == 8;

/*!
 * Check if the current platform u16 integer bit length is specifically 16 bits long.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must a 16-bit unsigned integer, instead
 * this value informs you if the platform does have 16-bit unsigned integers or not. This value is therefore always
 * there, even on platforms where there are not 16 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool u16Is16Bits = bitsInU16 == 16;

/*!
 * Check if the current platform u32 integer bit length is specifically 32 bits long.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must a 32-bit unsigned integer, instead
 * this value informs you if the platform does have 32-bit unsigned integers or not. This value is therefore always
 * there, even on platforms where there are not 32 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool u32Is32Bits = bitsInU32 == 32;

/*!
 * Check if the current platform u64 integer bit length is specifically 64 bits long.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must a 64-bit unsigned integer, instead
 * this value informs you if the platform does have 64-bit unsigned integers or not. This value is therefore always
 * there, even on platforms where there are not 64 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool u64Is64Bits = bitsInU64 == 64;

/*!
 * Check if the current platform u128 integer bit length is specifically 128 bits long.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must a 128-bit unsigned integer, instead
 * this value informs you if the platform does have 128-bit signed integers or not. This value is therefore always
 * there, even on platforms where there are not 128 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool u128Is128Bits = bitsInU128 == 128;

/*!
 * Check if the current platform supports 8 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must an 8-bit integer, instead this value
 * informs you if the platform does have 8-bit integers or not. This value is therefore always there, even on platforms
 * where there are not 8 bit integers.
 *
 * This library does not support any hardware device with bytes that are larger than 8 bits, and the C++ standard
 * requires that a byte can contain at least as much as 8 bits of data, so this should effectively always be true in all
 * cases this library supports. You should never need to check this, but it is here for pedantic completeness.
 */
[[maybe_unused]] constexpr bool reallyHas8BitTypes = i8Is8Bits && u8Is8Bits;

/*!
 * Check if the current platform supports 16 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must a 16-bit integer, instead this
 * value informs you if the platform does have 16-bit integers or not. This value is therefore always there, even on
 * platforms where there are not 16 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool reallyHas16BitTypes = i16Is16Bits && u16Is16Bits;

/*!
 * Check if the current platform supports 32 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must a 32-bit integer, instead this
 * value informs you if the platform does have 32-bit integers or not. This value is therefore always there, even on
 * platforms where there are not 32 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool reallyHas32BitTypes = i32Is32Bits && u32Is32Bits;

/*!
 * Check if the current platform supports 64 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must a 64-bit integer, instead this
 * value informs you if the platform does have 64-bit integers or not. This value is therefore always there, even on
 * platforms where there are not 64 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool reallyHas64BitTypes = i64Is64Bits && u64Is64Bits;

/*!
 * Check if the current platform supports 128 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't require that the platform must a 128-bit integer, instead this
 * value informs you if the platform does have 128-bit integers or not. This value is therefore always there, even on
 * platforms where there are not 128 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool reallyHas128BitTypes = i128Is128Bits && u128Is128Bits;

/*!
 * Check if the current platform supports signed 32 and 64 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't assert that the platform must have this functionality,
 * instead this value informs you if the platform does have 32 and 64 bit integers. This value is therefore always
 * there, even on platforms where there are not 32 and 64 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool i3264Are3264Bits = i32Is32Bits && i64Is64Bits;

/*!
 * Check if the current platform supports unsigned 32 and 64 bit integers.
 *
 * \copydetails i3264Are3264Bits
 */
[[maybe_unused]] constexpr bool u3264Are3264Bits = u32Is32Bits && u64Is64Bits;

/*!
 * Check if the current platform supports 32 and 64 bit integers.
 *
 * \copydetails i3264Are3264Bits
 */
[[maybe_unused]] constexpr bool reallyHas3264BitTypes = reallyHas32BitTypes && reallyHas64BitTypes;

/*!
 * Check if the current platform supports signed 16, 32, and 64 bit integers.
 *
 * Unlike the KH_MUST_HAVE_* directives, these don't assert that the platform must have this functionality,
 * instead this value informs you if the platform does have 8, 16, 32, and 64 bit integers. This value is therefore
 * always there, even on platforms where there are not 8, 16, 32, and 64 bit integers. In that case, its value is false.
 */
[[maybe_unused]] constexpr bool i163264Are163264Bits = i3264Are3264Bits && i16Is16Bits;

/*!
 * Check if the current platform supports unsigned 16, 32, and 64 bit integers.
 *
 * \copydetails i163264Are163264Bits
 */
[[maybe_unused]] constexpr bool u163264Are163264Bits = u3264Are3264Bits && u16Is16Bits;

/*!
 * Check if the current platform supports 16, 32, and 64 bit integers.
 *
 * \copydetails i163264Are163264Bits
 */
[[maybe_unused]] constexpr bool reallyHas163264BitTypes = reallyHas3264BitTypes && reallyHas16BitTypes;

} // namespace Platform

/*!
 * Namespace for Build information regarding libKirHut.
 */
namespace Build
{

/*!
 * Non Preprocessor equivalent to #KH_DEBUG.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool debug = false
#if defined(KH_DEBUG)
                                        or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_RELEASE.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool release = false
#if defined(KH_RELEASE)
                                          or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_USES_QT.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool usesQt = false
#if defined(KH_USES_QT)
                                         or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_QT5_15.
 *
 * \copydetails KirHut::Platform::windows
 *
 * These are defined regardless of whether or not Qt is used because if constexpr must still produce a legal
 * expression even if it is always false. See usesQt to check if Qt is included in the build.
 *
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool qt515 = false
#if defined(KH_QT5_15)
                                        or usesQt
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_QT6_2.
 *
 * \copydetails KirHut::Build::qt515
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool qt62 = false
#if defined(KH_QT6_2)
                                       or usesQt
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_QT6_5.
 *
 * \copydetails KirHut::Build::qt515
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool qt65 = false
#if defined(KH_QT6_5)
                                       or usesQt
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_QT6_8.
 *
 * \copydetails KirHut::Build::qt515
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool qt68 = false
#if defined(KH_QT6_8)
                                       or usesQt
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_COMPILED_WITH_GCC.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool gcc = false
#if defined(KH_COMPILED_WITH_GCC)
                                      or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_COMPILED_WITH_MSVC.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool msvc = false
#if defined(KH_COMPILED_WITH_MSVC)
                                       or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_COMPILED_WITH_ICX.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool intel = false
#if defined(KH_COMPILED_WITH_ICX)
                                        or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_COMPILED_WITH_CLANG.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool clang = false
#if defined(KH_COMPILED_WITH_CLANG)
                                        or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_COMPILED_WITH_APPLECLANG.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool appleClang = false
#if defined(KH_COMPILED_WITH_APPLECLANG)
                                             or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_COMPILED_WITH_IBMXL.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool openxl = false
#if defined(KH_COMPILED_WITH_IBMXL)
                                         or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_CUDA_ENABLED.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool cuda = false
#if defined(KH_CUDA_ENABLED)
                                       or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_COMPILED_WITH_ARMCLANG.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool armClang = false
#if defined(KH_COMPILED_WITH_ARMCLANG)
                                           or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_COMPILED_WITH_CRAY.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool cray = false
#if defined(KH_COMPILED_WITH_CRAY)
                                       or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_COMPILED_WITH_NVHPC.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool nvcxx = false
#if defined(KH_COMPILED_WITH_NVHPC)
                                        or true
#endif
    ;

/*!
 * Non Preprocessor equivalent to #KH_COMPILED_WITH_UNKNOWN.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool unknownCompiler =
    not(gcc or msvc or clang or intel or appleClang or openxl or armClang or nvcxx or cray);

/*!
 * \brief compilerDisplayString
 */
[[maybe_unused]] constexpr auto compilerDisplayString = KH_COMPILER_DISPLAY_STRING;

/*!
 * Non Preprocessor equivalent to #KH_CLANG_GCC_COMPATIBLE.
 *
 * \copydetails KirHut::Platform::windows
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool gccCompatible = false
#if defined(KH_CLANG_GCC_COMPATIBLE)
                                                or true
#endif
    ;

} // namespace Build

KH_END_INLINE_NAMESPACE

} // namespace KirHut
