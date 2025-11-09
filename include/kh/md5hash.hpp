/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/md5hash.hpp
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

#if defined(KH_INCLUDE_MD5HASH) or defined(KH_PRIV_DOCS)
# include "kh/base.hpp"
#endif

namespace KirHut
{

/*!
 * Indication boolean for when the KirHut library supports MD5 Hashing.
 *
 * When you need to check if this library includes the MD5Hash object under the KirHut namespace in an if constexpr
 * expression rather than the preprocessor, you can use this to check.
 *
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool hasMd5 = false
#if defined(KH_INCLUDE_MD5HASH)
                                         or true
#endif
    ;

#if defined(KH_INCLUDE_MD5HASH) or defined(KH_PRIV_DOCS)
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
    template <typename T>
    inline Md5Hash(size_t length, T const *data) noexcept :
        Md5Hash(length * sizeof(T), reinterpret_cast<byte const *>(data))
    {
        // No further implementation.
    }

    /*!
     * Input consuming constructor for Md5Hash.
     *
     * This constructor allows you to combine a construction with a provideInput() call, so the argument passed is
     * simply redirected to provideInput() after a valid state has been initialized.
     *
     * \param data A std::span<T,E> of the data to be added as input to the this Md5Hash sum.
     */
    template <typename T, size_t E>
    inline explicit Md5Hash(span<T, E> data) noexcept : Md5Hash(data.size_bytes(), std::as_bytes(data).data())
    {
        // No further implementation.
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
    inline explicit Md5Hash(std::basic_string_view<CharType> data) noexcept :
        Md5Hash(data.size() * sizeof(CharType), reinterpret_cast<byte const *>(data.data()))
    {
        // No further implementation.
    }

    /*!
     * Provides additional input to the Md5Hash sum.
     *
     * If you do not know the total size of the file or source you are hashing, this allows you to get multiple blocks
     * of input that you process in series. This allows for arbitrarily long input.
     *
     * If getMd5(byte*) has been called on this Md5Hash, then this method will do nothing at all. This method returns
     * zero instead of \p length if that is the case.
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
     * If getMd5(byte*) has been called on this Md5Hash, then this method will do nothing at all. This method returns
     * zero instead of \p length if that is the case.
     *
     * \param length The length of the passed data byte array.
     * \param data A pointer to an array of any type to be added as input to the this Md5Hash sum.
     * \return The number of bytes processed. Should be equal to \p length, or 0 if Md5Hash::getMd5() has been called.
     */
    inline size_t provideInput(size_t length, auto const *data) noexcept
    {
        return provideInput(length * sizeof(decltype(*data)), reinterpret_cast<byte const *>(data));
    }

    /*!
     * Provides additional input to the Md5Hash sum.
     *
     * If you do not know the total size of the file or source you are hashing, this allows you to get multiple blocks
     * of input that you process in series. This allows for arbitrarily long input.
     *
     * If getMd5(byte*) has been called on this Md5Hash, then this method will do nothing at all. This method returns
     * zero instead of \p length if that is the case.
     *
     * \param data A std::span<T> of the data to be added as input to the this Md5Hash sum.
     * \return The number of bytes processed. Should be equal to std::span::size_bytes(), or 0 if Md5Hash::getMd5() has
     * been called.
     */
    template <typename T, size_t E = std::dynamic_extent>
    inline size_t provideInput(span<T, E> data) noexcept
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
     * instead of the size of \p data in bytes if that is the case.
     *
     * \tparam Char_T The character type of the passed-in string_view.
     * \param data A std::span<T> of the data to be added as input to the this Md5Hash sum.
     * \return The number of bytes processed. Should be equal to data.size() * sizeof(Char_T), or 0 if Md5Hash::getMd5()
     * has been called.
     */
    template <typename Char_T>
    inline size_t provideInput(std::basic_string_view<Char_T> data) noexcept
    {
        return provideInput(data.size() * sizeof(Char_T), reinterpret_cast<byte const *>(data.data()));
    }

    /*!
     * Finishes the hash sum computation if it hasn't been already and returns the complete Md5Sum.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5Hash computation which does change the object state, so this cannot be called on a const object. Md5Hash
     * should never really be used const, as all methods can change the Md5Hash object's state, and if you need the hash
     * to be const you just pass a const Md5Sum instead.
     *
     * \return A std::array of MD5SUM_RETURN_SIZE std::byte values. which should be a total size of 16 bytes.
     */
    [[nodiscard]] inline Md5Sum getMd5() noexcept
    {
        Md5Sum sum;
        getMd5(sum.data());
        return sum;
    }

    /*!
     * Finishes the hash sum computation if it hasn't been already and writes the complete MD5 sum to \p output.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5Hash computation which does change the object state, so this cannot be called on a const object. Md5Hash
     * should never really be used const, as all methods can change the Md5Hash object's state, and if you need the hash
     * to be const you just pass a const Md5Sum instead.
     *
     * Passing a null pointer to this method will result in the call finishing the hash sum computation but otherwise
     * not writing the result to any buffer. This can be used to manually finish the computation without writing the
     * result to any buffer.
     *
     * \warning Passing a pointer to a buffer of bytes that is smaller than 16 bytes in length, or is pointing to a
     * buffer with a non-byte type object initialized within it results in undefined behavior. Under no circumstances
     * should you pass a pointer to a buffer of less than 16 allocated bytes for the purpose of containing an MD5 hash
     * sum.
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
    inline void getMd5(span<T, E> output) noexcept(E != std::dynamic_extent)
        requires(std::is_same_v<T, std::remove_cv_t<T>> and E >= MD5SUM_RETURN_SIZE)
    {
        if constexpr (E == std::dynamic_extent)
        {
            if (size_t smaller = output.size_bytes(); smaller < MD5SUM_RETURN_SIZE)
            {
                throwBadHashOutputSize(smaller);
            }
        }

        getMd5(reinterpret_cast<byte *>(output.data()));
    }

    /*!
     * Finishes the hash sum computation if it hasn't been already and writes the complete MD5 sum to fit in \p output.
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
     * \param output A std::span<Byte_T, byteExt> of the block which to output the MD5 sum to, which must be at least 16
     * bytes.
     */
    template <ByteType Byte_T = byte, size_t byteExt = std::dynamic_extent>
    inline void fitMd5(span<Byte_T, byteExt> output) noexcept
        requires(std::is_same_v<Byte_T, std::remove_cv_t<Byte_T>> and byteExt >= MD5SUM_RETURN_SIZE)
    {
        if constexpr (byteExt == std::dynamic_extent)
        {
            if (size_t smaller = output.size_bytes(); smaller < MD5SUM_RETURN_SIZE)
            {
                memcpy(output.data(), getMd5().data(), smaller);
                return;
            }
        }

        getMd5(reinterpret_cast<byte *>(output.data()));
    }

private:
    struct Impl;

    [[noreturn]] void throwBadHashOutputSize(size_t size);
};

/*!
 * A quick and simple MD5 hash implementation that works on an arbitrary set of constant bytes passed as \p data.
 *
 * This implementation does not throw exceptions and should always successfully return an array of byte \p data on all
 * supported platforms. This is intended to be very reliable so that it can work in all circumstances. The function is
 * guaranteed to return an array equal to the data's MD5 hash. On 8-bit byte platforms, the number of chars returned is
 * 16.
 *
 * While this implementation does its best to fully implement RFC1321, it is constrained in that this algorithm does NOT
 * accept any arbitrary number of bits, but rather the amount of data provided must be in bytes and therefore any MD5
 * hash of a value that takes a number of bits that isn't a multiple of BYTE_BITS is impossible to generate with this
 * function.
 *
 * The data passed can be of arbitrary length, passed as the first argument. Since \p length is passed separately, \p
 * data may contain bytes with all bits set to 0 and the hash will still work correctly.
 *
 * This version of the function outputs the MD5 hash (as an array of 16 bytes, not hexadecimal) to the output char
 * array. If you do not have an array to write to, use the other function.
 *
 * \warning Passing a pointer to a buffer of bytes that is smaller than 16 bytes in length, or is pointing to a
 * buffer with a non-byte type object initialized within it results in undefined behavior. Under no circumstances
 * should you pass a pointer to a buffer of less than 16 allocated bytes for the purpose of containing an MD5 hash
 * sum.
 *
 * \see KirHut::getMd5(size_t,auto*)
 * \param length A std::size_t of the data size. It should be in number of *elements*, not number of *bytes*!
 * \param data A pointer to any data to return a hash sum of.
 * \param[out] output A pointer to a buffer location to write the hash to. Must be at least #MD5SUM_RETURN_SIZE bytes in
 * length.
 */
inline void getMd5(size_t length, auto const *data, byte *output) noexcept
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
 * hash of a value that takes a number of bits that isn't a multiple of BYTE_BITS is impossible to generate with this
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
[[nodiscard]] inline Md5Sum getMd5(size_t length, auto const *data) noexcept
{
    return Md5Hash(length, data).getMd5();
}

/*!
 * A quick and simple MD5 hash implementation that works on an arbitrary set of constant bytes passed as data.
 *
 * This implementation does not throw exceptions and should always successfully return an array of byte data on all
 * supported platforms. This is intended to be very reliable so that it can work in all circumstances. The function is
 * guaranteed to return an array equal to the data's MD5 hash. On 8-bit byte platforms, the number of bytes returned is
 * 16.
 *
 * While this implementation does its best to fully implement RFC1321, it is constrained in that this algorithm does NOT
 * accept any arbitrary number of bits, but rather the amount of data provided must be in bytes and therefore any MD5
 * hash of a value that takes a number of bits that isn't a multiple of BYTE_BITS is impossible to generate with this
 * function.
 *
 * The data passed can be of arbitrary length, passed as the first argument. Since length is passed, data may contain
 * bytes with the value of 0 and the hash will still work correctly.
 *
 * This version of the function outputs the MD5 hash (as an array of 16 bytes, not hexadecimal) to the output char
 * array. If you do not have an array to write to, use the other function.
 *
 * \see KirHut::getMd5(span<T,tExt>)
 * \param data A span of any type T to return a hash sum of.
 * \param[out] output A pointer to a buffer location to write the hash to. Must be at least #MD5SUM_RETURN_SIZE bytes in
 * length.
 */
template <typename T, size_t tExt = std::dynamic_extent>
inline void getMd5(span<T const, tExt> data, byte *output) noexcept
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
 * hash of a value that takes a number of bits that isn't a multiple of BYTE_BITS is impossible to generate with this
 * function.
 *
 * The data passed can be of arbitrary length, passed as the first argument. Since length is passed, data may contain
 * bytes with the value of 0 and the hash will still work correctly.
 *
 * This version of the function returns an Md5Sum of the completed MD5 hash sum. If you want to write to a buffer, use
 * the other function.
 *
 * \see KirHut::getMd5(span<T,tExt>,byte*)
 * \param data A span of any type T to return a hash sum of.
 * \return A std::array of #MD5SUM_RETURN_SIZE char values. On 8-bit byte platforms, a total size of 16 bytes.
 */
template <typename T, size_t tExt = std::dynamic_extent>
[[nodiscard]] inline Md5Sum getMd5(span<T const, tExt> data) noexcept
{
    return Md5Hash(data).getMd5();
}

/*!
 * A quick and simple MD5 hash implementation that works on an arbitrary set of constant bytes passed as data.
 *
 * This implementation does not throw exceptions when provided a sufficiently large output buffer and should
 * successfully write the resulting MD5 sum in the \p output buffer. This is intended to be very reliable so that it can
 * work in all circumstances. The function is guaranteed to write an array equal to the data's MD5 hash. On 8-bit byte
 * platforms, the number of bytes written is 16.
 *
 * While this implementation does its best to fully implement RFC1321, it is constrained in that this algorithm does NOT
 * accept any arbitrary number of bits, but rather the amount of data provided must be in bytes and therefore any MD5
 * hash of a value that takes a number of bits that isn't a multiple of BYTE_BITS is impossible to generate with this
 * function.
 *
 * The data passed can be of arbitrary length, passed as the first argument. Since length is passed, data may contain
 * bytes with the value of 0 and the hash will still work correctly.
 *
 * This version of the function outputs the MD5 hash (as an array of 16 bytes, not hexadecimal) to the output char
 * array. If you do not have an array to write to, use a version of getMd5() that returns an Md5Sum object.
 *
 * \see KirHut::Md5Hash::getMd5(span<T,tExt>)
 * \tparam T Any input data type used as input through \p data.
 * \tparam Byte_T The byte type that will be written to for \p output.
 * \tparam tExt The compile-time size of \p data, usually std::dynamic_extent.
 * \tparam byteExt The compile-time size of the \p output buffer, usually std::dynamic_extent, but this function is
 * guaranteed not to throw exceptions if it is not.
 * \param data A std::span<T,E> to return a hash sum of.
 * \param[out] output A std::span to a buffer location to write the hash to. Should be at least #MD5SUM_RETURN_SIZE
 * bytes in length.
 * \throws IllegalArgument If the size of the \p output buffer is less than #MD5SUM_RETURN_SIZE bytes.
 */
template <typename T, ByteType Byte_T = byte, size_t tExt = std::dynamic_extent, size_t byteExt = std::dynamic_extent>
inline void getMd5(span<T const, tExt> data, span<Byte_T, byteExt> output) noexcept(byteExt != std::dynamic_extent)
    requires(std::is_same_v<Byte_T, std::remove_cv_t<Byte_T>> and byteExt >= MD5SUM_RETURN_SIZE)
{
    Md5Hash(data).getMd5(output);
}

#endif // defined(KH_INCLUDE_MD5HASH) or defined(KH_PRIV_DOCS)

} // namespace KirHut
