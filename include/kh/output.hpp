/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/output.hpp
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

#include "kh/baseio.hpp"

#include <format>

namespace KirHut::IO
{

/*!
 * An object that represents an abstract destination for character or byte output.
 *
 * \headerfile kh/output.hpp
 *
 * The Output interface is intended to represent a place to insert character or byte data so that it may be sent to
 * another location, stored for later use, or otherwise used to update some kind of back end without providing much
 * other than confirmation to the sender that it is complete.
 *
 * This Output interface is hopelessly dependent on C++20 std::format, so unless you are using a compiler that supports
 * that, this will not compile on your platform. Apple Clang advertises support for std::format as of XCode 15, so this
 * should work for all supported platforms. For the most part, users of this class should simply output using the
 * print(std::format_string<Args...>,Args&&...) or println(std::format_string<Args...>,Args&&...) method templates, as
 * these provide the powerful C++20 std::format functionality with the same compile-time type checking guarantees. The
 * class has several virtual methods that underlying implementations will provide implementations for, and those
 * methods are provided the data from the print() and println() methods after it is formatted.
 *
 * Only the printsv(string_view) and failedOut() methods need to be implemented, but it is highly encouraged that you
 * also implement the println() method. The isFull() method always returns false unless the underlying implementation
 * has a limited number of characters that can be input, then it would return true. Use the flush() method to flush the
 * underlying stream, however by default it does nothing assuming an underlying stream is always flushed.
 */
struct KH_EXPORT Output
{
    /*!
     * Output a std::string_view to the underlying output destination.
     *
     * This method is required to be implemented by a subclass. While this method is public, for the most part, it isn't
     * used by code that uses this interface. Instead, the primary method to use is the
     * print(std::format_string<Args...>,Args&&...) method template, which uses std::format to create a string and pass
     * the string view to this method. This is the only actual output method that is required to be implemented by the
     * subclass, as all other methods can be implemented in terms of this method.
     *
     * This method is deliberately called "printsv" and not simply "print" because of the ambiguity of passing a `const
     * char *`, as it would not know if it should convert to a std::format_string or to a std::string_view. The
     * underlying implementation may throw exceptions, but is discouraged from doing so in any recoverable instance.
     *
     * This method should simply output the passed bytes to the underlying implementation.
     *
     * \param str A std::string_view of the char data to output.
     * \return The actual number of bytes output to the underlying stream, which may be less than str.size().
     */
    virtual size_t printsv(string_view str) = 0;

    /*!
     * Check if the output stream is in an unrecoverable failure state.
     *
     * Unlike the failed state in std::ios_base, this state should be considered unrecoverable for the output stream.
     * This means that there are situations where both this method and goodOut() will return false, as the stream can
     * both be in a state that is not ready for output but also not failed. The subclass type should have methods that
     * can be used to put the underlying stream in a recovered state, and the means how and detection of those states
     * is outside of the scope of the Output class.
     *
     * \return Whether or not the underlying stream is in an unrecoverable failure state.
     */
    [[nodiscard]] virtual bool failedOut() const noexcept = 0;

    /*!
     * Check if this output stream supports output of raw bytes.
     *
     * Some streams will output to a formatted destination of some kind that has requirements above raw bytes, and
     * therefore cannot support raw byte output. However, in most cases (such as file output and fixed buffers), raw
     * output of bytes (including 0 bytes) are supported, and work as expected. This method by default returns false,
     * but may be overridden depending on the subclass.
     *
     * \return Whether or not this stream supports raw byte output.
     */
    [[nodiscard]] virtual bool hasByteOut() const noexcept;

    /*!
     * Use C++20 std::format to format a std::format_string and output the results to the underlying stream.
     *
     * C++20 introduced a much better system for output, and recently all supported platforms for KirHut software began
     * supporting std::format. As such, this library has moved on to using this superior output system, and provides
     * this output class as a means to completely replace std::ostream based output. An implementation does not need to
     * implement anything regarding formatting, as this is always handled by std::format, and instead all that needs to
     * be done is implementing output of a set of bytes as a std::string_view.
     *
     * This method template simply calls printsv(string_view) with the std::string returned from std::format(). This
     * does mean that this method may throw std::bad_alloc separately from the underlying implementation as
     * std::format() will create a new std::string object.
     *
     * \param fmt The format string to format the passed arguments into. Use printsv() if there are no arguments.
     * \param args The arguments that are input into the format string by std::format().
     * \throws std::bad_alloc If there is a memory allocation failure.
     * \throws any Exception thrown by the subclass printsv(string_view) method.
     * \return The actual number of bytes output to the underlying stream, which may be less than fmt.size().
     */
    template <typename... Args>
    inline size_t print(std::format_string<Args...> fmt, Args &&...args)
    {
        return printsv(std::format(fmt, forward<Args>(args)...));
    }

    /*!
     * Use C++20 std::vformat() to format a std::string_view and output the results to the underlying stream.
     *
     * This method is the runtime equivalent of print(std::format_string<Args...>,Args&&...), and is useful when you
     * need to format a dynamically created string rather than a constant string. This loses the compile-time type
     * checking, in exchange for throwing std::format_error, so the passed in string_view had better work with the
     * std::format_args provided! If you are using this method to simply get around a compile error with the print()
     * method, you are probably doing it wrong.
     *
     * This method by default simply calls printsv(string_view) with the std::string returned from std::vformat(). This
     * does mean that, unless overridden by a subclass, this method may also throw std::bad_alloc in addition to that
     * which may be thrown by printsv(string_view).
     *
     * \param fmt The format string to format the passed std::format_args into. Use printsv() if there are no arguments.
     * \param args The arguments that are input into the format string by std::vformat().
     * \throws std::bad_alloc If there is a memory allocation failure.
     * \throws std::format_error If there is a formatting issue thrown by std::vformat(string_view,std::format_args).
     * \throws any Exception thrown by the subclass printsv(string_view) method.
     * \return The actual number of bytes output to the underlying stream, which may be less than fmt.size().
     */
    virtual size_t vprint(string_view fmt, std::format_args args);

    /*!
     * Output to the underlying stream a raw block of bytes, of the \p amount of bytes provided, if this is supported.
     *
     * Output subclasses are encouraged to support raw byte output by returning true from hasByteOut() and implementing
     * this method. Most output streams should be able to provide this, but there are some conditions where this would
     * not be supported (such as outputting to the standard output stream). By default, this method simply returns 0 and
     * ignores the input, as by default output streams do not support raw output.
     *
     * \param toOutput A byte pointer to at least \p amount bytes of data to output.
     * \param amount The number of bytes to output to the underlying stream from \p toOutput.
     * \return The actual number of bytes output to the underlying stream, which may be less than \p amount.
     */
    virtual size_t rawOutput(byte const *toOutput, size_t amount);

    /*!
     * Output to the underlying stream a raw block of bytes, if this is supported.
     *
     * Output subclasses are encouraged to support raw byte output by returning true from hasByteOut() and implementing
     * the rawOutput(byte const*,size_t) method. This method simply delegates to that one, and will accept a span of
     * any type at all that can be cast to a block of byte, which should be any type. This method does **not** perform
     * any individual changing of the types passed in this span, instead the raw bytes of the data from the span are
     * directly output to the underlying stream, which may or may not be useful for your purpose. It is recommended that
     * you stick with direct byte output through this method, or possibly to pass a vector of char.
     *
     * \param toOutput A span of the raw data to output to the stream, which is directly cast into an array of byte.
     * \return The actual number of bytes output to the underlying stream, which may be less than toOutput.size_bytes().
     */
    template <typename T, size_t E = std::dynamic_extent>
    size_t rawOutput(span<T, E> toOutput)
    {
        return rawOutput(bit_cast<byte const *>(toOutput.data()), toOutput.size_bytes());
    }

    /*!
     * Use C++20 std::format to format a std::format_string and output the results to the underlying stream.
     *
     * C++20 introduced a much better system for output, and recently all supported platforms for KirHut software began
     * supporting std::format. As such, this library has moved on to using this superior output system, and provides
     * this output class as a means to completely replace std::ostream based output. An implementation does not need to
     * implement anything regarding formatting, as this is always handled by std::format, and instead all that needs to
     * be done is implementing output of a set of bytes as a std::string_view. This method also outputs a newline
     * character or characters at the end of the formatted bytes.
     *
     * This method template simply calls printsvln(string_view) with the std::string returned from std::format(). This
     * does mean that this method may throw std::bad_alloc separately from the underlying implementation as
     * std::format() will create a new std::string object.
     *
     * \param fmt The format string to format the passed arguments into. Use printsv() if there are no arguments.
     * \param args The arguments that are input into the format string by std::format().
     * \throws std::bad_alloc If there is a memory allocation failure.
     * \throws any Exception thrown by the subclass printsv(string_view) method.
     * \return The actual number of bytes output to the underlying stream, which may be less than fmt.size().
     */
    template <typename... Args>
    inline size_t println(std::format_string<Args...> fmt, Args &&...args)
    {
        return printsvln(std::format(fmt, forward<Args>(args)...));
    }

    /*!
     * Simply output a newline character or characters to the underlying output stream, and return that number of bytes.
     *
     * This method simply adds a new line to the output location. Subclasses may override this to provide a more
     * efficient implementation, however by default this simply calls printsv("\n"). This means that, unlike the C++
     * standard stream operation std::endl, this method does *not* flush the output by default. A subclass
     * implementation could choose to do this, however, but that must be documented in its according override
     * documentation.
     *
     * \throws any Exception thrown by the subclass printsv(string_view) method.
     * \return The actual number of bytes output to the underlying stream, which may be less than 1.
     */
    virtual size_t println();

    /*!
     * Output a std::string_view to the underlying output destination, followed by a newline.
     *
     * This method is identical to printsv(string_view), except that it also prints a new line. By default, this simply
     * calls printsv(string_view) followed by println().
     *
     * \param str A std::string_view of the char data to output.
     * \throws any Exception thrown by the subclass printsv(string_view) method or println() method.
     * \return The actual number of bytes output to the underlying stream, which may be less than str.size() + 1.
     */
    virtual size_t printsvln(string_view str);

    /*!
     * Use C++20 std::vformat() to format a std::string_view and output the results to the underlying stream.
     *
     * This method is the runtime equivalent of println(std::format_string<Args...>,Args&&...), and is useful when you
     * need to format a dynamically created string rather than a constant string. This loses the compile-time type
     * checking, in exchange for throwing std::format_error, so the passed in string_view had better work with the
     * std::format_args provided! If you are using this method to simply get around a compile error with the print()
     * method, you are probably doing it wrong.This method also outputs a newline character or characters at the end of
     * the formatted bytes.
     *
     * This method by default simply calls printsvln(string_view) with the std::string returned from std::vformat().
     * This does mean that, unless overridden by a subclass, this method may also throw std::bad_alloc in addition to
     * that which may be thrown by printsv(string_view).
     *
     * \param fmt The format string to format the passed std::format_args into. Use printsvln() if there are no
     * arguments.
     * \param args The arguments that are input into the format string by std::vformat().
     * \throws std::bad_alloc If there is a memory allocation failure.
     * \throws std::format_error If there is a formatting issue thrown by std::vformat(string_view,std::format_args).
     * \throws any Exception thrown by the subclass printsv(string_view) method.
     * \return The actual number of bytes output to the underlying stream, which may be less than fmt.size().
     */
    virtual size_t vprintln(string_view fmt, std::format_args args);

    /*!
     * Flush the output stream to the associated hardware or buffer. Simply put, "commit the changes."
     *
     * For buffered streams, this flushes the underlying data buffer to the actual output destination, performing any
     * necessary OS writes at that time. For unbuffered output streams, this usually does nothing.
     *
     * By default, this method does nothing.
     *
     * \throws any Exception that a subclass override throws, or nothing if it is not overridden.
     */
    virtual void flush();

    /*!
     * Report if the underlying output destination is "full". This may have different meanings depending on the output.
     *
     * This generally is used to report a state where the output stream cannot accept additional bytes but is not in an
     * actual failure state. For many streams, like stdout or file output, this will never report true, but for some
     * streams like FixedBufferOutput this occurs when the internal fixed buffer has been completely written to.
     *
     * By default, this method always returns false.
     *
     * \return Whether or not this output stream is "full".
     */
    [[nodiscard]] virtual bool isFull() const noexcept;

    /*!
     * Check if the output stream is in a "good" state, meaning it can perform insertion operations.
     *
     * The KirHut::Output class does not have the concept of a "fail" state like the standard output streams do. This is
     * because the KirHut::Output class does not do any output formatting at all, opting instead to write only raw bytes
     * and providing other tools to marshall the output bytes into usable data. So the "fail" state in this object is
     * more comparable to the "bad" state in a std::basic_ios, and recovery shouldn't be attempted in most situations.
     *
     * The "good" state of an output stream is true unless either the stream is in an unrecoverable state that cannot be
     * read from or the end of file/data has been reached. By default, goodOut() returns !(failedOut() || isFull()), but
     * unlike input streams, output streams can have more reasons than being full to not have a good out state, so this
     * method may return false for any reason the underlying subclass documents.
     *
     * \return Whether or not the output stream is in a "good" state.
     */
    [[nodiscard]] virtual bool goodOut() const noexcept;
};

/*!
 * An interface on top of the Output interface that allows the concept of a "cursor" position that may be moved.
 *
 * \headerfile kh/output.hpp
 *
 * This enshrines the notion of a cursor position within a stream, that may be moved
 */
struct KH_EXPORT SeekableOutput : public Output
{
    using enum Relation;

    /*!
     * \brief tellOut
     * \return
     */
    virtual SeekPos tellOut() const noexcept = 0;

    /*!
     * \brief seekOut
     * \param off
     * \param from
     */
    virtual void seekOut(i64 off, Relation from = CURRENT) = 0;
};

} // namespace KirHut::IO
