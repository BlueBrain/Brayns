/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "EngineFactory.h"

#include <brayns/engine/camera/projections/OrthographicCamera.h>
#include <brayns/engine/camera/projections/PerspectiveCamera.h>

#include <brayns/engine/light/types/AmbientLight.h>
#include <brayns/engine/light/types/DirectionalLight.h>
#include <brayns/engine/light/types/QuadLight.h>

#include <brayns/engine/material/types/CarPaint.h>
#include <brayns/engine/material/types/Default.h>
#include <brayns/engine/material/types/Emissive.h>
#include <brayns/engine/material/types/Glass.h>
#include <brayns/engine/material/types/Matte.h>
#include <brayns/engine/material/types/Metal.h>
#include <brayns/engine/material/types/Plastic.h>

#include <brayns/engine/renderer/types/Interactive.h>
#include <brayns/engine/renderer/types/Production.h>

namespace brayns
{
EngineFactory<Camera> EngineFactories::createCameraFactory()
{
    EngineFactory<Camera> factory;
    factory.registerType < Ortho
}

EngineFactory<Light> EngineFactories::createLightFactory()
{
}

EngineFactory<Material> EngineFactories::createMaterialFactory()
{
}

EngineFactory<Renderer> EngineFactories::createRendererFactory()
{
}
}
