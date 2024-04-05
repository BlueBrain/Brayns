/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "EngineFactory.h"

#include <brayns/core/engine/camera/Camera.h>
#include <brayns/core/engine/geometry/Geometry.h>
#include <brayns/core/engine/light/Light.h>
#include <brayns/core/engine/material/Material.h>
#include <brayns/core/engine/renderer/Renderer.h>
#include <brayns/core/engine/volume/Volume.h>

namespace brayns
{
struct EngineFactories
{
    EngineFactory<Camera> cameras;
    EngineFactory<Geometry> geometries;
    EngineFactory<Light> lights;
    EngineFactory<Material> materials;
    EngineFactory<Renderer> renderer;
    EngineFactory<Volume> volumes;
};

class EngineFactoriesInitializer
{
public:
    static void init(EngineFactories &factories);
};
}
