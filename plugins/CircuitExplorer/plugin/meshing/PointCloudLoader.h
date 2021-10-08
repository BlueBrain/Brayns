/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <string>

#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <brayns/network/entrypoint/EntrypointException.h>

struct PointCloudInfo
{
    std::string title;
    size_t modelId = 0.0;
    uint32_t frame = 0;
    double value = 0.0;
    double epsilon = 0.0;
    size_t gridSize = 0;
    double threshold = 0.0;
};

class PointCloudLoader
{
public:
    static void loadConvexHull(brayns::Scene& scene,
                               const PointCloudInfo& info);

    static void loadMetaballs(brayns::Scene& scene, const PointCloudInfo& info);
};