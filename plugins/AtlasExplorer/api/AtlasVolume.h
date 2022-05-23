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

#pragma once

#include <brayns/common/Bounds.h>
#include <brayns/common/MathTypes.h>

template<typename T>
class AtlasVolume
{
public:
    AtlasVolume(const brayns::Vector3ui &size, const brayns::Vector3f &dimension, std::vector<T> data)
        : _size(size)
        , _dimension(dimension)
        , _data(std::move(data))
    {
        if (glm::compMul(_size) != _data.size())
        {
            throw std::invalid_argument("Data size and volume size does not match");
        }
    }

    T &operator[](size_t linealIndex)
    {
        return _data[linealIndex];
    }

    T &operator[](size_t x, size_t y, size_t z)
    {
        return _data[_linealIndexFromCoordinates(x, y, z)];
    }

    T &at(size_t linealIndex)
    {
        return _data.at(linealIndex);
    }

    T &at(size_t x, size_t y, size_t z)
    {
        if (x >= _size_x || y >= _size.y || z >= _size.z)
        {
            throw std::invalid_argument("Coordinates are out of range");
        }

        return _data[_linealIndexFromCoordinates(x, y, z)];
    }

    const brayns::Vector3ui &getSize() const noexcept
    {
        return _size;
    }

    const brayns::Vector3f &getDimension() const noexcept
    {
        return _dimension;
    }

    brayns::Bounds getVoxelBounds(size_t x, size_t y, size_t z)
    {
        const auto world = brayns::Vector3f(x, y, z) * _dimension;
        return brayns::Bounds(
            world + brayns::Vector3f(0.f, 0.f, -_dimension.z),
            world + brayns::Vector3f(_dimension.x, _dimension.y, 0.f));
    }

private:
    size_t _linealIndexFromCoordinates(size_t x, size_t y, size_t z) noexcept
    {
        const auto frameSize = _size.x * _size.y;
        const auto sliceIndex = frameSize * z;
        const auto row = _size.x * y;
        const auto column = _size.x;
        return sliceIndex + row + column;
    }

private:
    brayns::Vector3ui _size;
    brayns::Vector3f _dimension;
    std::vector<T> _data;
};
