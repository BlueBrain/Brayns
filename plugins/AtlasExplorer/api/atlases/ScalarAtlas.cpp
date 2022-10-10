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

#include "ScalarAtlas.h"

#include <api/utils/DataUtils.h>

#include <cassert>
#include <cmath>

ScalarAtlas::ScalarAtlas(const brayns::Vector3ui &size, const brayns::Vector3f &spacing, const IDataMangler &data)
    : Atlas(size, spacing)
    , _data(data.asDoubles())
{
    auto minMax = DataMinMax::compute(_data);
    _min = minMax.first;
    _max = minMax.second;
}

double ScalarAtlas::getMinValue() const noexcept
{
    return _min;
}

double ScalarAtlas::getMaxValue() const noexcept
{
    return _max;
}

bool ScalarAtlas::isValidVoxel(size_t linealIndex) const
{
    _checkIndex(linealIndex);
    auto value = _data[linealIndex];
    return value > _min && std::isfinite(value);
}

double ScalarAtlas::operator[](size_t index) const noexcept
{
    return _data[index];
}

double ScalarAtlas::at(size_t index) const
{
    _checkIndex(index);
    return _data[index];
}

const std::vector<double> &ScalarAtlas::getValues() const noexcept
{
    return _data;
}

VoxelType ScalarAtlas::getVoxelType() const noexcept
{
    return type;
}
