/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/input.hpp
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
 * \file kh/input.hpp
 *
 * Header including the Input and SeekableInput interfaces.
 *
 * This header provides two useful class interfaces: the Input interface and the SeekableInput interface. The
 * SeekableInput interface is a child of the Input interface, so will have all of the same requirements and capabilities
 * of an Input implementation. Include this header in order to create a subclass for the Input or SeekableInput
 * interfaces, as generally speaking, if you are trying to use one of the subclass implementations, you would just
 * include the header for that implementation instead.
 */

#include "kh/base.hpp"
#include "kh/baseio.hpp"

#include <vector>

namespace KirHut::IO
{

/*!
 * An object that represents an abstract source of character or byte input.
 *
 * \headerfile kh/input.hpp
 *
 * The Input interface is intended to represent a source of byte or character input, based on the choice of the reader.
 * As all byte data can be represented both as std::byte and as char, this class provides access to both of them, as
 * well as several convenience methods for getting new string objects from the input.
 *
 * If libKirHut is built with span support this object allows providing a span to fill, otherwise it needs a pointer
 * to a buffer as well as the size of that buffer. Implementations of the Input class must provide all of the pure
 * virtual methods in this class. See the documentation of each pure virtual method to know what each one is supposed
 * to return. There are three "status" methods that just return information about the state of the underlying stream
 * (ready(), moreContent(), and failed()), and three "extraction" methods that get content from the underlying stream
 * and return it to the user.
 *
 * This class does **not** provide formatted input! All reading functionality of input is performed by separate objects
 * or using the standard library, it is not done by this class. As such, there is no way that input can fail because of
 * a formatted read error, like is so frequent with C++ standard library input streams.
 *
 * This class has a distinction between "more data" and "ready data." An input stream could potentially have more data
 * incoming that has not arrived yet, or is still being decrypted, or is otherwise not immediately available but is
 * nonetheless still there. In these cases, moreContent() should return true and ready() should only return true if
 * there is some data sitting in a buffer that is actively **ready** to be removed. The readUntil() and fillContent()
 * extraction methods should wait for sufficient data from the underlying input stream, so the active thread should be
 * blocked, but the fillReadyContent() extraction method should never block and always return the data most immediately
 * available for use. This makes it easy to distinguish what to do with input streams like stdin.
 *
 * The optional overrides of bytesReady() and bytesRemaining() should be overridden if it is possible to determine how
 * much is ready and how much data remains. For File input streams, this should be trivial, but for unknown streams like
 * stdin or network streams, this should not be overridden and the default value, Limits<size_t>::max(), will be
 * returned instead. See the documentation on those methods for more information.
 *
 * Finally, an important part of the design of this object is the TestObj and TestIm subclasses. These make it simple to
 * write tests and use those tests in custom Input classes. The readUntil method takes a TestObj reference, which allows
 * implementations to be made in a source file rather than a header, while still allowing the writers of a test on the
 * calling side of readUntil() to just write a lambda that takes a char or byte and returns boolean, and wrap it in a
 * TestIm object.
 */
struct KH_EXPORT Input
{
    /*!
     * Interface type that is passed to readUntil() method to test stream input as it is being read.
     *
     * \headerfile kh/input.hpp
     *
     * The readUntil() method will call one of these two operators on the passed-in object for each character that is
     * read from the underlying input stream. It will then expect the object to return false if it should keep reading,
     * and true if it is now time to stop reading. This object does **not** indicate if the tested object is kept in the
     * output string or not, this is determined by the *include* flag on the readUntil() method itself.
     *
     * Both methods are virtual and simply call the other method. If you create an instance of this class without
     * overriding either of the methods, you have created a guaranteed infinite loop. You must override one of the two
     * methods in this class. The easiest way to do this is to simply use the TestIm object to pass to readUntil().
     */
    struct KH_EXPORT TestObj
    {
        /*!
         * Input Test Operator for char Type Input.
         *
         * If not overridden, this method will simply call the byte version after casing \p c to a byte. This function
         * is supposed to return false if the char tested is not the one to stop at, and true if it is.
         *
         * \param c The character being tested.
         * \return False if readUntil() should keep reading, and true if it should now stop.
         */
        [[nodiscard]] virtual bool operator()(char c) const noexcept;

        /*!
         * Input Test Operator for byte Type Input.
         *
         * If not overridden, this method will simply call the char version after casing \p b to a char. This function
         * is supposed to return false if the byte tested is not the one to stop at, and true if it is.
         *
         * \param b The byte being tested.
         * \return False if readUntil() should keep reading, and true if it should now stop.
         */
        [[nodiscard]] virtual bool operator()(byte b) const noexcept;
    };

    /*!
     * Templated implementation of the TestObj interface, allowing a single override.
     *
     * \headerfile kh/input.hpp
     *
     * This class is intended to be wrapped around any std::invocable object that can be invoked with a ByteType. This
     * means that any object that has operator () and takes either a char, a byte, or an unsigned char is a valid input
     * class for this template class. Usually this is a lambda function, but obviously any arbitrary class that meets
     * these requirements will be accepted and used properly.
     *
     * This class will automatically identify the type being used by the std::invocable and override the appropriate
     * method in the TestObj interface. This class allows for stateful test objects.
     */
    template <ByteType B, std::invocable<B> TEST>
    class TestIm : public TestObj
    {
        TEST f;

    public:
        /*!
         * TestIm constructor.
         *
         * This class wraps a std::invocable of a ByteType, called the TEST type. This constructor takes the object as
         * an rvalue reference, so you may need to call std::move on the object being passed in.
         *
         * \param f The test object that the TestIm class wraps around.
         */
        TestIm(TEST &&f) noexcept : f(f)
        {
            // No implementation.
        }

        /*!
         * Invoke the wrapped test object with the passed ByteType.
         *
         * This method uses std::invoke to call the wrapped std::invocable with the argument passed to this method. The
         * value returned by the std::invocable is converted to a bool and returned by this method. This method is
         * designated as "override" as it is supposed to override one of the two operator() methods of the TestObj
         * class, so if it does not do so this method will fail to compile.
         *
         * \param c A ByteType that will be passed to the wrapped test object.
         * \return The same boolean value returned by the wrapped test object.
         */
        [[nodiscard]] bool operator()(B c) const noexcept override
        {
            return std::invoke(f, c);
        }
    };

    //! \cond
    // CTAD guide section for the TestIm class:
    template <std::invocable<byte> TEST>
    TestIm(TEST &&) -> TestIm<byte, TEST>;
    template <std::invocable<char> TEST>
    TestIm(TEST &&) -> TestIm<char, TEST>;
    //! \endcond

    /*!
     * Returns whether or not the input stream has any more content to read at all.
     *
     * This should return true unless the end of file has been reached, the user cannot provide further input, or the
     * source is otherwise exhausted of data. The data may require blocking the thread of execution to get the input
     * from its source. Use ready() to see if there is unblocking bytes ready to read.
     *
     * \return Whether or not there is any more content at all, including content that has yet to arrive or be read.
     */
    [[nodiscard]] virtual bool moreContent() const noexcept = 0;

    /*!
     * Returns whether or not the underlying input stream has failed for any reason.
     *
     * This should return true if the underlying stream has failed, otherwise it should return false. "Failed" implies
     * that there was some kind of unrecoverable issue, unlike with standard streams where "failed" means a recoverable
     * issue with formatting. KirHut::Input classes never provide formatted input, so any errors on the input stream are
     * treated as unrecoverable.
     *
     * The underlying class should document what failures can occur in its failed() override, as well as any mitigations
     * a user application can perform for that input stream. This is usually input stream specific, so is not provided
     * by the Input interface.
     *
     * \return Whether or not the underlying input stream has failed for a potentially unrecoverable reason.
     */
    [[nodiscard]] virtual bool failedIn() const noexcept = 0;

    /*!
     * Reads input from the underlying input stream until the character designated by \p to is found, then returns the
     * content as a std::string.
     *
     * This is one of the two fundamental ways to read an input stream. This method treats the underlying input
     * similarly to character input, and tests each byte using the TestObj to check if a character is the one to stop at
     * or not. This can be as simple as finding a particular character, but in most cases you wouldn't use this input
     * unless it was more complex than that (use readLine(char) to find a character). The TestObj type may be stateful
     * and it may use any means it so determines to decide which byte to break upon.
     *
     * This method is required to be implemented by subclass objects of this interface. The nextToken(),
     * readData(char,bool), readLine(char), and ignoreUntil(char) methods, by default, are implemented using this
     * method. The readReadyUntil(TestObj const&,bool) method is also, by default, implemented in terms of this method,
     * as well as all of the other methods that rely on that, however they all use an additional TestObj that also
     * counts the bytes read to equal that returned by bytesReady() and do not execute if ready() is false.
     *
     * This method is marked nodiscard because if you wish to ignore input until TestObj is satisfied, you should call
     * the ignoreUntil() method instead. This allows for underlying implementations to optimize ignore operations. The
     * default ignoreUntil() method is implemented using this method, and just ignores the output, but using that
     * instead is better to read and allows for optimization.
     *
     * \param to An object that implements the TestObj interface properly. An empty TestObj will result in an infinite
     * loop.
     * \param include Indicates whether or not the character TestObj returns true for should be included in the returned
     * string or not. If false, it will remain in the input stream, and if true, it will be removed.
     * \throws any exception that may be thrown by the underlying implementation (this should be documented).
     * \return A std::string of the content read from the underlying input stream.
     */
    [[nodiscard]] virtual string readUntil(TestObj const &to, bool include = true) = 0;

    /*!
     * Read input into the provided byte buffer until the \p amount requested is read, EOF is reached, or an error
     * occurs.
     *
     * This is one of the two fundamental ways to read an input stream. This method treats the underlying input as a
     * stream of bytes, and ignores everything about the content. The byte buffer is simply filled with input from the
     * underlying input stream until the \p amount requested is reached, the end of file is reached, or some other kind
     * of error occurs while performing input. This method **will block the user thread** until the appropriate amount
     * of input is provided by the underlying stream, so do not call this method if you are trying to avoid blocking the
     * active thread!
     *
     * This method is required to be implemented by subclass objects of this interface. The fillContent(char*,size_t)
     * fillContent(std::span<T,N>), and other fill methods are implemented in terms of this method. A default
     * implementation of this method does exist in terms of readUntil(TestObj const&,bool), but is extremely
     * inefficient. It simply creates a test object that counts the bytes read, and returns a string of that size, then
     * copies the bytes from that string to the input buffer. Usually, fillContent(byte*,size_t) should be *more*
     * efficient than readUntil(TestObj const&,bool), but using this default implementation would invert that! Only use
     * this if you effectively never intend on using the fillContent(byte*,size_t) methods on the subclass anyway.
     *
     * \warning It is undefined behavior to provide an address to a byte buffer that is smaller than \p amount in bytes.
     * It is also "undefined behavior" to provide a null pointer as the byte buffer, however implementations are
     * encouraged to simply ignore this call and return 0 when that happens.
     *
     * \param buffer A pointer to a buffer of bytes of at least \p amount size.
     * \param amount The expected number of bytes to read, if there are sufficient bytesRemaining().
     * \throws any exception that may be thrown by the underlying implementation (this should be documented).
     * \return The number of bytes that were actually read into the buffer. This may be less than \p amount, including
     * zero.
     */
    virtual size_t fillContent(byte *buffer, size_t amount) = 0;

    /*!
     * Returns whether or not there are bytes ready to be read from the input stream immediately.
     *
     * If there are bytes that can be read from the fillReadyContent() method without blocking, this method should
     * return true, otherwise it should return false. Some streams may only have content after some event occurs from
     * the user, a network, or a different source with a time delay. This method allows a way to signal to the
     * application that there is content waiting to be read or not.
     *
     * By default, this returns true if bytesReady() returns a value greater than zero. Generally speaking, this is a
     * suitable implementation, but subclasses may override if necessary.
     *
     * \return Whether or not there are bytes waiting to be read with fillReadyContent().
     */
    [[nodiscard]] virtual bool ready() const noexcept;

    /*!
     * Return the number of bytes that are "ready" to be read with the fillReadyContent() method.
     *
     * This should return the number of bytes waiting in an input buffer that are ready to read, or are otherwise
     * available to the application in a non-blocking manner. This may be the whole file content, zero, or a value more
     * appropriate for the input stream implementation.
     *
     * By default, this returns the same value that bytesRemaining() returns.
     *
     * \return The number of bytes that are ready to be read, or simply the bytesRemaining().
     */
    [[nodiscard]] virtual size_t bytesReady() const noexcept;

    /*!
     * Return the number of bytes that remain available to read from the input source, or UNKNOWN_BYTES if the amount is
     * not known.
     *
     * This is most obviously useful for sources that have known, limited sizes. A good example is a filesystem file, an
     * encrypted file, or a known size block of data from a network source.
     *
     * By default, this returns UNKNOWN_BYTES.
     *
     * \return The number of bytes that are remaining in the input source, or UNKNOWN_BYTES if that is not known.
     */
    [[nodiscard]] virtual size_t bytesRemaining() const noexcept;

    /*!
     * Check if the input stream is in a "good" state, meaning it can perform extraction operations.
     *
     * The KirHut::Input class does not have the concept of a "fail" state like the standard input streams do. This is
     * because the KirHut::Input class does not do any input formatting at all, opting instead to read only raw bytes
     * and providing other tools to marshall the input bytes into usable data. So the "fail" state in this object is
     * more comparable to the "bad" state in a std::basic_ios, and recovery shouldn't be attempted in most situations.
     *
     * The "good" state of an input stream is true unless either the stream is in an unrecoverable state that cannot be
     * read from or the end of file/data has been reached. By default, goodIn() returns !failedIn() && moreContent(),
     * and any subclass implementation should return an equivalent functionality (as in, if goodIn() returns false, then
     * either failedIn() returns true or moreContent() returns false).
     *
     * \return Whether or not the input stream has more content to extract and can do so.
     */
    [[nodiscard]] virtual bool goodIn() const noexcept;

    /*!
     * Take in the next "token" in the input stream, which should work identically to std::istream >> std::string.
     *
     * This uses whitespace as returned by std::isspace() to determine where whitespace is, and returns the next "token"
     * string that is between whitespace. First, the whitespace in front of any text is ignored, then this method reads
     * string data until the next set of whitespace is found. The remaining whitespace is ignored and left in the input
     * stream, and the string of non-whitespace is returned by this method.
     *
     * Subclass implementations should do the same, however this may be changed in the future with a Tokenizer class
     * that allows the user to provide different conditions to return the tokens with and what to ignore. As such, try
     * to avoid implementing an override to this method until this has been implemented. This method by default calls
     * ignoreUntil(TestObj const&,bool) and readUntil(TestObj const&,bool), so may throw exceptions from either of them.
     *
     * \throws any Exception thrown by the underlying implementation, either ignoreUntil(TestObj const&,bool) and
     * readUntil(TestObj const&,bool) or the subclass override.
     * \return A std::string of the next "token" as defined in this method's description.
     */
    [[nodiscard]] virtual string nextToken();

    /*!
     * Gather all bytes in the input stream until the target byte \p b is found, then return the data.
     *
     * The returned string may or may not \p include the byte \p b at the end. The \p b byte will remain in the input
     * stream if \p include is false. If you wish to both not include the \p b byte and not have it in the input, use
     * the readLine(char) method instead.
     *
     * This method's default implementation uses readUntil(TestObj const&,bool) with a TestObj that simply compares
     * the passed in byte to the byte provided as an argument here. A subclass should replace this with a more efficient
     * implementation if that is available to it, as this API is stable. Just like readUntil(TestObj const&,bool), this
     * method is blocking for the active thread.
     *
     * \param b A delimiter byte which should be in the input stream to stop reading data at.
     * \param include Whether or not to include \p b in the output and remove it from the input stream.
     * \throws any Exception thrown by the underlying implementation, either readUntil(TestObj const&,bool) or the
     * subclass override.
     * \return All the bytes from the first in the input stream to \p b, with \p b optionally included.
     */
    [[nodiscard]] virtual string readData(char b = '\n', bool include = true);

    /*!
     * \copydoc readData(char,bool)
     */
    [[nodiscard]] virtual std::vector<byte> readData(byte b, bool include = true);

    /*!
     * Gather all bytes in the input stream until the target byte \p b is found, then return the data.
     *
     * The returned string may or may not \p include the byte \p b at the end. The \p b byte will remain in the input
     * stream if \p include is false. If you wish to both not include the \p b byte and not have it in the input, use
     * the readReadyLine() method instead.
     *
     * This method's default implementation uses readReadyUntil(TestObj const&,bool) with a TestObj that simply compares
     * the passed in byte to the byte provided as an argument here. A subclass should replace this with a more efficient
     * implementation if that is available to it, as this API is stable. Just like readReadyUntil(TestObj const&,bool),
     * this method should not block the active thread.
     *
     * \param b A delimiter byte which should be in the input stream to stop reading data at.
     * \param include Whether or not to include \p b in the output and remove it from the input stream.
     * \throws any Exception thrown by the underlying implementation, either readUntil(TestObj const&,bool) or the
     * subclass override.
     * \return All the bytes from the first in the input stream to \p b, with \p b optionally included.
     */
    [[nodiscard]] virtual string readReadyData(char b = '\n', bool include = true);

    /*!
     * \copydoc readReadyData(char,bool)
     */
    [[nodiscard]] virtual std::vector<byte> readReadyData(byte b, bool include = true);

    /*!
     * Reads ready input from the underlying input stream until the character designated by \p to is found, then returns
     * the content as a std::string.
     *
     * This method is implemented on top of readUntil(TestObj const&,bool), but has an additional test condition that
     * ends reading when the value returned by bytesReady() has been exceeded. This default implementation is not ideal
     * for most streams, as it is stateful and relatively slow, but so long as the underlying input stream has done the
     * bytesReady() implementation correctly, this should not be a problem. An empty string will be returned and no call
     * to readUntil(TestObj const&,bool) will be made if ready() returns false.
     *
     * This method's default implementation uses readUntil(TestObj const&,bool), but should not be blocking. The
     * readReadyData(char,bool), readReadyLine(char), and ignoreReadyUntil(char) methods, by default, are implemented
     * using this method.
     *
     * This method is marked nodiscard because if you wish to ignore input until TestObj is satisfied, you should call
     * the ignoreReadyUntil(char) method instead. This allows for underlying implementations to optimize ignore
     * operations. The default ignoreReadyUntil(char) method is implemented using this method, and just ignores the
     * output, but using that instead is better to read and allows for optimization.
     *
     * \param to An object that implements the TestObj interface properly. An empty TestObj will result in an infinite
     * loop.
     * \param include Indicates whether or not the character TestObj returns true for should be included in the returned
     * string or not. If false, it will remain in the input stream, and if true, it will be removed.
     * \throws any Exception thrown by the underlying implementation, either readUntil(TestObj const&,bool) or the
     * subclass override.
     * \return A std::string of the content read from the underlying input stream.
     */
    [[nodiscard]] virtual string readReadyUntil(TestObj const &to, bool include = true);

    /*!
     * Gather all bytes in the input stream until the target byte \p b is found, then return the data.
     *
     * The returned string will not include the delimiter byte \p b, and the byte will also be removed from this input
     * stream. This is useful when you have a specific byte that is a genuine delimiter that you do not want to see in
     * the input (such as end line when going through lines). If you wish to keep the \p b byte, either in the returned
     * string or in the input stream, use readData(char,bool) instead.
     *
     * By default, this method is implemented using readData(char,bool), so it may throw any exception from that method
     * or its subclass override. Just like readData(char,bool), this method is blocking for the active thread.
     *
     * \param b A delimiter byte which should be in the input stream to stop reading data at.
     * \throws any Exception thrown by the underlying implementation, either readData(char,bool) or the subclass
     * override.
     * \return All the bytes from the first in the input stream to \p b, never including \p b.
     */
    [[nodiscard]] virtual string readLine(char b = '\n');

    /*!
     * \copydoc readLine(char)
     */
    [[nodiscard]] virtual std::vector<byte> readLine(byte b);

    /*!
     * Gather all bytes in the input stream until the target byte \p b is found, then return the data.
     *
     * The returned string will not include the delimiter byte \p b, and the byte will also be removed from this input
     * stream. This is useful when you have a specific byte that is a genuine delimiter that you do not want to see in
     * the input (such as end line when going through lines). If you wish to keep the \p b byte, either in the returned
     * string or in the input stream, use readReadyData(char,bool) instead.
     *
     * By default, this method is implemented using readReadyData(char,bool), so it may throw any exception from that
     * method or its subclass override. Just like readReadyData(char,bool), this method does not block the active
     * thread. This method may return less bytes than are available in the underlying stream, as further bytes are not
     * yet ready.
     *
     * \param b A delimiter byte which should be in the input stream to stop reading data at.
     * \throws any Exception thrown by the underlying implementation, either readReadyData(char,bool) or the subclass
     * override.
     * \return All the bytes from the first in the input stream to \p b, never including \p b.
     */
    [[nodiscard]] virtual string readReadyLine(char b = '\n');

    /*!
     * \copydoc readReadyLine(char)
     */
    [[nodiscard]] virtual std::vector<byte> readReadyLine(byte b);

    /*!
     * Ignore all bytes in the input stream until the target byte \p b is found.
     *
     * The target byte \p b may be removed from the stream or retained, depending on the value of \p include. This
     * method should be used in preference to simply ignoring input from the readData(char,bool) method because this
     * provides optimization opportunities for the underlying input stream, and the default implementation already does
     * just that.
     *
     * This method is implemented using readData(char,bool), so it may throw any exception from that method or its
     * subclass override. Just like readData(char,bool), this method is blocking for the active thread.
     *
     * \param b A delimiter byte signifying when to stop ignoring input data upon finding.
     * \param include Whether or not to also remove \p b from the input stream.
     * \throws any Exception thrown by the underlying implementation, either readData(char,bool) or the subclass
     * override.
     * \return The actual number of bytes that were ignored as a result of this call.
     */
    virtual size_t ignoreData(char b = '\n', bool include = true);

    /*!
     * \copydoc ignoreData(char,bool)
     */
    virtual size_t ignoreData(byte b, bool include = true);

    /*!
     * Ignore \p amount bytes in the input stream regardless of what they are.
     *
     * This method is very unlikely to find much use unless you know how large input data is supposed to be and you know
     * you can freely skip it. This method is simply meant to be an analog to the read commands as an ignore, to avoid
     * the temptation of using a get method to ignore input.
     *
     * The default implementation of this method uses getContent(size_t) to read the input data into a string, then
     * simply ignores the returned string. Use this method in preference of getContent(size_t) if you are trying to
     * ignore input to give the underlying implementation an opportunity to optimize ignore commands. The default
     * implementation uses getContent(size_t), so it may throw any exception from that method or its subclass override.
     * Just like getContent(size_t), this method is blocking for the active thread.
     *
     * \param amount The number of bytes to ignore in the input stream.
     * \throws any Exception thrown by the underlying implementation, either getContent(size_t) or the subclass
     * override.
     * \return The actual number of bytes that were ignored as a result of this call.
     */
    virtual size_t ignoreData(size_t amount);

    /*!
     * Ignore all bytes in the input stream until the test condition \p to returns true.
     *
     * This is the more generic version of ignoreData(char,bool) using TestObj to test each byte in the input stream.
     * The byte that \p returns true on may also be ignored depending on the state of \p include when this method is
     * called. Just like with ignoreData(char,bool), it is recommended to use this method instead of
     * readUntil(TestObj const&,bool) to allow for optimization opportunities for the subclass.
     *
     * This method is implemented using readUntil(TestObj const&,bool), so it may throw any exception from that method
     * or its subclass override. Just like readUntil(TestObj const&,bool), this method is blocking for the active
     * thread.
     *
     * \param to An object that implements the TestObj interface properly. An empty TestObj will result in an infinite
     * loop.
     * \param include Whether or not to also remove the byte \p to returned true for from the input stream.
     * \throws any Exception thrown by the underlying implementation, either readUntil(TestObj const&,bool) or the
     * subclass override.
     * \return The actual number of bytes that were ignored as a result of this call.
     */
    virtual size_t ignoreUntil(TestObj const &to, bool include = true);

    /*!
     * Ignore all ready bytes in the input stream until the target byte \p b is found.
     *
     * The target byte \p b may be removed from the stream or retained, depending on the value of \p include. This
     * method should be used in preference to simply ignoring input from the readReadyData(char,bool) method because
     * this provides optimization opportunities for the underlying input stream, and the default implementation already
     * does just that.
     *
     * This method is implemented using readReadyData(char,bool), so it may throw any exception from that method or its
     * subclass override. Just like readReadyData(char,bool), this method does not block the active thread.
     *
     * \param b A delimiter byte signifying when to stop ignoring input data upon finding.
     * \param include Whether or not to also remove \p b from the input stream.
     * \throws any Exception thrown by the underlying implementation, either readReadyData(char,bool) or the subclass
     * override.
     * \return The actual number of bytes that were ignored as a result of this call.
     */
    virtual size_t ignoreReadyData(char b = '\n', bool include = true);

    /*!
     * \copydoc ignoreReadyData(char,bool)
     */
    virtual size_t ignoreReadyData(byte b, bool include = true);

    /*!
     * Ignore \p amount ready bytes in the input stream regardless of what they are.
     *
     * This method is very unlikely to find much use unless you know how large input data is supposed to be and you know
     * you can freely skip it. This method is simply meant to be an analog to the read commands as an ignore, to avoid
     * the temptation of using a get method to ignore input.
     *
     * The default implementation of this method uses getContent(size_t) to read the input data into a string, then
     * simply ignores the returned string. Use this method in preference of getContent(size_t) if you are trying to
     * ignore input to give the underlying implementation an opportunity to optimize ignore commands. The default
     * implementation uses getContent(size_t), so it may throw any exception from that method or its subclass override.
     * Just like getContent(size_t), this method is blocking for the active thread.
     *
     * \param amount The number of bytes to ignore in the input stream.
     * \throws any Exception thrown by the underlying implementation, either getReadyContent(size_t) or the subclass
     * override.
     * \return The actual number of bytes that were ignored as a result of this call.
     */
    virtual size_t ignoreReadyData(size_t amount);

    /*!
     * Ignore all ready bytes in the input stream until the test condition \p to returns true.
     *
     * This is the more generic version of ignoreReadyData(char,bool) using TestObj to test each byte in the input
     * stream. The byte that \p returns true on may also be ignored depending on the state of \p include when this
     * method is called. Just like with ignoreData(char,bool), it is recommended to use this method instead of
     * readUntil(TestObj const&,bool) to allow for optimization opportunities for the subclass.
     *
     * This method is implemented using readReadyUntil(TestObj const&,bool), so it may throw any exception from that
     * method or its subclass override. Just like readReadyUntil(TestObj const&,bool), this method does not block the
     * active thread.
     *
     * \param to An object that implements the TestObj interface properly. An empty TestObj will result in an infinite
     * loop.
     * \param include Whether or not to also remove the byte \p to returned true for from the input stream.
     * \throws any Exception thrown by the underlying implementation, either readReadyUntil(TestObj const&,bool) or the
     * subclass override.
     * \return The actual number of bytes that were ignored as a result of this call.
     */
    virtual size_t ignoreReadyUntil(TestObj const &to, bool include = true);

    /*!
     * Read the \p amount of bytes requested into a string and return that string.
     *
     * This is a relatively simplistic way to use fillContent(char*,size_t) that returns a std::string instead of
     * filling a buffer that is provided by the user. This method otherwise works identically to
     * fillContent(char*,size_t), meaning that it will block the active thread if there is not sufficient ready content
     * and any exceptions thrown by that method or its subclass implementations may be thrown by this method.
     *
     * \param amount The number of bytes that should be read and returned by the string.
     * \throws any Exception thrown by the underlying implementation, either fillContent(char*,size_t) or the subclass
     * override.
     * \return The string of byte data from the underlying input stream.
     */
    [[nodiscard]] virtual string getContent(size_t amount);

    /*!
     * Read the \p amount of ready bytes requested into a string and return that string.
     *
     * This is a relatively simplistic way to use fillReadyContent(char*,size_t) that returns a std::string instead of
     * filling a buffer that is provided by the user. This method otherwise works identically to
     * fillReadyContent(char*,size_t), meaning that it will not block the active thread and any exceptions thrown by
     * that method or its subclass implementations may be thrown by this method.
     *
     * \param amount The number of bytes that should be read and returned by the string.
     * \throws any Exception thrown by the underlying implementation, either fillReadyContent(char*,size_t) or the
     * subclass override.
     * \return The string of byte data from the underlying input stream.
     */
    [[nodiscard]] virtual string getReadyContent(size_t amount);

    /*!
     * Read input into the provided byte buffer until the \p amount requested is read, EOF is reached, or an error
     * occurs.
     *
     * This is one of the two fundamental ways to read an input stream. This method treats the underlying input as a
     * stream of bytes, and ignores everything about the content. The byte buffer is simply filled with input from the
     * underlying input stream until the \p amount requested is reached, the end of file is reached, or some other kind
     * of error occurs while performing input. This method **will block the user thread** until the appropriate amount
     * of input is provided by the underlying stream, so do not call this method if you are trying to avoid blocking the
     * active thread!
     *
     * This method's default implementation uses fillContent(byte*,size_t), which a subclass is required to implement.
     * A subclass may also override this method to more efficiently run this, however this may not be necessary in many
     * cases. The getContent(size_t) and other forms of the fillContent methods are implemented using this method. If
     * a subclass implements the fillContent(byte*,size_t) method, this one mostly comes for free without additional
     * efficiency costs.
     *
     * \warning It is undefined behavior to provide an address to a byte buffer that is smaller than \p amount in bytes.
     * It is also "undefined behavior" to provide a null pointer as the byte buffer, however implementations are
     * encouraged to simply ignore this call and return 0 when that happens.
     *
     * \param buffer A pointer to a buffer of bytes of at least \p amount size.
     * \param amount The expected number of bytes to read, if there are sufficient bytesRemaining().
     * \throws any Exception thrown by the underlying implementation, either fillContent(byte*,size_t) or the subclass
     * override.
     * \return The number of bytes that were actually read into the buffer. This may be less than \p amount, including
     * zero.
     */
    virtual size_t fillContent(char *buffer, size_t amount);

    /*!
     * Read ready input into the provided byte buffer until the \p amount requested is read, there is no more ready
     * data, EOF is reached, or an error occurs.
     *
     * Some sources of content buffer their inputs when the input is unpredictable, like with a network connection or a
     * stream of user input. This method allows for those sources to provide input in a "non-blocking" manner, which can
     * be useful in a wide variety of contexts. The other "non-ready" input methods will block the active thread when
     * there is insufficient input to satisfy the requirements.
     *
     * By default, this method uses fillContent(byte*,size_t) directly, and maxes the amount to the amount returned by
     * bytesReady(). This call will not happen if ready() returns false. The getReadyContent() method and other forms of
     * the fillReadyContent() methods are implemented using this method.
     *
     * \warning It is undefined behavior to provide an address to a byte buffer that is smaller than \p amount in bytes.
     * It is also "undefined behavior" to provide a null pointer as the byte buffer, however implementations are
     * encouraged to simply ignore this call and return 0 when that happens.
     *
     * \param buffer A pointer to a buffer of bytes of at least \p amount size.
     * \param amount The maximum number of bytes to read, if there are sufficient bytesReady().
     * \throws any Exception thrown by the underlying implementation, either fillContent(byte*,size_t) or the subclass
     * override.
     * \return The number of bytes that were actually read into the buffer. This may be less than \p amount, including
     * zero.
     */
    virtual size_t fillReadyContent(byte *buffer, size_t amount);

    /*!
     * \copydoc fillReadyContent(byte*,size_t)
     */
    virtual size_t fillReadyContent(char *buffer, size_t amount);

    /*!
     * Read input into the provided byte buffer until the \p amount requested is read, EOF is reached, or an error
     * occurs.
     *
     * This is one of the two fundamental ways to read an input stream. This method treats the underlying input as a
     * stream of bytes, and ignores everything about the content. The byte buffer is simply filled with input from the
     * underlying input stream until the \p amount requested is reached, the end of file is reached, or some other kind
     * of error occurs while performing input. This method **will block the user thread** until the appropriate amount
     * of input is provided by the underlying stream, so do not call this method if you are trying to avoid blocking the
     * active thread!
     *
     * This method uses fillContent(byte*,size_t), which a subclass is required to implement. If a subclass implements
     * the fillContent(byte*,size_t) method, this one mostly comes for free without additional efficiency costs.
     *
     * \param buffer A std::span of any ByteType to fill with the content of the underlying input stream.
     * \throws any Exception that may be thrown by the underlying implementation (this should be documented).
     * \return The number of bytes that were actually read into the buffer. This may be smaller than \p buffer,
     * including zero.
     */
    template <ByteType T, size_t N = std::dynamic_extent>
    inline size_t fillContent(span<T, N> buffer)
    {
        return fillContent(bit_cast<byte *>(buffer.data()), buffer.size_bytes());
    }

    /*!
     * Read ready input into the provided byte buffer until the \p amount requested is read, there is no more ready
     * data, EOF is reached, or an error occurs.
     *
     * Some sources of content buffer their inputs when the input is unpredictable, like with a network connection or a
     * stream of user input. This method allows for those sources to provide input in a "non-blocking" manner, which can
     * be useful in a wide variety of contexts. The other "non-ready" input methods will block the active thread when
     * there is insufficient input to satisfy the requirements.
     *
     * This method uses fillReadyContent(byte*,size_t) directly, and caps the max \p amount to the amount returned by
     * bytesReady(). This call will not happen if ready() returns false. The getReadyContent() method and other forms of
     * the fillReadyContent() methods are implemented using this method.
     *
     * \param buffer A std::span of any ByteType to fill with the content of the underlying input stream.
     * \throws any Exception that may be thrown by the underlying implementation (this should be documented).
     * \return The number of bytes that were actually read into the buffer. This may be smaller than \p buffer,
     * including zero.
     */
    template <ByteType T, size_t N = std::dynamic_extent>
    inline size_t fillReadyContent(span<T, N> buffer)
    {
        return fillReadyContent(bit_cast<byte *>(buffer.data()), buffer.size_bytes());
    }
};

/*!
 * Interface representing an Input type that can be traversed, or "seeked" through.
 *
 * \headerfile kh/input.hpp
 *
 * The "Seekable" types provide the notion of a cursor within a set of bytes, and that cursor can be moved and have its
 * position reported upon. There is also an isFloating() method, which means that the input cursor is considered
 * "floating," which means that when an output cursor exists that outputs to a location prior to the input, the input
 * cursor will move its position along with the new output, rather than remaining in the same cursor position. This is
 * actually usually how you want it, so by default isFloating() should return true on most streams, unless there is a
 * good reason not to do that.
 *
 * This interface is only a small extension on the Input interface, so there are not that many methods. Users of this
 * interface, in addition to needing to implement the four pure virtual methods of Input must also implement the
 * tellIn() and seekIn(i64,KirHut::Relation) methods. This means that there are a grand total of six methods that an
 * implementation class will need to provide implementation for to create a new SeekableInput class.
 */
struct KH_EXPORT SeekableInput : public Input
{
    using enum Relation;

    /*!
     * Report if the input cursor is "floating" or it moves along with the text when output is inserted before the
     * cursor.
     *
     * A "floating" cursor will move along with the text when the text position changes. This is useful in cases where
     * there is both an input and output cursor on the same destination, and the output cursor starts inserting bytes
     * before the input cursor position. Frequently, it is better for the input cursor to remain in the same "relative"
     * position in the data stream instead of just staying in its absolute position, but this obviously depends on the
     * use case.
     *
     * By default, this method returns true and the setFloating(bool) method always returns true, indicating that it is
     * impossible to remove the stream floating state. This is to respect the most common implementation, however
     * implementations of SeekableInput may want to change this if the input is from an unusual source. It is also
     * legitimate to always return false from this method and prevent changing the floating status in setFloating(bool),
     * always returning false instead of true.
     *
     * \return A flag indicating if the SeekableInput cursor position is "floating" in the data stream.
     */
    virtual bool isFloating() const noexcept;

    /*!
     * Sets the "floating" state of the input cursor, or whether or not the input cursor moves along with the text when
     * output is inserted before the cursor.
     *
     * A "floating" cursor will move along with the text when the text position changes. This is useful in cases where
     * there is both an input and output cursor on the same destination, and the output cursor starts inserting bytes
     * before the input cursor position. Frequently, it is better for the input cursor to remain in the same "relative"
     * position in the data stream instead of just staying in its absolute position, but this obviously depends on the
     * use case.
     *
     * An implementation may require the input cursor to be floating, or require it to be not floating, but regardless
     * of how this is implemented, the returned bool is always the current "floating" state after the change, and the
     * passed state should be what is desired. If the returned state does not match the desired state, that is because
     * that desired state is not allowed. It is possible for an implementation to always or never be floating.
     *
     * \param shouldFloat
     * \return
     */
    virtual bool setFloating(bool shouldFloat = true) noexcept;

    /*!
     * Reports the position of the input cursor in this stream.
     *
     * The returned value indicates a location from 0 to Limits<SeekPos>::max() where 0 is the first byte to be provided
     * by this input stream.
     *
     * The subclass implementation is required to implement this method, as no default can be sanely provided.
     *
     * \return The position in the input stream the input cursor is currently located at.
     */
    virtual SeekPos tellIn() const noexcept = 0;

    /*!
     * Moves the position of the input cursor to a location at \p off in relation \p from a position.
     *
     * The Relation \p from indicates where to start the given offset \p off to determine the new stream position. If
     * the \p from value is Relation::BEGIN, the \p off location is relative to the 0 position of the input stream, and
     * should never be a negative value.
     * \param off
     * \param from
     */
    virtual void seekIn(i64 off, Relation from = CURRENT) = 0;
};

} // namespace KirHut::IO
