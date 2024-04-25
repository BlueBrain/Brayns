/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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
void LinearTransferFunction::setColors(SharedArray<Color3> colors)
{
    setParam("color", toSharedData(colors));
}

void LinearTransferFunction::setOpacities(SharedArray<float> opacities)
{
    setParam("opacity", toSharedData(opacities));
}

void LinearTransferFunction::setScalarRange(Box1 range)
{
    setParam("value", range);
}

void VolumetricModel::setVolume(const BaseVolume &volume)
{
    setParam("volume", volume.getHandle());
}

void VolumetricModel::setTransferFunction(const BaseTransferFunction &function)
{
    setParam("transferFunction", function.getHandle());
}

void VolumetricModel::setId(std::uint32_t id)
{
    setParam("id", id);
}

void StructuredRegularVolume::setData(const VolumeData &data)
{
    auto format = static_cast<OSPDataType>(data.type);
    auto shared = ospray::cpp::SharedData(data.data, format, data.size);
    setParam("data", shared);
}

void StructuredRegularVolume::setType(VolumeType type)
{
    auto cellCentered = type == VolumeType::CellCentered;
    setParam("cellCentered", cellCentered);
}

void StructuredRegularVolume::setOrigin(const Vector3 &origin)
{
    setParam("gridOrigin", origin);
}

void StructuredRegularVolume::setSpacing(const Vector3 &spacing)
{
    setParam("gridSpacing", spacing);
}

void StructuredRegularVolume::setFilter(VolumeFilter filter)
{
    setParam("filter", static_cast<OSPVolumeFilter>(filter));
}
}
