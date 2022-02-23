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

#include <brayns/io/loaders/mesh/MeshModel.h>

namespace brayns
{
MeshModel::MeshModel(const TriangleMesh& mesh)
{
    _meshGeometry.add(mesh);
}

Bounds MeshModel::computeBounds(const Matrix4f& transform) const noexcept
{
    return _meshGeometry.computeBounds(transform);
}

const Vector4f &MeshModel::getColor() const noexcept
{
    return _color;
}

void MeshModel::setColor(const Vector4f &newColor) noexcept
{
    _updateValue(_color, newColor);
}

uint64_t MeshModel::getGeometryModelSizeInBytes() const noexcept
{
    // Mesh geometry can only have 1 mesh, but we will still do the multiplicaion...
    return sizeof(MeshModel) + sizeof(TriangleMesh) * _meshGeometry.getNumGeometries();
}

void MeshModel::commitGeometryModel()
{
    _meshGeometry.commit();

    auto ospHandle = handle();
    auto geomHandle = _meshGeometry.handle();

    ospSetParam(ospHandle, "geometry", OSPDataType::OSP_GEOMETRY, &geomHandle);
    ospSetParam(ospHandle, "color", OSPDataType::OSP_VEC4F, &_color);
}
}
