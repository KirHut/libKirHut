/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/math.hpp
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
#pragma once

#include "kh/base.hpp"

namespace KirHut
{

/*!
 * \brief log10_2
 */
constexpr double log10_2 = Detail::log10_2;

/*!
 *
 */
template <std::floating_point Float_T>
struct FloatTraits
{
    using UInt                         = UIntOf<Float_T>;
    constexpr static int mantissaBits  = Limits<Float_T>::digits - 1;
    constexpr static int exponentBits  = (sizeof(Float_T) * Platform::bitsInByte) - mantissaBits - 1;
    constexpr static UInt exponentMask = ((static_cast<UInt>(1) << exponentBits) - 1) << mantissaBits;
    constexpr static UInt mantissaMask = (static_cast<UInt>(1) << mantissaBits) - 1;
    constexpr static int exponentBias  = std::numeric_limits<Float_T>::max_exponent - 1;
};

template <typename T>
concept EightyBitFloat = std::floating_point<T> and Limits<T>::digits == 64;

/*!
 * \brief The FloatTraits class
 */
template <EightyBitFloat Float_T>
struct FloatTraits<Float_T>
{
    using UInt                         = UIntOf<Float_T>;
    constexpr static int exponentBits  = 15;
    constexpr static int mantissaBits  = 64;
    constexpr static UInt exponentMask = ((static_cast<UInt>(1) << exponentBits) - 1) << mantissaBits;
    constexpr static UInt mantissaMask = (static_cast<UInt>(1) << mantissaBits) - 1;
    constexpr static int exponentBias  = std::numeric_limits<Float_T>::max_exponent - 1;
};

/*!
 * \brief frexp
 * \param x
 * \param exp
 * \return
 */
template <std::floating_point Float_T>
constexpr Float_T frexp(Float_T x, int &exp) noexcept
{
    using Traits = FloatTraits<Float_T>;

    auto bits = std::bit_cast<Traits::UInt>(x);
    if ((bits & Traits::exponentMask) == Traits::exponentMask)
    {
        return x;
    }

    auto mantissa = bits & Traits::mantissaMask;
    auto rawExp   = static_cast<int>((bits & Traits::exponentMask) >> Traits::mantissaBits);
    if constexpr (EightyBitFloat<Float_T>)
    {
        if (mantissa & (static_cast<Traits::UInt>(1) << (Traits::mantissaBits - 1)))
        {
            rawExp = 1;
        }
    }

    if (rawExp)
    {
    }
    else if (mantissa)
    {
    }
    else
    {
        exp = 0;
        return x;
    }

    if (rawExp == 0)
    {
        auto shift = Traits::mantissaBits - std::bit_width(mantissa);
        mantissa <<= shift;
        rawExp -= shift - 1;
    }

    bits &= ~Traits::exponentMask;
    bits |= static_cast<Traits::UInt>(Traits::exponentBias - 1) << Traits::mantissaBits;
    return std::bit_cast<Float_T>(bits);
}

} // namespace KirHut
