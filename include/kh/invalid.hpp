/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/invalid.hpp
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

#include "kh/base.hpp"

/*!
 * \file invalid.hpp "kh/invalid.hpp"
 *
 * Invalid template object and MaybeInv object header file. This header contains all of the baseline information for
 * error handling throughout all KirHut applications.
 */

namespace KirHut
{

/*!
 * Dumb data object containing a view to a static message and a WhyInvalid.
 *
 * This can be returned by some functions if a WhyInvalid fails to provide an adequate info string for the issue at hand
 * but there is still no need to perform dynamic memory allocation.
 */
template <typename Char_T>
struct MessageViewWhy
{
    /*!
     * A type alias for this object's character type.
     *
     * This allows access to the CharType through an alias of this type, such as a template argument or if the
     * MessageViewWhy is itself aliased.
     */
    using CharType = Char_T;

    /*!
     * Constant WhyInvalid data.
     *
     * This must be defined at construction, and cannot be changed.
     */
    WhyInvalid const why;

    /*!
     * Constant info view of a string.
     *
     * This must be defined at construction, and cannot be changed.
     */
    std::basic_string_view<CharType> const info;

    /*!
     * WhyTypeTraits accessible bool indicating that this type can be quick copied.
     */
    constexpr static bool isQuickCopy = true;

    /*!
     * \brief operator <=>
     * \param other
     */
    constexpr auto operator<=>(MessageViewWhy const &other) const noexcept = default;

    /*!
     * Returns this object's WhyInvalid type.
     *
     * Just returns why.
     *
     * \return The value of why.
     */
    constexpr WhyInvalid getWhyInvalid() const noexcept
    {
        return why;
    }

    /*!
     * Returns this object's info value.
     *
     * Just returns info.
     *
     * \return The value of info.
     */
    constexpr std::basic_string_view<CharType> getInvalidInfo() const noexcept
    {
        return info;
    }
};

/*!
 * Concept that an object type can model to meet the requirements of the ValidWhyType concept.
 *
 * Any object type that meets the WhyObject concept will automatically be a ValidWhyType, even if no WhyTypeTraits
 * specialization for the type is defined. This is because any object which models this concept is clearly intending to
 * be a ValidWhyType, as this is extremely unlikely to be done inadvertently.
 */
template <typename Why_T>
concept WhyObject = requires(Why_T const &why) {
    typename Why_T::CharType;
    { why.getWhyInvalid() } noexcept -> std::same_as<WhyInvalid>;
    { why.getInvalidInfo() } noexcept -> std::same_as<std::basic_string_view<typename Why_T::CharType>>;
};

namespace Detail
{

/*!
 * \internal
 *
 * Actual function implementation of the KirHut::getWhyInvalid WhyInvalid overload.
 *
 * \see KirHut::getWhyInvalid
 * \param why Same time that is returned by this function.
 * \return The same \p why passed as an argument.
 */
constexpr WhyInvalid getWhyInvalid(WhyInvalid why) noexcept
{
    return why;
}

/*!
 * \internal
 *
 * \brief getInvalidInfo
 * \param why
 * \return
 */
constexpr string_view getInvalidInfo(WhyInvalid why) noexcept
{
    using enum WhyInvalid;
    switch (why)
    {
        // clang-format off
    case Success:              return "The operation was successful."sv;
    case IncorrectInput:       return "The input provided was incorrect."sv;
    case FileNotFound:         return "A file or folder specified was not found."sv;
    case TooManyOpenFiles:     return "There are already too many open files."sv;
    case CouldntOpenFile:      return "A specified file could not be opened."sv;
    case InvalidHandle:        return "A handle to a file or resource is now invalid."sv;
    case ArenaTrashed:         return "An arena buffer has become invalid."sv;
    case OutOfBounds:          return "A request for data was made that was out of specified bounds."sv;
    case DataRemoved:          return "Expected data was removed before it should have been."sv;
    case BadEnvironment:       return "An environment variable or requirement was invalid or missing."sv;
    case PatternMismatch:      return "An expected pattern was not matched against."sv;
    case AlreadyInitialized:   return "An object or value being initialized was already initialized."sv;
    case OutOfMemory:          return "There was a failure to allocate memory."sv;
    case CurrentDirectory:     return "An operation cannot succeed because a selected directory is the current "
                                      "directory."sv;
    case BadCRCResult:         return "An essential cyclic redundancy check failed."sv;
    case HandleEndOfFile:      return "The end of a provided file was unexpectedly reached."sv;
    case DiskFullError:        return "The disk of a write destination is full."sv;
    case ConfigCmdInvalid:     return "A command configuration is invalid."sv;
    case IncorrectDataFormat:  return "Input provided is in an incorrect format."sv;
    case CannotOpenInput:      return "Input provided cannot be opened."sv;
    case UsernameUnknown:      return "Username provided is unknown."sv;
    case HostnameUnknown:      return "Hostname provided is unknown."sv;
    case ServiceUnavailable:   return "An expected system service was not available at time of request."sv;
    case SoftwareError:        return "A software programming error was encountered."sv;
    case OperatingSystemError: return "The operating system failed to provide an essential functionality."sv;
    case OSFileMissingError:   return "A critical operating system file was missing."sv;
    case CannotCreateFile:     return "A required file could not be created."sv;
    case InputOutputError:     return "An error was encountered during IO processing."sv;
    case TemporaryError:       return "A temporary error was encountered."sv;
    case ProtocolError:        return "An error was encountered with a protocol used in communication."sv;
    case PermissionDenied:     return "Permission to access a file, folder, or resource was denied."sv;
    case ConfigurationError:   return "An error was encountered with configuration."sv;
    case Unknown:              [[fallthrough]];
    default:                   return "There was an unknown error or failure."sv;
        // clang-format on
    }
}

/*!
 * \internal
 *
 * \brief getWhyInvalid
 * \param view
 * \return
 */
template <typename Char_T>
constexpr WhyInvalid getWhyInvalid([[maybe_unused]] std::basic_string_view<Char_T> view) noexcept
{
    return WhyInvalid::Unknown;
}

/*!
 * \internal
 *
 * \brief getInvalidInfo
 * \param view
 * \return
 */
template <typename Char_T>
constexpr std::basic_string_view<Char_T> getInvalidInfo(std::basic_string_view<Char_T> view) noexcept
{
    return view;
}

/*!
 * \internal
 *
 * \brief getWhyInvalid
 * \param str
 * \return
 */
template <typename Char_T>
constexpr WhyInvalid getWhyInvalid([[maybe_unused]] std::basic_string<Char_T> const &str) noexcept
{
    return WhyInvalid::Unknown;
}

/*!
 * \internal
 *
 * \brief getInvalidInfo
 * \param str
 * \return
 */
template <typename Char_T>
constexpr std::basic_string_view<Char_T> getInvalidInfo(std::basic_string<Char_T> const &str) noexcept
{
    return str;
}

/*!
 * \internal
 *
 * \brief getWhyInvalid
 * \param why
 * \return
 */
template <WhyObject Why_T>
constexpr WhyInvalid getWhyInvalid(Why_T const &why) noexcept
{
    return why.getWhyInvalid();
}

/*!
 * \internal
 *
 * \brief getInvalidInfo
 * \param why
 */
template <WhyObject Why_T>
constexpr auto getInvalidInfo(Why_T const &why) noexcept -> decltype(why.getInvalidInfo())
{
    return why.getInvalidInfo();
}

/*!
 * \internal
 *
 * \brief The GetWhyInvalidImpl class
 */
struct GetWhyInvalidImpl
{
    /*!
     * \internal
     *
     * \brief operator ()
     * \param why
     * \return
     */
    template <typename Why_T>
    constexpr WhyInvalid operator()(Why_T const &why) const noexcept
    {
        return getWhyInvalid(why);
    }
};

/*!
 * \internal
 *
 * \brief The GetInvalidInfoImpl class
 */
struct GetInvalidInfoImpl
{
    /*!
     * \internal
     *
     * \brief operator ()
     * \param why
     */
    template <typename Why_T>
    constexpr auto operator()(Why_T const &why) const noexcept -> decltype(getInvalidInfo(why))
    {
        return getInvalidInfo(why);
    }
};

/*!
 * \internal
 *
 * \brief The QuickCopyExtractor class
 */
template <WhyObject Why_T>
struct QuickCopyExtractor
{
    constexpr static bool isQuickCopy = false;
};

/*!
 * \internal
 *
 * \brief The QuickCopyExtractor class
 */
template <WhyObject Why_T>
requires requires {
    { Why_T::isQuickCopy } -> std::convertible_to<bool>;
}
struct QuickCopyExtractor<Why_T>
{
    constexpr static bool isQuickCopy = Why_T::isQuickCopy;
};

} // namespace Detail

#if defined(KH_PRIV_DOCS)
/*!
 * \brief getWhyInvalid
 * \param why
 * \return
 */
constexpr WhyInvalid getWhyInvalid(WhyInvalid why) noexcept;

/*!
 * \brief getInvalidInfo
 * \param why
 * \return
 */
constexpr string_view getInvalidInfo(WhyInvalid why) noexcept;

/*!
 * \brief getWhyInvalid
 * \param view
 * \return
 */
template <typename Char_T>
constexpr WhyInvalid getWhyInvalid(std::basic_string_view<Char_T> view) noexcept;
#endif

namespace
{

/*!
 * \internal
 *
 * \brief getWhyInvalid
 */
constexpr auto const &getWhyInvalid  = Detail::staticConstRef<Detail::GetWhyInvalidImpl>;

/*!
 * \internal
 *
 * \brief getInvalidInfo
 */
constexpr auto const &getInvalidInfo = Detail::staticConstRef<Detail::GetInvalidInfoImpl>;

} // namespace

/*!
 * \brief The WhyTypeTraits class
 */
template <typename Why_T>
struct WhyTypeTraits
{
    using CharType                      = char;
    constexpr static bool isImplemented = false;
    constexpr static bool isQuickCopy   = false;
};

/*!
 * \brief The WhyTypeTraits class
 */
template <>
struct WhyTypeTraits<WhyInvalid>
{
    using CharType                      = char;
    constexpr static bool isImplemented = true;
    constexpr static bool isQuickCopy   = true;
};

/*!
 * \brief The WhyTypeTraits class
 */
template <typename Char_T>
struct WhyTypeTraits<std::basic_string_view<Char_T>>
{
    using CharType                      = Char_T;
    constexpr static bool isImplemented = true;
    constexpr static bool isQuickCopy   = true;
};

/*!
 * \brief The WhyTypeTraits class
 */
template <typename Char_T>
struct WhyTypeTraits<std::basic_string<Char_T>>
{
    using CharType                      = Char_T;
    constexpr static bool isImplemented = true;
    constexpr static bool isQuickCopy   = false;
};

/*!
 * \brief The WhyTypeTraits class
 */
template <WhyObject Why_T>
struct WhyTypeTraits<Why_T>
{
    using CharType                      = Why_T::CharType;
    constexpr static bool isImplemented = true;
    constexpr static bool isQuickCopy   = Detail::QuickCopyExtractor<Why_T>::isQuickCopy;
};

/*!
 *
 */
template <typename T>
concept ValidWhyType = not std::is_reference_v<T> and not std::is_function_v<T> and WhyTypeTraits<T>::isImplemented and
                       requires(std::add_const_t<T> &t) {
                           typename WhyTypeTraits<T>::CharType;
                           { KirHut::getWhyInvalid(t) } noexcept -> std::same_as<WhyInvalid>;
                           {
                               KirHut::getInvalidInfo(t)
                           } noexcept -> std::same_as<std::basic_string_view<typename WhyTypeTraits<T>::CharType>>;
                       };

/*!
 *
 */
template <typename T>
concept QuickWhyType = ValidWhyType<T> and std::is_nothrow_move_constructible_v<T> and
                       std::is_nothrow_copy_constructible_v<T> and WhyTypeTraits<T>::isQuickCopy;

/*!
 * Provides a simple interface to an error type object with a message and a why for the failure.
 *
 * This class is designed to be flexible enough to use any type as the "why" for possible subclasses that may have
 * more complicated internal representations, but for most use cases you should just use the Invalid typedef of this
 * class. The Invalid typedef uses the enum WhyInvalid to provide a method for programmers to know what happened and
 * distinguish it from other failures, and only provides a generic, untranslated message for each of the isssues.
 * Generally, you should use the Invalid typedef (or a TaggedInvalid<Why_T> subclass typedef) to signal to the user
 * application that something went wrong, and if the user application cannot resolve the issue on its own, it can
 * then translate the string to the user's language and report the issue. If you are always going to report, you
 * could simply use a key that is always translated after the Invalid is returned.
 *
 * BasicInvalid objects, and their according MaybeInv objects, are safe to pass along to different threads as copies as
 * in most cases they are completely independent from one another. Copies of BasicInvalid using a complex Why_T type are
 * also guaranteed to be noexcept and safe to pass to different threads. The BasicInvalid object does enough thread
 * coordination to prevent data races and deadlocks between threads sharing the same complex Why_T object. However, this
 * class does not prevent data races and is not thread safe at all between two separate threads attempting to use the
 * same BasicInvalid object.
 *
 * As a consequence of thread safety for complex Why_T types, BasicInvalids that do not contain a simple type are not
 * constexpr.
 *
 * The BasicInvalid object is intended to be either returned in a MaybeInv object or, alternatively, thrown as an
 * exception from a method. The class is intended to be flexible enough to be used in both contexts properly,
 * including having a non-throwing copy constructor and being lightweight as a return type. This object also allows
 * returning error conditions from Qt Signals and Slots, as exceptions are completely unsupported using Signals and
 * Slots.
 *
 * All of the methods of this class are marked as constexpr, so this class should be completely usable in a
 * constexpr context. This allows the creation of functions and methods marked constexpr that return a MaybeInv<T>
 * or Invalid and this would compile.
 *
 * \tparam Why_T
 */
template <ValidWhyType Why_T>
class BasicInvalid
{
    /*!
     * \internal
     *
     * Internal data shared pointer.
     *
     * This class uses a shared pointer to store the internal data, which requires a separately allocated block of
     * memory outside of the Why_T for reference counting and mutex. This ensures that copies of BasicInvalid will never
     * throw an exception and ensures that copy destruction never results in deadlocks, but requires that all of the
     * complex BasicInvalid constructors can potentially throw.
     */
    SPtr<Why_T> data;

    static_assert(not WhyTypeTraits<Why_T>::isQuickCopy,
                  "The BasicInvalid's Why_T type trait's isQuickCopy is true, but Why_T is not nothrow copy and move "
                  "constructible.");

public:
    /*!
     * Typedef for BasicInvalid types to identify the character type of the returned info() string.
     */
    using CharType = WhyTypeTraits<Why_T>::CharType;

    /*!
     * \brief BasicInvalid
     * \param args The arguments to pass to the Why_T constructor to build this BasicInvalid object around.
     * \throws std::bad_alloc
     */
    template <typename... Arg_Ts>
    inline explicit(sizeof...(Arg_Ts) < 2) BasicInvalid(Arg_Ts &&...args)
        KH_THROWS_BADALLOC_OR(std::is_nothrow_constructible_v<Why_T, Arg_Ts...>)
            requires(std::is_constructible_v<Why_T, Arg_Ts...>)
        : data(make_shared<Why_T>(std::forward<Arg_Ts>(args)...))
    {
        // No further implementation.
    }

    /*!
     * Construct a BasicInvalid with a mandatory \p why value by copy.
     *
     * The class requires the why value to be initialized at construction, and it depends on the type if the message
     * info() can be changed. If the message can be set, then you can use setInfo() to modify the info() message. Some
     * BasicInvalid objects only allow the info() to be set once and never again, so consult the documentation on each
     * individual type of BasicInvalid you are using to know how setInfo() will work on those types. If the
     * WhyTypeTraits<Why_T>::isModifiable flag is set to false, then the setInfo() methods are not available at all.
     *
     * This is the complex BasicInvalid constructor and the passed Why_T is being copied here. This requires a Why_T
     * that has a copy constructor, and this constructor may throw exceptions if the Why_T copy constructor can also do
     * so.
     *
     * \param why A Why_T state (usually a WhyInvalid) of what caused the BasicInvalid to be raised or returned.
     * \throws any exception that is thrown by the copy constructor of Why_T (WhyInvalid has none).
     */
    inline explicit BasicInvalid(Why_T const &why) KH_THROWS_BADALLOC_OR(std::is_nothrow_copy_constructible_v<Why_T>)
        requires(std::is_copy_constructible_v<Why_T>)
        : data(make_shared<Why_T>(why))
    {
        // No further implementation.
    }

    /*!
     * Constructor with or without a message and a mandatory \p why value.
     *
     * The class requires the why value to be initialized at construction, and it may not be changed after the object
     * has been created. If the message passed here is empty, then the message can be changed using setMessage(), but
     * once set it is static. See the documentation on the multiple versions of setMessage() for more info on adding a
     * message to BasicInvalid objects without one.
     *
     * \param why A Why_T state (usually a WhyInvalid) of what caused the BasicInvalid to be raised or returned.
     */
    inline explicit BasicInvalid(Why_T &&why) KH_THROWS_BADALLOC_OR(std::is_nothrow_move_constructible_v<Why_T>)
        requires(std::is_move_constructible_v<Why_T>)
        : data(make_shared<Why_T>(std::move(why)))
    {
        // No further implementation.
    }

    /*!
     * Equality operator for complex BasicInvalid types.
     *
     * Both the simple and complex BasicInvalid types work the same for equality operators: They just delegate to the
     * underlying Why_T object for comparison. If two Why_T objects contained in a BasicInvalid would return true for
     * `data == other.data`, than so will the wrapping BasicInvalid objects, even if those two Why_T objects are
     * different instances.
     *
     * Complex BasicInvalids have an additional "empty" state where they only compare equal to \p other empty
     * BasicInvalid objects. The only way to create an "empty" BasicInvalid is to create one with an instance of the
     * object in it, then move it to another BasicInvalid using the move constructor or assignment operator.
     *
     * \param other The other BasicInvalid you are comparing this one to.
     * \return Whether or not this BasicInvalid is equal to \p other.
     */
    inline bool operator==(BasicInvalid const &other) const noexcept
    {
        if (data and other.data)
        {
            return *data == *other.data;
        }

        return data == other.data;
    }

    /*!
     * Get the reason for this BasicInvalid object's creation.
     *
     * This value can only be changed when the Invalid object is constructed or reassigned from another object. A
     * constant enumeration of why this Invalid object was returned. You should use this in a switch/case statement or
     * to match against possible options in an if statement.
     *
     * \return A reason why this BasicInvalid object was created.
     */
    [[nodiscard]] inline WhyInvalid why() const noexcept
    {
        if (Why_T const *internal = whyData())
        {
            return getWhyInvalid(*internal);
        }

        return WhyInvalid::DataRemoved;
    }

    /*!
     * Get the info contained in this Invalid object.
     *
     * The string pointed to by the returned string_view will remain valid for the lifetime of the Invalid object, and
     * will be destroyed afterward. Do not use the returned string_view object from this method after the Invalid
     * object has been destroyed, or you will invoke Undefined Behavior. There is little that can be done to rectify
     * this in code without some kind of wrapper class like std::weak_ptr is.
     *
     * \return A string_view of the info in this Invalid object.
     */
    [[nodiscard]] inline std::basic_string_view<CharType> info() const noexcept
    {
        if (Why_T const *internal = whyData())
        {
            return getInvalidInfo(*internal);
        }

        return "";
    }

    /*!
     * Constant expression flag for BasicInvalid types to distinguish between simple BasicInvalids and complex ones.
     *
     * A "Simple" BasicInvalid is one that simply copies the underlying Why_T to every instance of BasicInvalid, which
     * ensures the validity of the data in all cases, including when a BasicInvalid object is moved from. A complex
     * BasicInvalid becomes invalid itself after being moved from, so you should not use a BasicInvalid after it has
     * been moved from at all.
     *
     * If you do, the complex BasicInvalid does have a predictable result: Calls to why() will always return
     * WhyInvalid::DataRemoved and calls to info() will always return an empty string_view.
     */
    constexpr static bool simple = false;

protected:
    /*!
     * Protected accessor method to allow subclasses to get data from the internal representation.
     *
     * This class deliberately does not have a virtual table in it, so in order to allow subclasses to have custom data
     * in the BasicInvalid type and still delete the data properly, the Why_T type may be a custom type that models the
     * ValidWhyType concept. Those subclasses may use this method to access their custom data. There is both const and
     * non-const overloads for this method available.
     *
     * \return The internal Why_T type stored by this BasicInvalid.
     */
    [[nodiscard]] inline Why_T *whyData() noexcept
    {
        return data.get();
    }

    /*!
     * \copydoc whyData()
     */
    [[nodiscard]] inline Why_T const *whyData() const noexcept
    {
        return data.get();
    }
};

/*!
 * \brief The BasicInvalid class
 */
template <QuickWhyType Why_T>
class BasicInvalid<Why_T>
{
    /*!
     * \internal
     *
     * \brief data
     */
    Why_T data;

public:
    /*!
     * Typedef for BasicInvalid types to identify the character type of the returned info() string.
     */
    using CharType = WhyTypeTraits<Why_T>::CharType;

    /*!
     * \brief BasicInvalid
     * \param args
     */
    template <typename... Arg_Ts>
    constexpr explicit BasicInvalid(Arg_Ts &&...args) noexcept(std::is_nothrow_constructible_v<Why_T, Arg_Ts...>)
        requires(std::is_constructible_v<Why_T, Arg_Ts...>)
        : data{ std::forward<Arg_Ts>(args)... }
    {
        // No further implementation.
    }

    /*!
     * Constructor with a mandatory \p why value that is copied.
     *
     * The class requires the why value to be initialized at construction, and it may not be changed after the object
     * has been created. The void specialization of BasicInvalid does not have a message and omits those constructors
     * and methods.
     *
     * \param why A Why_T state (usually a WhyInvalid) of what caused the BasicInvalid to be raised or returned.
     * \throws any exception that is thrown by the copy constructor of WHY (WhyInvalid has none).
     */
    constexpr explicit BasicInvalid(Why_T const &why) noexcept : data(why)
    {
        // No further implementation.
    }

    /*!
     * Constructor with a mandatory \p why value that is moved.
     *
     * The class requires the why value to be initialized at construction, and it may not be changed after the object
     * has been created. The void specialization of BasicInvalid does not have a message and omits those constructors
     * and methods.
     *
     * \param why A WHY state (usually a WhyInvalid) of what actually caused the BasicInvalid to be raised or returned.
     * \throws any exception that is thrown by the move constructor of WHY (WhyInvalid has none).
     */
    constexpr explicit BasicInvalid(Why_T &&why) noexcept : data(std::move(why))
    {
        // No further implementation.
    }

    /*!
     * Equality operator for complex BasicInvalid types.
     *
     * Both the simple and complex BasicInvalid types work the same for equality operators: They just delegate to the
     * underlying Why_T object for comparison. If two Why_T objects contained in a BasicInvalid would return true for
     * `data == other.data`, than so will the wrapping BasicInvalid objects, even if those two Why_T objects are
     * different instances.
     *
     * Complex BasicInvalids have an additional "empty" state where they only compare equal to \p other empty
     * BasicInvalid objects. The only way to create an "empty" BasicInvalid is to create one with an instance of the
     * object in it, then move it to another BasicInvalid using the move constructor or assignment operator.
     *
     * \param other The other BasicInvalid you are comparing this one to.
     * \return Whether or not this BasicInvalid is equal to \p other.
     */
    constexpr bool operator==(BasicInvalid const &other) const noexcept = default;

    /*!
     * Get the reason for this Invalid object's creation.
     *
     * This value can only be changed when the Invalid object is constructed or reassigned from another object. A
     * constant enumeration of why this Invalid object was returned. You should use this in a switch/case statement or
     * to match against possible options in an if statement.
     *
     * \return A reason why this Invalid object was created.
     */
    [[nodiscard]] constexpr WhyInvalid why() const noexcept
    {
        return getWhyInvalid(data);
    }

    /*!
     * \brief info
     * \return
     */
    [[nodiscard]] constexpr std::basic_string_view<CharType> info() const noexcept
    {
        return getInvalidInfo(data);
    }

protected:
    /*!
     * Get the reason for this Invalid object's creation.
     *
     * This value can only be changed when the Invalid object is constructed or reassigned from another object. A
     * constant enumeration of why this Invalid object was returned. You should use this in a switch/case statement or
     * to match against possible options in an if statement.
     *
     * \return A reason why this Invalid object was created.
     */
    [[nodiscard]] constexpr Why_T *whyData() noexcept
    {
        return &data;
    }

    /*!
     * \copydoc whyData()
     */
    [[nodiscard]] constexpr std::add_const_t<Why_T> *whyData() const noexcept
    {
        return &data;
    }
};

/*!
 *
 */
template <typename Char_T>
requires(not std::same_as<Char_T, void>)
using BasicMessageInvalid = BasicInvalid<MessageViewWhy<Char_T>>;

//! \cond
// Poor Doxygen doesn't understand extern template declarations.
extern template class KH_EXPLICIT_TEMPLATE_EXPORT BasicInvalid<WhyInvalid>;
extern template class KH_EXPLICIT_TEMPLATE_EXPORT BasicInvalid<MessageViewWhy<char>>;
extern template class KH_EXPLICIT_TEMPLATE_EXPORT BasicInvalid<string>;
extern template class KH_EXPLICIT_TEMPLATE_EXPORT BasicInvalid<string_view>;
//! \endcond

/*!
 * The most common use case for the BasicInvalid class in a convenient typedef.
 *
 * The MaybeInv class returns exclusively this typedef of the BasicInvalid class, and is also the one you will most
 * likely want to use in your application. If you need a message included, the MessageInvalid typedef is also very
 * useful. If you need something more complicated than an enum and a static message, you can use your own ValidWhyType
 * or you can use a std::string as the why type for BasicInvalid.
 */
using Invalid = BasicInvalid<WhyInvalid>;

/*!
 * The standard BasicMessageInvalid type using char.
 *
 * All text in KirHut software is represented using UTF-8, so this is the only MessageInvalid type that is used in
 * KirHut software. The flexibility remains to use wide characters or other character types if you so choose.
 */
using MessageInvalid = BasicMessageInvalid<char>;

/*!
 * \brief The TaggedInvalid class
 */
template <WhyInvalid why, ValidWhyType Why_T>
struct TaggedInvalid : public BasicInvalid<Why_T>
{
    using BasicInvalid<Why_T>::BasicInvalid;
};

namespace Detail
{

// We need a special detail namespace *right here*! It can only go specifically right here!

/*!
 * \internal
 *
 * \brief throwNoValidData
 * \param inv
 */
[[noreturn]] KH_EXPORT void throwNoValidData(Invalid const &inv);

} // namespace Detail

template <typename Contained_T, typename Invalid_T>
class MaybeInv;

/*!
 * A function concept that modifies the data in a MaybeInv.
 *
 * The type used for FirstArg here will determine if this may be used in MaybeInv::then() methods or in
 * MaybeInv::orElse() methods, and in both cases the argument passed as the first argument in the invocable type is
 * either the type being used or an Invalid type. Since the type that may be used in a MaybeInv could be anything, this
 * concept places no constraints on the FirstArg type.
 */
template <typename Function, typename FirstArg, typename... Args>
concept MaybeTransform = std::invocable<Function, FirstArg, Args...> and
                         InstanceOf<std::invoke_result_t<Function, FirstArg, Args...>, MaybeInv>;

/*!
 * A class that resembles std::expected in C++23 but is slightly simpler.
 *
 * Since KirHut codebases are still only using and expecting C++20, types like std::expected have yet to make it into
 * use. Further, the std::expected class has some small deficiencies, namely the mostly unnecessary "unexpected" type,
 * which for our use cases would just be defined...
 *
 * ~~~
 * template <class Contained_T> using MaybeInv = std::expected<Contained_T, Invalid>;
 * ~~~
 *
 * ...has been removed. This class is deliberately designed to never throw an exception or invoke undefined behavior
 * when used correctly (after successful construction). Some methods, like a "value" method or `operator*`, have been
 * removed from the class, as you can simply get access using the get() method, or use the monadic take(Contained_T&&)
 * method to not deal with the potential null pointer being returned.
 *
 * The ideal way to use this class is like so:
 *
 * ~~~
 * MaybeInv<int> findIndex(string_view seek)
 * {
 *   int foundIndex = -1;
 *   // Search for your index, if you do not find it, do not set foundIndex.
 *   if (foundIndex == -1)
 *   {
 *     return Invalid{ WhyInvalid::NotFound };
 *   }
 *
 *   return foundIndex;
 * }
 * ~~~
 *
 * Then on the user side:
 *
 * ~~~
 * if (auto index = findIndex("some_value"))
 * {
 *   useIndex(*index.get());
 * }
 * else
 * {
 *   KirHut::IO::report("findIndex() failed!\nReason: {}", index.failure()->info());
 * }
 * ~~~
 *
 * This class implements a subset of the monadic functionality in C++23 std::expected, such as then() and orElse(). This
 * system will propagate an Invalid properly from its base point over a series of operations, and allows simple chaining
 * of failable operations in a sequence of functions.
 *
 * \warning Much like std::expected, this class is designed to be returned from a function and then the data contained
 * should be unwrapped and used, not retained in the MaybeInv. As such, there is **no** thread synchronization
 * functionality in this class whatsoever, and any interactions with this class between multiple threads will inevitably
 * lead to data race conditions. If you have two or more threads that need access to this MaybeInv's data, **always
 * remove the data first and store it in an atomic value or control access with a mutex!**
 */
template <typename Contained_T, typename Invalid_T = Invalid>
class MaybeInv final
{
    /*!
     * \internal
     *
     * Data object typedef to simplify certain other template calls.
     *
     * The data object is of this type.
     */
    using Data = Var<Contained_T, Invalid_T>;

    /*!
     * \internal
     *
     * The MaybeInv internal data representation.
     *
     * It is just a std::variant of the Contained_T type or an Invalid.
     */
    Data data;

public:
    /*!
     * Template typedef of the type expected to be returned by this MaybeInv.
     *
     * When working with templates that use MaybeInv, this will be the type returned by MaybeInv::take(). a MaybeInv is
     * wrapped in another typedef, like `typedef MaybeInv<int> MaybeInt`, this can be useful to get the type from it:
     *
     * ~~~
     * typedef MaybeInv<int> MaybeInt;
     *
     * MaybeInt::JustType getValueOrMessage(MaybeInt const &val, MaybeInt::JustType &&alt)
     * {
     *   if (val.isValid())
     *   {
     *     return *val.get();
     *   }
     *
     *   KirHut::IO::report("Invalid MaybeInt passed to getValueOrMessage!.");
     *   return { forward<MaybeInt::JustType>(alt) };
     * }
     * ~~~
     */
    typedef Contained_T JustType;

    /*!
     * Template typedef for a pointer to the type expected to be returned by this MaybeInv.
     *
     * This should be a pointer to the underlying JustType, whatever that type is.
     */
    typedef Contained_T *JustPtr;

    /*!
     * Good object copying constructor.
     *
     * Usually you would use the rvalue reference constructor so as not to make unnecessary copies, however if that is
     * not an option this constructor still exists. This constructor is guaranteed not to throw an exception if the
     * Contained_T type copy constructor does not throw an exception.
     *
     * \param goodData Valid data to return from a function or method returning MaybeInv.
     * \throws ... Any exception thrown by the copy constructor for \p goodData.
     */
    constexpr MaybeInv(Contained_T const &goodData) noexcept(std::is_nothrow_copy_constructible_v<Contained_T>) :
        data(goodData)
    {
        // No further implementation.
    }

    /*!
     * Good object rvalue reference constructor.
     *
     * This constructor is called whenever you return a temporary object from a function or method, return an automatic
     * storage variable that falls out of scope after the return statement, or, ironically, use std::move to return a
     * named member variable. Generally you do not generall explicitly return a MaybeInv, and instead return a type
     * Contained_T, the MaybeInv object actually being returned still benefits from C++ RVO. Since this object wraps the
     * good data being returned, you want to move that data object if you can!
     *
     * \param goodData An rvalue reference to valid data to return from a function or method returning MaybeInv.
     * \throws ... Any exception thrown by the move constructor for \p goodData.
     */
    constexpr MaybeInv(Contained_T &&goodData) noexcept(std::is_nothrow_move_constructible_v<Contained_T>) :
        data(std::move(goodData))
    {
        // No further implementation.
    }

    /*!
     * Good object in-place constructor.
     *
     * Call this constructor to construct the returned good object in-place instead of needing to move it into the
     * returned MaybeInv. This makes it so there is only one move that is actually needed, the move from the MaybeInv to
     * the resulting place it needs to be. This can also be avoided by using the monadic interface to interact with the
     * object inside of the MaybeInv. This makes it so MaybeInv is a bit more efficient than std::expected.
     *
     * \param flag The Flags::emplace signifier to perform in-place construction.
     * \param ...args The arguments to pass to the constructor for the contained type of this MaybeInv.
     */
    template <typename... Args>
    requires std::constructible_from<Contained_T, Args...>
    constexpr explicit(sizeof...(Args) == 0) MaybeInv([[maybe_unused]] EmplaceFlag flag, Args &&...args)
        noexcept(std::is_nothrow_constructible_v<Contained_T, Args...>) :
        data(inpT, forward<Args>(args)...)
    {
        // No further implementation.
    }

    /*!
     * Invalid object copying constructor.
     *
     * The Invalid object can be copied into a MaybeInv when it is constructed, though this is relatively inefficient
     * and probably not useful unless you have no other option. This is a backup constructor in case you cannot use the
     * in-place or moving Invalid constructors for one reason or another. This method may throw std::bad_alloc.
     *
     * This constructor is marked as explicit (unlike the rvalue constructor version) to avoid unnecessary allocations
     * if an Invalid is returned from a function or method returning MaybeInv.
     *
     * \param inv Invalid lvalue object to copy from for the MaybeInv's internal Invalid object.
     * \throws std::bad_alloc If allocating the new Invalid object fails.
     */
    constexpr explicit MaybeInv(Invalid const &inv) : data(inv)
    {
        // No further implementation.
    }

    /*!
     * Invalid object rvalue reference constructor.
     *
     * Generally speaking, it is easier to simply construct the Invalid object in-place using the appropriate
     * constructor MaybeInv(WhyInvalid,string&&)noexcept. When an Invalid object needs specific changes or may be of
     * different values depending on processing, you can move an Invalid object into a MaybeInv cheaply without any need
     * for copying the underlying string.
     *
     * \param inv An Invalid object rvalue reference to return from a function or method returning MaybeInv.
     */
    constexpr MaybeInv(Invalid &&inv) noexcept : data(std::move(inv))
    {
        // No further implementation.
    }

    /*!
     * Invalid MaybeInv constructor without a string message.
     *
     * This constructor is useful for when you need to
     * \param why
     */
    constexpr explicit MaybeInv(WhyInvalid why) noexcept : data(inpInvalid, why)
    {
        // No further implementation.
    }

    /*!
     * Invalid MaybeInv constructor with a message as an rvalue reference.
     *
     * This can be called using a temporary string or one that has been explicitly std::move()d. The passed in string
     * will be in an invalid state after this constructor call. This allows accepting a string message without throwing
     * an exception, as the move constructor of std::string is guaranteed not to throw any exceptions. You may use the
     * string_view constructor to copy a string, however it may throw std::bad_alloc.
     *
     * \param why A WhyInvalid enumeration of what actually caused the Invalid return value.
     * \param message A message to log into the error log, or to display to the user after translation.
     */
    constexpr MaybeInv(WhyInvalid why, string &&message) noexcept : data(inpInvalid, why, std::move(message))
    {
        // No further implementation.
    }

    /*!
     * Constructor with a message that will be copied from a constant source.
     *
     * This constructor may throw std::bad_alloc, so it should only be used when you are prepared for that
     * possibility. The passed in \p message will be copied, so this will not invalidate any passed in objects, and
     * it can be used with a portion of another set of string data that isn't null terminated. You should use the
     * `noexcept` constructors unless you have a good reason to use this one.
     *
     * \param why A WhyInvalid enumeration of what actually caused the Invalid return value.
     * \param message A message to log into the error log, or to display to the user after translation.
     * \throws std::bad_alloc If copying the \p message fails to allocate memory.
     */
    constexpr MaybeInv(WhyInvalid why, char const *message) : data(inpInvalid, why, message)
    {
        // No further implementation.
    }

    /*!
     * Constructor with a message that will be copied from a constant source.
     *
     * This constructor may throw std::bad_alloc, so it should only be used when you are prepared for that possibility.
     * The passed in \p message will be copied, so this will not invalidate any passed in objects, and it can be used
     * with a portion of another set of string data that isn't null terminated. You should use the `noexcept`
     * constructors unless you have a good reason to use this one.
     *
     * \param why A WhyInvalid enumeration of what actually caused the Invalid return value.
     * \param message A message to log into the error log, or to display to the user after translation.
     * \throws std::bad_alloc If copying the \p message fails to allocate memory.
     */
    constexpr MaybeInv(WhyInvalid why, string_view message) : data(inpInvalid, why, message)
    {
        // No further implementation.
    }

    /*!
     * Method to check if the MaybeInv has valid data or not.
     *
     * If the function called that created this object successfully returned data, this method will return true. If this
     * MaybeInv was built using an Invalid object, this method will return false.
     *
     * \return A boolean indicating whether or not the data is valid.
     */
    [[nodiscard]] constexpr bool isValid() const noexcept
    {
        return std::holds_alternative<Contained_T>(data);
    }

    /*!
     * Returns the results of isValid() when MaybeInv is coerced to a bool.
     *
     * If the function called that created this object successfully returned data, this operation will return true. If
     * this MaybeInv was built using an Invalid object, this operation will return false.
     *
     * \return A boolean indicating whether or not the data is valid.
     */
    constexpr operator bool() const noexcept
    {
        return isValid();
    }

    /*!
     * Return the valid data enclosed in this MaybeInv, or a nullptr if this object contains an Invalid.
     *
     * The object may be removed using std::move(), but this should not be done unless you must do so. If you move the
     * contained object without using the take() method, this object is vulnerable to undefined behavior if you then
     * try to use that object through the MaybeInv again. There is no method in the C++ language to avoid this
     * consequence of removing the object from this MaybeInv.
     *
     * This object could return a nullptr. You should already know if this object contains valid data before using it,
     * or you can detect if there is valid data by simply getting the pointer and checking if it is null.
     *
     * \return A pointer to the contained valid data, or nullptr if there is not valid data.
     */
    [[nodiscard]] constexpr Contained_T const *get() const noexcept
    {
        return std::get_if<Contained_T>(&data);
    }

    /*!
     * \copydoc get()const noexcept
     */
    [[nodiscard]] constexpr Contained_T *get() noexcept
    {
        return std::get_if<Contained_T>(&data);
    }

    /*!
     * Take the object out of this method, providing an alternate object if there is no valid data.
     *
     * You may or may not pass an rvalue reference to another Contained_T object to this method as an alternate in the
     * case that you are attempting to take() from a MaybeInv with Invalid data. When you do provide an alternate, this
     * method is guaranteed not to throw exceptions except those thrown by the contained object's move constructor.
     *
     * \param alt An alternative object to return in the place of the data if the data is not valid.
     * \throws ... Any exception thrown by the move constructor for type Contained_T.
     * \return The data in this object, returned using an rvalue reference to move the value, or the alternate if the
     * data is not valid.
     */
    [[nodiscard]] constexpr Contained_T take(Contained_T &&alt) noexcept(safeMove)
    {
        Invalidator cleaner(data);

        // Trust in RVO. This should result in only one move construction.
        return isValid() ? std::move(*get()) : std::move(alt);
    }

    /*!
     * Take the object out of this method, and fatally crash if no such object exists.
     *
     * You may or may not pass an rvalue reference to another Contained_T object to this method as an alternate in the
     * case that you are attempting to take() from a MaybeInv with Invalid data. When you do provide an alternate, this
     * method is guaranteed not to throw exceptions except those thrown by the contained object's move constructor.
     *
     * \warning If you call this method and there is not valid data in this object to take, this method will call
     * std::terminate()! This includes if there used to be valid data and it was already taken! You should always use
     * the method with an alternate, use get(), or check this object is valid before calling this method without an
     * alternate.
     *
     * \throws NoValidData If the MaybeInv is in an Invalid state when this method is called.
     * \throws ... Any exception thrown by the move constructor for type Contained_T.
     * \return The data in this object, returned after move construction out of this object.
     */
    [[nodiscard]] constexpr Contained_T take()
    {
        if (isValid())
        {
            Invalidator cleaner(data);

            // Trust in RVO. This should result in only one move construction.
            return std::move(*get());
        }

        // This shouldn't happen if the object isn't invalid. Never call this method on an Invalid MaybeInv.
        Detail::throwNoValidData(*failure());
    }

    /*!
     * Return a const pointer to the Invalid object used in this object, or nullptr if the MaybeInv contains valid data.
     *
     * This method works identically to the get() method, except that it does not allow you to modify or take the
     * Invalid object because this object is never intended to be reused. You should check for null or ensure that the
     * object is not valid before using the Invalid object pointed to.
     *
     * \return A pointer to an Invalid object if this object contains that, or nullptr otherwise.
     */
    [[nodiscard]] constexpr Invalid const *failure() const noexcept
    {
        return std::get_if<Invalid>(&data);
    }

    /*!
     * Monadic transform method that returns a modified MaybeInv with a different type as returned by \p function.
     *
     * \param function
     * \param args
     * \return A MaybeInv of the type returned by \p function. It may be initialized with this MaybeInv's Invalid.
     */
    template <typename... Args>
    constexpr auto then(MaybeTransform<Contained_T const &, Args...> auto &function, Args &&...args)
        const & -> std::invoke_result_t<std::remove_reference_t<decltype(function)>, Contained_T const &, Args &&...>
    {
        using RetType =
            std::invoke_result_t<std::remove_reference_t<decltype(function)>, Contained_T const &, Args &&...>;
        return isValid() ? function(*get(), std::forward<Args>(args)...) : RetType{ Invalid{ *failure() } };
    }

    /*!
     * Monadic transform method that returns a modified MaybeInv with a different type as returned by \p function.
     *
     * \param function
     * \param args
     * \return A MaybeInv of the type returned by \p function. It may be initialized with this MaybeInv's Invalid.
     */
    template <typename... Arg_Ts, MaybeTransform<Contained_T &&, Arg_Ts...> Function_T>
    constexpr auto
    then(Function_T &function, Arg_Ts &&...args) &&
        noexcept(safeMove and std::is_nothrow_invocable_v<Function_T, Contained_T &&, Arg_Ts...>)
        -> std::invoke_result_t<Function_T, Contained_T &&, Arg_Ts...>
    {
        using RetType = std::invoke_result_t<Function_T, Contained_T &&, Arg_Ts...>;
        return isValid() ? function(std::move(take()), std::forward<Arg_Ts>(args)...)
                         : RetType{ std::move(*failure()) };
    }

    /*!
     * Monadic response method that always returns itself as this does not make a modified version of the MaybeInv.
     *
     * This method is useful for an orElse if you do not intend on transforming the value, as it will never make a copy
     * of the MaybeInv's Invalid object.
     * \param function
     * \param args
     * \return
     */
    template <typename... Args>
    constexpr MaybeInv<Contained_T> &orElse(std::invocable<Args &&...> auto &function, Args &&...args)
    {
        if (!isValid())
        {
            function(forward<Args>(args)...);
        }

        return *this;
    }

    /*!
     * Monadic transform method that returns a modified MaybeInv, either with a new Invalid, or in a good state.
     *
     * Unlike with the then() method, this always returns the same type of MaybeInv as there is no transforming good
     * data because there is no good data. This method otherwise works very similarly to then(), except it runs when the
     * MaybeInv is in an invalid state rather than in a good state. Usually you would put this at the end of multiple
     * then() methods, and report what the Invalid says in user visible window, for example, or log it somewhere.
     *
     * Alternatively, this could be used to provide a default value to some more monadic transforms if you know what
     * would be a suitable default, and you can restore a failed chain this way.
     *
     * \param function
     * \param args
     * \return The result of calling \p function on the failure() object and \p args, or unchanged if it hasn't failed.
     */
    template <typename... Args>
    constexpr auto orElse(MaybeTransform<Invalid const &, Args &&...> auto &function, Args &&...args)
        const & -> std::invoke_result_t<std::remove_reference_t<decltype(function)>, Invalid const &, Args &&...>
    {
        using RetType = decltype(function(*failure(), forward<Args>(args)...));
        return !isValid() ? function(*failure(), forward<Args>(args)...) : RetType{ *this };
    }

    /*!
     * Swap for MaybeInv types that swaps the contained information.
     *
     * This includes if one of them is an Invalid, if both are Invalid, or if neither are. They will be swapped
     * in the way that you would imagine, so if \p m1 is an Invalid and \p m2 is not, after the swap \p m2 will be
     * invalid and \p m1 will not be. The valid data will also be swapped.
     *
     * This can only be done on MaybeInv types that contain the same type, not any that contain different types.
     *
     * \param m1 First MaybeInv argument.
     * \param m2 Second MaybeInv argument.
     */
    friend constexpr void swap(MaybeInv &m1, MaybeInv &m2) noexcept(std::is_nothrow_swappable_v<Contained_T>)
    {
        using std::swap;
        swap(m1.data, m2.data);
    }

    /*!
     * Constexpr boolean indicating if the Contained_T is nothrow move constructible.
     *
     * This is used by several methods to test noexcept, and left accessible here for userland code to know if certain
     * methods are nothrow safe or not.
     */
    constexpr static auto safeMove = std::is_nothrow_move_constructible_v<Contained_T>;

private:
    /*!
     * \internal
     *
     * Simple alias for Flags::typeEmplace<Contained_T>.
     */
    constexpr static auto inpT = Flags::typeEmplace<Contained_T>;

    /*!
     * \internal
     *
     * Simple alias for Flags::typeEmplace<Invalid>
     */
    constexpr static auto inpInvalid = Flags::typeEmplace<Invalid>;

    struct Invalidator
    {
        Invalidator(Data &d) : dat(d)
        {
            // No further implementation.
        }

        ~Invalidator() noexcept
        {
            dat = Invalid{ WhyInvalid::DataRemoved };
        }

        Data &dat;
    };

    static_assert(not std::is_reference_v<Contained_T>, "MaybeInv cannot contain a reference type.");
    static_assert(not std::is_function_v<Contained_T>,
                  "MaybeInv cannot contain a function type. Return a function object instead.");
};

} // namespace KirHut
