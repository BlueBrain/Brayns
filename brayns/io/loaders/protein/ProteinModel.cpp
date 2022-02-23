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

#include <brayns/io/loaders/protein/ProteinModel.h>

namespace brayns
{
ProteinModel::ProteinModel(const std::vector<Sphere>& spheres,
                           std::vector<Vector4f>&& colorMap,
                           std::vector<uint8_t>&& colorMapIndices)
 : _colorMap(std::move(colorMap))
 , _colorMapIndices(std::move(colorMapIndices))
{
    _geometry.add(spheres);
}

Bounds ProteinModel::computeBounds(const Matrix4f& transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

uint64_t ProteinModel::getGeometryModelSizeInBytes() const noexcept
{
    auto baseSize = sizeof(ProteinModel);

    baseSize += _colorMap.size() * sizeof(Vector4f);
    baseSize += _colorMapIndices.size() * sizeof(uint8_t);
    baseSize += _geometry.getNumGeometries() * sizeof(Sphere);

    return baseSize;
}

void ProteinModel::commitGeometryModel()
{
    auto ospHandle = handle();

    _geometry.commit();

    auto geomHandle = _geometry.handle();
    ospSetParam(ospHandle, "geometry", OSPDataType::OSP_GEOMETRY, &geomHandle);

    auto colorData = ospNewSharedData(_colorMap.data(), OSPDataType::OSP_VEC4F, _colorMap.size());
    ospSetParam(ospHandle, "color", OSPDataType::OSP_DATA, &colorData);
    ospRelease(colorData);

    auto colorIndexData = ospNewSharedData(_colorMapIndices.data(), OSPDataType::OSP_UCHAR, _colorMapIndices.size());
    ospSetParam(ospHandle, "index", OSPDataType::OSP_DATA, &colorIndexData);
    ospRelease(colorIndexData);
}
}
