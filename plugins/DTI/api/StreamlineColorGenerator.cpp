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

#include "StreamlineColorGenerator.h"

namespace dti
{
std::vector<brayns::Vector4f> StreamlineColorGenerator::generate(const std::vector<brayns::Capsule> &primitives)
{
    std::vector<brayns::Vector4f> colors;
    colors.reserve(primitives.size());

    for (auto &primitive : primitives)
    {
        auto &p1 = primitive.p0;
        auto &p2 = primitive.p1;
        auto dir = brayns::math::normalize(p2 - p1);
        auto n = brayns::Vector3f(0.5f) + dir * 0.5f;
        colors.emplace_back(n, 1.f);
    }

    return colors;
}
}
