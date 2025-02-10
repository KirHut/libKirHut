/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/iotools.hpp
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

/*!
 * \file
 *
 * Header including the KirHut::IO utility classes such as FixedBufferOutput and StringOutput.
 */

#include "kh/output.hpp"

#include <vector>

namespace KirHut::IO
{

/*!
 * \brief END_POS
 */
constexpr static SeekPos END_POS = Limits<SeekPos>::max();

/*!
 * Perform all output operations upon a fixed size buffer of bytes, which you can then peruse as necessary or use.
 *
 * \headerfile kh/iotools.hpp
 *
 * This is like a String buffer in that it outputs stream operations to a buffer in memory, however instead of it being
 * a variable size std::string object containing the data, it is a vector of std::byte that is fixed in size and does
 * not grow despite output operations. The buffer may be resized, and can be removed from the object (and a new buffer
 * input) by moving out the std::vector of std::byte.
 *
 * By default, if you move the internal cursor back to previous positions, it will overwrite any bytes that are
 * successive from that position. To change this, use the \p inserting parameter in the constructor or change the
 * "inserting" semantics of this class using the setInserting(bool) method.
 *
 * Use this when you want an exact amount of output to be retained and to never exceed that regardless of further
 * output commands.
 *
 * \warning This object is not thread safe **at all**. Do not perform stream output operations, resize operations, read
 * operations, or any other operation on this object from multiple threads. This object is primarily useful for building
 * a buffer with the IO API, so simply avoid using this class with multiple threads.
 */
class KH_EXPORT FixedBufferOutput final : public SeekableOutput
{
    std::vector<byte> data;
    SeekPos cursorPosition;
    bool insertState;

public:
    /*!
     * Construct a new seekable Output with a fixed size buffer as the destination.
     *
     * The size entered should be of some value greater than zero, but zero is a legal value to enter. If this object
     * has a zero sized buffer, this means that all output operations will always fail and it will always report true
     * on calls to isFull(). This is also the case if the buffer is resized to zero.
     *
     * The FixedBufferOutput object may have output operations using "inserting" semantics, or using "overwriting"
     * semantics. By default, this is overwriting, so if you move the output cursor to a previous position in the
     * buffer, newly output bytes will be overwrite the previous data in the buffer. If, instead, you want the bytes
     * to be shifted from the output cursor position, and the bytes on the end of the buffer are lost instead, you may
     * pass true as \p inserting, which will set this buffer to inserting semantics.
     *
     * \param initialBufferSize The initial size of the buffer for this object.
     * \param inserting True if this buffer "inserts" bytes between existing bytes in the buffer, or false otherwise.
     * \throws std::bad_alloc If there is a memory allocation failure.
     */
    explicit FixedBufferOutput(size_t initialBufferSize, bool inserting = false);

    /*!
     * Resize the internal buffer to \p newBufferSize.
     *
     * This method will both grow or shrink a buffer. The cursor position will remain unchanged, unless \p newBufferSize
     * is less than the current cursor position, in which it will move to the end of the new buffer and isFull() will
     * return true. The data within the new buffer will be copied over from the old buffer.
     *
     * New bytes will be left uninitialized, so could be anything. Do not assume they will be initialized to zero,
     * however the only way to access them would be through buffer() or bufPtr(), so just don't use those until your
     * isFull() method returns true.
     *
     * \param newBufferSize The size of the new buffer after the resize.
     * \throws std::bad_alloc If allocation of the new buffer fails.
     * \return The size of the buffer after the change, so it may return something other than \p newBufferSize.
     */
    size_t resize(size_t newBufferSize);

    /*!
     * Returns the current inserting state of this FixedBufferOutput.
     *
     * If this returns true, when output operations are performed on this stream, it will "insert" the characters
     * in-between the characters currently existing in the buffer, moving those characters further down the buffer and
     * placing the content in that area. The cursor position will be between the newly inserted characters and the
     * beginning of the characters that were in the stream in front of the cursor position previously. Any characters at
     * the end of the buffer that were shifted off are lost.
     *
     * If this returns false, when output operations are performed on this stream, it will overwrite the characters
     * currently in the fixed buffer. Any characters past the input operation are retained in their same positions, but
     * previous characters are overwritten. The cursor position will be advanced to the end of the newly inserted bytes
     * and any characters that were between where the cursor position began and it now is are lost.
     *
     * \return The inserting state of this buffer.
     */
    bool isInserting() const noexcept;

    /*!
     * Change the inserting operation type, and return if the inserting state changed.
     *
     * There is no condition where changing the inserting state will fail for a FixedBufferOutput, however if the state
     * passed is the same as the current one, this method will return false. It will return true if this call resulted
     * in changing the inserting state. It **does not return the current inserting state!**
     *
     * The FixedBufferOutput object may have output operations using "inserting" semantics, or using "overwriting"
     * semantics. By default, the state is overwriting, so if you move the output cursor to a previous position in the
     * buffer, newly output bytes will be overwrite the previous data in the buffer. If, instead, you want the bytes
     * to be shifted from the output cursor position, and the bytes on the end of the buffer are lost instead, you may
     * pass true as \p insert, which will set this buffer to inserting semantics.
     *
     * \param insert The new desired inserting state.
     * \return Whether or not the inserting state of this object changed as a result of this call.
     */
    bool setInserting(bool insert) noexcept;

    /*!
     * Get a span view of the internal fixed buffer.
     *
     * There are no thread-safety guarantees with this or the bufPtr() methods. If the resize() method of this object is
     * called while operating on this span, it will be invalidated. You should always refresh all spans or pointers to
     * the internal buffer after a resize() operation.
     *
     * There is both a const and non-const overload of this method.
     *
     * \return A view to the internal buffer.
     */
    [[nodiscard]] std::span<const byte> buffer() const noexcept;

    /*!
     * \copydoc buffer()const noexcept
     */
    [[nodiscard]] std::span<byte> buffer() noexcept;

    /*!
     * Get a pointer to the internal fixed buffer.
     *
     * There are no thread-safety guarantees with this or the buffer() methods. If the resize() method of this object is
     * called while using this pointer, it will be invalidated. You should always refresh all spans or pointers to the
     * internal buffer after a resize() operation.
     *
     * There is both a const and non-const overload of this method.
     *
     * \return A pointer to the internal buffer.
     */
    [[nodiscard]] byte const *bufPtr() const noexcept;

    /*!
     * \copydoc bufPtr()const noexcept
     */
    [[nodiscard]] byte *bufPtr() noexcept;

    /*!
     * Create a separate copy of this object's internal buffer and return it as a std::vector of bytes.
     *
     * This returns a copy of the internal buffer's current state, so that taking the data will not modify the object
     * itself. This prevents invalidation of the data if an application still needs to perform additional modification
     * after creating a copy of it.
     *
     * \throws std::bad_alloc If initializing the new std::vector<byte> cannot allocate sufficient memory.
     * \return A copy of the internal buffer as a std::vector of bytes.
     */
    [[nodiscard]] std::vector<byte> copyBuffer() const;

    /*!
     * Get the underlying byte buffer as a std::vector and initialize a new internal buffer of \p newSize bytes.
     *
     * This method does *not* copy the internal buffer and return it, instead it uses std::move() to return the internal
     * buffer, and creates a new std::vector internally of the size provided. If you intend on simply destroying this
     * FixedBufferOutput after this call, use takeBuffer(), which will not call any allocation on the internal buffer.
     *
     * When \p newSize is equal to SAME_SIZE, the new buffer will be equal to the size of the old buffer. If \p newSize
     * is zero, than this method will not perform an allocation and it is effectively identical to takeBuffer().
     *
     * \param newSize The intended size (in bytes) of the new fixed internal byte buffer.
     * \throws std::bad_alloc If \p newSize is greater than 0 and allocating the new buffer fails.
     * \return A std::vector<byte> of the std::move()d data from the internal buffer.
     */
    [[nodiscard]] std::vector<byte> takeBuffer(size_t newSize);

    /*!
     * Get the underlying byte buffer as a std::vector and leaves this object in an invalid state.
     *
     * This method does *not* copy the internal buffer and return it, instead it uses std::move() to return the internal
     * buffer, and creates a new std::vector internally of the size provided. If you intend on reusing this
     * FixedBufferOutput object, then use takeBuffer(size_t) to give it some buffer amount.
     *
     * \return A std::vector<byte> of the std::move()d data from the internal buffer.
     */
    [[nodiscard]] std::vector<byte> takeBuffer() noexcept;

    /*!
     * Output a given string_view to the internal buffer.
     *
     * This uses the current output method (either inserting or overwriting) to place data into the internal buffer by
     * copying the data to the buffer. This means that the data will be represented internally as std::byte instead of
     * as a char, but this shouldn't negatively affect usability in any real way.
     *
     * \param str
     * \return
     */
    size_t printsv(string_view str) noexcept override;

    /*!
     * \brief failedOut
     * \return
     */
    [[nodiscard]] bool failedOut() const noexcept override;

    /*!
     * \brief isFull
     * \return
     */
    [[nodiscard]] bool isFull() const noexcept override;

    /*!
     * \brief tellOut
     * \return
     */
    [[nodiscard]] SeekPos tellOut() const noexcept override;

    /*!
     * \brief seekOut
     * \param offset
     * \param from
     */
    void seekOut(i64 offset, Relation from = CURRENT) override;

    /*!
     * Constant value representing using the same size buffer for the new internal buffer on takeBuffer(size_t) calls.
     *
     * This just makes it explicit what Limits<size_t>::max() does as an argument in takeBuffer(size_t). Pass this to
     * make sure future developers don't have to wonder why the largest size_t value is being passed as a buffer size.
     */
    [[maybe_unused]] constexpr static size_t SAME_SIZE = Limits<size_t>::max();
};

/*!
 * A SeekableOutput class that outputs to a std::string object.
 *
 * \headerfile kh/iotools.hpp
 *
 * This class uses the default copy constructor, the default copy assignment operator, the default move constructor, the
 * default move assignment operator, and the default destructor. As such, making copies of the StringOutput object works
 * as you would expect: It creates a copy of the object that may throw exceptions. The two objects will be completely
 * independent and you may call modifications and takeData()/peekData() with the expected standard functionality, as
 * they are completely separate objects. The move constructor and assignment operator will call the move assignment
 * operator of std::string for the underlying data of the passed in argument, causing them to become invalidated as
 * though with a call to takeData()&&noexcept.
 *
 * \warning This object is not thread safe **at all**. Do not perform stream output operations, resize operations, read
 * operations, or any other operation on this object from multiple threads. This object is primarily useful for building
 * a buffer with the IO API, so simply avoid using this class with multiple threads.
 */
class KH_EXPORT StringOutput final : public SeekableOutput
{
    string data;
    SeekPos cursorPosition = 0;

public:
    /*!
     * Construct a new seekable Output with a string object buffer as the destination.
     *
     * This object mostly takes the place of the std::stringstream in allowing you to create a string from a stream and
     * then use it in code. This object, unlike the std::stringstream, does not take a reference to a user-created
     * string object, instead it uses an internal string object. To take the internal string object without copying it,
     * use the takeData() method, or use the copyData() method to get a copy of it.
     */
    StringOutput() noexcept = default;

    /*!
     * Constructs a StringOutput class starting with a string filled with \p start and a \p pos cursor position.
     *
     * \param start The content that the underlying string data should start with.
     * \param pos The starting cursor position of this SeekableOutput stream.
     * \throws std::bad_alloc If there is a memory allocation failure.
     */
    explicit StringOutput(char const *start, SeekPos pos = END_POS);

    /*!
     * Constructs a StringOutput class starting with a string filled with \p start and a \p pos cursor position.
     *
     * \param start The content that the underlying string data should start with.
     * \param pos The starting cursor position of this SeekableOutput stream.
     * \throws std::bad_alloc If there is a memory allocation failure.
     */
    explicit StringOutput(string_view start, SeekPos pos = END_POS);

    /*!
     * Constructs a StringOutput class starting with a moved string \p start and a \p pos cursor position.
     *
     * This allows creating a StringOutput object without using an empty string as the starting position. This can be
     * useful for two reasons: You could have string data that was created dynamically and you do not want more memory
     * allocations than necessary, and you could also set the capacity of the input string to ensure that only a single
     * allocation ever occurs while dynamically creating your string with StringOutput. This is far more efficient than
     * using multiple allocations and a small cost to ensure there are no dangling references when using this class
     * (unlike the std::stringstream).
     *
     * \param start The content that the underlying string data should start with.
     * \param pos The starting cursor position of this SeekableOutput stream.
     */
    explicit StringOutput(string &&start, SeekPos pos = END_POS) noexcept;

    /*!
     * \brief StringOutput
     * \param start
     * \param bufSize
     * \param pos
     * \throws std::bad_alloc If there is a memory allocation failure.
     */
    StringOutput(char const *start, size_t bufSize, SeekPos pos);

    /*!
     * \brief StringOutput
     * \param other
     */
    StringOutput(StringOutput const &other) = default;

    /*!
     * \brief StringOutput
     * \param other
     */
    StringOutput(StringOutput &&other) noexcept;

    /*!
     * Default StringOutput destructor.
     *
     * This is included to respect the Rule of 5.
     */
    ~StringOutput() noexcept = default;

    /*!
     * \brief operator =
     * \param other
     * \return
     */
    StringOutput &operator=(StringOutput const &other) = default;

    /*!
     * \brief operator =
     * \param other
     * \return
     */
    StringOutput &operator=(StringOutput &&other);

    /*!
     * Get the current state of the underlying string data.
     *
     * The string_view may become invalidated after any call to a non-const method of this object (with the exception of
     * seekOut()), so do not store the returned string_view for any period longer than this StringOutput object could
     * potentially be modified.
     *
     * \return A string_view pointing to the underlying string data.
     */
    [[nodiscard]] string_view peekData() const noexcept;

    /*!
     * Returns a string move-constructed from the internal string and reinitializes an empty internal string.
     *
     * This method does *not* copy the internal string and return it, instead it uses std::move() to return the internal
     * string, and creates a new std::string internally that is left empty. If you intend on simply destroying this
     * StringOutput after this call, use std::move(*this).takeData(), which will not create a new internal std::string
     * and leave this object in an unspecified state.
     *
     * \return A string with the data of the internal string.
     */
    [[nodiscard]] string takeData() & noexcept;

    /*!
     * Returns a string move-constructed from the internal string.
     *
     * This method does *not* copy the internal string and return it, instead it uses std::move() to return the internal
     * string, and does not initialize the internal string. If you intend on using this StringOutput after this call,
     * use takeData()&, which will create a new internal std::string and leave this object in a reusable state.
     *
     * \return A string with the data of the internal string.
     */
    [[nodiscard]] string takeData() && noexcept;

    /*!
     * Returns a copy of the internal string.
     *
     * This method copies the internal string and returns it, instead of using std::move() to return the internal
     * string. This ensures that the internal data will remain consistent after this call, and will not modify this
     * object. As such, you can use this on a const StringOutput, though why you would be using one of those is itself a
     * mystery.
     *
     * \throws std::bad_alloc If there is an error allocating sufficient memory for the returned string.
     * \return A copy of the internal string.
     */
    [[nodiscard]] string copyData() const;

    /*!
     * Reserve at least the number of bytes in the underlying string's \p capacity.
     *
     * This is usually used to reduce the number of allocations when you are creating a large string using StringOutput.
     * If you call this after creating the StringOutput object you can avoid creating a string with a large capacity
     * then moving it into a newly constructed StringOutput, instead simply using the default constructor.
     *
     * This simply calls std::basic_string::reserve() on the underlying string data. This library requires C++20, and
     * thus this method will do nothing if the passed-in \p capacity is lower than the current data string's capacity.
     *
     * \param capacity An amount of bytes that must be available in the underlying string data after this call.
     * \throws std::bad_alloc If there is a memory allocation failure while resizing the underlying capacity.
     */
    void reserve(size_t capacity);

    /*!
     * Set the internal string's data \p capacity.
     *
     * This is usually used to reduce the number of allocations when you are creating a large string using StringOutput.
     * If you call this after creating the StringOutput object you can avoid creating a string with a large capacity
     * then moving it into a newly constructed StringOutput, instead simply using the default constructor.
     *
     * If the capacity is smaller than the current size of the string, this method use the larger of the passed in
     * \p capacity or the size of the current data string. The StringOutput class currently has no method to truncate
     * the data and this modification must be done on a returned string itself.
     *
     * \note After C++20, the std::basic_string::reserve() method does not shrink the capacity at all of the underlying
     * data, but this method still does. Unfortunately, the price of this is that, if the passed in \p capacity is below
     * the current capacity but also above the current data string size, this method will call **two allocations and
     * copies**, first in a call to std::basic_string::shrink_to_fit(), and second in a call to
     * std::basic_string::reserve() afterward.
     *
     * \param capacity The new capacity of the underlying string data.
     * \throws std::bad_alloc If there is a memory allocation failure while resizing the underlying capacity.
     */
    void setCapacity(size_t capacity);

    /*!
     * \brief dataCapacity
     *
     *
     * \return
     */
    [[nodiscard]] size_t dataCapacity() const noexcept;

    /*!
     * Outputs the data passed as a string_view to the underlying data string at the current cursor position.
     *
     * This method returns the number of bytes actually placed into the underlying data string. This is always going to
     * be str.size(), unless this method throws an IllegalArgument Exception. The only reason that IllegalArgument
     * Exception would be thrown is if inserting the passed-in \p str would result in the internal string being larger
     * than takeData().max_size().
     *
     * \param str
     * \throws IllegalArgument If the passed in \p str is too large to output to the underlying string.
     * \return
     */
    size_t printsv(string_view str) override;

    /*!
     * \brief failedOut
     * \return
     */
    [[nodiscard]] bool failedOut() const noexcept override;

    /*!
     * Returns false unless the string is full and the capacity is already at the max string size.
     *
     * This method generally always returns false, however it is possible for it to return true in a very, *very*
     * limited circumstance: if peekData().size() is equal to takeData().capacity(), and takeData().capacity() is equal
     * to takeData().max_size(). This method is likely not very useful on a StringOutput object.
     *
     * \return False in the great grand majority of cases, true if it is impossible to expand the string further.
     */
    [[nodiscard]] bool isFull() const noexcept override;

    /*!
     * Return the current position of the output cursor.
     *
     * When this method returns peekData().size(), this means that the output cursor position is at the very end of the
     * string, so all output operations will append to the string. Otherwise, output operations will insert the data
     * into the string at the position returned by this method.
     *
     * \return The current output cursor position.
     */
    [[nodiscard]] SeekPos tellOut() const noexcept override;

    /*!
     * Move the cursor position by the given \p offset \p from the given Relation point.
     *
     * A StringOutput object has a cursor position that starts at position 0 for the first byte in the underlying string
     * object, and ends at the underlying string object's current size. Since output operations can increase the size of
     * the underlying string, use peekData().size() to determine the size of the current string.
     *
     * By default, the \p offset is based on the output cursor's current position. If the offset \p from the given
     * Relation position results in a cursor position that is beyond the current bounds of the string (IE less than 0 or
     * greater than peekData().size()), this method will move the cursor to that bound and no further. Use tellOut() to
     * determine the current cursor position.
     *
     * This method does not throw any exceptions, unlike the method it is overriding.
     *
     * \param offset
     * \param from
     */
    void seekOut(i64 offset, Relation from = CURRENT) noexcept override;
};

} // namespace KirHut::IO
