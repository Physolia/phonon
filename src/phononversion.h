/*
    Copyright (C) 2005-2008 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PHONONVERSION_H
#define PHONONVERSION_H

#include "phononexport.h"
#include "phononversiondata.h"

/**
 * Helper macro that can be used like
 * \code
 * #if (PHONON_VERSION >= PHONON_VERSION_CHECK(5, 1, 2))
 * \endcode
 */
#define PHONON_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

namespace Phonon
{
PHONON_EXPORT const char *phononVersion();
} // namespace Phonon

#endif // PHONONVERSION_H