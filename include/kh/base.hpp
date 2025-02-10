/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/base.hpp
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
 * \file kh/base.hpp
 *
 * The KirHut Standard (or "Base") include file.
 *
 * This file provides all of the basic types and build information provided by kh/global.hpp and also includes several
 * basic standard library types in the KirHut namespace. This file also provides some string conversion functions and
 * the Invalid and MaybeInv types. These are used universally throughout KirHut software to prevent Exceptions where
 * they are not needed.
 *
 * \see KirHut
 */

#include "kh/global.hpp" // IWYU pragma: export

#include <string>
#include <concepts>
#include <string_view>
#include <memory>
#include <array>
#include <bit>
#include <bitset>
#include <span>
#include <utility>
#include <variant>
#include <optional>
#include <cstring>

/*!
 * The primary namespace for all KirHut software, including libraries, applications, and plugins.
 *
 * This namespace is used everywhere in KirHut C++ software, and ensures that no name conflicts should ever occur when
 * mixing KirHut software with any other software, including software not within a namespace (Like Qt software). Within
 * this namespace there should be additional namespaces, primarily the application namespaces (which by convention are
 * simple abbreviations of the software name, like "PSM" for the KirHut Passwords and Secrets Manager or "DVT" for the
 * KirHut Data Verification Tool). There are other sub namespaces where functionality is provided, and they are
 * documented individually within this namespace.
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
using std::bitset;
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

// Place forward declarations here:
class Invalid;
template <typename Contained>
class MaybeInv;

/*!
 * Alias name for std::unique_ptr.
 *
 * The smart pointer names are a bit long, so this makes it easier to type out and have in method signatures.
 */
template <typename Type, typename Deleter = std::default_delete<Type>>
using UPtr = std::unique_ptr<Type, Deleter>;

/*!
 * Alias name for std::shared_ptr.
 *
 * \copydetails KirHut::UPtr
 */
template <typename Type>
using SPtr = std::shared_ptr<Type>;

/*!
 * Alias name for std::variant.
 *
 * This is used purely to shorten the name since this is common in method signatures.
 */
template <typename... Types>
using Var = std::variant<Types...>;

/*!
 * Alias name for std::optional.
 *
 * Slightly shorter name that is more descriptive of intent then "optional."
 */
template <typename Type>
using Maybe = std::optional<Type>;

/*!
 * An empty class that is used as a flag to signify that another usually template type needs to create a new instance.
 *
 * You shouldn't make instances of MakeFlag on your own, even though there is no real mechanism stopping you. Simply use
 * the KirHut::make value instead, as that is an instance of this type and there is no distinction between any of the
 * instances.
 */
struct MakeFlag
{
    // No implementation.
};

/*!
 * The universal implementation of MakeFlag that can be passed anywhere.
 *
 * Just pass this object when you want to use the constructing "MakeFlag" type to signify that a class should build a
 * new instance of usually a template type. There are several situations this is used, and this type is likely to be
 * used when those types are also in use.
 */
[[maybe_unused]] constexpr MakeFlag make;

/*!
 * KirHut Private namespace.
 *
 * This namespace, along with the KirHut::*::Priv namespace in each nested namespace, is a private, reserved namespace
 * that should never be used, as it is portions of the library intended to implement functionality within the library
 * and is not stable for use. Any types in this namespace are intentionally left out of the documentation, and anything
 * within this namespace is subject to both ABI and API changes without warning, version number updates, or any other
 * notification whatsoever. If you use these, you are deliberately breaking your own application.
 *
 * Leave this namespace alone.
 */
namespace Priv
{
//! \cond

// Credit to Lawrence Murray for this InstanceOfValue implementation to determine if a type is a given template type.
// I just do not possess the profound, n-ary galaxy brain necessary to concoct this monstrosity. Thank you!
// https://indii.org/blog/is-type-instantiation-of-template/

template <typename WrongType, template <typename...> typename Template>
constexpr bool InstanceOfValue = false;

template <template <typename...> typename Template, typename... Args>
constexpr bool InstanceOfValue<Template<Args...>, Template> = true;

template <typename Tested, template <typename...> typename Template>
concept InstanceOf = InstanceOfValue<Tested, Template>;

template <typename WrongType, typename ElementType>
constexpr bool SpanOfValue = false;

template <typename ElementType, size_t Extent>
constexpr bool SpanOfValue<span<ElementType, Extent>, ElementType> = true;

template <typename Tested, typename ElementType>
concept SpanOf = SpanOfValue<Tested, std::remove_cv_t<ElementType>>;

template <typename VarType, typename Contains>
concept HasTypeOption =
    (InstanceOf<VarType, Var> or InstanceOf<VarType, std::variant>) and requires(VarType v) { get<Contains>(v); };

class NoChild final
{
    [[maybe_unused]] int val;
};

template <typename EBO>
class EboChild final : public EBO
{
    [[maybe_unused]] int val;
};

[[noreturn]] void throwNoValidData(Invalid const &);

template <typename T>
byte *spanBytes(SpanOf<T> auto sp)
{
    return std::as_writable_bytes(sp);
}

//! \endcond
} // namespace Priv

/*!
 * Concept to identify if a type is one of a set of distinct types.
 *
 * \headerfile kh/base.hpp
 *
 * There are times when you just want a concept to be some fixed set of types you want to accept, but don't want to just
 * make separate overloads of that function for each type. This concept simplifies taking those arguments, and you can
 * easily define other concepts in terms of this concept. You can also check the return types of expressions to ensure
 * that they return one of a distinct set of types.
 */
template <typename Tested, typename Type, typename... Types>
concept IsOneOf = std::same_as<Tested, Type> or (std::same_as<Tested, Types> or ...);

/*!
 * Concept to identify if a type is one of a set of distinct template types.
 *
 * \headerfile kh/base.hpp
 *
 * This is similar to the IsOneOf concept with non-templated types, but matches with any given template type. This
 * allows matching against a given set of template types, without needing a bunch of template specializations or
 * overloads. This concept simplifies taking template arguments, and you can easily define other concepts in terms of
 * this concept.
 */
template <typename Tested, template <typename...> typename Template, template <typename...> typename... Templates>
concept InstanceOf = Priv::InstanceOf<Tested, Template> or (Priv::InstanceOf<Tested, Templates> or ...);

/*!
 * Concept to identify types that are std::span of a given set of ElementTypes, or simply a span if no ElementTypes are
 * given.
 *
 * \headerfile kh/base.hpp
 *
 * This is similar to InstanceOf in that it is for identifying a std::span template type, however it only works for
 * std::span and InstanceOf cannot work because it only works with types that have a set of typenames they take as
 * parameters, concrete parameters like size_of break InstanceOf. This is the workaround, but unlike InstanceOf, this
 * concept accepts no elements and will match with any std::span whatsoever. This is useful to remove the need to
 * explicitly accept an Extent parameter in abbreviated template functions and methods.
 */
template <typename Tested, typename... ElementTypes>
concept SpanOf = requires {
    typename Tested::element_type;
    Priv::SpanOf<Tested, typename Tested::element_type>;
    ((sizeof...(ElementTypes) == 0) or ... or Priv::SpanOf<Tested, ElementTypes>);
};

/*!
 * Concept representing the different types that may bypass strict aliasing rules in C++.
 *
 * \headerfile kh/base.hpp
 *
 * Some types are not undefined behavior to dereference from a different type. Those three types are considered "byte
 * types" in KirHut software. Obviously, those three types are `char`, `unsigned char`, and std::byte.
 */
template <typename Tested>
concept ByteType = IsOneOf<Tested, char, byte, unsigned char, std::byte>;

/*!
 * Concept representing a span of some kind of ByteType.
 *
 * \headerfile kh/base.hpp
 *
 * The ByteType concept is useful for detecting particular types, but when you are specifically trying to get a span to
 * one of those types, this concept is a better fit for that purpose. Like SpanOf, this works despite the fact that
 * InstanceOf does not work for types that contain non-typename template parameters, namely size_t.
 */
template <typename Tested>
concept ByteSpan = SpanOf<Tested, char, byte, unsigned char, std::byte>;

/*!
 * Concept representing some Var or std::variant object that contains one of the given HasTypes.
 *
 * \headerfile kh/base.hpp
 *
 * This matches with types of std::variant (or the Var alias) that contain a given HasType as one of its type options.
 * This has a separated HasType and HasTypes types in the template because in abbreviated template method signatures or
 * when applying this concept to requires clauses, having an empty set of HasTypes should fail to compile, and the
 * separate HasType ensures that happens.
 */
template <typename VarType, typename HasType, typename... HasTypes>
concept HasTypeOption = Priv::HasTypeOption<VarType, HasType> or (Priv::HasTypeOption<VarType, HasTypes> or ...);

/*!
 * Concept that identifies an "empty" class, or a type that, when used as a parent class, will not increase the size of
 * the object.
 *
 * \headerfile kh/base.hpp
 *
 * This basically allows easy identification of types that can have the Empty Base Optimization applied to them or used
 * in a [[KH_NO_UNIQUE_ADDRESS]] context.
 */
template <typename EBO>
concept EmptyClass = sizeof(EBO) == 1 and sizeof(Priv::NoChild) == sizeof(Priv::EboChild<EBO>);

/*!
 * Concept representing a type T that can be contained in a given Var or std::variant.
 *
 * \headerfile kh/base.hpp
 *
 * This matches with types that are accepted by the std::variant (or the Var alias) given as type V as one of its type
 * options.
 */
template <typename HasType, typename VarType>
concept TypeOptionOf = Priv::HasTypeOption<VarType, HasType>;

/*!
 * Function that wraps a C pointer type in an unique pointer and calls the passed \p deleter once it falls out of scope.
 *
 * This is a very handy way to create \ref UPtr "UPtr"s to a C type of some kind from a C dependency. This is most
 * useful when it is itself wrapped in a factory function with a lambda that calls the C cleanup method with the pointer
 * passed to it. Then you call your factory function and get a C smart pointer that properly cleans up the given C
 * resource.
 *
 * \param cType The C struct type that needs to be cleaned up using a C function.
 * \param deleter The C function (or a function object wrapper) that will clean up the C struct.
 * \return A std::unique_ptr that contains the C type and will delete the type correctly.
 */
template <typename CType, std::invocable<CType *> Deleter>
UPtr<CType, Deleter> wrapCType(CType *cType, Deleter &&deleter) noexcept
{
    return { cType, forward<Deleter>(deleter) };
}

/*!
 * Function that wraps a C pointer type in an shared pointer and calls the passed \p deleter once it falls out of scope.
 *
 * This is a very handy way to create to a C type of some kind from a C dependency. This is most useful when it is
 * itself wrapped in a factory function with a lambda that calls the C cleanup method with the pointer passed to it.
 * Then you call your factory function and get a C smart pointer that properly cleans up the given C resource.
 *
 * \param cType The C struct type that needs to be cleaned up using a C function.
 * \param deleter The C function (or a function object wrapper) that will clean up the C struct.
 * \throws std::bad_alloc If allocation of the block used for the shared pointer resources fails.
 * \return A std::shared_ptr that contains the C type and will delete the type correctly.
 */
template <typename CType, std::invocable<CType *> Deleter>
SPtr<CType> wrapCTypeShared(CType *cType, Deleter &&deleter)
{
    return { cType, forward<Deleter>(deleter) };
}

/*!
 * Concept to identify objects that have constructors for a given argument that are marked explicit.
 *
 * \headerfile kh/base.hpp
 *
 * The standard library for some reason does not have a way to check in the type system whether or not a given
 * converting constructor is marked explicit, so this concept provides that.
 */
template <typename To, typename From>
concept ExplicitCtor = std::constructible_from<To, From> and not std::convertible_to<From, To>;

/*!
 * A function concept that modifies the data in a MaybeInv.
 *
 * \headerfile kh/base.hpp
 *
 * The type used for FirstArg here will determine if this may be used in MaybeInv::then() methods or in
 * MaybeInv::orElse() methods, and in both cases the argument passed as the first argument in the invocable type is
 * either the type being used or an Invalid type. Since the type that may be used in a MaybeInv could be anything, this
 * concept places no constraints on the FirstArg type.
 */
template <typename Function, typename FirstArg, typename... Args>
concept MaybeTransform = requires(Function func, FirstArg &&typeOrInv, Args... args) {
    { func(typeOrInv, std::forward<Args>(args)...) } -> InstanceOf<MaybeInv>;
};

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
    UnknownReason, //!< Invalid returned for unknown reason, either wasn't specified or couldn't be determined.
    BadArgument, //!< Invalid returned because an argument was not valid for some reason.
    NotFound, //!< Invalid returned because the object or file requested was not found.
    CouldntOpen, //!< Invalid returned because the file or data being accessed could not be opened.
    CorruptData, //!< Invalid returned because the data provided was corrupted or unparseable.
    OutOfBounds, //!< Invalid returned because the location requested was outside of the legal bounds.
    CantMatch, //!< Invalid returned because a pattern or value to match against could not be matched with the data set.
    DataRemoved, //!< Invalid returned because the data requested is no longer available.
    BadAllocation, //!< Invalid returned because a memory allocation failed.
};

/*!
 * Provides a simple interface with a message and a why for the failure.
 *
 * \headerfile kh/base.hpp
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
 *
 * All of the methods of this class are marked as constexpr, so this class should be completely useable in a constexpr
 * context. This allows the creation of functions and methods marked constexpr that return a MaybeInv<T> or Invalid and
 * this would compile.
 */
class KH_EXPORT Invalid final
{
    string info_d;
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
    constexpr explicit Invalid(WhyInvalid why) noexcept : why_d(why)
    {
        // No implementation.
    }

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
    constexpr Invalid(WhyInvalid why, string &&message) noexcept : info_d(std::move(message)), why_d(why)
    {
        // No implementation.
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
    constexpr Invalid(WhyInvalid why, char const *message) : Invalid(why, string_view(message))
    {
        // No implementation.
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
    constexpr Invalid(WhyInvalid why, string_view message) : info_d(message), why_d(why)
    {
        // No implementation.
    }

    /*!
     * Rule of 5 respecting destructor that is just the default.
     *
     * This class requires a non-standard move constructor and move assignment operator, so this exists to respect the
     * Rule of 5. It is simply set to the default.
     */
    constexpr ~Invalid() = default;

    /*!
     * Standard copy constructor.
     *
     * This is literally implemented with `= default`, so it is very much a standard copy constructor. This copies the
     * why and message objects, so it may throw a std::bad_alloc exception when creating the new string object. This is
     * here to respect the Rule of 5, as this class requires a non-standard move assignment operator and move
     * constructor.
     *
     * \param other The Invalid object to create this one from.
     * \throws std::bad_alloc If copying the \p other Invalid fails to allocate memory.
     */
    constexpr Invalid(Invalid const &other) = default;

    /*!
     * Standard move constructor.
     *
     * This moves the `why` and `message` properties, so it should never throw an exception as moving a std::string does
     * not throw exceptions.
     *
     * \param other The Invalid object to create this one from. The passed-in Invalid will be the same but have an
     * empty message.
     */
    constexpr Invalid(Invalid &&other) noexcept : Invalid(other.why(), std::move(other.info_d))
    {
        other.info_d.clear();
    }

    /*!
     * Standard copy assignment operator.
     *
     * This is literally implemented with `= default`, so it is very much a standard copy assignment operator. This
     * copies the why and message objects, so it may throw a std::bad_alloc exception when creating the new string
     * object. This is here to respect the Rule of 5, as this class requires a non-standard move assignment operator and
     * move constructor.
     *
     * \param other The Invalid object to copy from. The passed-in Invalid will be unmodified.
     */
    constexpr Invalid &operator=(Invalid const &other) noexcept = default;

    /*!
     * Standard move assignment operator.
     *
     * This moves the `why` and `message` properties, so it should never throw an exception as moving a std::string does
     * not throw exceptions.
     *
     * \param other The Invalid object to create this one from. The passed-in Invalid will be the same but have an
     * empty message.
     */
    constexpr Invalid &operator=(Invalid &&other) noexcept
    {
        why_d  = other.why();
        info_d = std::move(other.info_d);

        other.info_d = {};
        return *this;
    }

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
        return why_d;
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
    [[nodiscard]] constexpr string_view info() const noexcept
    {
        return info_d;
    }

    /*!
     * Set the message returned by info() to the passed in \p message.
     *
     * Use this method to set the \p message for the Invalid **if that message has not been set yet**. If the \p message
     * in this Invalid has been set (either through the constructor or a previous call to setInfo() or overwriteInfo()),
     * then this method will silently ignore the command. Use the overwriteInfo() method to guarantee that the content
     * of the string will be replaced.
     *
     * This is useful if you have several possible messages that can be chosen, and you can just set it to the message
     * if a setting is true and keep checking others. If one has already been set, the next set command is just ignored.
     *
     * The string rvalue reference overload of this method is noexcept, so it should be preferred, however sometimes you
     * must copy a string, in which case use the `const char *` or string_view overloads.
     *
     * \param message The content to set the Invalid's message to unless it has already been set.
     */
    constexpr void setInfo(string &&message) noexcept
    {
        if (info_d.empty())
        {
            overwriteInfo(std::move(message));
        }
    }

    /*!
     * \copydoc setInfo(string&&)noexcept
     *
     * \throws std::bad_alloc If copying the \p message fails to allocate memory.
     */
    constexpr void setInfo(char const *message)
    {
        if (info_d.empty())
        {
            overwriteInfo(message);
        }
    }

    /*!
     * \copydoc setInfo(string&&)noexcept
     *
     * \throws std::bad_alloc If copying the \p message fails to allocate memory.
     */
    constexpr void setInfo(string_view message)
    {
        if (info_d.empty())
        {
            overwriteInfo(message);
        }
    }

    /*!
     * Replace the current message with the passed in \p message.
     *
     * Use this method to set the message for the Invalid **regardless of whether or not it has already been set**. This
     * is usually what is thought of as a setter, but this explicitly states you want to replace a message in an Invalid
     * object, which is an uncommon thing to do.
     *
     * The string rvalue reference overload of this method is noexcept, so it should be preferred, however sometimes you
     * need to copy a string, in which case use the `const char *` or string_view overloads.
     *
     * \param message The content to set the Invalid's message to, even if it is already set.
     */
    constexpr void overwriteInfo(string &&message) noexcept
    {
        info_d = std::move(message);
    }

    /*!
     * \copydoc overwriteInfo(string&&)noexcept
     *
     * \throws std::bad_alloc If copying the \p message fails to allocate memory.
     */
    constexpr void overwriteInfo(char const *message)
    {
        info_d = message;
    }

    /*!
     * \copydoc overwriteInfo(string&&)noexcept
     *
     * \throws std::bad_alloc If copying the \p message fails to allocate memory.
     */
    constexpr void overwriteInfo(string_view message)
    {
        info_d = message;
    }

private:
    friend constexpr void swap(Invalid &, Invalid &) noexcept;
};

/*!
 * Basic swap method for the Invalid class.
 * \param i1
 * \param i2
 */
constexpr void swap(Invalid &i1, Invalid &i2) noexcept
{
    swap(i1.info_d, i2.info_d);
    auto temp = i1.why();
    i1.why_d  = i2.why();
    i2.why_d  = temp;
}

/*!
 * A class that resembles std::expected in C++23 but is slightly simpler.
 *
 * \headerfile kh/base.hpp
 *
 * Since KirHut codebases are still only using and expecting C++20, types like std::expected have yet to make it into
 * use. Further, the std::expected class has some small deficiencies, namely the mostly unnecessary "unexpected" type
 * (which for our use cases would just be defined `template <class Contained> using MaybeInv = std::expected<Contained,
 * Invalid>;`) has been removed, and this class is deliberately designed to never throw an exception or invoke undefined
 * behavior under any circumstances (after successful construction). Some methods, like a "value" method or `operator*`,
 * have been removed from the class, as you can simply get access using the get() method, or use the monadic
 * take(Contained&&) method to not deal with the potential null pointer being returned.
 *
 * The ideal way to use this class is like so:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Then on the user side:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * if (auto index = findIndex("some_value"))
 * {
 *   useIndex(*index.get());
 * }
 * else
 * {
 *   KirHut::out() << "findIndex() failed!\nReason: " << index.failure()->message() << "\n";
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * This class implements a subset of the monadic functionality in C++23 std::expected, such as then() and orElse(). This
 * system will propagate an Invalid properly from its base point over a series of operations, and allows simple chaining
 * of failable operations in a sequence of functions.
 */
template <typename Contained>
class MaybeInv final
{
    Var<Contained, Invalid> data;

public:
    /*!
     * Template typedef of the type expected to be returned by this MaybeInv.
     *
     * When working with templates that use MaybeInv, this will be the type returned by MaybeInv::take(). a MaybeInv is
     * wrapped in another typedef, like `typedef MaybeInv<int> MaybeInt`, this can be useful to get the type from it:
     *
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * typedef MaybeInv<int> MaybeInt;
     *
     * template <typename Contained>
     * Maybe<Contained>::JustType getValueOrMessage(Maybe<Contained> const &val, Maybe<Contained>::JustType &&alt)
     * {
     *     if (val.isValid())
     *     {
     *         return *val.get();
     *     }
     *
     *     IO::out().print("Invalid value attempted to convert.");
     *     return { forward<Maybe<Contained>::JustType>(alt) };
     * }
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    typedef Contained JustType;

    /*!
     * Template typedef for a pointer to the type expected to be returned by this MaybeInv.
     *
     * This should be a pointer to the underlying JustType, whatever that type is.
     */
    typedef Contained *JustPtr;

    /*!
     * Good object copying constructor.
     *
     * Usually you would use the rvalue reference constructor so as not to make unnecessary copies, however if that is
     * not an option this constructor still exists. This constructor is guaranteed not to throw an exception if the
     * Contained type copy constructor does not throw an exception.
     *
     * \param goodData Valid data to return from a function or method returning MaybeInv.
     * \throws ... Any exception thrown by the copy constructor for \p goodData may be thrown by this constructor.
     */
    constexpr MaybeInv(Contained const &goodData) noexcept(noexcept(Contained(goodData))) : data(goodData)
    {
        // No implementation.
    }

    /*!
     * Good object rvalue reference constructor.
     *
     * This constructor is called whenever you return a temporary object from a function or method, return an automatic
     * storage variable that falls out of scope after the return statement, or, ironically, use std::move to return a
     * named member variable. Generally you do not generall explicitly return a MaybeInv, and instead return a type
     * Contained, the MaybeInv object actually being returned still benefits from C++ RVO. Since this object wraps the
     * good data being returned, you want to move that data object if you can!
     *
     * \param goodData An rvalue reference to valid data to return from a function or method returning MaybeInv.
     * \throws ... Any exception thrown by the move constructor for \p goodData may be thrown by this constructor.
     */
    constexpr MaybeInv(Contained &&goodData) noexcept(noexcept(Contained(std::declval<Contained>()))) :
        data(std::move(goodData))
    {
        // No implementation.
    }

    /*!
     * Good object in-place constructor.
     *
     * Call this constructor to construct the returned good object in-place instead of needing to move it into the
     * returned MaybeInv. This makes it so there is only one move that is actually needed, the move from the MaybeInv to
     * the resulting place it needs to be. This can also be avoided by using the monadic interface to interact with the
     * object inside of the MaybeInv. This makes it so MaybeInv is a bit more efficient than std::expected.
     *
     * \param flag The KirHut::make object signifier to perform in-place construction.
     * \param ...args The arguments to pass to the constructor for the contained type of this MaybeInv.
     */
    template <typename... Args>
    requires std::constructible_from<Contained, Args...> constexpr explicit(sizeof...(Args) == 0)
        MaybeInv([[maybe_unused]] MakeFlag const &flag,
                 Args &&...args) noexcept(noexcept(Contained(forward<Args>(args)...))) :
        data(inpT, forward<Args>(args)...)
    {
        // No implementation.
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
        // No implementation.
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
        // No implementation.
    }

    /*!
     * Invalid MaybeInv constructor without a string message.
     *
     * This constructor is useful for when you need to
     * \param why
     */
    constexpr explicit MaybeInv(WhyInvalid why) noexcept : data(inpInvalid, why)
    {
        // No implementation.
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
        // No implementation.
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
        // No implementation.
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
    [[nodiscard]] constexpr bool isValid() const noexcept
    {
        return std::holds_alternative<Contained>(data);
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
    [[nodiscard]] constexpr Contained const *get() const noexcept
    {
        return std::get_if<Contained>(&data);
    }

    /*!
     * \copydoc get()const noexcept
     */
    [[nodiscard]] constexpr Contained *get() noexcept
    {
        return std::get_if<Contained>(&data);
    }

    /*!
     * Take the object out of this method, providing an alternate object if there is no valid data.
     *
     * You may or may not pass an rvalue reference to another Contained object to this method as an alternate in the
     * case that you are attempting to take() from a MaybeInv with Invalid data. When you do provide an alternate, this
     * method is guaranteed not to call std::terminate().
     *
     * \param alt An alternative object to return in the place of the data if the data is not valid.
     * \throws ... Any exception thrown by the move constructor for type Contained.
     * \return The data in this object, returned using an rvalue reference to move the value, or the alternate if the
     * data is not valid.
     */
    [[nodiscard]] constexpr Contained take(Contained &&alt) noexcept(noexcept(Contained(std::declval<Contained>())))
    {
        // Trust in NRVO. This should result in only one move construction.
        Contained ret = isValid() ? std::move(*get()) : std::move(alt);
        data  = Invalid{ WhyInvalid::DataRemoved };
        return ret;
    }

    /*!
     * Take the object out of this method, and fatally crash if no such object exists.
     *
     * You may or may not pass an rvalue reference to another Contained object to this method as an alternate in the
     * case that you are attempting to take() from a MaybeInv with Invalid data. When you do provide an alternate, this
     * method is guaranteed not to call std::terminate().
     *
     * \warning If you call this method and there is not valid data in this object to take, this method will call
     * std::terminate()! This includes if there used to be valid data and it was already taken! You should always use
     * the method with an alternate, use get(), or check this object is valid before calling this method without an
     * alternate.
     *
     * \param alt An alternative object to return in the place of the data if the data is not valid.
     * \throws NoValidData If the MaybeInv is in an Invalid state when this method is called.
     * \throws ... Any exception thrown by the move constructor for type Contained.
     * \return The data in this object, returned using an rvalue reference to move the value, or the alternate if the
     * data is not valid.
     */
    [[nodiscard]] constexpr Contained take()
    {
        if (isValid())
        {
            // Trust in NRVO. This should result in only one move construction.
            Contained ret = std::move(*get());
            data  = Invalid{ WhyInvalid::DataRemoved };
            return ret;
        }

        // This shouldn't happen if the object isn't invalid. Never call this method on an Invalid MaybeInv.
        Priv::throwNoValidData(*failure());
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
     * This function requires that
     * \param function
     * \param args
     * \return A MaybeInv of the type returned by \p function. It may be initialized with this MaybeInv's Invalid.
     */
    template <typename... Args>
    constexpr auto then(MaybeTransform<Contained, Args...> auto &function, Args &&...args) const &
    {
        typedef decltype(function(*get(), std::forward<Args>(args)...)) RetType;
        return isValid() ? function(*get(), std::forward<Args>(args)...) : RetType{ Invalid{ *failure() } };
    }

    template <typename... Args>
    constexpr auto then(MaybeTransform<Contained, Args...> auto &function, Args &&...args) &
    {
        typedef decltype(function(*get(), std::forward<Args>(args)...)) RetType;
        return isValid() ? function(*get(), std::forward<Args>(args)...) : RetType{ Invalid{ *failure() } };
    }

    /*!
     * \copydoc then(MaybeTransform<T,Args...>auto&,Args&&...)const&
     */
    template <typename... Args>
    constexpr auto then(MaybeTransform<Contained, Args...> auto &function, Args &&...args) &&
    {
        typedef decltype(function(*get(), std::forward<Args>(args)...)) RetType;
        return isValid() ? function(take(), std::forward<Args>(args)...) : RetType{ std::move(*failure()) };
    }

    /*!
     * Monadic response method that always returns itself as this does not make a modified version of the MaybeInv.
     *
     * This method is useful for an orElse if you do not intend on transforming the value, as it will never make a copy
     * of the
     * \param function
     * \param args
     * \return
     */
    template <typename... Args>
    constexpr MaybeInv<Contained> &orElse(std::invocable<Args &&...> auto &function, Args &&...args)
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
     * If you do not need the Invalid
     * \param function
     * \param args
     * \return
     */
    template <typename... Args>
    constexpr auto orElse(MaybeTransform<Invalid const &, Args &&...> auto &function, Args &&...args)
    {
        typedef decltype(function(*failure(), forward<Args>(args)...)) RetType;
        return !isValid() ? function(*failure(), forward<Args>(args)...) : RetType{ *this };
    }

private:
    friend constexpr void swap(MaybeInv<Contained> &, MaybeInv<Contained> &);

    constexpr static auto inpT       = std::in_place_type<Contained>;
    constexpr static auto inpInvalid = std::in_place_type<Invalid>;
};

/*!
 * Swap for MaybeInv types that swaps the contained information.
 *
 * This includes if one of them is an Invalid, if both are Invalid, or if neither are. They will be swapped in the way
 * that you would imagine, so if \p m1 is an Invalid and \p m2 is not, after the swap \p m2 will be invalid and \p m1
 * will not be. The valid data will also be swapped.
 *
 * This can only be done on MaybeInv types that contain the same type, not any that contain different types.
 *
 * \param m1 First MaybeInv argument.
 * \param m2 Second MaybeInv argument.
 */
template <typename Contained>
constexpr void swap(MaybeInv<Contained> &m1, MaybeInv<Contained> &m2) noexcept(noexcept(swap(m1.data, m2.data)))
{
    swap(m1.data, m2.data);
}

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
 * Convert an array of constant char data that ends with '\0' to a string.
 *
 * This simply constructs a std::string around the data and does nothing else. Since this copies the data this function
 * is not marked `noexcept`, and it is only here for completeness with the other versions of toStr(). There is otherwise
 * no reason to use this function.
 *
 * \param data A pointer to an array of chars that ends with '\0'. The behavior is undefined if this is not the case.
 * \return A string of the data passed to this function.
 */
[[nodiscard]] string toStr(char const *data);

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
 * \throws std::bad_alloc If the std::u8string memory allocation fails.
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

/*!
 * Get the current time as a count of "ticks," or the number of units in the smallest measure available on the system.
 *
 * The returned value isn't guaranteed to mean much more than that it will be greater than the values returned in
 * previous calls, and that it will continue to change through successive calls of the method on the same system. There
 * is little way to know what unit was actually returned. It *should* never loop, given how large u64 is, but even this
 * is something that this method cannot actually guarantee. This should be useful as input to a non-secure random number
 * seed.
 *
 * All this method does is call `std::chrono::high_resolution_clock::now().time_since_epoch().count()`.
 *
 * \return A number of "ticks" since the beginning of some system dependent time period.
 */
[[nodiscard]] KH_EXPORT u64 currentTicks() noexcept;

/*!
 * Assert that something is true in a way that affects both Debug and Release builds in helpful ways.
 *
 * Unlike the standard C assert macro that compiles to nothing when compiled in release mode, this function is just a
 * function that is always called and the condition is always executed. As such, this is not vulnerable to the kinds of
 * bugs that expect the condition to execute as part of its functionality, as it will always execute, however as a
 * disadvantage, this method will not be compiled out of an executable.
 *
 * In Debug mode, this method just calls assert(condition), which allows it to have the functionality of an assert in
 * your IDE. In release mode, this throws a KirHutSucksAtProgramming Exception with the given message that is passed in
 * as an argument. If this method does not throw, the string passed in as an argument will not be modified.
 *
 * \param condition Usually an expression that should evaluate to true. If it is false, this calls assert or throws.
 * \param message A message, that may be a temporary string, a string_view, or a const char *.
 * \throws KirHutSucksAtProgramming if the \p condition is false and KirHut::Build::release is true.
 */
KH_EXPORT void khAssert(bool condition, string &&message);

/*!
 * \copydoc khAssert(bool,string&&)
 * \throws std::bad_alloc If a KirHutSucksAtProgramming Exception would have been thrown but the string failed to
 * allocate memory for the copy.
 */
KH_EXPORT void khAssert(bool condition, string_view message);

/*!
 * \copydoc khAssert(bool,string&&)
 * \throws std::bad_alloc If a KirHutSucksAtProgramming Exception would have been thrown but the string failed to
 * allocate memory for the copy.
 */
KH_EXPORT void khAssert(bool condition, char const *message);

/*!
 * An Assert version that is never meant to be used, only compiled against.
 *
 * This method unconditionally throws KirHutSucksAtProgramming, because this function is never meant to be called. This
 * function is meant to allow compilation of an invalid if constexpr condition when there are no arguments in the
 * KH_ASSERT macro. If you are trying to assert on a condition with no message, use KH_ASSERT instead.
 *
 * \param condition A condition that is ignored and this method always throws KirHutSucksAtProgramming, even in Debug.
 */
KH_EXPORT void khAssert(bool condition);

/*!
 * \def KH_ASSERT
 *
 * Similar to the assert functionality in C and C++, however throws an exception at Runtime instead of compiling out.
 *
 * Conditions that are "asserted" to be true should always be true regardless of what possible issues there are. This
 * macro, unlike the C macro, is executed in both Debug and Release mode, so you can count on the condition being
 * executed and this simply does two different things depending on the mode. In Debug, this calls assert(), and in
 * Release, this checks the condition and throws a KirHutSucksAtProgramming Exception if it is false. This macro is
 * meant to be used instead of the lowercase khAssert methods in nearly every case because the macro allows this assert
 * to get the character text of the actual condition being used, which is otherwise impossible in standard C++. This
 * assert macro must be ended with a semicolon like the standard C assert() macro or this will fail to compile.
 *
 * The signature reports to take multiple arguments, but it will just fail to compile if you pass more than one. It is
 * written this way to accept if there are no arguments, however it is effectively just one. The argument is for a
 * message of some kind, either a string, a string_view, or a `const char *`. If no message is provided, a default
 * message that states "Assertion failed: " and a string of the condition is thrown instead.
 *
 * \param COND An expression that is the condition being asserted. True will mean this does nothing, and false means
 * this has failed the assertion.
 * \hideinitializer
 */
#define KH_ASSERT(COND, ...)                          \
    if constexpr (string_view("") == #__VA_ARGS__)    \
        khAssert((COND), "Assertion failed: " #COND); \
    else                                              \
        khAssert((COND), __VA_ARGS__)

} // namespace KirHut
