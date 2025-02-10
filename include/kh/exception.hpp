/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/exception.hpp
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
 * \file kh/exception.hpp
 *
 * Exception Header file for the Exception type definition.
 *
 * Many parts of KirHut software do not inherently use exceptions, so it isn't advantageous to always include the
 * Exception object in all KirHut sources, which adding Exception to the base.hpp header would do. As such, this object
 * has been separated from te base.hpp file to ensure that you are not required to include KirHut::Exception in an
 * object file.
 *
 * This file also includes several different default Exception objects. They are almost universally implemented like
 * this:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * struct KH_EXPORT SomeExceptionName : public Exception
 * {
 *     using Exception::Exception;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * This is the standard method of using KirHut exceptions, as it is a very simple syntax that allows using the C++ type
 * system to catch specific exceptions and also the more general KirHut::Exception object.
 *
 * Exeptions should be used sparingly and only in conditions that should cause the application to exit. These should be
 * used as a substitute for asserts (and one of these exceptions is thrown by KH_ASSERT), and thrown when the
 * application has been provided conditions it can no longer run under.
 */

#include "kh/base.hpp"

#include <exception>
#include <cassert>

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
 * struct MyException : public Exception
 * {
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
    struct StringState
    {
        string_view str;
        WhyInvalid why;
    };

    struct RootState
    {
        Invalid inv;
        Exception const *next;
    };

    struct CopyState
    {
        Exception const *prev, *next;
    };

    typedef Var<StringState, RootState, CopyState> Data;
    mutable Data data;

public:
    /*!
     * The non-copying string_view Exception constructor.
     *
     * This constructor requires that the string pointed at by \p info is null-terminated as the pointer returned by
     * what() is simply the data pointer of the \p info std::string_view. Use the copying string_view constructor if
     * this Exception object should copy the passed string_view, which is a requirement if you cannot determine if \p
     * info is null-terminated. This constructor is separated from the copying string_view constructor so that it may be
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
     * The WhyInvalid reason returned by this Exception will be WhyInvalid::UnknownReason. By default, the info string
     * is empty. This is not a very good default and should be changed!
     *
     * \warning The \p info data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object, and any copies of the Exception object! If you cannot provide this, use the copying string_view
     * constructor! If this is not done, the behavior is undefined!
     *
     * \param info A std::string_view to the data returned by info() and what().
     */
    explicit Exception(string_view info = string_view()) noexcept;

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
     * The WhyInvalid reason returned by this Exception will be WhyInvalid::UnknownReason.
     *
     * \warning The \p info data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object, and any copies of the Exception object! If you cannot provide this, use the copying StringView
     * constructor! If this is not done, the behavior is undefined!
     *
     * \param info A constant char pointer to the data that will be returned by info() and what().
     */
    explicit Exception(char const *info) noexcept;

    /*!
     * The copying StringView Exception constructor.
     *
     * Generally speaking, unless you pass true to \p copy, this constructor should not be used. You should use the
     * noexcept constructor, however this constructor does support being called with false in the case that you need to
     * choose between creating a copy or not.
     *
     * This constructor requires that the string pointed at by \p info is null-terminated if \p copy is false as the
     * pointer returned by what() is simply the data pointer of the \p info std::string_view in that case. Use the \p
     * copy boolean to indicate if this Exception object should copy the passed string_view, which is a requirement if
     * you cannot determine if \p info is null-terminated.
     *
     * The WhyInvalid reason returned by this Exception will be WhyInvalid::UnknownReason.
     *
     * \param flag A MakeFlag type flag that indicates this Exception should copy the input string. Usually just "make".
     * \param info A string_view to the data returned by info() and what().
     * \throws bad_alloc May be thrown if \p copy is true and allocating the underlying memory failed.
     */
    Exception(MakeFlag const &flag, string_view info);

    /*!
     * The non-copying string_view Exception constructor with WhyInvalid.
     *
     * This constructor requires that the string pointed at by \p info is null-terminated as the pointer returned by
     * what() is simply the data pointer of the \p info std::string_view. Use the copying string_view constructor if
     * this Exception object should copy the passed string_view, which is a requirement if you cannot determine if \p
     * info is null-terminated. This constructor is separated from the copying string_view constructor so that it may be
     * marked as `noexcept`.
     *
     * This is mostly useful for passing in a constant string, like the examples below:
     * ~~~
     * Exception ex4(WhyInvalid::CorruptData, "The data was in an invalid format."sv);
     * ~~~
     * The C++ standard requires that the above strings are null-terminated, including the one that is wrapped by the
     * `sv` operator. The `sv` operator can be made slightly more efficient by a compiler as the compiler can save the
     * string_view as a constant value and does not need to compute the length at runtime.
     *
     * By default, the info string is empty. This is not a very good default and should be changed!
     *
     * \param why A WhyInvalid enumeration of what actually caused the Exception.
     * \param info A message to log into the error log, or to display to the user after translation.
     */
    Exception(WhyInvalid why, string_view info = string_view()) noexcept;

    /*!
     * The non-copying `const char` Exception constructor with WhyInvalid.
     *
     * This constructor accepts a constant char array as an argument, then simply wraps that string in a string_view
     * and passes it to the non-copying string_view constructor. As such, this constructor is effectively identical to
     * the non-copying string_view constructor, and for all intents and purposes, may be treated as that constructor in
     * all other relevant documentation. This constructor fixes the ambiguity between the non-copying string_view
     * constructor and the String rvalue reference constructor when using a `const char` array as input.
     *
     * This is mostly useful for passing in a constant string, like the examples below:
     * ~~~
     * Exception ex3(WhyInvalid::BadArgument, "The argument passed was invalid.");
     * ~~~
     *
     * The C++ standard requires that the above strings are null-terminated. The `sv` operator can potentially be made
     * slightly more efficient by a compiler as the compiler can save the string_view as a constant value and does not
     * need to compute the length at runtime.
     *
     * \param why A WhyInvalid enumeration of what actually caused the Exception.
     * \param info A message to log into the error log, or to display to the user after translation.
     */
    Exception(WhyInvalid why, char const *info) noexcept;

    /*!
     * The copying StringView Exception constructor with WhyInvalid.
     *
     * Generally speaking, unless you pass true to \p copy, this constructor should not be used. You should use the
     * noexcept constructor, however this constructor does support being called with false in the case that you need to
     * choose between creating a copy or not.
     *
     * This constructor requires that the string pointed at by \p info is null-terminated if \p copy is false as the
     * pointer returned by what() is simply the data pointer of the \p info std::string_view in that case. Use the \p
     * copy boolean to indicate if this Exception object should copy the passed string_view, which is a requirement if
     * you cannot determine if \p info is null-terminated.
     *
     * \param flag A MakeFlag type flag that indicates this Exception should copy the input string. Usually just "make".
     * \param why A WhyInvalid enumeration of what actually caused the Exception.
     * \param info A message to log into the error log, or to display to the user after translation.
     * \throws bad_alloc May be thrown if \p copy is true and allocating the underlying memory failed.
     */
    Exception(MakeFlag const &flag, WhyInvalid why, string_view info);

#if KH_PRIV_DOCS || __cpp_char8_t
    /*!
     * The non-copying u8string_view Exception constructor.
     *
     * This constructor requires that the string pointed at by \p info is null-terminated as the pointer returned by
     * what() is simply the data pointer of the \p info std::u8string_view. Use the copying u8string_view constructor if
     * this Exception object should copy the passed u8string_view, which is a requirement if you cannot determine if
     * \p info is null-terminated. This constructor is separated from the copying u8string_view constructor so that it
     * may be marked as `noexcept`.
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
     * The WhyInvalid reason returned by this Exception will be WhyInvalid::UnknownReason.
     *
     * \warning The \p info data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object, and any copies of the Exception object! If you cannot provide this, use the copying string_view
     * constructor! If this is not done, the behavior is undefined!
     *
     * \param info A std::u8string_view to the data returned by info() and what().
     */
    explicit Exception(std::u8string_view info) noexcept;

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
     * The WhyInvalid reason returned by this Exception will be WhyInvalid::UnknownReason.
     *
     * \warning The \p info data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object, and any copies of the Exception object! If you cannot provide this, use the copying StringView
     * constructor! If this is not done, the behavior is undefined!
     *
     * \param info A constant char8_t pointer to the data that will be returned by info() and what().
     */
    explicit Exception(char8_t const *info) noexcept;

    /*!
     * The copying std::u8string_view Exception constructor.
     *
     * Generally speaking, unless you pass true to \p copy, this constructor should not be used. You should use the
     * noexcept constructor, however this constructor does support being called with false in the case that you need to
     * choose between creating a copy or not.
     *
     * This constructor requires that the string pointed at by \p info is null-terminated if \p copy is false as the
     * pointer returned by what() is simply the data pointer of the \p info std::u8string_view in that case. Use the
     * \p copy boolean to indicate if this Exception object should copy the passed string_view, which is a requirement
     * if you cannot determine if \p info is null-terminated.
     *
     * This constructor works identically with the normal copying string_view constructor, as all strings in KirHut
     * software are assumed to be UTF-8. As such, the std::u8string_view data is just copied directly to the underlying
     * std::string object.
     *
     * The WhyInvalid reason returned by this Exception will be WhyInvalid::UnknownReason.
     *
     * \param flag A MakeFlag type flag that indicates this Exception should copy the input string. Usually just "make".
     * \param info A string_view to the data returned by info() and what().
     * \throws bad_alloc May be thrown if \p copy is true and allocating the underlying memory failed.
     */
    Exception(MakeFlag const &flag, std::u8string_view info);

    /*!
     * The non-copying u8string_view Exception constructor with WhyInvalid.
     *
     * This constructor requires that the string pointed at by \p info is null-terminated as the pointer returned by
     * what() is simply the data pointer of the \p info std::u8string_view. Use the copying u8string_view constructor if
     * this Exception object should copy the passed u8string_view, which is a requirement if you cannot determine if
     * \p info is null-terminated. This constructor is separated from the copying u8string_view constructor so that it
     * may be marked as `noexcept`.
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
     * The WhyInvalid reason returned by this Exception will be WhyInvalid::UnknownReason.
     *
     * \warning The \p info data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object, and any copies of the Exception object! If you cannot provide this, use the copying string_view
     * constructor! If this is not done, the behavior is undefined!
     *
     * \param why A WhyInvalid enumeration of what actually caused the Exception.
     * \param info A std::u8string_view to the data returned by info() and what().
     */
    Exception(WhyInvalid why, std::u8string_view info) noexcept;

    /*!
     * The non-copying `const char8_t` Exception constructor with WhyInvalid.
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
     * The WhyInvalid reason returned by this Exception will be WhyInvalid::UnknownReason.
     *
     * \warning The \p info data block MUST not be deallocated or modified for the entire lifetime of the Exception
     * object, and any copies of the Exception object! If you cannot provide this, use the copying StringView
     * constructor! If this is not done, the behavior is undefined!
     *
     * \param why A WhyInvalid enumeration of what actually caused the Exception.
     * \param info A constant char8_t pointer to the data that will be returned by info() and what().
     */
    explicit Exception(WhyInvalid why, char8_t const *info) noexcept;

    /*!
     * The copying std::u8string_view Exception constructor with WhyInvalid.
     *
     * Generally speaking, unless you pass true to \p copy, this constructor should not be used. You should use the
     * noexcept constructor, however this constructor does support being called with false in the case that you need to
     * choose between creating a copy or not.
     *
     * This constructor requires that the string pointed at by \p info is null-terminated if \p copy is false as the
     * pointer returned by what() is simply the data pointer of the \p info std::u8string_view in that case. Use the
     * \p copy boolean to indicate if this Exception object should copy the passed string_view, which is a requirement
     * if you cannot determine if \p info is null-terminated.
     *
     * This constructor works identically with the normal copying string_view constructor, as all strings in KirHut
     * software are assumed to be UTF-8. As such, the std::u8string_view data is just copied directly to the underlying
     * std::string object.
     *
     * The WhyInvalid reason returned by this Exception will be WhyInvalid::UnknownReason.
     *
     * \param flag A MakeFlag type flag that indicates this Exception should copy the input string. Usually just "make".
     * \param why A WhyInvalid enumeration of what actually caused the Exception.
     * \param info A string_view to the data returned by info() and what().
     * \throws bad_alloc May be thrown if \p copy is true and allocating the underlying memory failed.
     */
    Exception(MakeFlag const &flag, WhyInvalid why, std::u8string_view info);
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
     * \param info An rvalue to a string with the message this Exception will have.
     */
    explicit Exception(string &&info) noexcept;

    /*!
     * The string Exception constructor with a WhyInvalid.
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
     * \param why A WhyInvalid enumeration of what actually caused the Invalid return value.
     * \param message A message to log into the error log, or to display to the user after translation.
     */
    Exception(WhyInvalid why, string &&info) noexcept;

    /*!
     * The Invalid Exception constructor.
     *
     * This constructor is most useful when you are attempting to handle a non-throwing error that returns an Invalid,
     * but you have no method to address this other than closing the application. In that case, throwing an appropriate
     * Exception subtype with the returned Invalid is efficient and extremely useful, as the info() method will return
     * the same string returned by Invalid::info(), and you can provide a custom WhyInvalid reason.
     *
     * \param invalid An Invalid to take ownership of in this Exception.
     */
    explicit Exception(Invalid &&invalid) noexcept;

    /*!
     * The copying Invalid Exception constructor.
     *
     * This constructor is most useful when you are attempting to handle a non-throwing error that returns an Invalid,
     * but you have no method to address this other than closing the application. In that case, throwing an appropriate
     * Exception subtype with the returned Invalid is efficient and extremely useful, as the info() method will return
     * the same string returned by Invalid::info(), and you can provide a custom WhyInvalid reason.
     *
     * This version of the Exception constructor copies the passed-in Invalid rather than takes ownership of it. As
     * such, this may throw std::bad_alloc.
     *
     * \param invalid An Invalid object to copy in this Exception.
     * \throws std::bad_alloc If there is a failure to allocate memory copying \p invalid.
     */
    explicit Exception(Invalid const &invalid);

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
    virtual ~Exception() noexcept;

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
     * Get the Exceptions underlying reason as a WhyInvalid.
     *
     * The Exception object is implemented with an Invalid underneath, which has the message returned by info() and
     * what() stored in it. This method returns the result of calling Invalid::why() on that underlying object.
     *
     * \return A reason for this Exception object being thrown, or UnknownReason if one was not provided.
     */
    [[nodiscard]] virtual WhyInvalid why() const noexcept;

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
};

/*!
 * Adds the Exception object as a type that can be passed to KH_ASSERT.
 *
 * This allows you to select an Exception object that is not KirHutSucksAtProgramming as the exception thrown by a
 * KH_ASSERT call in a Release build. Unlike the standard C assert macro that compiles to nothing when compiled in
 * release mode, this function is just a function that is always called and the condition is always executed. As such,
 * this is not vulnerable to the kinds of bugs that expect the condition to execute as part of its functionality, as it
 * will always execute, however as a disadvantage, this method will not be compiled out of an executable.
 *
 * In Debug mode, this method just calls assert(condition), which allows it to have the functionality of an assert in
 * your IDE. In release mode, this checks if condition is false, and if it is, it throws a KirHutSucksAtProgramming
 * Exception with the given \p ex that is passed in as an argument. If this method does not throw, the Exception passed
 * in as an argument will not be modified.
 *
 * This is implemented as a template so that it throws the most derived type instead of just a plain std::exception
 * object.
 *
 * \param condition Usually an expression that should evaluate to true. If it is false, this calls assert or throws.
 * \param ex An alternative Exception object to throw from this method.
 * \throws ExceptType If the \p condition is false and KirHut::Build::release is true.
 */
template <std::derived_from<Exception> ExceptType>
void khAssert(bool condition, std::convertible_to<ExceptType> auto &&ex)
{
    if constexpr (Build::release)
    {
        if (!condition)
        {
            throw ExceptType{ forward<std::remove_reference_t<decltype(ex)>>(ex) };
        }
    }

    assert(condition);
}

/*!
 * Throw an Invalid object as an Exception of any given type.
 *
 * This basically allows calling the explicit Exception::Exception(Invalid&&) constructor without wrapping and using a
 * throw expression. It is basically a simple way to save some boilerplate as it is common to convert an Invalid into an
 * Exception.
 *
 * \param invalid A reference to the Invalid type to throw from. Guaranteed to move construct if an rvalue.
 */
template <std::derived_from<Exception> ExceptType>
[[noreturn]] void throwInvalid(std::same_as<Invalid> auto &&invalid)
{
    throw ExceptType{ forward<Invalid>(invalid) };
}

} // namespace KirHut
