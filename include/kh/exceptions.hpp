/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/exceptions.hpp
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
 * \file kh/exceptions.hpp
 *
 * File providing default Exceptions found within libKirHut.
 *
 * The kh/exception.hpp header does not include default Exception objects like AlreadyInitialized or
 * KirHutSucksAtProgramming, because the Exception object does not require these to work correctly. However, there are
 * methods and objects that will throw these Exception objects specifically, and those headers will include this header
 * in that case.
 */

#include "kh/exception.hpp"

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
 * Exception thrown when an underlying expected value has already been initialized when it shouldn't be.
 *
 * This can happen when an initializer object has already been created but you attempt to create another one. This is
 * almost always a programming error so this throws an Exception.
 */
struct KH_EXPORT AlreadyInitialized : public Exception
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

/*!
 * Exception thrown when there never was or is no longer valid data available in an object or container.
 *
 * This can happen, notably, when calling MaybeInv<T>::take() and isValid() is false at that time. This could happen
 * quite spuriously in multithreaded environments that use the same MaybeInv, so the best option in that case is to
 * remove the T from the MaybeInv container, then provide multithreaded API to T.
 */
struct KH_EXPORT NoValidData : public Exception
{
    using Exception::Exception;
};

/*!
 * Exception thrown when the internal state of an object is invalid or otherwise unusable for a given command.
 *
 * Usually, it is acceptable to just return a default value when the internal state isn't in a usable state, however
 * there are times when you want the application to respond more forcefully to bad input, and this Exception can be used
 * for just such a circumstance.
 */
struct KH_EXPORT BadState : public Exception
{
    using Exception::Exception;
};

} // namespace KirHut
