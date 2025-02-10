/***********************************************************************************************************************
** The KirHut Application Development Library
** filesystem.hpp
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
 * \file kh/filesystem.hpp
 *
 * File System Header that includes the standard filesystem namespace under KirHut::FS.
 */

#include <filesystem> // IWYU pragma: export

/*!
 * Namespace used for filesystem operations used in libKirHut.
 *
 * This namespace is really just the std::filesystem namespace with an easier to refer to name. Currently there are no
 * aliases used under this namespace so for all intents and purposes you can just use it like std::filesystem. If you
 * are using the KirHut namespace as recommended (using namespace KirHut), then this should be as easy as just using
 * "FS::" in code to get everything in the std::filesystem namespace.
 */
namespace KirHut::FS
{

using namespace std::filesystem;

} // namespace KirHut::FS
