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

#include "LayerDistanceAtlas.h"

#include <cassert>
#include <iostream>

namespace
{
class LayerDistanceParser
{
public:
    explicit LayerDistanceParser(const IDataMangler &data)
    {
        auto doubles = data.asDoubles();

        auto halfSize = doubles.size() / 2;
        values.reserve(halfSize);

        lowerLimits = brayns::Vector2f(std::numeric_limits<float>::max());
        higherLimits = brayns::Vector2f(std::numeric_limits<float>::lowest());

        for (size_t i = 0; i < doubles.size(); i = i + 2)
        {
            auto &value = values.emplace_back(_safeDoubleCast(doubles[i]), _safeDoubleCast(doubles[i + 1]));
            lowerLimits = brayns::math::min(lowerLimits, value);
            higherLimits = brayns::math::max(higherLimits, value);
        }
    }

    std::vector<brayns::Vector2f> values;
    brayns::Vector2f lowerLimits;
    brayns::Vector2f higherLimits;

private:
    float _safeDoubleCast(double value)
    {
        if (!std::isfinite(value))
        {
            return std::numeric_limits<float>::quiet_NaN();
        }

        if (value < static_cast<double>(std::numeric_limits<float>::lowest()))
        {
            return std::numeric_limits<float>::lowest();
        }

        if (value > static_cast<double>(std::numeric_limits<float>::max()))
        {
            return std::numeric_limits<float>::max();
        }

        return static_cast<float>(value);
    }
};
}

LayerDistanceAtlas::LayerDistanceAtlas(
    const brayns::Vector3ui &size,
    const brayns::Vector3f &spacing,
    const IDataMangler &dataMangler):
    Atlas(size, spacing)
{
    auto parser = LayerDistanceParser(dataMangler);
    _bounds = std::move(parser.values);
    _lowerLimits = parser.lowerLimits;
    _higherLimits = parser.higherLimits;
}

bool LayerDistanceAtlas::isValidVoxel(size_t index) const noexcept
{
    assert(_isValidIndex(index));
    return brayns::math::reduce_min(brayns::math::isfinite(_bounds[index]));
}

VoxelType LayerDistanceAtlas::getVoxelType() const noexcept
{
    return type;
}

const brayns::Vector2f &LayerDistanceAtlas::operator[](size_t index) const noexcept
{
    assert(_isValidIndex(index));
    return _bounds[index];
}

const brayns::Vector2f &LayerDistanceAtlas::getLowerLimits() const noexcept
{
    return _lowerLimits;
}

const brayns::Vector2f &LayerDistanceAtlas::getHigherLimits() const noexcept
{
    return _higherLimits;
}
