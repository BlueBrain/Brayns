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

#include <brayns/engine/DefaultEngineObjects.h>

#include <brayns/engine/cameras/PerspectiveCamera.h>

#include <brayns/engine/lights/DirectionalLight.h>

#include <brayns/engine/materials/MatteMaterial.h>

#include <brayns/engine/renderers/InteractiveRenderer.h>

namespace brayns
{
void DefaultEngineObjects::registerObjects(Engine &engine)
{
    auto& factories = engine.getObjectFactories();

    auto& cameras = factories.cameraFactory;
    cameras.registerType<PerspectiveCamera>("perspective");

    auto& lights = factories.lightFactory;
    lights.registerType<DirectionalLight>("directional");

    auto& materials = factories.materialFactory;
    materials.registerType<MatteMaterial>("matte");

    auto& renderers = factories.rendererFactory;
    renderers.registerType<InteractiveRenderer>("interactive");
}
}
