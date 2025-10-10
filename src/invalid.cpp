/***********************************************************************************************************************
** The KirHut Application Development Library
** invalid.cpp
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
#include "kh/invalid.hpp"

#include "kh/errors.hpp"

namespace KirHut
{

void Detail::throwNoValidData(Invalid const &inv)
{
    throw NoValidData(inv.info());
}

template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::SoftwareError>;
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::AlreadyInitialized>;
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::BadEnvironment>;
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::IllegalArgument>;
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::DataUninitialized>;
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::DataRemoved>;
template struct KH_EXPLICIT_TEMPLATE_INSTANCE Error<WhyInvalid::InvalidState>;

template class KH_EXPLICIT_TEMPLATE_INSTANCE BasicInvalid<WhyInvalid>;
template class KH_EXPLICIT_TEMPLATE_INSTANCE BasicInvalid<MessageViewWhy<char>>;
template class KH_EXPLICIT_TEMPLATE_INSTANCE BasicInvalid<string>;
template class KH_EXPLICIT_TEMPLATE_INSTANCE BasicInvalid<string_view>;

} // namespace KirHut
