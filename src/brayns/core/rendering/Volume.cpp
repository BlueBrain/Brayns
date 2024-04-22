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

#include <string>

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
const std::string colorKey = "color";
const std::string opacityKey = "opacity";
const std::string valueKey = "value";

const std::string idKey = "id";
const std::string transferFunctionKey = "transferFunction";

const std::string dataKey = "data";
const std::string cellCenteredKey = "cellCentered";
const std::string gridOriginKey = "gridOrigin";
const std::string gridSpacingKey = "gridSpacing";
const std::string filterKey = "filter";
}

namespace brayns
{
TransferFunction::TransferFunction(ospray::cpp::TransferFunction function):
    _function(std::move(function))
{
}

ospray::cpp::TransferFunction TransferFunction::getHandle() const
{
    return _function;
}

void TransferFunction::setColors(const std::vector<Color3> &colors)
{
    _function.setParam(colorKey, ospray::cpp::SharedData(colors));
}

void TransferFunction::setOpacities(const std::vector<float> &opacities)
{
    _function.setParam(opacityKey, ospray::cpp::SharedData(opacities));
}

void TransferFunction::setScalarRange(Box1 range)
{
    _function.setParam(valueKey, range);
}

void TransferFunction::commit()
{
    _function.commit();
}

VolumeModel::VolumeModel(ospray::cpp::VolumetricModel model):
    _model(std::move(model))
{
}

ospray::cpp::VolumetricModel VolumeModel::getHandle() const
{
    return _model;
}

void VolumeModel::setId(std::uint32_t id)
{
    _model.setParam(idKey, id);
}

void VolumeModel::setTransferFunction(ospray::cpp::TransferFunction function)
{
    _model.setParam(transferFunctionKey, function);
}

void VolumeModel::commit()
{
    _model.commit();
}

StructuredRegularVolume::StructuredRegularVolume(ospray::cpp::Volume volume):
    _volume(std::move(volume))
{
}

ospray::cpp::Volume StructuredRegularVolume::getHandle() const
{
    return _volume;
}

void StructuredRegularVolume::setData(const VolumeData &data)
{
    auto format = static_cast<OSPDataType>(data.type);
    auto shared = ospray::cpp::SharedData(data.data, format, data.size);
    _volume.setParam(dataKey, shared);
}

void StructuredRegularVolume::setType(VolumeType type)
{
    auto cellCentered = type == VolumeType::CellCentered;
    _volume.setParam(cellCenteredKey, cellCentered);
}

void StructuredRegularVolume::setOrigin(const Vector3 &origin)
{
    _volume.setParam(gridOriginKey, origin);
}

void StructuredRegularVolume::setSpacing(const Vector3 &spacing)
{
    _volume.setParam(gridSpacingKey, spacing);
}

void StructuredRegularVolume::setFilter(VolumeFilter filter)
{
    _volume.setParam(filterKey, static_cast<OSPVolumeFilter>(filter));
}

void StructuredRegularVolume::commit()
{
    _volume.commit();
}
}
