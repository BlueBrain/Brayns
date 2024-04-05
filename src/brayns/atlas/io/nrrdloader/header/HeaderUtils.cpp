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

#include "HeaderUtils.h"

brayns::Vector3ui HeaderUtils::get3DSize(const NRRDHeader &header)
{
    const auto &sizes = header.sizes;
    assert(sizes.size() >= 3);

    brayns::Vector3ui result;
    const auto start = sizes.size() - 3;
    for (size_t i = start; i < sizes.size(); ++i)
    {
        result[i - start] = sizes[i];
    }
    return result;
}

brayns::Vector3f HeaderUtils::get3DDimensions(const NRRDHeader &header)
{
    brayns::Vector3f result(1.f);

    const auto &spaceDirections = header.spaceDirections;
    if (!spaceDirections)
    {
        return result;
    }

    const auto &directions = *spaceDirections;
    assert(directions.size() == 3);

    for (size_t i = 0; i < directions.size(); ++i)
    {
        const auto &direction = directions[i];
        brayns::Vector3f vector;
        for (size_t j = 0; j < 3; ++j)
        {
            vector[j] = direction[j];
        }

        result[i] = brayns::math::length(vector);
    }
    return result;
}

brayns::Transform HeaderUtils::getTransform(const NRRDHeader &header)
{
    auto &spaceDimension = header.spaceDimensions;
    if (!spaceDimension || (*spaceDimension) != 3)
    {
        return brayns::Transform();
    }

    auto transform = brayns::Transform();

    auto &spaceOrigin = header.spaceOrigin;
    if (spaceOrigin)
    {
        auto &origin = *spaceOrigin;
        transform.translation = brayns::Vector3f(origin[0], origin[1], origin[2]);
    }

    auto &spaceDirection = header.spaceDirections;
    if (spaceDirection)
    {
        auto &vectors = *spaceDirection;
        auto vx = brayns::math::normalize(brayns::Vector3f(vectors[0][0], vectors[0][1], vectors[0][2]));
        auto vy = brayns::math::normalize(brayns::Vector3f(vectors[1][0], vectors[1][1], vectors[1][2]));
        auto vz = brayns::math::normalize(brayns::Vector3f(vectors[2][0], vectors[2][1], vectors[2][2]));
        transform.rotation = brayns::Quaternion(vx, vy, vz);
    }

    return transform;
}
