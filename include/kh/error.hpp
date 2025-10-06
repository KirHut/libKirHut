/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/error.hpp
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

/*!
 * \file kh/error.hpp
 *
 * Header file for the Error type and its according methods and helper objects.
 *
 * KirHut software uses an universal invalid type that is used to identify any problem that may have occurred in
 * processing some kind of request. The type is deliberately meant to be lightweight so it can be returned from a method
 * or function without worrying about copying too much data, as an Invalid object is as small as 4 bytes on most
 * platforms. However, there are conditions where a lightweight type is insufficient for describing what has happened,
 * and this is almost always the case when an unrecoverable error has occurred.
 *
 * The kh/errors.hpp header has a set of default Error types that can be used, and are used throughout the library.
 * Please consult the documentation on that header for more information regarding these types.
 *
 * If you wish to throw an Error
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

#include "kh/invalid.hpp"

#include <cassert>

namespace KirHut
{

namespace Detail
{

/*!
 * \internal
 *
 * \brief The ErrorState class
 */
template <WhyInvalid why>
struct ErrorState final
{
    using CharType = char;

    string info;

    explicit ErrorState(string_view in) : info(in)
    {
        // No further implementation.
    }

    explicit ErrorState(char const *in) : info(in)
    {
        // No further implementation.
    }

    explicit ErrorState(string &&in) : info(std::move(in))
    {
        // No further implementation.
    }

    constexpr WhyInvalid getWhyInvalid() const noexcept
    {
        return why;
    }

    constexpr string_view getInvalidInfo() const noexcept
    {
        return info;
    }
};

} // namespace Detail

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
 *
 * ~~~
 * struct MyException : public Exception
 * {
 *     using Exception::Exception;
 * }
 * ~~~
 *
 * This ensures that you have all of the constructors that Exception has, don't have to provide any additional
 * implementation whatsoever, and can take advantage of C++'s type system for catch statements.
 *
 * This class prefers using info() to get the user returned exception info, and this method will always return the
 * complete string used to construct this class from it. The what() method is considered a legacy method that should
 * only be used by catch() blocks that catch std::exception instead of Exception. The what() method will return the
 * same UTF-8 string as info(), just as a `char` array instead of a std::string_view.
 */
template <WhyInvalid reason>
struct Error : public TaggedInvalid<reason, Detail::ErrorState<reason>>
{
    /*!
     * The non-copying string_view Exception constructor.
     *
     * This constructor requires that the string pointed at by \p info is null-terminated as the pointer returned by
     * what() is simply the data pointer of the \p info std::string_view. Use the copying string_view constructor if
     * this Exception object should copy the passed string_view, which is a requirement if you cannot determine if \p
     * info is null-terminated. This constructor is separated from the copying string_view constructor so that it may be
     * marked as `noexcept`.
     *
     * This is mostly useful for passing in a constant string, like the example below:
     *
     * ~~~
     * Exception ex2("The function return value was invalid."sv);
     * ~~~
     *
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
    inline explicit Error(string_view info = string_view()) : Parent(info)
    {
        // No further implementation.
    }

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
    inline explicit Error(char const *info) : Error(string_view{ info })
    {
        // No further implementation.
    }

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
     * \param info A string_view to the data returned by info() and what().
     * \throws bad_alloc May be thrown if \p copy is true and allocating the underlying memory failed.
     */
    inline Error(string &&info) noexcept : Parent(std::move(info))
    {
        // No further implementation.
    }

    /*!
     * Legacy support for the std::exception::what() method.
     *
     * This method should return the same string as returned by info(), except that this is a C style string. The string
     * may be null-terminated, but this object does not enforce this requirement when the Error is constructed, so this
     * cannot be completely relied upon. If you are creating an Error object that is likely to be read from what(),
     * ensure the string passed has a null-terminating character or prefer the copying string_view constructor.
     *
     * \return The same string as info().
     */
    inline char const *what() const noexcept
    {
        return this->info().data();
    }

private:
    using Parent = TaggedInvalid<reason, Detail::ErrorState<reason>>;
};

} // namespace KirHut
