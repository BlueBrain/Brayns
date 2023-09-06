/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "VolumeView.h"

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
struct VolumeViewParameters
{
    static inline const std::string colorRamp = "transferFunction";
};
}

namespace brayns
{
VolumeView::VolumeView(const Volume &volume):
    _handle(volume.getHandle())
{
}

void VolumeView::setColorRamp(const ColorRamp &colorRamp)
{
    _colorRamp.set(colorRamp);
    _handle.setParam(VolumeViewParameters::colorRamp, _colorRamp.getHandle());
    _flag = true;
}

bool VolumeView::commit()
{
    if (!_flag)
    {
        return false;
    }
    _flag = false;
    _handle.commit();
    return true;
}

const ospray::cpp::VolumetricModel &VolumeView::getHandle() const noexcept
{
    return _handle;
}
}
