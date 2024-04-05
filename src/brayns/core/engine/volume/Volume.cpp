/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "Volume.h"

namespace brayns
{
Volume::Volume(Volume &&other) noexcept
{
    *this = std::move(other);
}

Volume &Volume::operator=(Volume &&other) noexcept
{
    _handleName = std::move(other._handleName);
    _volumeName = std::move(other._volumeName);
    _handle = std::move(other._handle);
    _data = std::move(other._data);
    _flag = std::move(other._flag);
    return *this;
}

Volume::Volume(const Volume &other)
{
    *this = other;
}

Volume &Volume::operator=(const Volume &other)
{
    _handleName = other._handleName;
    _volumeName = other._volumeName;
    _handle = ospray::cpp::Volume(_handleName);
    _data = other._data->clone();
    _flag.setModified(true);
    return *this;
}

bool Volume::commit()
{
    if (!_flag)
    {
        return false;
    }

    _data->pushTo(_handle);
    _handle.commit();
    _flag = false;
    return true;
}

Bounds Volume::computeBounds(const TransformMatrix &matrix) const noexcept
{
    return _data->computeBounds(matrix);
}

const ospray::cpp::Volume &Volume::getHandle() const noexcept
{
    return _handle;
}

}
