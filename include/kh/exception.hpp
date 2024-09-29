/***********************************************************************************************************************
** The KirHut Application Development Library
** exception.hpp
** Copyright (C) 2024 KirHut Software Company
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

#include "base.hpp"

#include <exception>

namespace KirHut
{

/*!
 * The Exception class is a subclass of std::exception that prefers the use of C++17 std::string_view instead of
 * `const char *`.
 *
 * This class has two primary purposes: first make an exception class that allows the use of constant strings and
 * std::string_view for exception free Exception constructors, and second to have a class that allows construction of
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
 * same UTF-8 string as info(), just as a `char` array instead of a std::string_view.
 */
class KH_EXPORT Exception : public std::exception
{
public:
    /*!
     * The non-copying string_view Exception constructor.
     *
     * This constructor requires that the string pointed at by \p sv is null-terminated as the pointer returned by
     * what() is simply the data pointer of the \p sv std::string_view. Use the copying string_view constructor if this
     * Exception object should copy the passed string_view, which is a requirement if you cannot determine if \p sv
     * is null-terminated. This constructor is separated from the copying string_view constructor so that it may be
     * marked as `noexcept`.
     *
     * This is mostly useful for passing in a constant string, like the examples below:
     * ~~~
     * Exception ex2("The function return value was invalid."sv);
     * ~~~
     * The C++ standard requires that the above strings are null-terminated, including the one that is wrapped by the
     * `sv` operator. The `sv` operator can be made slightly more efficient by a compiler as the compiler can save the
     * string_view as a constant value and does not need to compute the length at runtime.
     *
     * \warning The \p sv data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object, and any copies of the Exception object! If you cannot provide this, use the copying string_view
     * constructor! If this is not done, the behavior is undefined!
     *
     * \param sv A std::string_view to the data returned by info() and what().
     */
    explicit Exception(string_view sv) noexcept;

    /*!
     * The non-copying `const char` Exception constructor.
     *
     * This constructor accepts a constant char array as an argument, then simply wraps that string in a string_view
     * and passes it to the non-copying string_view constructor. As such, this constructor is effectively identical to
     * the non-copying string_view constructor, and for all intents and purposes, may be treated as that constructor in
     * all other relevant documentation. This constructor fixes the ambiguity between the non-copying string_view
     * constructor and the String rvalue reference constructor when using a `const char` array as input.
     *
     * This is mostly useful for passing in a constant string, like the examples below:
     * ~~~
     * Exception ex("The argument passed was invalid.");
     * ~~~
     *
     * The C++ standard requires that the above strings are null-terminated. The `sv` operator can potentially be made
     * slightly more efficient by a compiler as the compiler can save the string_view as a constant value and does not
     * need to compute the length at runtime.
     *
     * \warning The \p ptr data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object, and any copies of the Exception object! If you cannot provide this, use the copying StringView
     * constructor! If this is not done, the behavior is undefined!
     *
     * \param ptr A constant char pointer to the data that will be returned by info() and what().
     */
    explicit Exception(char const *ptr) noexcept;

    /*!
     * The copying StringView Exception constructor.
     *
     * Generally speaking, unless you pass true to \p copy, this constructor should not be used. You should use the
     * noexcept constructor, however this constructor does support being called with false in the case that you need to
     * choose between creating a copy or not.
     *
     * This constructor requires that the string pointed at by \p sv is null-terminated if \p copy is false as the
     * pointer returned by what() is simply the data pointer of the \p sv std::string_view in that case. Use the \p copy
     * boolean to indicate if this Exception object should copy the passed string_view, which is a requirement if you
     * cannot determine if \p sv is null-terminated.
     *
     * \note This constructor is guaranteed not to throw an exception if \p copy is false.
     *
     * \warning The \p sv data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object,and any copies of the Exception object, if \p copy is false! If you cannot provide this, set \p copy to
     * true! If this is not done, the behavior is undefined!
     *
     * \param sv A string_view to the data returned by info() and what().
     * \param copy A boolean indicating if this Exception object should make an underlying deep copy. Default is false.
     * \throws bad_alloc May be thrown if \p copy is true and allocating the underlying memory failed.
     */
    Exception(string_view sv, bool copy);

#if KH_PRIV_DOCS || __cpp_char8_t
    /*!
     * The non-copying u8string_view Exception constructor.
     *
     * This constructor requires that the string pointed at by \p sv is null-terminated as the pointer returned by
     * what() is simply the data pointer of the \p sv std::u8string_view. Use the copying u8string_view constructor if
     * this Exception object should copy the passed u8string_view, which is a requirement if you cannot determine if
     * \p sv is null-terminated. This constructor is separated from the copying u8string_view constructor so that it may
     * be marked as `noexcept`.
     *
     * This is mostly useful for passing in a constant string, like the examples below:
     * ~~~
     * Exception ex2(u8"The function return value was invalid."sv);
     * ~~~
     * The C++ standard requires that the above strings are null-terminated, including the one that is wrapped by the
     * `sv` operator. The `sv` operator can be made slightly more efficient by a compiler as the compiler can save the
     * string_view as a constant value and does not need to compute the length at runtime.
     *
     * This constructor works identically with the normal non-copying string_view constructor, as all strings in KirHut
     * software are assumed to be UTF-8. As such, the underlying data is just casted to a char array and saved as an
     * underlying std::string_view.
     *
     * \warning The \p sv data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object, and any copies of the Exception object! If you cannot provide this, use the copying string_view
     * constructor! If this is not done, the behavior is undefined!
     *
     * \param sv A std::u8string_view to the data returned by info() and what().
     */
    explicit Exception(std::u8string_view sv) noexcept;

    /*!
     * The non-copying `const char8_t` Exception constructor.
     *
     * This constructor accepts a constant char8_t string as an argument, then simply wraps that string in a StringView
     * and passes it to the non-copying StringView constructor. As such, this constructor is effectively identical to
     * the non-copying string_view constructor, and for all intents and purposes, may be treated as that constructor in
     * all other relevant documentation. This constructor fixes the ambiguity between the non-copying StringView
     * constructor and the String rvalue reference constructor when using a `const char8_t` array instead.
     *
     * This is mostly useful for passing in a constant string, like the examples below:
     * ~~~
     * Exception ex(u8"The argument passed was invalid.");
     * ~~~
     *
     * The C++ standard requires that the above strings are null-terminated. The `sv` operator can potentially be made
     * slightly more efficient by a compiler as the compiler can save the StringView as a constant value and does not
     * need to compute the length at runtime.
     *
     * This constructor works identically with the normal non-copying `const char` constructor, as all strings in KirHut
     * software are assumed to be UTF-8. As such, the underlying data is just casted to a char array and saved as an
     * underlying std::string_view.
     *
     * \warning The \p ptr data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object, and any copies of the Exception object! If you cannot provide this, use the copying StringView
     * constructor! If this is not done, the behavior is undefined!
     *
     * \param ptr A constant char8_t pointer to the data that will be returned by info() and what().
     */
    explicit Exception(char8_t const *ptr) noexcept;

    /*!
     * The copying std::u8string_view Exception constructor.
     *
     * Generally speaking, unless you pass true to \p copy, this constructor should not be used. You should use the
     * noexcept constructor, however this constructor does support being called with false in the case that you need to
     * choose between creating a copy or not.
     *
     * This constructor requires that the string pointed at by \p sv is null-terminated if \p copy is false as the
     * pointer returned by what() is simply the data pointer of the \p sv std::u8string_view in that case. Use the
     * \p copy boolean to indicate if this Exception object should copy the passed string_view, which is a requirement
     * if you cannot determine if \p sv is null-terminated.
     *
     * \note This constructor is guaranteed not to throw an exception if \p copy is false.
     *
     * This constructor works identically with the normal copying string_view constructor, as all strings in KirHut
     * software are assumed to be UTF-8. As such, the std::u8string_view data is just copied directly to the underlying
     * std::string object.
     *
     * \warning The \p sv data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object,and any copies of the Exception object, if \p copy is false! If you cannot provide this, set \p copy to
     * true! If this is not done, the behavior is undefined!
     *
     * \param sv A string_view to the data returned by info() and what().
     * \param copy A boolean indicating if this Exception object should make an underlying deep copy. Default is false.
     * \throws bad_alloc May be thrown if \p copy is true and allocating the underlying memory failed.
     */
    Exception(std::u8string_view sv, bool copy);
#endif // __cpp_char8_t

    /*!
     * The string Exception constructor.
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
     * \note There is no std::u8string constructor because the underlying data structure maintaining the message this
     * Exception holds is a std::string, which would require copying the std::u8string data to the std::string, which
     * would perform a dynamic allocation, rendering this function not noexcept. A non-noexcept version of this
     * constructor may be added in the future if it is necessary.
     *
     * \param str An rvalue to a string with the message this Exception will have.
     */
    explicit Exception(string &&str) noexcept;

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
     * string_view constructor, in which case they will both be valid and this is the equivalent of a copy.
     *
     * \param other The Exception object being moved from, which may be invalid afterward.
     */
    Exception(Exception &&other) noexcept;

    /*!
     * Exception destructor.
     *
     * This destructor will only deallocate dynamic memory if this is the last object that refers to an underlying
     * String object containing the info() string. If this Exception, or the Exception this was copied from, was created
     * using the non-copying string_view constructor, this destructor will never deallocate.
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
     * method returns a C++17 std::string_view as well as working without compromise in the case of an invalid
     * underlying string passed to the non-copying string_view constructor.
     *
     * \return A StringView of the string used to construct this Exception.
     */
    [[nodiscard]] virtual string_view info() const noexcept;

    /*!
     * Legacy override of the std::exception::what() method.
     *
     * This method should return the same string as returned by info(), except that this is a C style string. The string
     * should be null-terminated, but this object does not enforce this requirement when the Exception is constructed
     * using the non-copying string_view constructor, so this cannot be completely relied upon. If you are creating an
     * Exception object that is likely to be read from what(), ensure the string_view passed has a null-terminating
     * character or prefer the copying string_view or string constructors.
     *
     * \return The same string as info().
     */
    virtual char const *what() const noexcept override;

private:
    struct Impl;

    // This data is marked mutable because the implementation needs to do all sorts of backend tricks to prevent
    // allocations when making and destroying potentially const copies of Exception.
    string mutable data;
    string_view infoView;

    // These are used to define an absolute order of copies of Exception objects. This is only used when the String
    // constructor is called to create the base Exception object, then all copies of the Exception place themselves in
    // a doubly linked list to determine the "owner" of the string.
    Exception mutable const *prev, *next;
};

} // namespace KirHut
