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

#include "Atlas.h"

namespace
{
class VoxelCoordinates
{
public:
    static brayns::Vector3ui linealToCartesian(const brayns::Vector3ui &size, size_t index)
    {
        auto frameSize = size.x * size.y;
        auto z = index / frameSize;
        auto localFrame = index % frameSize;
        auto y = localFrame / size.x;
        auto x = localFrame % size.x;
        return brayns::Vector3ui(x, y, z);
    }

    static size_t cartesianToLineal(const brayns::Vector3ui &size, const brayns::Vector3ui &coordinates)
    {
        auto framePos = coordinates.y * size.x + coordinates.x;
        return size.x * size.y * coordinates.z + framePos;
    }
};

class VoxelBounds
{
public:
    static brayns::Bounds compute(const brayns::Vector3ui &coordinates, const brayns::Vector3f &spacing)
    {
        auto worldX = coordinates.x * spacing.x;
        auto worldY = coordinates.y * spacing.y;
        auto worldZ = coordinates.z * spacing.z;
        auto minCorner = brayns::Vector3f(worldX, worldY, worldZ - spacing.z);
        auto maxCorner = brayns::Vector3f(worldX + spacing.x, worldY + spacing.y, worldZ);
        return brayns::Bounds(minCorner, maxCorner);
    }
};
}

Atlas::Atlas(const brayns::Vector3ui &size, const brayns::Vector3f &spacing)
    : _size(size)
    , _spacing(spacing)
{
}

const brayns::Vector3ui &Atlas::getSize() const noexcept
{
    return _size;
}

const brayns::Vector3f &Atlas::getSpacing() const noexcept
{
    return _spacing;
}

size_t Atlas::getVoxelCount() const noexcept
{
    return glm::compMul(_size);
}

brayns::Bounds Atlas::getVoxelBounds(const brayns::Vector3ui &coordinates) const
{
    if (glm::max(coordinates, _size) != _size)
    {
        throw std::invalid_argument("Coordinates out of bounds");
    }
    return VoxelBounds::compute(coordinates, _spacing);
}

brayns::Bounds Atlas::getVoxelBounds(size_t linealIndex) const
{
    _checkIndex(linealIndex);
    auto coordinates = VoxelCoordinates::linealToCartesian(_size, linealIndex);
    return VoxelBounds::compute(coordinates, _spacing);
}

bool Atlas::isValidVoxel(const brayns::Vector3ui &coordinates) const
{
    auto linealIndex = VoxelCoordinates::cartesianToLineal(_size, coordinates);
    return isValidVoxel(linealIndex);
}

void Atlas::_checkIndex(size_t linealIndex) const
{
    if (!(linealIndex < getVoxelCount()))
    {
        throw std::invalid_argument("Lineal index out of bounds");
    }
}