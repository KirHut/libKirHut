/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/errors.hpp
** Copyright © KirHut Software Company
**
** Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
** the License. You may obtain a copy of the License at
**
**   http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
** an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
** specific language governing permissions and limitations under the License.
***********************************************************************************************************************/
#pragma once

/*!
 * \file kh/errors.hpp
 *
 * File providing default Errors found within libKirHut.
 *
 * The kh/error.hpp header does not include default Error objects like AlreadyInitialized or KirHutSucksAtProgramming,
 * because the Error object does not require these to work correctly. However, there are methods and objects that will
 * throw these Error objects specifically, and those headers will include this header in that case.
 */

#include "kh/error.hpp"

namespace KirHut
{

KH_INLINE_NAMESPACE_V1

/*!
 * Error thrown when an impossible condition arises due to a programming error.
 *
 * This error should never really be attempted to be caught except at the base of an application or at the base of a
 * thread that can't send the error to the main thread. The application should terminate as a result of this error, and
 * it should be logged how the error happened (which should be provided by the info() in the Invalid object).
 *
 * If you encounter this exception while working with a released version of an application written by KirHut, please
 * report it to the KirHut Bug Reporting system! This will help make the software better in the future.
 */
using KirHutSucksAtProgramming = Error<WhyInvalid::SoftwareError>;

/*!
 * Error thrown when an underlying expected value has already been initialized when it shouldn't be.
 *
 * This can happen when an initializer object has already been created but you attempt to create another one. This is
 * almost always a programming error so this should use exception throwing to indicate the error.
 */
using AlreadyInitialized = Error<WhyInvalid::AlreadyInitialized>;

/*!
 * Error thrown when an impossible environmental condition was encountered.
 *
 * Like most error, there should be no reason for this error to ever be thrown, however there are times when some kind
 * of alert must be provided to a user. Also like most errors, there should be no real attempt to catch this error, and
 * just log the error at the base of the application and exit.
 *
 * If you encounter this error while using a KirHut application, check what specific problem is happening to the
 * environment and try to correct it if you can. Otherwise, please report it to the KirHut Bug Reporting system!
 */
using EnvironmentProblem = Error<WhyInvalid::BadEnvironment>;

/*!
 * Error thrown when an object received an argument that is illegal.
 *
 * This is similar to the standard library exception, but is a KirHut::Error instead of a std::exception.
 */
using IllegalArgument = Error<WhyInvalid::IllegalArgument>;

/*!
 * Exception thrown when an underlying expected value has not been initialized yet.
 *
 * This can happen when a non-static object needs to be initialized by some process but it has not yet, but you are now
 * requesting that uninitialized object.
 */
using NotInitializedYet = Error<WhyInvalid::DataUninitialized>;

/*!
 * Exception thrown when there never was or is no longer valid data available in an object or container.
 *
 * This can happen, notably, when calling MaybeInv<T>::take() and isValid() is false at that time. This could happen
 * quite spuriously in multithreaded environments that use the same MaybeInv, so the best option in that case is to
 * remove the T from the MaybeInv container, then provide multithreaded API to T.
 */
using NoValidData = Error<WhyInvalid::DataRemoved>;

/*!
 * Exception thrown when the internal state of an object is invalid or otherwise unusable for a given command.
 *
 * Usually, it is acceptable to just return a default value when the internal state isn't in a usable state, however
 * there are times when you want the application to respond more forcefully to bad input, and this Exception can be used
 * for just such a circumstance.
 */
using BadState = Error<WhyInvalid::InvalidState>;

//! \cond
// Poor Doxygen doesn't understand extern template instantiations with KH_EXPORT...
extern template struct KH_EXPLICIT_TEMPLATE_EXPORT Error<WhyInvalid::SoftwareError>;
extern template struct KH_EXPLICIT_TEMPLATE_EXPORT Error<WhyInvalid::AlreadyInitialized>;
extern template struct KH_EXPLICIT_TEMPLATE_EXPORT Error<WhyInvalid::BadEnvironment>;
extern template struct KH_EXPLICIT_TEMPLATE_EXPORT Error<WhyInvalid::IllegalArgument>;
extern template struct KH_EXPLICIT_TEMPLATE_EXPORT Error<WhyInvalid::DataUninitialized>;
extern template struct KH_EXPLICIT_TEMPLATE_EXPORT Error<WhyInvalid::DataRemoved>;
extern template struct KH_EXPLICIT_TEMPLATE_EXPORT Error<WhyInvalid::InvalidState>;
//! \endcond

KH_END_INLINE_NAMESPACE

} // namespace KirHut
