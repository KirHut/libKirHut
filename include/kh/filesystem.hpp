/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/filesystem.hpp
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

/*!
 * \file kh/filesystem.hpp
 *
 * File System Header that includes the standard filesystem namespace under KirHut::FS.
 */

#include "kh/global.hpp"

#if defined(KH_INCLUDE_FILESYSTEM)
# include <filesystem> // IWYU pragma: export
#endif

/*!
 * Namespace used for filesystem operations used in libKirHut.
 *
 * This namespace is really just the std::filesystem namespace with an easier to refer to name. Currently there are no
 * aliases used under this namespace so for all intents and purposes you can just use it like std::filesystem. If you
 * are using the KirHut namespace as recommended (using namespace KirHut), then this should be as easy as just using
 * "FS::" in code to get everything in the std::filesystem namespace.
 *
 * This namespace will be empty unless the KH_INCLUDE_FILESYSTEM option is ON. This is by default, so you would need to
 * manually turn this option off to remove KirHut::FS support. This could be useful when attempting to compile libKirHut
 * for platforms that do not properly support std::filesystem.
 */
namespace KirHut::FS
{

/*!
 * Indication boolean for when std::filesystem can be found under KirHut::FS in libKirHut.
 *
 * When you need to check if this library includes std::filesystem under KirHut::FS in an if constexpr expression rather
 * than the preprocessor, you can use this to check.
 *
 * \hideinitializer
 */
[[maybe_unused]] constexpr bool hasFS = false
#if defined(KH_INCLUDE_FILESYSTEM)
                                        or true
#endif
    ;

#if defined(KH_INCLUDE_FILESYSTEM)
using namespace std::filesystem;
#endif

} // namespace KirHut::FS
