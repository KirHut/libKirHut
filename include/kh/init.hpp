/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/init.hpp
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

#include "kh/base.hpp"

namespace KirHut::Init
{

/*!
 * Set of flags indicating which subsystems to initialize when the InitTag is created.
 *
 * Each subsystem is described by their according INIT constexpr value.
 */
using InitFlags = bitset<8>;

/*!
 * \brief INIT_ALL
 */
constexpr InitFlags INIT_ALL = 0b11111111;

/*!
 * \brief INIT_ARGS
 *
 * This indicates whether or not to change the args into UTF-8, not whether or not to parse the arguments! Argument
 * Parsing is done with a separate library.
 *
 * The arguments returned by argc and argv will always be considered "modified" by Qt, so this is an effective way to
 * prevent using GetCommandLine() on Windows to get the arguments.
 */
[[maybe_unused]] constexpr InitFlags INIT_ARGS = 0b1;

/*!
 * \brief INIT_IO
 *
 * This indicates whether or not to initialize the IO subsystem. Currently IO does not require initialization so this
 * does very little in practice.
 */
[[maybe_unused]] constexpr InitFlags INIT_IO = 0b10;

/*!
 * \brief INIT_LOG
 *
 * This indicates whether or not to initialize the logging subsystem. Currently logging does not require initialization
 * so this does very little in practice.
 */
[[maybe_unused]] constexpr InitFlags INIT_LOG = 0b100;

/*!
 * \brief INIT_NONE
 *
 * This indicates to initialize none of the subsystems. You could simply not create an InitTag instead to get the same
 * effect, but this could be useful when it is chosen at runtime.
 */
[[maybe_unused]] constexpr InitFlags INIT_NONE = 0;

/*!
 * Tag object representing successful initialization of the KirHut library functionality.
 *
 * Most of what libKirHut provides depends on this InitTag being created very early in the application's lifetime. For
 * example, this should be initialized prior to creating a QApplication object in a Qt project. It should be either the
 * first object in, or near the first object in, an end user Application object implementation. You could also inherit
 * this object in an Application object and everything will "just work". An inherited Application object should never be
 * referred to using only the InitTag type or have the destructor called from that reference, as the destructor is not
 * marked as virtual (to avoid adding a vtable to the class)!
 *
 * The InitTag constructor performs all of the initialization of this library. If you wish to initialize portions of the
 * library separately, this is possible using InitTag's various static methods. The cleanest and most encouraged way to
 * use this library is to simply create an InitTag object. This also performs necessary cleanup when the application is
 * closing.
 *
 * Here is the current set of things that this object initializes:
 * - IO: There is nothing to do for IO currently, so this does nothing.
 * - Logging: There is nothing to do for logging currently, so this does nothing.
 *
 * This object is also responsible for converting argc and argv into something useable, which is accessible using the
 * methods InitTag::argc() and InitTag::argv().
 */
class InitTag final
{
    struct Impl;
    UPtr<Impl> im;

public:
    /*!
     * InitTag constructor using just the initialization flags and not application arguments.
     *
     * This constructor is for initialization of an application that does not have to parse user arguments in any way.
     * This makes it easy to write KirHut software that does not require arguments.
     */
    InitTag(InitFlags flags = INIT_ALL);

    /*!
     * Standard InitTag constructor that performs argument parsing and library initialization.
     *
     *
     * \param argc
     * \param argv
     * \param flags
     */
    InitTag(int argc, char **argv, InitFlags flags = INIT_ALL);

    /*!
     * InitTag destructor.
     *
     * This destructor is responsible for cleaning up some resources, and should be ran before exiting the application.
     * As such, prefer Exceptions over crashing behavior.
     */
    ~InitTag();

    /*!
     * \brief argc
     * \return
     */
    size_t argc() const noexcept;

    /*!
     * \brief argv
     * \param arg
     * \return
     */
    char const *argv(size_t arg) const noexcept;
};

} // namespace KirHut::Init
