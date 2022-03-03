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

#include <brayns/engine/EngineObjectRegisterer.h>
#include <brayns/engine/cameras/OrthographicCamera.h>
#include <brayns/engine/cameras/PerspectiveCamera.h>
#include <brayns/engine/renderers/InteractiveRenderer.h>
#include <brayns/engine/renderers/ProductionRenderer.h>

namespace brayns
{
void EngineObjectRegisterer::registerDefaultTypes(Engine &engine) noexcept
{
    auto& cameraFactory = engine.getCameraFactory();

    cameraFactory.registerType<OrthographicCamera>("orthographic");
    cameraFactory.registerType<PerspectiveCamera>("perspective");

    auto& rendererFactory = engine.getRendererFactory();
    rendererFactory.registerType<InteractiveRenderer>("interactive");
    rendererFactory.registerType<ProductionRenderer>("production");
}
}
