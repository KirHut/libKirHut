/***********************************************************************************************************************
** The KirHut Application Development Library
** base.cpp
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
#include "kh/base.hpp"

#include "kh/ranges.hpp"
#include "kh/errors.hpp"

#include <chrono>
#include <cassert>

#if KH_USES_QT
# include <QString>
#endif

namespace KirHut
{

void Detail::throwTooSmallSpan(string_view message)
{
    throw IllegalArgument(message);
}

// Using a std::span here is more annoying than just using a contiguous_range because the QByteArray in one of the
// toStr() methods would need to be an lvalue.
template <typename StrType, R::contiguous_range RangeType>
inline StrType toStrImpl(RangeType rng)
{
    // The below code is shorter, but runs about 5-10% slower on most compilers. This is because memcpy is heavily
    // optimized.
    // return { R::begin(rng), R::end(rng) };

    StrType ret(R::size(rng), 0);
    memcpy(ret.data(), R::data(rng), R::size(rng));
    return ret;
}

string toStr(const char *from)
{
    return string{ from };
}

#if KH_USES_QT
QString toQStr(string_view in) noexcept
{
    return { QByteArray::fromRawData(in.data(), in.size()) };
}

string toStr(QString const &in) noexcept
{
    // This used to do something else, but now it just calls a QString method. It is retained for source compatibility.
    return in.toStdString();
}
#endif // KH_USES_QT

u64 currentTicks() noexcept
{
    return static_cast<u64>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

} // namespace KirHut
