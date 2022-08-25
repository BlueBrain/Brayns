/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "VolumeRendererComponent.h"
#include "ColorRampComponent.h"

namespace brayns
{
Bounds VolumeRendererComponent::computeBounds(const Matrix4f &transform) const noexcept
{
    return _volume.computeBounds(transform);
}

void VolumeRendererComponent::onCreate()
{
    Model &model = getModel();
    auto &group = model.getGroup();
    group.fromVolume(_volumeView);
    model.addComponent<ColorRampComponent>();
}

bool VolumeRendererComponent::commit()
{
    auto &model = getModel();
    auto &colorRampComponent = model.getComponent<ColorRampComponent>();
    auto &colorRamp = colorRampComponent.getColorRamp();
    if (colorRamp.isModified())
    {
        _volumeView.setColorRamp(colorRamp);
        colorRamp.resetModified();
    }

    auto volumeCommitted = _volume.commit();
    auto viewCommitted = _volumeView.commit();
    return volumeCommitted || viewCommitted;
}
}
