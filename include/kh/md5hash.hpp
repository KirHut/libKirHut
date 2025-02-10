/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/md5hash.hpp
** Copyright © KirHut Software Company
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
#pragma once

#include "kh/base.hpp"

namespace KirHut
{

static_assert(Build::hasU83264,
              "The libKirHut MD5 Hash implementation requires unsigned 8, 32, and 64 bit integers.\n"
              "The target platform does not support the MD5 implementation, if your software does "
              "not require MD5, simply remove the KH_MD5_SUPPORT definition in your build.");

/*!
 * The total size in bytes of the returned value from getMd5().
 *
 * An MD5 hash is guaranteed to be 128 bits long, so regardless of the number of bits in a byte, the return value is
 * equal to whatever is necessary to store 128 bits.
 */
constexpr auto MD5SUM_RETURN_SIZE = bytesNeededForBits(128);

/*!
 * Simple typedef of an array that will fit an MD5 Hash sum.
 *
 * Use it just like a std::array. The underlying type is std::byte, not char or unsigned char, so you will need to
 * convert if you need a char type.
 */
typedef array<byte, MD5SUM_RETURN_SIZE> Md5Sum;

/*!
 * The MD5 Hash sum generator class.
 *
 * This class is designed to have an efficient and easy to verify C++ hash implementation that never throws exceptions
 * and follows modern C++20 practices. This class does not allocate memory and is safe to use in memory constrained
 * situations. The object does not yet support byte lengths that aren't 8 bits, but other than that it should support
 * ints of arbitrary length (though it does require the platform to have an integer type of at least 32 and 64 bits).
 *
 * As an object following modern C++ practices, input is accepted as const std::byte rather than const char, allows
 * passing std::span<T> arguments of arbitrary length, and returning an object if needed instead of just filling a
 * buffer for the user. This MD5 hash sum algorithm is intended to be as convenient for modern use in C++ as possible
 * while also being as efficient as the best C implementations.
 *
 * The truth about an MD5 class is that you should never really need it, so compiling with KH_NO_MD5_SUPPORT is always
 * a good idea unless you are actually using it.
 *
 * \note All of the methods in this object can potentially change the object's state, so having a "const Md5Hash" makes
 * no sense and should be avoided. If you want to have a constant hash sum, use
 * `const Md5Sum mySum = myMd5Hash.getMd5()` to keep the hash sum constant, but don't `const` the Md5Hash object itself.
 */
class KH_EXPORT Md5Hash
{
    // MD5 blocks are 512 bits in size per RFC1321. This holds that amount in bytes.
    constexpr static auto BLOCK_SIZE = bytesNeededForBits(512);

    // The four accumulators for MD5. These are initialized in the constructor to keep the initial values out of the
    // header.
    u32 A, B, C, D;

    u64 totalInput = 0;
    byte buffer[BLOCK_SIZE]{ byte(0) };
    size_t bufferPos = 0;

    // Because this object has an 8 byte alignment, this bool takes 8 bytes. C'est la vie.
    bool finished = false;

public:
    /*!
     * Basic constructor that creates a Md5Hash with no input.
     *
     * The state is valid, so you can call getMd5() on this Md5Hash and get a valid hash with no input provided to the
     * algorithm (as though it had an empty string as input).
     */
    Md5Hash() noexcept;

    /*!
     * Input consuming constructor for Md5Hash.
     *
     * This constructor allows you to combine a construction with a provideInput() call, so the arguments passed are
     * simply redirected to provideInput() after a valid state has been initialized.
     *
     * \param length The length of the passed data byte array.
     * \param data A pointer to an array of `const std::byte` to be added as input to the this Md5Hash sum.
     */
    Md5Hash(size_t length, byte const *data) noexcept;

    /*!
     * Input consuming constructor for Md5Hash.
     *
     * This constructor allows you to combine a construction with a provideInput() call, so the arguments passed are
     * simply redirected to provideInput() after a valid state has been initialized.
     *
     * \param length The length of the passed data byte array.
     * \param data A pointer to an array of any const data to be added as input to the this Md5Hash sum.
     */
    Md5Hash(size_t length, auto const *data) noexcept :
        Md5Hash(length * sizeof(decltype(*data)), std::bit_cast<byte const *>(data))
    {
        // No implementation.
    }

    /*!
     * Input consuming constructor for Md5Hash.
     *
     * This constructor allows you to combine a construction with a provideInput() call, so the argument passed is
     * simply redirected to provideInput() after a valid state has been initialized.
     *
     * \param data A std::span<T,E> of the data to be added as input to the this Md5Hash sum.
     */
    template <typename T, size_t E = std::dynamic_extent>
    explicit Md5Hash(span<T, E> data) noexcept : Md5Hash(data.size_bytes(), std::as_bytes(data).data())
    {
        // No implementation.
    }

    /*!
     * Input consuming constructor for Md5Hash.
     *
     * This constructor allows you to combine a construction with a provideInput() call, so the argument passed is
     * simply redirected to provideInput() after a valid state has been initialized.
     *
     * \param data A std::basic_string_view<CharType> of the data to be added as input to the this Md5Hash sum.
     */
    template <typename CharType>
    explicit Md5Hash(std::basic_string_view<CharType> data) noexcept :
        Md5Hash(data.size() * sizeof(CharType), std::bit_cast<byte const *>(data.data()))
    {
        // No implementation.
    }

    /*!
     * Provides additional input to the Md5Hash sum.
     *
     * If you do not know the total size of the file or source you are hashing, this allows you to get multiple blocks
     * of input that you process in series. This allows for arbitrarily long input.
     *
     * If getMd5() has been called on this Md5Hash, then this method will do nothing at all. This method returns zero
     * instead of \p length if that is the case.
     *
     * \param length The length of the passed data byte array.
     * \param data A pointer to an array of `const std::byte` to be added as input to the this Md5Hash sum.
     * \return The number of bytes processed. Should be equal to \p length, or 0 if Md5Hash::getMd5() has been called.
     */
    size_t provideInput(size_t length, byte const *data) noexcept;

    /*!
     * Provides additional input to the Md5Hash sum.
     *
     * If you do not know the total size of the file or source you are hashing, this allows you to get multiple blocks
     * of input that you process in series. This allows for arbitrarily long input.
     *
     * If getMd5() has been called on this Md5Hash, then this method will do nothing at all. This method returns zero
     * instead of \p length if that is the case.
     *
     * \param length The length of the passed data byte array.
     * \param data A pointer to an array of any type to be added as input to the this Md5Hash sum.
     * \return The number of bytes processed. Should be equal to \p length, or 0 if Md5Hash::getMd5() has been called.
     */
    size_t provideInput(size_t length, auto const *data) noexcept
    {
        return provideInput(length * sizeof(decltype(*data)), std::bit_cast<byte const *>(data));
    }

    /*!
     * Provides additional input to the Md5Hash sum.
     *
     * If you do not know the total size of the file or source you are hashing, this allows you to get multiple blocks
     * of input that you process in series. This allows for arbitrarily long input.
     *
     * If getMd5() has been called on this Md5Hash, then this method will do nothing at all. This method returns zero
     * instead of \p length if that is the case.
     *
     * \param data A std::span<T> of the data to be added as input to the this Md5Hash sum.
     * \return The number of bytes processed. Should be equal to std::span::size_bytes(), or 0 if Md5Hash::getMd5() has
     * been called.
     */
    template <typename T, size_t E = std::dynamic_extent>
    size_t provideInput(span<T, E> data) noexcept
    {
        return provideInput(data.size_bytes(), std::as_bytes(data).data());
    }

    /*!
     * Provides additional input to the Md5Hash sum.
     *
     * If you do not know the total size of the file or source you are hashing, this allows you to get multiple blocks
     * of input that you process in series. This allows for arbitrarily long input.
     *
     * If getMd5() has been called on this Md5Hash, then this method will do nothing at all. This method returns zero
     * instead of \p length if that is the case.
     *
     * \param data A std::span<T> of the data to be added as input to the this Md5Hash sum.
     * \return The number of bytes processed. Should be equal to std::span::size_bytes(), or 0 if Md5Hash::getMd5() has
     * been called.
     */
    template <typename CharType>
    size_t provideInput(std::basic_string_view<CharType> data) noexcept
    {
        return provideInput(data.size() * sizeof(CharType), std::bit_cast<byte const *>(data.data()));
    }

    /*!
     * Finishes the hash sum computation if it hasn't been already and returns the complete Md5Sum.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5Hash computation which does change the object state, so this cannot be called on a const object. Md5Hash
     * should never really be used const, as all methods can change the Md5Hash object's state, and if you need the hash
     * to be const you just pass a const Md5Sum instead.
     *
     * \return A std::array of MD5SUM_RETURN_SIZE std::byte values. On 8-bit byte platforms, a total size of 16 bytes.
     */
    [[nodiscard]] Md5Sum getMd5() noexcept;

    /*!
     * Finishes the hash sum computation if it hasn't been already and writes the complete MD5 sum to \p output.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5Hash computation which does change the object state, so this cannot be called on a const object. Md5Hash
     * should never really be used const, as all methods can change the Md5Hash object's state, and if you need the hash
     * to be const you just pass a const Md5Sum instead.
     *
     * The output array must point to a memory location with at least 16 bytes of available data. This method will not
     * write any more than 16 bytes.
     *
     * \param output A std::byte pointer to a location in memory at least 16 bytes long to write the completed hash sum.
     */
    void getMd5(byte *output) noexcept;

    /*!
     * Finishes the hash sum computation if it hasn't been already and writes the complete MD5 sum to \p output.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5Hash computation which does change the object state, so this cannot be called on a const object. Md5Hash
     * should never really be used const, as all methods can change the Md5Hash object's state, and if you need the hash
     * to be const you just pass a const Md5Sum instead.
     *
     * This method takes a span, and will fail to compile when the span is less than MD5SUM_RETURN_SIZE bytes in length.
     * If the span uses std::dynamic_extent, then it will provide either MD5SUM_RETURN_SIZE bytes if
     * `output.size_bytes()` returns greater than MD5SUM_RETURN_SIZE, and will truncate the bytes up to that amount if
     * it is less than MD5SUM_RETURN_SIZE. This is to ensure this method never throws exceptions.
     *
     * \param output A std::span<T, E> of the block which to output the MD5 sum to, which must be at least 16 bytes.
     */
    template <ByteType T = byte, size_t E = std::dynamic_extent>
    void getMd5(span<T, E> output) noexcept requires((not std::is_const_v<T>) and E >= MD5SUM_RETURN_SIZE)
    {
        if constexpr (E == std::dynamic_extent)
        {
            if (size_t smaller = output.size_bytes(); smaller < MD5SUM_RETURN_SIZE)
            {
                memcpy(output.data(), getMd5().data(), smaller);
                return;
            }
        }

        getMd5(std::bit_cast<byte *>(output.data()));
    }

private:
    struct Impl;
};

/*!
 * A quick and simple MD5 hash implementation that works on an arbitrary set of constant bytes passed as data.
 *
 * This implementation does not throw exceptions and should always successfully return an array of byte data on all
 * supported platforms. This is intended to be very reliable so that it can work in all circumstances. The function is
 * guaranteed to return an array equal to the data's MD5 hash. On 8-bit byte platforms, the number of chars returned is
 * 16.
 *
 * While this implementation does its best to fully implement RFC1321, it is constrained in that this algorithm does NOT
 * accept any arbitrary number of bits, but rather the amount of data provided must be in bytes and therefore any MD5
 * hash of a value that takes a number of bits that isn't a multiple of #BYTE_BITS is impossible to generate with this
 * function.
 *
 * The data passed can be of arbitrary length, passed as the first argument. Since length is passed, data may contain
 * bytes with the value of 0 and the hash will still work correctly.
 *
 * This version of the function outputs the MD5 hash (as an array of 16 bytes, not hexadecimal) to the output char
 * array. If you do not have an array to write to, use the other function.
 *
 * \see KirHut::getMd5(size_t,auto*)
 * \param length A std::size_t of the data size. It should be in number of *elements*, not number of *bytes*!
 * \param data A pointer to any data to return a hash sum of.
 * \param[out] output A pointer to a buffer location to write the hash to. Must be at least #MD5SUM_RETURN_SIZE bytes in
 * length.
 */
void getMd5(size_t length, auto const *data, byte *output) noexcept
{
    Md5Hash(length, data).getMd5(output);
}

/*!
 * A quick and simple MD5 hash implementation that works on an arbitrary set of constant bytes passed as data.
 *
 * This implementation does not throw exceptions and should always successfully return an array of byte data on all
 * supported platforms. This is intended to be very reliable so that it can work in all circumstances. The function is
 * guaranteed to return an array equal to the data's MD5 hash. On 8-bit byte platforms, the number of chars returned is
 * 16.
 *
 * While this implementation does its best to fully implement RFC1321, it is constrained in that this algorithm does NOT
 * accept any arbitrary number of bits, but rather the amount of data provided must be in bytes and therefore any MD5
 * hash of a value that takes a number of bits that isn't a multiple of #BYTE_BITS is impossible to generate with this
 * function.
 *
 * The data passed can be of arbitrary length, passed as the first argument. Since length is passed, data may contain
 * bytes with the value of 0 and the hash will still work correctly.
 *
 * This version of the function returns an Md5Sum of the completed MD5 hash sum. If you want to write to a buffer, use
 * the other function.
 *
 * \see KirHut::getMd5(size_t,auto*,byte*)
 * \param length A std::size_t of the data size. It should be in number of *elements*, not number of *bytes*!
 * \param data A pointer to any const data to return a hash sum of.
 * \return A std::array of #MD5SUM_RETURN_SIZE char values. On 8-bit byte platforms, a total size of 16 bytes.
 */
[[nodiscard]] Md5Sum getMd5(size_t length, auto const *data) noexcept
{
    return Md5Hash(length, data).getMd5();
}

/*!
 * A quick and simple MD5 hash implementation that works on an arbitrary set of constant bytes passed as data.
 *
 * This implementation does not throw exceptions and should always successfully return an array of byte data on all
 * supported platforms. This is intended to be very reliable so that it can work in all circumstances. The function is
 * guaranteed to return an array equal to the data's MD5 hash. On 8-bit byte platforms, the number of chars returned is
 * 16.
 *
 * While this implementation does its best to fully implement RFC1321, it is constrained in that this algorithm does NOT
 * accept any arbitrary number of bits, but rather the amount of data provided must be in bytes and therefore any MD5
 * hash of a value that takes a number of bits that isn't a multiple of #BYTE_BITS is impossible to generate with this
 * function.
 *
 * The data passed can be of arbitrary length, passed as the first argument. Since length is passed, data may contain
 * bytes with the value of 0 and the hash will still work correctly.
 *
 * This version of the function outputs the MD5 hash (as an array of 16 bytes, not hexadecimal) to the output char
 * array. If you do not have an array to write to, use the other function.
 *
 * \see KirHut::getMd5(span<T,E>)
 * \param data A std::span<T,E> to return a hash sum of.
 * \param[out] output A pointer to a buffer location to write the hash to. Must be at least #MD5SUM_RETURN_SIZE bytes in
 * length.
 */
template <typename T, size_t E = std::dynamic_extent>
void getMd5(span<T, E> const data, byte *output) noexcept
{
    Md5Hash(data).getMd5(output);
}

/*!
 * A quick and simple MD5 hash implementation that works on an arbitrary set of constant bytes passed as data.
 *
 * This implementation does not throw exceptions and should always successfully return an array of byte data on all
 * supported platforms. This is intended to be very reliable so that it can work in all circumstances. The function is
 * guaranteed to return an array equal to the data's MD5 hash. On 8-bit byte platforms, the number of chars returned is
 * 16.
 *
 * While this implementation does its best to fully implement RFC1321, it is constrained in that this algorithm does NOT
 * accept any arbitrary number of bits, but rather the amount of data provided must be in bytes and therefore any MD5
 * hash of a value that takes a number of bits that isn't a multiple of #BYTE_BITS is impossible to generate with this
 * function.
 *
 * The data passed can be of arbitrary length, passed as the first argument. Since length is passed, data may contain
 * bytes with the value of 0 and the hash will still work correctly.
 *
 * This version of the function returns an Md5Sum of the completed MD5 hash sum. If you want to write to a buffer, use
 * the other function.
 *
 * \see KirHut::getMd5(span<T,E>,byte*)
 * \param data A std::span<T,E> to return a hash sum of.
 * \return A std::array of #MD5SUM_RETURN_SIZE char values. On 8-bit byte platforms, a total size of 16 bytes.
 */
template <typename T, size_t E = std::dynamic_extent>
[[nodiscard]] Md5Sum getMd5(span<T, E> const data) noexcept
{
    return Md5Hash(data).getMd5();
}

/*!
 * A quick and simple MD5 hash implementation that works on an arbitrary set of constant bytes passed as data.
 *
 * This implementation does not throw exceptions and should always successfully return an array of byte data on all
 * supported platforms. This is intended to be very reliable so that it can work in all circumstances. The function is
 * guaranteed to return an array equal to the data's MD5 hash. On 8-bit byte platforms, the number of chars returned is
 * 16.
 *
 * While this implementation does its best to fully implement RFC1321, it is constrained in that this algorithm does NOT
 * accept any arbitrary number of bits, but rather the amount of data provided must be in bytes and therefore any MD5
 * hash of a value that takes a number of bits that isn't a multiple of #BYTE_BITS is impossible to generate with this
 * function.
 *
 * The data passed can be of arbitrary length, passed as the first argument. Since length is passed, data may contain
 * bytes with the value of 0 and the hash will still work correctly.
 *
 * This version of the function outputs the MD5 hash (as an array of 16 bytes, not hexadecimal) to the output char
 * array. If you do not have an array to write to, use the other function.
 *
 * \see KirHut::Md5Hash::getMd5(span<T,E>)
 * \param data A std::span<T,E> to return a hash sum of.
 * \param[out] output A std::span to a buffer location to write the hash to. Should be at least #MD5SUM_RETURN_SIZE
 * bytes in length.
 */
template <typename T, ByteType B = byte, size_t E = std::dynamic_extent, size_t S = std::dynamic_extent>
void getMd5(span<T, E> const data, span<B, S> output) noexcept
{
    Md5Hash(data).getMd5(output);
}

} // namespace KirHut
