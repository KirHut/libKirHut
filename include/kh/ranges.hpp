/***********************************************************************************************************************
** The KirHut Application Development Library
** ranges.hpp
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

#include <ranges> // IWYU pragma: export

namespace KirHut
{

/*!
 * Namespace alias for std::ranges.
 *
 * Allows for efficient and fast access to ranges algorithms.
 */
namespace R = std::ranges;

/*!
 * Namespace alias for std::views.
 *
 * Allows for efficient and fast access to views algorithms.
 */
namespace V = std::views;

/*!
 * Simple Utility range function that puts the std::ranges::begin() and std::ranges::end() iterators in a std::pair.
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
[[nodiscard]] constexpr auto getIters(R::range auto &r) noexcept -> decltype(std::pair{ R::begin(r), R::end(r) })
{
    return std::pair{ R::begin(r), R::end(r) };
}

using std::back_inserter;

} // namespace KirHut
