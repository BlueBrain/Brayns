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

#include "StreamlineComponentBuilder.h"

#include <brayns/engine/geometry/types/Capsule.h>

#include <components/DTIComponent.h>

namespace dti
{
void StreamlineComponentBuilder::build(
    const std::map<uint64_t, StreamlineData> &streamlines,
    float radius,
    brayns::Model &model)
{
    std::vector<std::vector<brayns::Capsule>> geometries;
    geometries.reserve(streamlines.size());

    for (const auto &[row, streamline] : streamlines)
    {
        const auto &points = streamline.points;

        auto &geometry = geometries.emplace_back();
        geometry.reserve(points.size() - 1);

        for (size_t i = 1; i < points.size(); ++i)
        {
            const auto &start = points[i - 1];
            const auto &end = points[i];
            geometry.push_back(brayns::CapsuleFactory::cylinder(start, end, radius));
        }
    }
    model.addComponent<dti::DTIComponent>(std::move(geometries));
}
}
