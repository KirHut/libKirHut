/***********************************************************************************************************************
** The KirHut Library for the Public Benefit
** base.hpp
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

/*!
 * \file base.hpp
 *
 * The KirHut Standard (or "Base") include file.
 *
 * This file provides all of the basic types and build information provided by kh/global.hpp and also includes several
 * basic standard library types in the KirHut namespace.
 */

#include "kh/global.hpp"

#include <string>
#include <string_view>
#include <memory>
#include <array>
#include <variant>
#include <optional>

/*!
 * The primary namespace for all KirHut software, including libraries, applications, and plugins.
 *
 * This namespace is used everywhere in KirHut C++ software, and ensures that no name conflicts should ever occur when
 * mixing KirHut software with any other software, including software not within a namespace (Like Qt software). Within
 * this namespace there should be additional namespaces, primarily the application namespaces (which by convention are
 * simple abbreviations of the software name, like "PSM" for the KirHut Passwords and Secrets Manager or "SDV" for the
 * KirHut Simple Data Verifier. There are other sub namespaces where functionality is provided, and they are documented
 * individually within this namespace.
 *
 * The KirHut namespace also import a selection of C++ standard library names. These names are usually aliased, but are
 * also frequently not, so it will depend on the situation and they are also individually documented. The ideal way to
 * use the KirHut namespace in code is to directly use the entire namespace, a 'la `using namespace KirHut`. The KirHut
 * libraries avoid the hazards of `using namespace std` because this does not include the entire standard library, and
 * name conflicts are far less likely when the standard names are aliased and is a curated set of the functionality.
 *
 * All KirHut software written in C++ requires C++20 as a base.
 */
namespace KirHut
{

/*!
 * Alias name for std::u8string.
 *
 * All KirHut applications attempt to use UTF-8 wherever possible, despite the fact that Qt uses UTF-16 strings.
 */
using String = std::u8string;

/*!
 * Alias name for std::u8string_view.
 *
 * \copydetails KirHut::String
 */
using StringView = std::u8string_view;

/*!
 * Alias name for std::unique_ptr.
 *
 * The smart pointer names are a bit long, so this makes it easier to type out and have in method signatures.
 */
template <typename T>
using UPtr = std::unique_ptr<T>;
/*!
 * Alias name for std::shared_ptr.
 *
 * \copydetails KirHut::UPtr
 */
template <typename T>
using SPtr = std::shared_ptr<T>;
/*!
 * Import of std::make_unique into the KirHut namespace.
 */
using std::make_unique;
/*!
 * Import of std::make_shared into the KirHut namespace.
 */
using std::make_shared;
/*!
 * Import of std::bad_alloc
 *
 * Shit happens.
 */
using std::bad_alloc;

/*!
 * Alias name for std::variant.
 *
 * This is used purely to shorten the name since this is common in method signatures.
 */
template <typename... Ts>
using Var = std::variant<Ts...>;
/*!
 * Import of the std::get function into the KirHut namespace.
 */
using std::get;

/*!
 * Alias name for std::optional.
 *
 * Slightly shorter name that is more descriptive of intent then "optional."
 */
template <typename T>
using Maybe = std::optional<T>;

/*!
 * Import of std::array type into the KirHut namespace.
 */
using std::array;

using namespace std::literals::string_view_literals;

/*!
 * Return a `char const *` from a StringView.
 *
 * This accesses the underlying data of the StringView using a `char const *` instead of a `char8_t const *`. It is
 * frequently useful to do this, and because C++ `char` type is one of the "blessed types" that can pierce through the
 * strict aliasing rules of C++. This makes the operation as simple as a `reinterpret_cast`.
 *
 * Because this uses `reinterpret_cast`, this cannot be marked `constexpr`. Otherwise it would be.
 *
 * \param in The StringView object to get the underlying data of and cast to `char const *`.
 * \return A `char const *` to the underlying data from \p in.
 */
inline char const *charPtr(StringView in) noexcept
{
    return reinterpret_cast<char const *>(in.data());
}

#if KH_PRIV_DOCS || KH_USES_QT
/*!
 * Conversion function for String to QString.
 *
 * This creates a QByteArray with the data pointed to by \p in and the length, ensuring no copies are made and the
 * QString ctor does not need to get the length of the data. This function still runs at **O(n)** because it must still
 * make at least one copy of the string data to the underlying UTF-16 QString.
 *
 * \param in A std::u8string_view (with alias StringView) to convert to a QString.
 * \throws bad_alloc If the QString constructor fails to allocate memory.
 * \return A QString of the contents in the passed String.
 */
KH_EXPORT QString toQStr(StringView in);

/*!
 * Conversion function for String to QString.
 *
 * This uses QString::toUtf8() to create a UTF-8 QByteArray of the QString data, then must perform a second copy of that
 * UTF-8 data to the returned String objects `char8_t` data buffer due to C++'s strict aliasing rules. It will then
 * dispose of the QByteArray. This function still runs at **O(n)** time efficiency, despite the dual copies.
 *
 * \param in A QString to convert to a std::u8string (with alias String).
 * \throws bad_alloc If the String memory allocation fails.
 * \return A String copy of the contents in the passed QString.
 */
KH_EXPORT String toStr(QString const &in);
#endif

} // namespace KirHut
