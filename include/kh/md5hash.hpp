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

#include "kh/global.hpp"

#if defined(KH_INCLUDE_MD5HASH) or defined(KH_PRIV_DOCS)
# include "kh/base.hpp"
#endif

namespace KirHut
{

KH_INLINE_NAMESPACE_V1

namespace Hash
{

/*!
 * Indication boolean for when the KirHut library supports MD5 Hashing.
 *
 * When you need to check if this library includes the Md5 object under the KirHut::Hash namespace in an if constexpr
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
constexpr auto md5SumSize = bytesNeededForBits(128);

/*!
 * Simple typedef of an array that will fit an MD5 Hash sum.
 *
 * Use it just like a std::array. The underlying type is std::byte, not char or unsigned char, so you will need to
 * convert if you need a char type.
 */
using Md5Sum = array<byte, md5SumSize>;

/*!
 * Simple typedef of a span over a block of bytes that an MD5 Hash sum could be output to.
 *
 * You usually use this by wrapping some other set of data or span that can fit within md5SumSize with this span, and
 * passing that to the Md5::getHash() function.
 */
using Md5SumSpan = span<byte, md5SumSize>;

/*!
 * The MD5 Hash sum generator class.
 *
 * This class is designed to have an efficient and easy to verify C++ hash implementation that never throws exceptions
 * and follows modern C++20 practices. This class does not allocate memory and is safe to use in memory constrained
 * situations. The object does not yet support byte lengths that aren't 8 bits, but other than that it should support
 * ints of arbitrary length (though it does require the platform to have an integer type of at least 32 and 64 bits).
 *
 * As an object following modern C++ practices, input is preferred as const std::byte rather than const char, but allows
 * passing std::spans of any type and of arbitrary length, and returning an Md5Sum as a result instead of filling a
 * buffer provided by the user. This MD5 hash sum algorithm is intended to be a model implementation of the algorithm,
 * providing accurate documentation, flexible implementations, and equivalent resource and speed efficiency as the best
 * hand-optimized C implementations.
 *
 * This object correctly follows Rule of Zero, with copy/move operations and destruction all working the way you would
 * expect them to. If you create an Md5 object, provide it some input, then copy that Md5 object and provide
 * different input to the two separate copies, you will generate two different MD5 sums that act as if the first part of
 * the input was the same and the second portion of the input was different. You can create an Md5 object, provide
 * it input, then make a copy. The original can then be finished with a call to getHash(), while still computing
 * additional hashes using the same input with the copy of the Md5 object. This allows you to attempt to author an
 * addition to a message that is equal to the original MD5 hash (if your intent is to try and calculate a collision).
 *
 * \note All of the methods in this object can potentially change the object's state, so having a "const Md5" makes
 * no sense and should be avoided. If you want to have a constant hash sum, use `const Md5Sum mySum = myMd5.getHash()`
 * to keep the hash sum constant, but don't `const` the Md5 object itself.
 */
class KH_EXPORT Md5
{
    // MD5 blocks are 512 bits in size per RFC1321. This holds that amount in bytes.
    constexpr static auto BLOCK_SIZE = bytesNeededForBits(512);
    using BufferBlock                = array<byte, BLOCK_SIZE>;

    // The four accumulators for MD5. These are initialized in the constructor to keep the initial values out of the
    // header.
    u32 A, B, C, D;

    bool finished  = false;
    i32 bufferPos  = 0;
    u64 totalInput = 0;
    BufferBlock buffer{};

public:
    /*!
     * Basic constructor that creates a Md5 with no input.
     *
     * The state is valid, so you can call getHash() on this Md5 and get a valid hash with no input provided to the
     * algorithm (as though it had an empty string as input).
     */
    Md5() noexcept;

    /*!
     * Input consuming constructor for Md5.
     *
     * This constructor allows you to combine a construction with a provideInput() call, so the arguments passed are
     * simply redirected to provideInput() after a valid state has been initialized. Note that this does **not** finish
     * the MD5 hashing state, so you may perform additional calls to provideInput() before calling getHash() to finish
     * the computation.
     *
     * \param input A std::span of `const std::byte` to be added as input to the this Md5 sum.
     */
    explicit Md5(span<byte const> input) noexcept;

    /*!
     * Input consuming constructor for Md5.
     *
     * This constructor allows you to combine a construction with a provideInput() call, so the arguments passed are
     * simply redirected to provideInput() after a valid state has been initialized. Note that this does **not** finish
     * the MD5 hashing state, so you may perform additional calls to provideInput() before calling getHash() to finish
     * the computation.
     *
     * \param length The length of the passed array in elements.
     * \param input A contiguous iterator (such as a pointer) to an array of any const data to use as input to this
     * Md5 sum.
     */
    inline Md5(size_t length, std::contiguous_iterator auto input) noexcept;

    /*!
     * Input consuming constructor for Md5.
     *
     * This constructor allows you to combine a construction with a provideInput() call, so the arguments passed are
     * simply redirected to provideInput() after a valid state has been initialized. Note that this does **not** finish
     * the MD5 hashing state, so you may perform additional calls to provideInput() before calling getHash() to finish
     * the computation.
     *
     * \param input A contiguous iterator (such as a pointer) to an array of any const data to use as input to this
     * Md5 sum.
     * \param length The length of the passed array in elements.
     */
    inline Md5(std::contiguous_iterator auto input, size_t length) noexcept;

    /*!
     * Input consuming constructor for Md5.
     *
     * This constructor allows you to combine a construction with a provideInput() call, so the argument passed is
     * simply redirected to provideInput() after a valid state has been initialized. Note that this does **not** finish
     * the MD5 hashing state, so you may perform additional calls to provideInput() before calling getHash() to finish
     * the computation.
     *
     * \param input A std::span of any data type to be used as input to the this Md5 sum.
     */
    inline explicit Md5(ReadableSpanOf auto input) noexcept
        requires(not std::same_as<byte, std::remove_cv_t<typename decltype(input)::element_type>>);

    /*!
     * Input consuming constructor for Md5.
     *
     * This constructor allows you to combine a construction with a provideInput() call, so the argument passed is
     * simply redirected to provideInput() after a valid state has been initialized. Note that this does **not** finish
     * the MD5 hashing state, so you may perform additional calls to provideInput() before calling getHash() to finish
     * the computation.
     *
     * \param input A std::basic_string_view<Char_T> of the data to be used as input to the this Md5 sum.
     */
    inline explicit Md5(StringLike auto input) noexcept;

    /*!
     * Provides additional input to the Md5 sum.
     *
     * If you do not know the total size of the file or source you are hashing, this allows you to get multiple blocks
     * of input that you process in series. This allows for arbitrarily long input.
     *
     * If any getHash() function has been called on this Md5 prior to a provideInput() call, then this method will do
     * nothing at all. This method returns zero instead of the size of \p input if that is the case.
     *
     * \param input A span of constant bytes that come from any data source (usually through a call to std::as_bytes()).
     * \return The number of bytes processed. Should be equal to the size of \p input, or 0 if getHash() has been
     * called.
     */
    size_t provideInput(span<byte const> input) noexcept;

    /*!
     * Provides additional input to the Md5 sum.
     *
     * If you do not know the total size of the file or source you are hashing, this allows you to get multiple blocks
     * of input that you process in series. This allows for arbitrarily long input.
     *
     * If any getHash() function has been called on this Md5 prior to a provideInput() call, then this method will do
     * nothing at all. This method returns zero instead of \p length if that is the case.
     *
     * \param length The length of the passed data byte array, in elements.
     * \param input A contiguous iterator (such as a pointer) to an array of any type to be added as input to the this
     * Md5 sum.
     * \return The number of bytes processed. Should be equal to \p length times the type of \p data, or 0 if getHash()
     * has been called.
     */
    inline size_t provideInput(size_t length, std::contiguous_iterator auto input) noexcept;

    /*!
     * Provides additional input to the Md5 sum.
     *
     * If you do not know the total size of the file or source you are hashing, this allows you to get multiple blocks
     * of input that you process in series. This allows for arbitrarily long input.
     *
     * If any getHash() function has been called on this Md5 prior to a provideInput() call, then this method will do
     * nothing at all. This method returns zero instead of \p length if that is the case.
     *
     * \param input A contiguous iterator (such as a pointer) to an array of any type to be added as input to the this
     * Md5 sum.
     * \param length The length of the passed data byte array, in elements.
     * \return The number of bytes processed. Should be equal to \p length times the type of \p data, or 0 if getHash()
     * has been called.
     */
    inline size_t provideInput(std::contiguous_iterator auto input, size_t length) noexcept;

    /*!
     * Provides additional input to the Md5 sum.
     *
     * If you do not know the total size of the file or source you are hashing, this allows you to get multiple blocks
     * of input that you process in series. This allows for arbitrarily long input.
     *
     * If any getHash() function has been called on this Md5 prior to a provideInput() call, then this method will do
     * nothing at all. This method returns zero instead of the size of \p input if that is the case.
     *
     * \param input A span of any data type to be added as input to the this Md5 sum.
     * \return The number of bytes processed. Should be equal to the size of \p input, or 0 if getHash() has been
     * called.
     */
    inline size_t provideInput(ReadableSpanOf auto input) noexcept
        requires(not std::same_as<byte, std::remove_cv_t<typename decltype(input)::element_type>>);

    /*!
     * Provides additional input to the Md5 sum.
     *
     * If you do not know the total size of the file or source you are hashing, this allows you to get multiple blocks
     * of input that you process in series. This allows for arbitrarily long input.
     *
     * If any getHash() function has been called on this Md5 prior to a provideInput() call, then this method will do
     * nothing at all. This method returns zero instead of the size of \p input if that is the case.
     *
     * \param input Any StringLike type of string data to be added as input to the this Md5 sum.
     * \return The number of bytes processed. Should be equal to `input.size() * sizeof(Char_T)`, or 0 if getHash() has
     * been called.
     */
    inline size_t provideInput(StringLike auto input) noexcept;

    /*!
     * Finishes the hash sum computation if it hasn't been already and returns a copy of the complete Md5Sum.
     *
     * This method will output an Md5Sum of the result of completing the MD5 hash sum operation, by value. The Md5Sum
     * type is generally 16 bytes large, so there is no real cost to copying it. This method is the preferred way to
     * finish an Md5 object and get the result of that computation.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5 computation which does change the object state, so this cannot be called on a const object or const
     * reference to Md5. Md5 **should never be used const**, as every method can change the Md5 object's
     * state, and if you need the result hash to be const you just pass a const Md5Sum instead. There are **no** legal
     * operations on an Md5 object that does not change the state of that object!
     *
     * \return A std::array of std::byte with a total of #md5SumSize bytes containing the results of this MD5
     * computation.
     */
    [[nodiscard]] inline Md5Sum getHash() noexcept;

    /*!
     * Finishes the hash sum computation if it hasn't been already and writes the complete MD5 sum to \p output.
     *
     * This method will write, as raw bytes, the result of completing the MD5 hash sum operation to the given \p output
     * span. This method uses a fixed size span of bytes, to ensure it is not a template method and may be implemented
     * in source. This method to get the MD5 sum data should only be used if you specifically want to avoid the other
     * methods for some reason, since this is a specific an rather inflexible overload, intended for template method
     * implementations to use.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5 computation which does change the object state, so this cannot be called on a const object or const
     * reference to Md5. Md5 **should never be used const**, as every method can change the Md5 object's
     * state, and if you need the result hash to be const you just pass a const Md5Sum instead. There are **no** legal
     * operations on an Md5 object that does not change the state of that object!
     *
     * \param[out] output A std::span of byte to a location in memory to write the completed hash sum.
     */
    void getHash(Md5SumSpan output) noexcept;

    /*!
     * Finishes the hash sum computation if it hasn't been already and writes the complete MD5 sum to \p output.
     *
     * This method will write, as raw bytes, the result of completing the MD5 hash sum operation to the given \p output
     * iterator. This method operates on std::output_iterator types that also model std::contiguous_iterator, as these
     * can be used directly in a std::span constructor. This is, for example, the overload that is used when you pass a
     * pointer to the getHash() method. If you provide an output iterator that is not a contiguous iterator, this object
     * is forced to create a separate Md5Sum object then copy the results over to the output iterator instead of
     * directly writing. This is an overload for convenience, usually when you would otherwise pass just a pointer, as
     * this method simply wraps that in an Md5SumSpan for the appropriate getHash() call.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5 computation which does change the object state, so this cannot be called on a const object or const
     * reference to Md5. Md5 **should never be used const**, as every method can change the Md5 object's
     * state, and if you need the result hash to be const you just pass a const Md5Sum instead. There are **no** legal
     * operations on an Md5 object that does not change the state of that object!
     *
     * Passing a null pointer to this method will result in the call finishing the hash sum computation but otherwise
     * not writing the result to any buffer. This can be used to manually finish the computation without writing the
     * result to any buffer.
     *
     * \warning Passing a pointer or iterator to a buffer of bytes that is smaller than 16 bytes in length, or is
     * pointing to a buffer with a non-byte type object initialized within it, will result in undefined behavior. Under
     * no circumstances should you pass a pointer or iterator to a buffer of less than 16 allocated bytes for the
     * purpose of containing an MD5 hash sum.
     *
     * \tparam Iterator_T A contiguous output iterator type that should be inferred from the \p output argument.
     * \param[out] output A std::contiguous_iterator of a byte type (such as a pointer) to a location in memory at least
     * 16 bytes long to write the completed hash sum.
     */
    template <std::contiguous_iterator Iterator_T>
    requires std::output_iterator<Iterator_T, std::iter_value_t<Iterator_T>> and ByteType<std::iter_value_t<Iterator_T>>
    inline void getHash(Iterator_T output) noexcept;

    /*!
     * Finishes the hash sum computation if it hasn't been already and writes the complete MD5 sum to \p output.
     *
     * This method will write, as raw bytes, the result of completing the MD5 hash sum operation to the given \p output
     * iterator. This method is an overload for std::output_iterator of byte types that do **not** model
     * std::contiguous_iterator, which forces this object to use a potentially slower algorithm. This overload will
     * create an Md5Sum object of the results of the MD5 operation and then copy over the bytes to the given \p output
     * iterator in a loop, instead of using a quicker memory copy routine. That said, this overload is the best for any
     * output stream to a std::byte sink that writes to non-contiguous memory.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5 computation which does change the object state, so this cannot be called on a const object or const
     * reference to Md5. Md5 **should never be used const**, as every method can change the Md5 object's
     * state, and if you need the result hash to be const you just pass a const Md5Sum instead. There are **no** legal
     * operations on an Md5 object that does not change the state of that object!
     *
     * This overload specifically outputs to std::output_iterator<byte>, which is one of the only two output_iterator
     * types that are supported that isn't a std::contiguous_iterator. This is due to limitations with the C++ language
     * as the template type of a concept cannot be inferred from function arguments. This limitation may be corrected in
     * a future revision of this library.
     *
     * \warning Passing an iterator to a buffer of bytes that is smaller than 16 bytes in length, or is pointing to a
     * buffer with a non-byte type object initialized within it, will result in undefined behavior. Under no
     * circumstances should you pass an iterator to a buffer of less than 16 allocated bytes for the purpose of
     * containing an MD5 hash sum.
     *
     * \param[out] output A std::output_iterator of byte to a location in memory at least 16 bytes long to write the
     * completed hash sum.
     */
    inline void getHash(std::output_iterator<byte> auto output) noexcept
        requires(not std::contiguous_iterator<decltype(output)>);

    /*!
     * Finishes the hash sum computation if it hasn't been already and writes the complete MD5 sum to \p output.
     *
     * This method will write, as raw bytes, the result of completing the MD5 hash sum operation to the given \p output
     * iterator. This method is an overload for std::output_iterator of char types that do **not** model
     * std::contiguous_iterator, which forces this object to use a potentially slower algorithm. This overload will
     * create an Md5Sum object of the results of the MD5 operation and then copy over the bytes to the given \p output
     * iterator in a loop, instead of using a quicker memory copy routine. That said, this overload is the best for any
     * output stream to a char sink that writes to non-contiguous memory.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5 computation which does change the object state, so this cannot be called on a const object. Md5
     * should never really be used const, as all methods can change the Md5 object's state, and if you need the hash
     * to be const you just pass a const Md5Sum instead.
     *
     * This overload specifically outputs to std::output_iterator<char>, which is one of the only two output_iterator
     * types that are supported that isn't a std::contiguous_iterator. This is due to limitations with the C++ language
     * and that, ordinarily, this object passes the output iterator to a std::span, which requires a
     * std::contiguous_iterator. When that is not possible, this method must instead create an Md5Sum using a different
     * getHash() method, then copy over the bytes (as chars) to the output iterator.
     *
     * \warning Passing an iterator to a buffer of chars that is smaller than 16 chars in length, or is pointing to a
     * buffer with a non-char type object initialized within it, will result in undefined behavior. Under no
     * circumstances should you pass an iterator to a buffer of less than 16 allocated chars for the purpose of
     * containing an MD5 hash sum.
     *
     * \param[out] output A std::output_iterator of char to a location in memory at least 16 chars long to write the
     * completed hash sum.
     */
    inline void getHash(std::output_iterator<char> auto output) noexcept
        requires(not std::contiguous_iterator<decltype(output)>);

    /*!
     * Finishes the hash sum computation if it hasn't been already and writes the complete MD5 sum to \p output.
     *
     * This method will write, as raw bytes, the result of completing the MD5 hash sum operation to the given \p output
     * span. This method uses any size of span, so long as the number of bytes in it is larger than #md5SumSize.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5 computation which does change the object state, so this cannot be called on a const object. Md5
     * should never really be used const, as all methods can change the Md5 object's state, and if you need the hash
     * to be const you just pass a const Md5Sum instead.
     *
     * This method takes a span, and will fail to compile when the span is less than md5SumSize bytes in length.
     * If the span uses std::dynamic_extent, then it will provide either md5SumSize bytes if
     * `output.size_bytes()` returns greater than md5SumSize, and will truncate the bytes up to that amount if
     * it is less than md5SumSize. This is to ensure this method never throws exceptions.
     *
     * \tparam Byte_T The byte type of the span to \p output to, which must be non-const. This should be inferred.
     * \tparam extent The compile-time size of the \p output span. This should be inferred.
     * \param[out] output A std::span<Byte_T, extent> of the block which to output the MD5 sum to, which must be at
     * least 16 bytes.
     */
    template <ByteType Byte_T, size_t extent>
    requires(not std::is_const_v<Byte_T> and extent >= md5SumSize)
    inline void getHash(span<Byte_T, extent> output) noexcept(extent != std::dynamic_extent);

    /*!
     * Finishes the hash sum computation if it hasn't been already and writes the complete MD5 sum to fit in \p output.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5 computation which does change the object state, so this cannot be called on a const object. Md5
     * should never really be used const, as all methods can change the Md5 object's state, and if you need the hash
     * to be const you just pass a const Md5Sum instead.
     *
     * This method takes a span and will write either md5SumSize bytes if `output.size_bytes()` returns greater than or
     * equal to md5SumSize, and will truncate the bytes up to `output.size_bytes()` if it is less than md5SumSize. This
     * is to ensure this method never throws exceptions.
     *
     * \param[out] output A std::span<Byte_T, extent> of the block which to output the MD5 sum to, which must be at
     * least 16 bytes.
     */
    template <std::contiguous_iterator OutputIt_T>
    requires(ByteType<std::iter_value_t<OutputIt_T>> and not std::is_const_v<std::iter_value_t<OutputIt_T>>)
    inline void fitHash(OutputIt_T output, size_t length) noexcept;

    /*!
     * Finishes the hash sum computation if it hasn't been already and writes the complete MD5 sum to fit in \p output.
     *
     * If the hash has already been finished this will simply return the computed result. Otherwise it will finish the
     * Md5 computation which does change the object state, so this cannot be called on a const object. Md5
     * should never really be used const, as all methods can change the Md5 object's state, and if you need the hash
     * to be const you just pass a const Md5Sum instead.
     *
     * This method takes a span and will write either md5SumSize bytes if `output.size_bytes()` returns greater than or
     * equal to md5SumSize, and will truncate the bytes up to `output.size_bytes()` if it is less than md5SumSize. This
     * is to ensure this method never throws exceptions.
     *
     * \param[out] output A std::span<Byte_T, extent> of the block which to output the MD5 sum to, which must be at
     * least 16 bytes.
     */
    template <std::contiguous_iterator OutputIt_T>
    requires(ByteType<std::iter_value_t<OutputIt_T>> and not std::is_const_v<std::iter_value_t<OutputIt_T>>)
    inline void fitHash(size_t length, OutputIt_T output) noexcept;

    /*!
     * Finishes the hash sum computation if it hasn't been already and writes the complete MD5 sum to fit in \p output.
     *
     * This method will write, as raw bytes, the result of completing the MD5 hash sum operation to the given \p output
     * span of any given \p Byte_T. This method is guaranteed never to exceed the size of the passed span, so will
     * truncate the result of the hash operation to the size of \p output if it is smaller than md5SumSize.
     *
     * If the hash has already been finished this will simply write the computed result. Otherwise it will finish the
     * Md5 computation which does change the object state, so this cannot be called on a const object. Md5
     * should never be used const, as all methods can change the Md5 object's state, and if you need the hash
     * to be const you just pass a const Md5Sum instead.
     *
     * This method takes a span and will write either md5SumSize bytes if `output.size_bytes()` returns greater than or
     * equal to md5SumSize, and will truncate the bytes up to `output.size_bytes()` if it is less than md5SumSize. This
     * is to ensure this method never throws exceptions.
     *
     * \tparam Byte_T The byte type of the \p output span. This should be inferred.
     * \tparam extent The compile-time size of the \p output span. This should be inferred.
     * \param[out] output A std::span of a byte type designating a block to output the MD5 sum to, which must be at
     * least 16 bytes.
     */
    template <ByteType Byte_T, size_t extent>
    inline void fitHash(span<Byte_T, extent> output) noexcept requires(not std::is_const_v<Byte_T>);

private:
    struct Impl;

    [[noreturn]] void throwBadHashOutputSize(size_t size);
    void finish() noexcept;
};

Md5::Md5(size_t length, std::contiguous_iterator auto input) noexcept : Md5(std::as_bytes(span{ input, length }))
{
    // No further implementation.
}

Md5::Md5(std::contiguous_iterator auto input, size_t length) noexcept : Md5(std::as_bytes(span{ input, length }))
{
    // No further implementation.
}

Md5::Md5(ReadableSpanOf auto input) noexcept
    requires(not std::same_as<byte, std::remove_cv_t<typename decltype(input)::element_type>>)
    : Md5(std::as_bytes(input))
{
    // No further implementation.
}

Md5::Md5(StringLike auto input) noexcept :
    Md5(std::as_bytes(span{ std::basic_string_view{ input }.data(), std::basic_string_view{ input }.size() }))
{
    // No further implementation.
}

size_t Md5::provideInput(size_t length, std::contiguous_iterator auto input) noexcept
{
    return provideInput(std::as_bytes(span{ input, length }));
}

size_t Md5::provideInput(std::contiguous_iterator auto input, size_t length) noexcept
{
    return provideInput(std::as_bytes(span{ input, length }));
}

size_t Md5::provideInput(ReadableSpanOf auto input) noexcept
    requires(not std::same_as<byte, std::remove_cv_t<typename decltype(input)::element_type>>)
{
    return provideInput(std::as_bytes(input));
}

size_t Md5::provideInput(StringLike auto input) noexcept
{
    auto inputView = std::basic_string_view{ input };
    return provideInput(std::as_bytes(span{ inputView.data(), inputView.size() }));
}

Md5Sum Md5::getHash() noexcept
{
    Md5Sum sum;
    getHash(sum);
    return sum;
}

template <std::contiguous_iterator Iterator_T>
requires std::output_iterator<Iterator_T, std::iter_value_t<Iterator_T>> and ByteType<std::iter_value_t<Iterator_T>>
inline void Md5::getHash(Iterator_T output) noexcept
{
    if constexpr (std::is_pointer_v<Iterator_T>)
    {
        if (output == nullptr)
        {
            finish();
            return;
        }
    }

    getHash(Md5SumSpan{ output, md5SumSize });
}

void Md5::getHash(std::output_iterator<byte> auto output) noexcept
    requires(not std::contiguous_iterator<decltype(output)>)
{
    for (byte sumByte : getHash())
    {
        *output = sumByte;
        output++;
    }
}

void Md5::getHash(std::output_iterator<char> auto output) noexcept
    requires(not std::contiguous_iterator<decltype(output)>)
{
    for (byte sumByte : getHash())
    {
        *output = std::bit_cast<char>(sumByte);
        output++;
    }
}

template <ByteType Byte_T, size_t extent>
requires(not std::is_const_v<Byte_T> and extent >= md5SumSize)
void Md5::getHash(span<Byte_T, extent> output) noexcept(extent != std::dynamic_extent)
{
    if constexpr (extent == std::dynamic_extent)
    {
        if (size_t smaller = output.size_bytes(); smaller < md5SumSize)
        {
            throwBadHashOutputSize(smaller);
        }
    }

    getHash(std::as_writable_bytes(output));
}

template <std::contiguous_iterator OutputIt_T>
requires(ByteType<std::iter_value_t<OutputIt_T>> and not std::is_const_v<std::iter_value_t<OutputIt_T>>)
void Md5::fitHash(OutputIt_T output, size_t length) noexcept
{
    fitHash(span{ output, length });
}

template <std::contiguous_iterator OutputIt_T>
requires(ByteType<std::iter_value_t<OutputIt_T>> and not std::is_const_v<std::iter_value_t<OutputIt_T>>)
void Md5::fitHash(size_t length, OutputIt_T output) noexcept
{
    fitHash(span{ output, length });
}

template <ByteType Byte_T, size_t extent>
void Md5::fitHash(span<Byte_T, extent> output) noexcept requires(not std::is_const_v<Byte_T>)
{
    auto fitSum = [&] {
        Md5Sum sum   = getHash();
        auto sumIter = sum.begin();
        for (Byte_T &outByte : output)
        {
            outByte = std::bit_cast<Byte_T>(*sumIter++);
        }
    };

    if constexpr (extent < md5SumSize)
    {
        fitSum();
    }
    else if constexpr (extent == std::dynamic_extent)
    {
        if (size_t smaller = output.size_bytes(); smaller < md5SumSize)
        {
            fitSum();
            return;
        }

        getHash(std::as_writable_bytes(output));
    }
    else
    {
        getHash(std::as_writable_bytes(output));
    }
}

/*!
 * A quick and simple MD5 hash implementation that works on an arbitrary set of constant bytes passed as \p input.
 *
 * While this implementation does its best to fully implement RFC1321, it is constrained in that this algorithm does NOT
 * accept any arbitrary number of bits, but rather the amount of data provided must be in bytes and therefore any MD5
 * hash of a value that takes a number of bits that isn't a multiple of Platform::bitsInByte is impossible to generate
 * with this function.
 *
 * The data passed can be of arbitrary length, passed as the first argument. Since \p length is passed separately, \p
 * input may contain bytes with all bits set to 0 and the hash will still work correctly.
 *
 * This version of the function outputs the MD5 hash (as an array of 16 bytes, not hexadecimal) to the output iterator
 * destination. If you do not have an existing buffer to output to, simply use an alternate getMd5() overload.
 *
 * \warning Passing a pointer as \p output to a buffer of bytes that is smaller than 16 bytes in length, or is pointing
 * to a buffer with a non-byte type object initialized within it results in undefined behavior. Under no circumstances
 * should you pass a pointer to a buffer of less than 16 allocated bytes for the purpose of containing an MD5 hash
 * sum.
 *
 * \see KirHut::Hash::
 * \see KirHut::getHash(Iterator_T)
 * \tparam OutputIt_T The output iterator type used to output the hash to. This should be inferred.
 * \param length A std::size_t of the data size. It should be in number of *elements*, not number of *bytes*!
 * \param input A pointer to any data to return a hash sum of.
 * \param[out] output A pointer to a buffer location to write the hash to. Must be at least #md5SumSize bytes in length.
 */
template <std::contiguous_iterator OutputIt_T>
requires(std::output_iterator<OutputIt_T, std::iter_value_t<OutputIt_T>> and ByteType<std::iter_value_t<OutputIt_T>>)
inline void getMd5(size_t length, std::contiguous_iterator auto input, OutputIt_T output) noexcept
{
    Md5(input, length).getHash(output);
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
 * \return A std::array of #md5SumSize char values. On 8-bit byte platforms, a total size of 16 bytes.
 */
[[nodiscard]] inline Md5Sum getMd5(size_t length, std::contiguous_iterator auto input) noexcept
{
    return Md5(input, length).getHash();
}

/*!
 * A quick and simple MD5 hash implementation that uses an arbitrary set of constant bytes passed as \p input.
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
 * \param input A span of any type T to return a hash sum of.
 * \param[out] output A pointer to a buffer location to write the hash to. Must be at least #md5SumSize bytes in length.
 */
template <typename Input_T, size_t extent = std::dynamic_extent>
inline void getMd5(span<Input_T const, extent> input, byte *output) noexcept
{
    Md5(input).getHash(output);
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
 * \return A std::array of #md5SumSize char values. On 8-bit byte platforms, a total size of 16 bytes.
 */
template <typename Input_T, size_t extent = std::dynamic_extent>
[[nodiscard]] inline Md5Sum getMd5(span<Input_T const, extent> data) noexcept
{
    return Md5(data).getHash();
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
 * \see KirHut::Md5::getMd5(span<T,tExt>)
 * \tparam Input_T Any input data type used as input through \p data.
 * \tparam Byte_T The byte type that will be written to for \p output.
 * \tparam inputExtent The compile-time size of \p data, usually std::dynamic_extent.
 * \tparam outputExtent The compile-time size of the \p output buffer, usually std::dynamic_extent, but this function is
 * guaranteed not to throw exceptions if it is not.
 * \param input A std::span<T,E> to return a hash sum of.
 * \param[out] output A std::span to a buffer location to write the hash to. Should be at least #md5SumSize
 * bytes in length.
 * \throws IllegalArgument If the size of the \p output buffer is less than #md5SumSize bytes.
 */
template <typename Input_T,
          ByteType Byte_T,
          size_t inputExtent  = std::dynamic_extent,
          size_t outputExtent = std::dynamic_extent>
inline void getMd5(SpanOf auto input, span<Byte_T, outputExtent> output) noexcept(outputExtent != std::dynamic_extent)
    requires(std::is_same_v<Byte_T, std::remove_cv_t<Byte_T>> and outputExtent >= md5SumSize)
{
    Md5(input).getHash(output);
}

constexpr string sumToHex(Md5Sum sum)
{
    constexpr auto hexDigitIndex = "0123456789abcdef"sv;
    string ret;
    ret.reserve(sum.size() * 2);
    for (byte sumByte : sum)
    {
        auto byteValue = static_cast<int>(sumByte);
        ret.push_back(hexDigitIndex[byteValue >> 4]);
        ret.push_back(hexDigitIndex[byteValue & 0xf]);
    }

    return ret;
}

#endif // defined(KH_INCLUDE_MD5HASH) or defined(KH_PRIV_DOCS)

} // namespace Hash

KH_END_INLINE_NAMESPACE

} // namespace KirHut
