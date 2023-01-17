/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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
        auto index = static_cast<glm::length_t>(i - start);
        result[index] = sizes[i];
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
            vector[static_cast<glm::length_t>(j)] = direction[j];
        }

        result[static_cast<glm::length_t>(i)] = glm::length(vector);
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
        auto matrix = brayns::Matrix3f();
        for (glm::length_t i = 0; i < 3; ++i)
        {
            matrix[i] = glm::normalize(brayns::Vector3f(vectors[i][0], vectors[i][1], vectors[i][2]));
        }
        transform.rotation = glm::quat_cast(matrix);
    }

    return transform;
}
