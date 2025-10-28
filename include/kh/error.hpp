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
 * and this is much more frequently the case when an unrecoverable error has occurred. C++ exceptions are great for
 * two things, and one of them is emitting when an unrecoverable error has occurred and providing a detialed message of
 * what that problem was.
 *
 * The kh/errors.hpp header has a set of default Error types that can be used, and are used throughout the library.
 * Please consult the documentation on that header for more information regarding these types. This header is used for
 * creating custom Error types, or more specifically, for using an Error that is not one of the default Errors in the
 * errors.hpp header, or when the WhyInvalid tag is insufficient to distinguish this type of Error from the parent type.
 * Simply put, this is pretty unlikely, but still possible. You may also need to override the why(), info() or what()
 * methods in a subclass.
 *
 * For most cases, when there just isn't a default Error typedef for the WhyInvalid you want, you just do this:
 *
 * ~~~
 * using TemporaryCrashingError = KirHut::Error<KirHut::WhyInvalid::TemporaryError>;
 * ~~~
 *
 * This is the standard method of using KirHut Errors, as it is a very simple syntax that allows using the C++ type
 * system to catch specific Errors and also the more general GenericError object.
 *
 * Errors should be used sparingly and only in conditions that should cause the application to exit. The main() method
 * of an application should attempt to catch all KirHut::Errors, and log accordingly. The GenericError type is provided
 * to make this easy to do, while also allowing separate catch clauses for more specific Error types.
 */

#include "kh/invalid.hpp"

#include <cassert>

#include "kh/priv/format.hpp"

namespace KirHut
{

template <WhyInvalid why>
struct Error;

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

    explicit inline ErrorState(string_view in) : info(in)
    {
        // No further implementation.
    }

    explicit inline ErrorState(char const *in) : info(in)
    {
        // No further implementation.
    }

    explicit constexpr ErrorState(string &&in) noexcept : info(std::move(in))
    {
        // No further implementation.
    }

    constexpr bool operator==(ErrorState const &other) const noexcept = default;

    constexpr WhyInvalid getWhyInvalid() const noexcept
    {
        return why;
    }

    constexpr string_view getInvalidInfo() const noexcept
    {
        return info;
    }
};

template <typename T>
constexpr bool isError = false;

template <WhyInvalid why>
constexpr bool isError<::KirHut::Error<why>> = true;

} // namespace Detail

/*!
 * A Generic "Error" type that all Error class objects subclass from.
 *
 * This allows access to the why(), info() and what() methods of Error objects using a generic parent type if you do not
 * know what Error has been thrown. The Invalid type itself intentionally lacks a vtable, and virtual table function
 * calls are far more useful with thrown exceptions than with returned Invalid types anyway. As such, this parent type
 * is included separately for Errors so that Error objects can still benefit from virtual dispatch while not polluting
 * the entire Invalid tree with it.
 *
 * This class is **not** intended to be subclassed by types other than the Error type. This is enforced with the
 * constructor that must be initialized with a pointer to the subclass itself. The type of the subclass will be checked
 * to ensure that it is an instance of the Error class.
 */
struct GenericError
{
    /*!
     * Standard virtual destructor for objects that support destruction from the parent type.
     */
    virtual ~GenericError() = default;

    /*!
     * Returns the value of the underlying Error type.
     * \return
     */
    virtual WhyInvalid why() const noexcept = 0;

    /*!
     * \brief info
     * \return
     */
    virtual string_view info() const noexcept = 0;

    /*!
     * \brief what
     * \return
     */
    virtual char const *what() const noexcept = 0;

protected:
    template <typename T>
    constexpr GenericError([[maybe_unused]] T *child)
    {
        static_assert(Detail::isError<T>);
    }

    GenericError() = delete;
};

/*!
 * The Error class is similar to std::exception except that prefers the use of C++17 std::string_view instead of
 * `const char *`.
 *
 * This class is primarily meant to be used as a "throwable Invalid" type that is intentionally meant to signal that the
 * application should likely close as a result of this throw. The other major purpose of C++ exceptions is met with the
 * FlowEnder class (not yet implemented),
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
struct Error : public TaggedInvalid<reason, Detail::ErrorState<reason>>, public GenericError
{
    /*!
     * The copying string_view Error constructor.
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
    inline explicit Error(string_view info = string_view()) : Parent(info), GenericError(this)
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
    inline Error(string &&info) noexcept : Parent(std::move(info)), GenericError(this)
    {
        // No further implementation.
    }

    /*!
     * \brief Error
     * \param str
     * \param firstArg
     * \param args
     */
    template <typename Arg_T, typename... Arg_Ts>
    inline Error(FMT::format_string<Arg_T, Arg_Ts...> str, Arg_T firstArg, Arg_Ts... args) :
        Error(Flags::runtime, str.get(), FMT::make_format_args(firstArg, args...))
    {
        // No further implementation.
    }

    /*!
     * \brief Error
     * \param rt
     * \param format
     * \param args
     */
    inline Error([[maybe_unused]] RuntimeFlag rt, FMT::string_view format, FMT::format_args const &args) :
        Parent(FMT::vformat(format, args)),
        GenericError(this)
    {
        // No further implementation.
    }

    /*!
     * \brief why
     * \return
     */
    inline WhyInvalid why() const noexcept override
    {
        return Parent::why();
    }

    /*!
     * \brief info
     * \return
     */
    inline string_view info() const noexcept override
    {
        return Parent::info();
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
    inline char const *what() const noexcept override
    {
        return Parent::info().data();
    }

private:
    using Parent = TaggedInvalid<reason, Detail::ErrorState<reason>>;
};

} // namespace KirHut
