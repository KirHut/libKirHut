/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/baseio.hpp
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
 * \file kh/baseio.hpp
 *
 * The KirHut IO Base include file.
 *
 * This file provides all of the basic requirements for the KirHut IO system that is found under the KirHut::IO
 * namespace. Generally speaking, you would not need to include this file directly in your code because it will be
 * included in any header file that has classes or methods under that namespace.
 *
 * \see KirHut::IO
 */

#include "kh/global.hpp"

/*!
 * Namespace for all IO functionality found within libKirHut.
 *
 * I'll finish this description later...
 */
namespace KirHut::IO
{

/*!
 * An enumeration that signifies which location that a given index is *in relation* to another location.
 *
 * The standard IO streams have the notion of seeking the cursor position based on the beginning, the end, or the
 * current position in the input/output stream. This is retained with KirHut::SeekableInput and KirHut::SeekableOutput,
 * however the in relation information was put into an enumeration for ease of use and readability.
 */
enum class Relation
{
    BEGIN, //!< Move in relation to the beginning of the data.
    CURRENT, //!< Move in relation to the current cursor position.
    END, //!< Move in relation to the end of the data.
};

/*!
 * Type representing a position in a Seekable stream like SeekableOutput or SeekableInput.
 *
 * This is generally just the largest unsigned integer type on the platform, or size_t.
 */
typedef size_t SeekPos;

/*!
 * Constant value representing an invalid SeekPos.
 *
 * This is just the maximum value that can fit in SeekPos. There should be no buffer ever that is that large, so this is
 * a safe value to use.
 */
[[maybe_unused]] constexpr SeekPos INVALID_POS = Limits<SeekPos>::max();

} // namespace KirHut::IO
