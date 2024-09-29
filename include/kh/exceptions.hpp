/***********************************************************************************************************************
** The KirHut Application Development Library
** exceptions.hpp
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

#include "exception.hpp"

namespace KirHut
{

/*!
 * Exception thrown when an impossible condition arises likely due to a programming error.
 *
 * This exception should never really be attempted to be caught except at the base of an application or at the base of
 * a thread that can't send the exception to the main thread. The application should terminate as a result of this
 * exception, and it should be logged how the exception happened (which will be provided by the message in the Exception
 * object).
 *
 * If you encounter this exception while working with a released version of an application written by KirHut, please
 * report it to the KirHut Bug Reporting system! This will help make the software better in the future.
 */
struct KH_EXPORT KirHutSucksAtProgramming : public Exception
{
    using Exception::Exception;
};

/*!
 * Exception thrown when an impossible environmental condition was encountered.
 *
 * Like most exceptions, there should be no reason for this exception to ever be thrown, however there are times when
 * some kind of alert must be provided to a user. Like most exceptions, there should be no real attempt to catch this
 * exception, and just log the exception at the base of the application and exit.
 *
 * If you encounter this exception while using a KirHut application, check what specific problem is happening to the
 * environment and try to correct it if you can. Otherwise, please report it to the KirHut Bug Reporting system!
 */
struct KH_EXPORT BullshitEnvironment : public Exception
{
    using Exception::Exception;
};

/*!
 * Exception thrown when an object received an argument that is illegal.
 *
 * This is similar to the standard library exception, but is a KirHut::Exception instead of a std::exception only.
 */
struct KH_EXPORT IllegalArgument : public Exception
{
    using Exception::Exception;
};

/*!
 * Exception thrown when an underlying expected value has not been initialized yet.
 *
 * This can happen when a non-static object needs to be initialized by some process but it has not yet, but you are now
 * requesting that uninitialized object.
 */
struct KH_EXPORT NotInitializedYet : public Exception
{
    using Exception::Exception;
};

} // namespace KirHut
