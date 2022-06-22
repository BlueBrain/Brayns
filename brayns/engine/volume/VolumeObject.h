/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#pragma once

#include "Volume.h"

#include <ospray/ospray_cpp/TransferFunction.h>
#include <ospray/ospray_cpp/VolumetricModel.h>

namespace brayns
{
template<typename T>
class VolumeObject
{
public:
    VolumeObject(T data)
        : VolumeObject(Volume<T>(std::move(data)))
    {
    }

    void setTransferFunction(const ospray::cpp::TransferFunction &transferFunction)
    {
        _osprayObject.setParam("transferFunction", transferFunction);
        _modified = true;
    }

    bool commit()
    {
        if (_volume.commit() || _modified)
        {
            _osprayObject.commit();
            _modified = false;
            return true;
        }
        return false;
    }

    Bounds computeBounds(const Matrix4f &transform) const noexcept
    {
        return _volume.computeBounds(transform);
    }

    Volume<T> &getVolume() noexcept
    {
        return _volume;
    }

    const ospray::cpp::VolumetricModel &getOsprayObject() const noexcept
    {
        return _osprayObject;
    }

private:
    Volume<T> _volume;
    ospray::cpp::VolumetricModel _osprayObject;
    bool _modified = true;
};
}
