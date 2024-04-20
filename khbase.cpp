/***********************************************************************************************************************
** The KirHut Library for the Public Benefit
** khbase.cpp
** Copyright (C) 2024 KirHut Security Company
**
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
** later version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
** warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
** details.
**
** You should have received a copy of the GNU Affero General Public License along with this program.  If not, see
** <http://www.gnu.org/licenses/>.
***********************************************************************************************************************/
#include "khbase.hpp"

#if KH_USES_QT
#include <QString>
#endif

namespace KirHut
{

#if KH_USES_QT
QString toQStr(StringView in)
{
    return QByteArray::fromRawData(charPtr(in), in.size());
}

String toStr(QString const &in)
{
    // This does not use toStdString() because that method just uses toUtf8() internally anyway, so if a temporary
    // QByteArray needs to be created we may as well just use that directly as the source of the bytes to copy from.
    auto u8bytes = in.toUtf8();
    String retStr(u8bytes.size(), 0);
    std::memcpy(retStr.data(), u8bytes.constData(), u8bytes.size());
    return retStr;
}
#endif

} // namespace KirHut
