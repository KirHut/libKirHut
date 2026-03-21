/***********************************************************************************************************************
** The KirHut Application Development Library
** invalid.cpp
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
#include "kh/invalid.hpp"

#include "kh/errors.hpp"

#if defined(__cpp_lib_stacktrace)
# include <stacktrace>
# include <sstream>
#endif

namespace KirHut
{

KH_INLINE_NAMESPACE_V1

string Detail::tryGetStackTrace() noexcept
{
#if defined(__cpp_lib_stacktrace)
    try
    {
        // std::stacktrace throws **implementation defined** exceptions, so I have no idea what the hell this can throw!
        std::stringstream ret;
        for (auto const &ste : std::stacktrace::current())
        {
            ret << ste << '\n';
        }

        return std::move(ret).str();
    }
    catch (...)
    {
        // Give up and just use the fallback return nothing.
    }
#endif

    return {};
}

void Detail::throwNoValidData(Invalid const &inv)
{
    throw NoValidData(inv.info());
}

template class KH_EXPLICIT_TEMPLATE_INSTANCE BasicInvalid<WhyInvalid>;
template class KH_EXPLICIT_TEMPLATE_INSTANCE BasicInvalid<MessageViewWhy<char>>;
template class KH_EXPLICIT_TEMPLATE_INSTANCE BasicInvalid<string>;
template class KH_EXPLICIT_TEMPLATE_INSTANCE BasicInvalid<string_view>;

#if not defined(KH_NO_EXCEPTIONS)
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::SoftwareError>;
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::AlreadyInitialized>;
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::BadEnvironment>;
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::IllegalArgument>;
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::DataUninitialized>;
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::DataRemoved>;
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::InvalidState>;
#endif

KH_END_INLINE_NAMESPACE

} // namespace KirHut
