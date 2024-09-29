/***********************************************************************************************************************
** The KirHut Application Development Library
** base.hpp
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

/*!
 * \file base.hpp
 *
 * The KirHut Standard (or "Base") include file.
 *
 * This file provides all of the basic types and build information provided by kh/global.hpp and also includes several
 * basic standard library types in the KirHut namespace. This file also provides some string conversion functions and
 * the Invalid and MaybeInv types. These are used universally throughout KirHut software to prevent Exceptions where
 * they are not needed.
 */

#include "global.hpp" // IWYU pragma: export

#include <string>
#include <concepts>
#include <string_view>
#include <memory>
#include <array>
#include <bit>
#include <span>
#include <utility>
#include <variant>
#include <optional>

/*!
 * The primary namespace for all KirHut software, including libraries, applications, and plugins.
 *
 * This namespace is used everywhere in KirHut C++ software, and ensures that no name conflicts should ever occur when
 * mixing KirHut software with any other software, including software not within a namespace (Like Qt software). Within
 * this namespace there should be additional namespaces, primarily the application namespaces (which by convention are
 * simple abbreviations of the software name, like "PSM" for the KirHut Passwords and Secrets Manager or "SDV" for the
 * KirHut Simple Data Verifier). There are other sub namespaces where functionality is provided, and they are documented
 * individually within this namespace.
 *
 * The KirHut namespace also import a selection of C++ standard library names. These names are usually aliased, but are
 * also frequently not, so it will depend on the situation and they are also individually documented. The ideal way to
 * use the KirHut namespace in code is to directly use the entire namespace, a 'la `using namespace KirHut`. The KirHut
 * libraries avoid the hazards of `using namespace std` because this does not include the entire standard library, and
 * name conflicts are far less likely when the standard names are aliased and limited to a curated subset of the
 * functionality.
 *
 * All KirHut software written in C++ requires C++20 as a base. The following standard identifiers are imported into the
 * KirHut namespace implicitly:
 * - std::byte
 * - std::array
 * - std::bit_cast
 * - std::forward
 * - std::get
 * - std::make_shared
 * - std::make_unique
 * - std::memcpy
 * - std::pair
 * - std::span
 * - std::string
 * - std::string_view
 * - std::tuple
 * - The entire std::literals::string_literals namespace
 * - The entire std::literals::string_view_literals namespace
 *
 * Ensure none of your identifiers conflict with the above standard identifiers when using the KirHut namespace.
 */
namespace KirHut
{

using std::array;
using std::bit_cast;
using std::forward;
using std::get;
using std::make_shared;
using std::make_unique;
using std::memcpy;
using std::pair;
using std::span;
using std::string;
using std::string_view;
using std::tuple;

using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

/*!
 * Alias name for std::unique_ptr.
 *
 * The smart pointer names are a bit long, so this makes it easier to type out and have in method signatures.
 */
template <typename T, typename U = std::default_delete<T>>
using UPtr = std::unique_ptr<T, U>;

/*!
 * Alias name for std::shared_ptr.
 *
 * \copydetails KirHut::UPtr
 */
template <typename T>
using SPtr = std::shared_ptr<T>;

/*!
 * Alias name for std::variant.
 *
 * This is used purely to shorten the name since this is common in method signatures.
 */
template <typename... Ts>
using Var = std::variant<Ts...>;

/*!
 * Alias name for std::optional.
 *
 * Slightly shorter name that is more descriptive of intent then "optional."
 */
template <typename T>
using Maybe = std::optional<T>;

/*!
 * Concept representing the different types that may bypass strict aliasing rules in C++.
 *
 * Some types are not undefined behavior to dereference from a different type. Those three types are considered "byte
 * types" in KirHut software.
 */
template <typename T>
concept ByteType = std::same_as<char, T> or std::same_as<byte, T> or std::same_as<unsigned char, T>;

/*!
 * Enumeration of the different reasons the Invalid class is returned (usually inside of a MaybeInv).
 *
 * There are no fixed number of WhyInvalid reasons, and this can be changed regularly so you should never count on this
 * being the same or even the values for each enumeration to remain stable. You must always match it up with the
 * current build version's WhyInvalid type for the function or method that returned it. You should never save the
 * enumeration value expecting it to be same from version to version.
 */
enum class WhyInvalid
{
    NotInvalid, //!< There is no invalid state. This is useful when storing a WhyInvalid outside of the Invalid class.
    BadArgument, //!< Invalid returned because an argument was not valid for some reason.
    NotFound, //!< Invalid returned because the object or file requested was not found.
    CouldntOpen, //!< Invalid returned because the file or data being accessed could not be opened.
    CorruptData, //!< Invalid returned because the data provided was corrupted or unparseable.
    OutOfBounds, //!< Invalid returned because the location requested was outside of the legal bounds.
    CantMatch, //!< Invalid returned because a pattern or value to match against could not be matched with the data set.
    DataRemoved //!< Invalid returned because the data requested is no longer available.
};

/*!
 * Provides a simple interface with a message and a why for the failure.
 *
 * This class uses the enum WhyInvalid to provide a method for programmers to know what happened and distinguish it
 * from other failures, and a message that is **never translated** to the user's language and is meant to be placed in
 * an English log for the original developers. Generally, you should use the WhyInvalid enum to tell the user why the
 * issue occurred using your own translation system, but you may also use the returned string as a key in your
 * translation system (like the Qt Translation system can do).
 *
 * The Invalid object, along with the MaybeInv object, are not designed to be thread safe, and should only be used by
 * the thread that created them. While there is nothing actually stopping you from moving it across threads, and if only
 * one thread operates on the data it should be fine, it is on you to ensure that there are no race conditions between
 * two or more threads for the same Invalid object. Use a single thread to write the data to a shared memory buffer that
 * is properly thread safe instead of relying on these objects for it.
 *
 * Invalid should never be thrown from a function or method because this value is supposed to be returned within a
 * MaybeInv in noexcept-based functions and pointers to indicate failures that are a result of something the user did
 * and the program can usefully inform the user about. This object also allows returning error conditions from Qt
 * Signals and Slots, as exceptions are completely unsupported using Signals and Slots.
 *
 * This object is frustratingly large, at 32 bytes release on Clang and 40 bytes release on MSVC and GCC. This is mostly
 * a result of the underlying std::string object, which is 24 bytes on Clang and 32 bytes on MSVC and GCC. The other
 * 8 bytes are taken up by a WhyInvalid. As such, try not to constantly return these or make a ton of them in an array,
 * as that can consume a lot of memory or processor speed. That said, 32 and 40 bytes isn't *that* terrible, so this
 * should be acceptable in every case I can think of.
 */
class KH_EXPORT Invalid final
{
    string message_d;
    WhyInvalid why_d;

public:
    /*!
     * Constructor with no message, only a WhyInvalid value.
     *
     * The class requires the why value to be initialized at construction, and it may not be changed after the object
     * has been created. The message can be changed using setMessage(), or overwriteMessage(). See the documentation on
     * those methods to learn the differences betwen them.
     *
     * \param why A WhyInvalid enumeration of what actually caused the Invalid return value.
     */
    Invalid(WhyInvalid why) noexcept;

    /*!
     * Constructor with a message as an rvalue reference.
     *
     * This can be called using a temporary string or one that has been explicitly std::move()d. The passed in string
     * will be in an invalid state after this constructor call. This allows accepting a string message without throwing
     * an exception, as the move constructor of std::string is guaranteed not to throw any exceptions. You may use the
     * string_view constructor to copy a string, however it may throw std::bad_alloc.
     *
     * \param why A WhyInvalid enumeration of what actually caused the Invalid return value.
     * \param message A message to log into the error log, or to display to the user after translation.
     */
    Invalid(WhyInvalid why, string &&message) noexcept;

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
    Invalid(WhyInvalid why, char const *message);

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
    Invalid(WhyInvalid why, string_view message);

    /*!
     * Standard copy constructor.
     *
     * This is literally implemented with `= default`, so it is very much a standard copy constructor. This copies the
     * why and message objects, so it may throw a std::bad_alloc exception when creating the new string object.
     *
     * \param other The Invalid object to create this one from.
     * \throws std::bad_alloc If copying the \p other Invalid fails to allocate memory.
     */
    Invalid(Invalid const &other) = default;

    /*!
     * Standard move constructor.
     *
     * This moves the `why` and `message` properties, so it should never throw an exception as moving a std::string does
     * not throw exceptions.
     *
     * \param other The Invalid object to create this one from. The passed-in Invalid will be the same but have an
     * empty message.
     */
    Invalid(Invalid &&other) noexcept;

    /*!
     * Standard move assignment operator.
     *
     * This moves the `why` and `message` properties, so it should never throw an exception as moving a std::string does
     * not throw exceptions.
     *
     * \param other The Invalid object to create this one from. The passed-in Invalid will be the same but have an
     * empty message.
     */
    Invalid &operator=(Invalid &&other) noexcept = default;

    /*!
     * Get the reason for this Invalid object's creation.
     *
     * This value can only be changed when the Invalid object is constructed or reassigned from another object. A
     * constant enumeration of why this Invalid object was returned. You should use this in a switch/case statement or
     * to match against possible options in an if statement.
     *
     * \return A reason why this Invalid object was created.
     */
    [[nodiscard]] WhyInvalid why() const noexcept;

    /*!
     * Get the message contained in this Invalid object.
     *
     * The string pointed to by the returned string_view will remain valid for the lifetime of the Invalid object, and
     * will be destroyed afterward. Do not use the returned string_view object from this method after the Invalid
     * object has been destroyed, or you will invoke Undefined Behavior. There is little that can be done to rectify
     * this in code without some kind of wrapper class like std::weak_ptr is.
     *
     * \return A string_view of the message in this Invalid object.
     */
    [[nodiscard]] string_view message() const noexcept;

    /*!
     * Set the message to the passed in \p msg.
     *
     * Use this method to set the message for the Invalid **if that message has not been set yet**. If the message in
     * this Invalid has been set (either through the constructor or a previous call to setMessage() or
     * overwriteMessage(), then this method will silently ignore the command. Use the overwriteMessage() method to
     * guarantee that the content of the string will be replaced.
     *
     * This is useful if you have several possible messages that can be chosen, and you can just set it to the message
     * if a setting is true and keep checking others. If one has already been set, the next set command is just ignored.
     *
     * The string rvalue reference overload of this method is noexcept, so it should be preferred, however sometimes you
     * must copy a string, in which case use the string_view overload.
     *
     * \param msg The content to set the Invalid's message to unless it has already been set.
     */
    void setMessage(string &&msg) noexcept;

    /*!
     * \copydoc setMessage(string&&)noexcept
     *
     * \throws std::bad_alloc If copying the \p msg fails to allocate memory.
     */
    void setMessage(string_view msg);

    /*!
     * Overwrite the current message with the content passed in \p msg.
     *
     * Use this method to set the message for the Invalid **regardless of whether or not it has already been set**. This
     * is usually what is thought of as a setter, but this explicitly states you want to replace a message in an Invalid
     * object, which is an uncommon thing to do.
     *
     * The string rvalue reference overload of this method is noexcept, so it should be preferred, however sometimes you
     * need to copy a string, in which case use the string_view overload.
     *
     * \param msg The content to set the Invalid's message to, even if it is already set.
     */
    void overwriteMessage(string &&msg) noexcept;

    /*!
     * \copydoc overwriteMessage(string&&)noexcept
     *
     * \throws std::bad_alloc If copying the \p msg fails to allocate memory.
     */
    void overwriteMessage(string_view msg);
};

/*!
 * A class that resembles std::expected in C++23 but is slightly smaller and better.
 *
 * Since KirHut codebases are still only using and expecting C++20, types like std::expected have yet to make it into
 * use. Further, the std::expected class has some small deficiencies, namely the mostly unnecessary "unexpected" type
 * (which for our use cases would just be defined `template <class T> using MaybeInv = std::expected<T, Invalid>;`)
 * has been removed, and this class is deliberately designed to never throw an exception or invoke undefined behavior
 * under any circumstances (after successful construction). Some methods, like a "value" method or `operator*`, have
 * been removed from the class, as you can simply get access using the get() method, or use the monadic getOr() method
 * to not deal with the potential null pointer being returned.
 *
 * The ideal way to use this class is like so:
 * ~~~
 * MaybeInv<int> findIndex(string_view seek)
 * {
 *   int foundIndex = -1;
 *   // Search for your index, if you do not find it, do not set foundIndex.
 *   if (foundIndex == -1)
 *   {
 *     return Invalid{ WhyInvalid::NotFound, "An index could not be found for "s + seek };
 *   }
 *
 *   return foundIndex;
 * }
 * ~~~
 * Then on the user side:
 * ~~~
 * if (auto index = findIndex("some_value"))
 * {
 *   useIndex(*index.get());
 * }
 * else
 * {
 *   KirHut::out() << "findIndex() failed!\nReason: " << index.failure()->message() << "\n";
 * }
 * ~~~
 */
template <typename T>
class MaybeInv final
{
    Var<T, Invalid> data;

public:
    /*!
     * Good object copying constructor.
     *
     * Usually you would use the rvalue reference constructor so as not to make unnecessary copies, however if that is
     * not an option this constructor still exists. This constructor is guaranteed not to throw an exception if the
     * T type copy constructor does not throw an exception.
     *
     * \param goodData Valid data to return from a function or method returning MaybeInv.
     * \throws ... Any exception thrown by the copy constructor for \p goodData may be thrown by this constructor.
     */
    MaybeInv(T const &goodData) noexcept(noexcept(T(std::add_lvalue_reference_t<T>()))) : data(goodData)
    {
        // No implementation.
    }

    /*!
     * Good object rvalue reference constructor.
     *
     * This constructor is called whenever you return a temporary object from a function or method or, ironically, use
     * std::move to return a named value. Generally you do not explicitly return a MaybeInv, and instead return a type
     * T, so the MaybeInv object actually being returned still benefits from C++ RVO. Since this object wraps the good
     * data being returned, you want to move that data object if you can!
     *
     * To ensure not confusing future readers or poor language servers thinking `return std::move(x);` is bad practice,
     * it is recommended that you surround the std::move with braces, like `return { std::move(x) };`. This makes it
     * explicit that you are constructing a new object to return, and C++ will automatically correctly select this
     * constructor by inferring it from the return type and argument passed.
     *
     * \param goodData An rvalue reference to valid data to return from a function or method returning MaybeInv.
     * \throws ... Any exception thrown by the move constructor for \p goodData may be thrown by this constructor.
     */
    MaybeInv(T &&goodData) noexcept(noexcept(T(std::declval<T>()))) : data(forward<T>(goodData))
    {
        // No implementation.
    }

    /*!
     * Invalid object rvalue reference constructor.
     *
     * The Invalid object can only be constructed with the MaybeInv if MaybeInv takes ownership of the Invalid, there
     * is no copy constructor. The reason for this is because Invalid should never be constructed until you know there
     * is an error, and that is when you are returning it. A copying constructor does not make sense for the Invalid
     * object.
     *
     * \param inv An Invalid object rvalue reference to return from a function or method returning MaybeInv.
     */
    MaybeInv(Invalid &&inv) noexcept : data(forward<Invalid>(inv))
    {
        // No implementation.
    }

    /*!
     * Method to check if the MaybeInv has valid data or not.
     *
     * If the function called that created this object successfully returned data, this method will return true. If this
     * MaybeInv was built using an Invalid object, this method will return false.
     *
     * \return A boolean indicating whether or not the data is valid.
     */
    [[nodiscard]] inline bool isValid() const noexcept
    {
        return std::holds_alternative<T>(data);
    }

    /*!
     * Returns the results of isValid() when MaybeInv is coerced to a bool.
     *
     * If the function called that created this object successfully returned data, this operation will return true. If
     * this MaybeInv was built using an Invalid object, this operation will return false.
     *
     * \return A boolean indicating whether or not the data is valid.
     */
    inline operator bool() const noexcept
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
    [[nodiscard]] inline T const *get() const noexcept
    {
        return std::get_if<T>(&data);
    }

    /*!
     * \copydoc get()const noexcept
     */
    [[nodiscard]] inline T *get() noexcept
    {
        return std::get_if<T>(&data);
    }

    /*!
     * Take the object out of this method, providing an alternate object if there is no valid data.
     *
     * You may or may not pass an rvalue reference to another T object to this method as an alternate in the case that
     * you are attempting to take() from a MaybeInv with Invalid data. When you do provide an alternate, this method is
     * guaranteed not to call std::terminate().
     *
     * \param alt An alternative object to return in the place of the data if the data is not valid.
     * \return The data in this object, returned using an rvalue reference to move the value, or the alternate if the
     * data is not valid.
     */
    [[nodiscard]] inline T take(T &&alt) noexcept(noexcept(T(std::declval<T>())))
    {
        if (isValid())
        {
            T ret = std::move(*get());
            data  = Invalid{ WhyInvalid::DataRemoved };
            return ret;
        }

        return forward<T>(alt);
    }

    /*!
     * Take the object out of this method, and fatally crash if no such object exists.
     *
     * You may or may not pass an rvalue reference to another T object to this method as an alternate in the case that
     * you are attempting to take() from a MaybeInv with Invalid data. When you do provide an alternate, this method is
     * guaranteed not to call std::terminate().
     *
     * \warning If you call this method and there is not valid data in this object to take, this method will call
     * std::terminate()! This includes if there used to be valid data and it was already taken! You should always use
     * the method with an alternate, use get(), or check this object is valid before calling this method without an
     * alternate.
     *
     * \param alt An alternative object to return in the place of the data if the data is not valid.
     * \return The data in this object, returned using an rvalue reference to move the value, or the alternate if the
     * data is not valid.
     */
    [[nodiscard]] inline T take() noexcept(noexcept(T(std::declval<T>())))
    {
        if (isValid())
        {
            T ret = std::move(*get());
            data  = Invalid{ WhyInvalid::DataRemoved };
            return ret;
        }

        // This shouldn't happen if the object isn't invalid. Never call this method on an Invalid MaybeInv.
        std::terminate();
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
    [[nodiscard]] inline Invalid const *failure() const noexcept
    {
        return std::get_if<Invalid>(&data);
    }
};

/*!
 * Constant expression of the number of bytes needed to hold a given number of bits.
 *
 * For basically all processors supported by KirHut, the value is just the number passed in divided by 8, plus one if
 * there are any additional bits. This is because nearly all processors use an 8 bit byte. There is the extremely vague
 * possibility of using something with a differently sized number of bytes, so this function protects against that,
 * while also labeling the purpose of some given magic number.
 *
 * \param numBits The number of bits that need to fit in the number of returned bytes.
 * \return The number of bytes that will completely contain the number of bits passed as \p numBits.
 */
[[nodiscard]] constexpr ue32 bytesNeededForBits(ue32 numBits) noexcept
{
    return (numBits + BYTE_BITS - 1) / BYTE_BITS;
}

/*!
 * Convert a pointer to any type into a pointer to char.
 *
 * This is just a bit_cast to `char *` wrapped in a more friendly function name. It is constexpr because ideally the
 * compiler should just optimize out the function call entirely.
 *
 * There is an appropriate `const` overload for input `const` pointers to retain `const`.
 *
 * \param ptr A pointer to some kind of type data.
 * \return A pointer to char that was casted from the input pointer.
 */
[[nodiscard]] constexpr char const *toCharPtr(auto const *ptr) noexcept
{
    return bit_cast<char const *>(ptr);
}

/*!
 * \copydoc toCharPtr(auto const*)noexcept
 */
[[nodiscard]] constexpr char *toCharPtr(auto *ptr) noexcept
{
    return bit_cast<char *>(ptr);
}

/*!
 * Convert an object of any type to a pointer to char.
 *
 * This just gets a pointer to the address of the object passed as an argument and casts it to a `char *` using the
 * pointer overload of toCharPtr().
 *
 * There is an appropriate `const` overload for input `const` objects to retain `const`.
 *
 * \param obj An object of some kind to get the `char` data of.
 * \return A pointer to `char` data that was casted from the input object.
 */
[[nodiscard]] constexpr char const *toCharPtr(auto const &obj) noexcept
{
    return toCharPtr(&obj);
}

/*!
 * \copydoc toCharPtr(auto const&)noexcept
 */
[[nodiscard]] constexpr char *toCharPtr(auto &obj) noexcept
{
    return toCharPtr(&obj);
}

/*!
 * Convert a pointer to any type into a pointer to std::byte.
 *
 * This is just a bit_cast to `byte *` wrapped in a more friendly function name. It is constexpr because ideally the
 * compiler should just optimize out the function call entirely.
 *
 * There is an appropriate `const` overload for input `const` pointers to retain `const`.
 *
 * \param ptr A pointer to some kind of type data.
 * \return A pointer to byte that was casted from the input pointer.
 */
[[nodiscard]] constexpr byte const *toBytes(auto const *ptr) noexcept
{
    return bit_cast<byte const *>(ptr);
}

/*!
 * \copydoc toBytes(auto const*)noexcept
 */
[[nodiscard]] constexpr byte *toBytes(auto *ptr) noexcept
{
    return bit_cast<byte *>(ptr);
}

/*!
 * Convert an object of any type to a pointer to std::byte.
 *
 * This just gets a pointer to the address of the object passed as an argument and casts it to a `byte *` using the
 * pointer overload of toBytes().
 *
 * There is an appropriate `const` overload for input `const` objects to retain `const`.
 *
 * \param obj An object of some kind to get the `byte` data of.
 * \return A pointer to `byte` data that was casted from the input object.
 */
[[nodiscard]] constexpr byte const *toBytes(auto const &obj) noexcept
{
    return toBytes(&obj);
}

/*!
 * \copydoc toBytes(auto const&)noexcept
 */
[[nodiscard]] constexpr byte *toBytes(auto &obj) noexcept
{
    return toBytes(&obj);
}

[[nodiscard]] string toStr(char const *data);

#if __cpp_lib_span
/*!
 * Convert any span to a `byte` pointer of the underlying \p data.
 *
 * This just gets a pointer to the address of the first element in \p data and casts it to a `byte *` using
 * std::as_bytes() or std::as_writable_bytes() and returning the data() pointer from there. It is constexpr because
 * ideally the compiler should just optimize out the function call entirely.
 *
 * There is an appropriate `const` overload for input `const` objects to retain `const`.
 *
 * \param data A span to memory of any type to get a `byte *` pointing to.
 * \return A pointer to `byte` data that starts at the first element of \p data.
 */
template <typename T, size_t E = std::dynamic_extent>
[[nodiscard]] constexpr byte const *toBytes(span<T const, E> data) noexcept
{
    return std::as_bytes(data).data();
}

/*!
 * \copydoc toBytes(std::span<T const,E>)noexcept
 */
template <typename T, size_t E = std::dynamic_extent>
[[nodiscard]] constexpr byte *toBytes(span<T, E> data) noexcept
{
    return std::as_writable_bytes(data).data();
}

/*!
 * Convert any span to a `char` pointer of the underlying \p data.
 *
 * This just gets a pointer to the address of the first element in \p data and casts it to a `char *`. It is constexpr
 * because ideally the compiler should just optimize out the function call entirely.
 *
 * There is an appropriate `const` overload for input `const` objects to retain `const`.
 *
 * \param data A span to memory of any type to get a `char *` pointing to.
 * \return A pointer to `byte` data that starts at the first element of \p data.
 */
template <typename T, size_t E = std::dynamic_extent>
[[nodiscard]] constexpr char const *toCharPtr(span<T const, E> data) noexcept
{
    return bit_cast<char const *>(data.data());
}

/*!
 * \copydoc toCharPtr(std::span<T const,E>)noexcept
 */
template <typename T, size_t E = std::dynamic_extent>
[[nodiscard]] constexpr char *toCharPtr(span<T, E> data) noexcept
{
    return bit_cast<char *>(data.data());
}
#endif // __cpp_lib_span

#if KH_PRIV_DOCS || KH_USES_QT
/*!
 * Conversion function for String to QString.
 *
 * This creates a QByteArray with the data pointed to by \p in and the length, ensuring no copies are made and the
 * QString ctor does not need to get the length of the data. This function still runs at **O(n)** because it must still
 * make at least one copy of the string data to the underlying UTF-16 QString data block.
 *
 * \param in A std::string_view to convert to a QString.
 * \return A QString of the contents in the passed String.
 */
[[nodiscard]] KH_EXPORT QString toQStr(string_view in) noexcept;

/*!
 * Conversion function for QString to String.
 *
 * This uses QString::toUtf8() to create a UTF-8 QByteArray of the QString data, then must perform a second copy of that
 * UTF-8 data to the returned string objects data buffer because there is no way to initialize a std::string with a
 * char array that does not make a copy. It will then dispose of the QByteArray. This function still runs at **O(n)**
 * time efficiency, despite the dual copies.
 *
 * \param in A QString to convert to a std::string.
 * \return A String copy of the contents in the passed QString.
 */
[[nodiscard]] KH_EXPORT string toStr(QString const &in) noexcept;
#endif // KH_USES_QT

#if KH_PRIV_DOCS || __cpp_char8_t
/*!
 * Conversion function for std::u8string_view to std::string.
 *
 * The `char8_t` type is an unfortunate wart on C++ currently. I honestly would love to have a sensible UTF-8 type that
 * would handle all of the underlying formatting issues for me, but C++'s solution was not just ineffective, it was
 * actively damaging to existing code bases and more broken than just doing nothing at all would have been. The C++
 * standards committee *should* have just created a std::u8string type that has char as the underlying data type, and
 * included additional functionality to get entire UTC characters (like something that converted non-ASCII UTF-8 code
 * points into a `char32_t` to iterate over, etc.). This was not done, instead we got this hackneyed solution that for
 * some reason has a `char8_t` type that requires copying to convert to `char` which literally every string processing
 * method uses, so it is actively detrimental to use `char8_t` in any meaningful way.
 *
 * If you do not build with `-fno-char8_t` or `/Zc:char8_t-`, this uses memcpy() to copy the data from the
 * std::u8string_view to a std::string which is the default string type used by KirHut software. If you only need access
 * to the underlying char data it is much easier to simply use toCharPtr().
 *
 * \warning The underlying char data of the passed in std::string_view is not verified within this method, it is simply
 * directly copied. If the data contained isn't a valid UTF-8 string, then the behavior of the resulting std::u8string
 * is undefined.
 *
 * \param in A std::u8string_view to convert to a string.
 * \throws bad_alloc If the string memory allocation fails.
 * \return A string copy of the contents in the passed std::u8string_view, directly copied.
 */
[[nodiscard]] KH_EXPORT string toStr(std::u8string_view in);

/*!
 * Conversion function for std::string_view to std::u8string.
 *
 * The `char8_t` type is an unfortunate wart on C++ currently. I honestly would love to have a sensible UTF-8 type that
 * would handle all of the underlying formatting issues for me, but C++'s solution was not just ineffective, it was
 * actively damaging to existing code bases and more broken than just doing nothing at all would have been. The C++
 * standards committee *should* have just created a std::u8string type that has char as the underlying data type, and
 * included additional functionality to get entire UTC characters (like something that converted non-ASCII UTF-8 code
 * points into a `char32_t` to iterate over, etc.). This was not done, instead we got this hackneyed solution that for
 * some reason has a `char8_t` type that requires copying to convert to `char` which literally every string processing
 * method uses, so it is actively detrimental to use `char8_t` in any meaningful way.
 *
 * If you do not build with `-fno-char8_t` or `/Zc:char8_t-`, this uses memcpy() to copy the data from the
 * std::string_view to a std::u8string which may be necessary when working with other C++ software. This function runs
 * at **O(n)** due to the requirement to copy the string.
 *
 * \warning The underlying char data of the passed in std::string_view is not verified within this method, it is simply
 * directly copied. If the data contained isn't a valid UTF-8 string, then the behavior of the resulting std::u8string
 * is undefined.
 *
 * \param in A std::string_view to convert to a std::u8string.
 * \throws bad_alloc If the std::u8string memory allocation fails.
 * \return A std::u8string copy of the contents in the passed string_view.
 */
[[nodiscard]] KH_EXPORT std::u8string toU8Str(string_view in);

# if KH_PRIV_DOCS || KH_USES_QT
/*!
 * Conversion function for QString to std::u8string.
 *
 * The `char8_t` type is an unfortunate wart on C++ currently. I honestly would love to have a sensible UTF-8 type that
 * would handle all of the underlying formatting issues for me, but C++'s solution was not just ineffective, it was
 * actively damaging to existing code bases and more broken than just doing nothing at all would have been. The C++
 * standards committee *should* have just created a std::u8string type that has char as the underlying data type, and
 * included additional functionality to get entire UTC characters (like something that converted non-ASCII UTF-8 code
 * points into a `char32_t` to iterate over, etc.). This was not done, instead we got this hackneyed solution that for
 * some reason has a `char8_t` type that requires copying to convert to `char` which literally every string processing
 * method uses, so it is actively detrimental to use `char8_t` in any meaningful way.
 *
 * If you do not build with `-fno-char8_t` or `/Zc:char8_t-`, this uses QString::toUtf8() to create a UTF-8 QByteArray
 * of the QString data, then must perform a second copy of that UTF-8 data to the returned std::u8string object's data
 * buffer because there is no way to initialize a std::u8string with a char array that does not make a copy. It will
 * then dispose of the QByteArray. This function still runs at **O(n)** time efficiency, despite the dual copies.
 *
 * \warning The underlying char data of the passed in std::string_view is not verified within this method, it is simply
 * directly copied. If the data contained isn't a valid UTF-8 string, then the behavior of the resulting std::u8string
 * is undefined.
 *
 * \param in A QString to convert to a std::u8string.
 * \return A std::u8string copy of the contents in the passed QString.
 */
[[nodiscard]] KH_EXPORT std::u8string toU8Str(QString const &in) noexcept;

/*!
 * Conversion function for std::u8string to QString.
 *
 * The `char8_t` type is an unfortunate wart on C++ currently. I honestly would love to have a sensible UTF-8 type that
 * would handle all of the underlying formatting issues for me, but C++'s solution was not just ineffective, it was
 * actively damaging to existing code bases and more broken than just doing nothing at all would have been. The C++
 * standards committee *should* have just created a std::u8string type that has char as the underlying data type, and
 * included additional functionality to get entire UCS characters (like something that converted non-ASCII UTF-8 code
 * points into a `char32_t` to iterate over, etc.). This was not done, instead we got this hackneyed solution that for
 * some reason has a `char8_t` type that requires copying to convert to `char` which literally every string processing
 * method uses, so it is actively detrimental to use `char8_t` in any meaningful way.
 *
 * If you do not build with `-fno-char8_t` or `/Zc:char8_t-`, this creates a QByteArray with the casted data pointed to
 * by \p in and the length, ensuring no copies are made and the QString ctor does not need to get the length of the
 * data. This function still runs at **O(n)** because it must still make at least one copy of the string data to the
 * underlying UTF-16 QString data block.
 *
 * \warning The underlying char data of the passed in std::string_view is not verified within this method, it is simply
 * directly copied. If the data contained isn't a valid UTF-8 string, then the behavior of the resulting QString is
 * undefined.
 *
 * \param in A QString to convert to a std::u8string.
 * \return A std::u8string copy of the contents in the passed QString.
 */
[[nodiscard]] KH_EXPORT QString toQStr(std::u8string_view in) noexcept;
# endif // KH_USES_QT
#endif // __cpp_char8_t

} // namespace KirHut
