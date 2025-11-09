/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/ranges.hpp
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

#include <ranges> // IWYU pragma: export

#include "kh/base.hpp"

/*!
 * \file ranges.hpp
 * \brief Range-based utility types and adaptors.
 *
 * This header provides lightweight range utilities for the KirHut project, intended to extend the C++20 ranges library
 * with small, practical helpers. These are designed for simplicity and constexpr-friendliness, not for replacing
 * full-featured parsing libraries.
 *
 * Currently includes:
 *
 *  - KirHut::R::WordView: a forward range over tokens in a string delimited by separators.
 *  - KirHut::V::words: a range adaptor object for using a WordView object over a given string_view.
 *  - KirHut::R::getIters: a method that returns the begin() and end() iterators of a range in a pair.
 */

namespace KirHut
{

/*!
 * Namespace for range utilities in libKirHut.
 *
 * This mostly is just an alias to std::ranges, but also comes with some additional range-based utility classes and
 * functions.
 */
namespace R
{

using namespace std::ranges;

/*!
 * Simple Utility function that puts the std::ranges::begin() and std::ranges::end() iterators in a std::pair.
 *
 * The returned value is always a std::pair of std::ranges iterators. The intent is to assign the returned value to a
 * structured binding to make getting the begin and end iterators of a range very easy.
 *
 * ~~~
 * auto [begin, end] = getIters(someRange);
 * ~~~
 *
 * Why doesn't this exist in the standard library? I haven't the faintest idea.
 *
 * \param r A std::ranges::range type to get the begin and end iterators from.
 * \return A std::pair of the std::range::begin() and std::range::end() iterators.
 */
[[nodiscard]] constexpr auto getIters(range auto &r) noexcept -> decltype(std::pair{ R::begin(r), R::end(r) })
{
    return std::pair{ R::begin(r), R::end(r) };
}

/*!
 * Class representing a group of separators for a WordView.
 *
 * This class merely wraps a std::string_view object of separator characters as a tag type. The class signifies that the
 * contained string_view is a set of separators and will be treated as a different type by the compiler.
 *
 * \tparam Char_T The character type used by this Separators object for the internal string_view.
 * \see WordView
 */
template <typename Char_T>
struct Separators final
{
    /*!
     * \brief separators
     */
    std::basic_string_view<Char_T> separators;

    /*!
     * Construct a Separators from a given string of separator characters.
     *
     * This object does not copy the passed in character buffer, so you must ensure that the buffer provided to this
     * object will be valid for the entire lifetime of the Separators object. The advantage of making no copies is this
     * constructor is extremely fast and guaranteed to work.
     *
     * \param seps
     */
    explicit constexpr Separators(std::basic_string_view<Char_T> seps) noexcept : separators(seps)
    {
        // No further implementation.
        // TODO: Implement some compile-time checking for seps if desired.
    }

    /*!
     * Construct a Separators from a given string of separator characters.
     *
     * This object does not copy the passed in character buffer, so you must ensure that the buffer provided to this
     * object will be valid for the entire lifetime of the Separators object. The advantage of making no copies is this
     * constructor is extremely fast and guaranteed to work.
     *
     * \param seps
     */
    explicit constexpr Separators(Char_T const *seps) noexcept : separators(seps)
    {
        // No further implementation.
        // TODO: Implement some compile-time checking for seps if desired.
    }

    /*!
     * Implicit conversion from Separators<Char_T> to std::basic_string_view<Char_T>.
     *
     * This object is just a wrapper type that stores a string view of separator characters. This allows for easier
     * access to the separators within this object.
     */
    constexpr operator std::basic_string_view<Char_T>() const noexcept
    {
        return separators;
    }
};

/*!
 * A lightweight view that splits a string into "words" given a series of ignored characters.
 *
 * WordView provides a forward range over "words," or rather groups of symbols that are not in the passed-in group of
 * "separator" characters. This is commonly done with whitespace splitting, but is also very useful in CSV processing,
 * command parsing, and other simple splitting purposes. Each word is delimited by a set of "Separator" characters that,
 * if any appear in the string, are used as the "word boundaries" between the split groups of words.
 *
 * This object models `std::ranges::forward_range` and can be used in range-based for loops, or with `<algorithm>` /
 * `<ranges>` algorithms. It is also models the `std::ranges::view` concept, meaning that copies and move operations are
 * trivial for this object.
 *
 * This class is intended for small, compile-time–friendly parsing tasks (e.g. validating option strings), not for
 * heavy-duty text processing.
 *
 * Example usage:
 *
 * ~~~
 * auto printline = [](std::string_view tok) { std::cout << tok << "\n"; };
 * R::WordView tv("alpha beta gamma");
 * R::WordView csv("options, in, csv, 123", Separators{ " ," });
 * R::for_each(tv, printline);
 * R::for_each(csv, printline);
 *
 * // Output:
 * // alpha
 * // beta
 * // gamma
 * // options
 * // in
 * // csv
 * // 123
 * ~~~
 *
 * \tparam Char_T
 */
template <typename Char_T>
class WordView final : public view_interface<WordView<Char_T>>
{
    std::basic_string_view<Char_T> fullString, separators;

public:
    class iterator;

    /*!
     * Required WordView default constructor.
     *
     * This does not initialize the WordView object into any kind of useful state, other than one to be reassigned to
     * from another WordView. The returned iterators will always just be the default end iterator.
     */
    constexpr WordView() noexcept : WordView("")
    {
        // No further implementation.
    }

    /*!
     * Construct a WordView over a string with optional separators.
     *
     * WordView is a view that maps over a given \p fullStr of Char_T characters and each element of this view is the
     * "words" that exist between a fixed set of \p seps Separators. Each returned word is a substring within the actual
     * data in \p fullStr, separated by one or more characters found in \p seps. Unlike with `std::ranges::split()`,
     * this object will split each token when any character in \p seps is found within the \p fullStr, the separation
     * does *not* have to match the full \p seps string.
     *
     * \param fullStr The string to tokenize. Must not be empty.
     * \param seps A set of separator characters. Defaults to whitespace (space, tab, carriage return, newline).
     */
    constexpr explicit WordView(std::basic_string_view<Char_T> fullStr,
                                Separators<Char_T> seps = Separators<Char_T>{ " \t\r\n" }) noexcept :
        fullString(fullStr),
        separators(seps)
    {
        // No further implementation.
    }

    /*!
     * \brief WordView
     * \param seps
     * \param fullStr
     */
    constexpr WordView(Separators<Char_T> seps, std::basic_string_view<Char_T> fullStr) noexcept :
        WordView(fullStr, seps)
    {
        // No further implementation.
    }

    /*!
     * Get an iterator to the first word.
     *
     * See the documentation on the iterator type for more information on how to use the returned iterator.
     *
     * \internal
     * \note The definition of this method is found outside of the class body, unlike most class methods, because it
     * must return an object which has not itself been defined yet (the iterator object). As such, it must be defined
     * under the definition of the iterator object.
     * \endinternal
     *
     * \return An iterator to the first word found in the underlying string_view, or the default end iterator if none
     * are found.
     */
    constexpr iterator begin() const noexcept;

    /*!
     * Get an end sentinel iterator.
     *
     * The returned iterator is equivalent to default constructing WordView<Char_T>::iterator.
     *
     * \internal
     * \note The definition of this method is found outside of the class body, unlike most class methods, because it
     * must return an object which has not itself been defined yet (the iterator object). As such, it must be defined
     * under the definition of the iterator object.
     * \endinternal
     *
     * \return The default end sentinel iterator.
     */
    constexpr iterator end() const noexcept;

    /*!
     * Compare two WordViews for equality.
     *
     * Two WordViews are equal if they have identical source strings and separator sets.
     */
    constexpr bool operator==(WordView const &other) const noexcept
    {
        // A naked string_view::operator== comparison would perform string comparisons with fullString and separators,
        // which is not the correct thing to do for either collection. If two different WordView objects are pointing at
        // two different string locations that happen to have the same character data, they should still be considered
        // different WordViews since those views are viewing two different places. So this comparison uses the data()
        // addresses and size()s returned by the two fullStrings to compare, to ensure that it is actually pointing at
        // the same data. For separators, even if two sets of separators are in different order, the result is still
        // identical, so those WordView objects are considered equal. As such, we just check that all of the separators
        // found in this one, and vice versa.

        for (Char_T c : other.separators)
        {
            if (separators.find(c) == npos)
            {
                return false;
            }
        }

        for (Char_T c : separators)
        {
            if (other.separators.find(c) == npos)
            {
                return false;
            }
        }

        return fullString.data() == other.fullString.data() and fullString.size() == other.fullString.size();
    }

private:
    /*!
     * \internal
     *
     * Simple alias name for the enclosed std::basic_string_view object's npos value.
     */
    constexpr static size_t npos = std::basic_string_view<Char_T>::npos;
};

template <StringLike String_T>
WordView(String_T) -> WordView<StringLikeType<String_T>>;

template <StringLike String_T>
WordView(String_T, R::Separators<StringLikeType<String_T>>) -> WordView<StringLikeType<String_T>>;

/*!
 * Forward iterator over the tokens in a WordView.
 *
 * Iterators yield `std::string_view` elements referring into the underlying string. Incrementing skips over separator
 * runs and returns the next token, or an empty view when finished.
 *
 * End iterators are a special type of iterator called a "sentinel iterator" that is what an iterator becomes once it
 * no longer points to any word data. This sentinel iterator always returns an empty string on dereference, always
 * compares equal to all other sentinel iterators (including those returned by other WordView objects),
 *
 * \tparam Char_T The character type of this iterator's WordView type.
 */
template <typename Char_T>
class WordView<Char_T>::iterator final
{
    /*!
     * \internal
     *
     * \brief parent
     */
    WordView const *parent = nullptr;

    /*!
     * \internal
     *
     * \brief last
     */
    std::basic_string_view<Char_T> last = {};

public:
    /*!
     * The type of each element in WordView.
     *
     * A WordView is templated to a character type, and the string view objects returned by the iterator on dereference
     * are basic_string_view types templated to that Char_T.
     */
    using value_type = std::basic_string_view<Char_T>;

    /*!
     * A difference type (unused).
     *
     * This type is a requirement of the iterator concept, but WordView does not model a random access view, so the
     * difference_type is never used. It is just set to std::ptrdiff_t.
     */
    using difference_type = std::ptrdiff_t;

    /*!
     * What iterator concept this iterator models.
     *
     * This iterator models a C++ forward iterator, so this returns std::forward_iterator_tag.
     */
    constexpr static auto iterator_concept = std::forward_iterator_tag{};

    /*!
     * What iterator category this iterator is under.
     *
     * This iterator is a C++ forward iterator, so this returns std::forward_iterator_tag.
     */
    constexpr static auto iterator_category = iterator_concept;

    /*!
     * Construct an iterator bound to a WordView.
     *
     * This will create an iterator based on a given WordView, which will start with operator*() returning the first
     * valid token in the stream, or an empty string_view if the string has no valid tokens. If this is the case, the
     * WordView::empty() method will return true.
     *
     * \param p The WordView this iterator will walk. Passed as reference to ensure a nullptr is not passed.
     */
    constexpr iterator(WordView const &p) noexcept : parent(&p)
    {
        if (p.fullString.empty())
        {
            parent = nullptr;
        }

        nextToken();
    }

    /*!
     * Default constructor, creates an "end" sentinel.
     *
     * The iterator type returned is guaranteed to match with all iterators returned by any KirHut::R::WordView::end()
     * method, and any iterator returned by KirHut::R::WordView::begin() that has been incremented beyond the last
     * valid token. This includes if there are no valid tokens, in which case `begin() == end()` is true.
     */
    constexpr iterator() noexcept = default;

    /*!
     * Dereference the current token.
     *
     * \return A string_view of the current token.
     */
    constexpr value_type operator*() const noexcept
    {
        return last;
    }

    /*!
     * Advance to the next token.
     *
     * \return Reference to this iterator.
     */
    constexpr iterator &operator++() noexcept
    {
        nextToken();
        return *this;
    }

    /*!
     * Post-increment: advance and return old iterator.
     */
    constexpr iterator operator++(int) noexcept
    {
        iterator ret = *this;
        ++*this;
        return ret;
    }

    /*!
     * Compare two iterators for equality.
     *
     * \note Two default-constructed iterators (ends) always compare equal, even if they come from different WordView
     * objects.
     */
    constexpr bool operator==(iterator const &other) const noexcept
    {
        if (parent == nullptr or other.parent == nullptr)
        {
            return last == other.last;
        }

        return *parent == *other.parent and last == other.last;
    }

private:
    /*!
     * \internal
     *
     * \brief nextToken
     */
    constexpr void nextToken() noexcept
    {
        if (not parent)
        {
            // Silently ignore nextToken() calls...
            return;
        }

        size_t fromPos = 0;
        if (not last.empty())
        {
            fromPos = static_cast<size_t>(last.data() - parent->fullString.data()) + last.size();
        }

        if (parent->separators.empty())
        {
            last = parent->fullString.substr(fromPos);
            return;
        }

        size_t start = parent->fullString.find_first_not_of(parent->separators, fromPos);
        if (start == std::string_view::npos)
        {
            last = {};
            return;
        }

        size_t end = parent->fullString.find_first_of(parent->separators, start);
        if (end == std::string_view::npos)
        {
            end = parent->fullString.size();
        }

        last = { &parent->fullString[start], end - start };
    }
};

// Documented in object definition.
template <typename Char_T>
constexpr WordView<Char_T>::iterator WordView<Char_T>::begin() const noexcept
{
    return { *this };
}

// Documented in object definition.
template <typename Char_T>
constexpr WordView<Char_T>::iterator WordView<Char_T>::end() const noexcept
{
    return {};
}

} // namespace R

/*!
 * Namespace alias for std::views.
 *
 * Allows for efficient and fast access to views algorithms.
 */
namespace V
{

using namespace std::views;

/*!
 * \internal
 *
 * Detail namespace for KirHut::V.
 *
 * Just another Detail namespace, see KirHut::Detail for information.
 *
 * \see KirHut::Detail
 */
namespace Detail
{

/*!
 * \internal
 *
 * CPO for the V::words function.
 *
 * \see V::words()
 */
struct WordsFn final
{
    /*!
     * \internal
     *
     * The WithSeparators class
     */
    template <typename Char_T>
    struct WithSeparators final
    {
        /*!
         * \internal
         *
         * \brief separators
         */
        R::Separators<Char_T> separators;

        /*!
         * \internal
         *
         * \brief operator ()
         * \param s
         * \return
         */
        constexpr R::WordView<Char_T> operator()(R::contiguous_range auto &&data) const noexcept
        {
            return R::WordView(std::forward<std::remove_reference_t<decltype(data)>>(data), separators);
        }

        /*!
         * \internal
         *
         * \brief operator |
         * \param data
         * \param wseps
         */
        friend constexpr auto operator|(R::contiguous_range auto &&data, WithSeparators const &wseps) noexcept
        {
            return wseps(std::forward<std::remove_reference_t<decltype(data)>>(data));
        }
    };

    /*!
     * \internal
     *
     * \brief operator ()
     * \param s
     * \return
     */
    constexpr auto operator()(R::contiguous_range auto &&data) const noexcept
    {
        return R::WordView(std::forward<std::remove_reference_t<decltype(data)>>(data));
    }

    /*!
     * \internal
     *
     * \brief operator ()
     * \param s
     * \return
     */
    template <typename Char_T>
    constexpr R::WordView<Char_T> operator()(R::contiguous_range auto &&data, R::Separators<Char_T> seps) const noexcept
    {
        return R::WordView(std::forward<std::remove_reference_t<decltype(data)>>(data), seps);
    }

    /*!
     * \internal
     *
     * \brief operator ()
     * \param seps
     * \return
     */
    template <typename Char_T>
    constexpr WithSeparators<Char_T> operator()(R::Separators<Char_T> seps) const noexcept
    {
        return WithSeparators(seps);
    }

    /*!
     * \internal
     *
     * \brief operator |
     * \param data
     * \param self
     */
    friend constexpr auto operator|(R::contiguous_range auto &&data, WordsFn const &self) noexcept
    {
        return self(std::forward<std::remove_reference_t<decltype(data)>>(data));
    }
};

} // namespace Detail

/*!
 * \fn auto words(R::contiguous_range auto &&data)
 * Convenience factory CPO function for WordView.
 *
 * This creates a WordView over the provided string, splitting on the default
 * whitespace separator set.
 *
 * Equivalent to:
 *
 * ~~~
 * WordView tv(data);
 * ~~~
 *
 * but more concise in range-based contexts:
 *
 * ~~~
 * for (auto word : words("a b c")) {
 *     // ...
 * }
 * ~~~
 *
 * \param data The string to split.
 * \return A WordView over the given string.
 */
#if defined(KH_PRIV_DOCS)
auto words(R::contiguous_range auto &&data);
#endif

namespace
{

/*!
 * \internal
 *
 * Reference to Detail::WordsFn using a static const reference generating template type, to avoid ODR violations.
 *
 * All CPO implementations in libKirHut follow the recommendations found in Open-Std N4381:
 * https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html
 *
 * \see KirHut::Detail::staticConstRef
 */
[[maybe_unused]] constexpr auto &words = KirHut::Detail::staticConstRef<Detail::WordsFn>;

} // namespace

} // namespace V

using std::back_inserter;

} // namespace KirHut
