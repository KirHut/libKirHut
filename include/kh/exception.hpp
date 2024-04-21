/***********************************************************************************************************************
** The KirHut Library for the Public Benefit
** exception.hpp
** Copyright (C) 2024 KirHut Security Company
**
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
** later version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
** warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
** details.
**
** You should have received a copy of the GNU Affero General Public License along with this program.  If not, see
** <http://www.gnu.org/licenses/>.
***********************************************************************************************************************/
#pragma once

#include "kh/base.hpp"

#include <exception>

namespace KirHut
{

/*!
 * The Exception class is a subclass of std::exception that prefers the use of C++20 std::u8string_view instead of
 * `const char *`.
 *
 * This class has two primary purposes: first make an exception class that allows the use of constant strings and
 * std::u8string_view for exception free Exception constructors, and second to have a class that allows construction of
 * copies of Exceptions with custom strings built at runtime without creation of additional copies of the underlying
 * std::string or ever throwing an exception.
 *
 * Simply put, this class should never throw exceptions under any circumstances unless a subclass does so in its
 * constructor, with the exception of the copying StringView constructor. This includes bad_alloc, as this class does
 * not require memory allocations for any operation outside of the copying StringView constructor at all.
 *
 * The preferred way to use this class is to document that your function or method throws a subclass of this class, and
 * to create your subclass like so:
 * ~~~
 * class MyException : public Exception
 * {
 * public:
 *     using Exception::Exception;
 * }
 * ~~~
 * This ensures that you have all of the constructors that Exception has, don't have to provide any additional
 * implementation whatsoever, and can take advantage of C++'s type system for catch statements.
 *
 * This class prefers using info() to get the user returned exception info, and this method will always return the
 * complete string used to construct this class from it. The what() method is considered a legacy method that should
 * only be used by catch() blocks that catch std::exception instead of Exception. The what() method will return the
 * same UTF-8 string as info(), just as a `char` instead of a `char8_t`.
 */
class KH_EXPORT Exception : public std::exception
{
public:
    /*!
     * The non-copying StringView Exception constructor.
     *
     * This constructor requires that the string pointed at by \p sv is null-terminated as the pointer returned by
     * what() is simply the data pointer of the \p sv std::u8string_view. Use the copying StringView constructor if this
     * Exception object should copy the passed u8string_view, which is a requirement if you cannot determine if \p sv
     * is null-terminated.
     *
     * This is mostly useful for passing in a constant string, like the examples below:
     * ~~~
     * Exception ex(u8"The argument passed was invalid.");
     * Exception ex2(u8"The function return value was invalid."sv);
     * ~~~
     *
     * \warning The \p sv data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object, and any copies of the Exception object! If you cannot provide this, use the copying StringView
     * constructor! If this is not done, the behavior is undefined!
     *
     * \param sv A StringView to the data returned by info() and what().
     */
    Exception(StringView sv) noexcept;

    /*!
     * The copying StringView Exception constructor.
     *
     * Generally speaking, unless you pass true to \p copy, this constructor should not be used. You should use the
     * noexcept constructor, however this constructor does support being called with false in the case that you need to
     * choose between creating a copy or not.
     *
     * This constructor requires that the string pointed at by \p sv is null-terminated if \p copy is false as the
     * pointer returned by what() is simply the data pointer of the \p sv std::u8string_view in that case. Use the
     * \p copy boolean to indicate if this Exception object should copy the passed StringView, which is a requirement if
     * you cannot determine if \p sv is null-terminated.
     *
     * \note This constructor is guaranteed not to throw an exception if \p copy is false.
     *
     * \warning The \p sv data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object,and any copies of the Exception object, if \p copy is false! If you cannot provide this, set \p copy to
     * true! If this is not done, the behavior is undefined!
     *
     * \param sv A StringView to the data returned by info() and what().
     * \param copy A boolean indicating if this Exception object should make an underlying deep copy. Default is false.
     * \throws bad_alloc May be thrown if \p copy is true and allocating the underlying memory failed.
     */
    Exception(StringView sv, bool copy);

    /*!
     * The String Exception constructor.
     *
     * This constructor is the most useful for generic Exception messages as it allows the caller to create any
     * arbitrary string, including with data that is runtime determined in the catch statement. This does require that
     * the caller creates a std::u8string object which inherently requires the use of memory allocations, and those
     * allocations may throw, however it is the responsibility of the caller to handle those issues, and once the String
     * is complete the Exception object is guaranteed to never throw any further exceptions.
     *
     * All copies of this Exception are guaranteed not to create any additional String objects, to not lose the String
     * data, and to not throw any exceptions.
     *
     * \param str An rvalue to a String with the message this Exception will have.
     */
    Exception(String &&str) noexcept;

    /*!
     * Exception copy constructor.
     *
     * This copy constructor complies with the std::exception object requirements to have a non-throwing copy
     * constructor. This class also provides a non-throwing move constructor.
     *
     * \param other The other Exception class to copy.
     */
    Exception(Exception const &other) noexcept;

    /*!
     * Exception move constructor.
     *
     * This constructor is provided for convenience in case an Exception is created somewhere it is not thrown.
     *
     * \note The Exception passed will be invalidated after this call, unless it was created using the non-copying
     * StringView constructor, in which case they will both be valid and this is the equivalent of a copy.
     *
     * \param other The Exception object being moved from, which may be invalid afterward.
     */
    Exception(Exception &&other) noexcept;

    /*!
     * Exception destructor.
     *
     * This destructor will only deallocate dynamic memory if this is the last object that refers to an underlying
     * String object containing the info() string. If this Exception, or the Exception this was copied from, was created
     * using the non-copying StringView constructor, this destructor will never deallocate.
     */
    virtual ~Exception();

    /*!
     * Exception copy assignment operator.
     *
     * This operator complies with the std::exception object requirements to have a non-throwing assignment operator.
     * This class also provides a non-throwing move assignment operator.
     *
     * \param other The Exception object this one is copying.
     * \return A reference to `*this` Exception object.
     */
    Exception &operator=(Exception const &other) noexcept;

    /*!
     * Exception move assignment operator.
     *
     * This move assignment operator is provided for convenience in case an Exception is created in a different context
     * than the one it is thrown.
     *
     * \note The Exception passed will be invalidated after this call, unless it was created using the non-copying
     * StringView constructor, in which case they will both be valid and this is the equivalent of a copy.
     *
     * \param other The Exception object being moved from, which may be invalid afterward.
     * \return A reference to `*this` Exception object.
     */
    Exception &operator=(Exception &&other) noexcept;

    /*!
     * Returns a StringView to the string passed into the constructor.
     *
     * You should always prefer using this method to see what was thrown in the Exception over using what(), as this
     * method returns a C++20 std::u8string_view as well as working without compromise in the case of an invalid
     * underlying string passed to the non-copying StringView constructor.
     *
     * \return A StringView of the string used to construct this Exception.
     */
    virtual StringView info() const noexcept;

    /*!
     * Legacy override of the std::exception::what() method.
     *
     * This method should return the same string as returned by info(), except that this is a C style string. The string
     * should be null-terminated, but this object does not enforce this requirement when the Exception is constructed
     * using the non-copying StringView constructor, so this cannot be completely relied upon. If you are creating an
     * Exception object that is likely to be read from what(), ensure the StringView passed has a null-terminating
     * character or prefer the copying StringView or String constructors.
     *
     * The returned string is a direct cast of the underlying std::u8string data, as that is permitted in C++. This
     * ensures **O(1)** time efficiency.
     *
     * \return The same string as info().
     */
    virtual char const *what() const noexcept override;

private:
    struct Impl;

    // This data is marked mutable because the implementation needs to do all sorts of backend tricks to prevent
    // allocations when making and destroying potentially const copies of Exception.
    String mutable data;
    StringView infoView;

    // These are used to define an absolute order of copies of Exception objects. This is only used when the
    // String constructor is called to create the base Exception object, then all copies of the Exception place
    // themselves in a doubly linked list to determine the "owner" of the string.
    Exception mutable const *prev, *next;
};

} // namespace KirHut
