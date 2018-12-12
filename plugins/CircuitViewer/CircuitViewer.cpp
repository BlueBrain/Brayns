/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Juan Hernando <juan.hernando@epfl.ch>
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

#include "CircuitViewer.h"
#include "io/CircuitLoader.h"
#include "io/MorphologyLoader.h"
#include "renderer/AdvancedSimulationRenderer.h" // enum Shading

#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/parameters/GeometryParameters.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
namespace
{
void _addAdvancedSimulationRenderer(Engine& engine)
{
    // clang-format off
    PropertyMap properties;
    properties.setProperty(
        {"aoDistance", 10000., {"Ambient occlusion distance"}});
    properties.setProperty(
        {"aoWeight", 0., 0., 1., {"Ambient occlusion weight"}});
    properties.setProperty({"detectionDistance", 15., {"Detection distance"}});
    properties.setProperty(
        {"shading",
         int32_t(AdvancedSimulationRenderer::Shading::none),
         brayns::enumNames<AdvancedSimulationRenderer::Shading>(),
         {"Shading"}});
    properties.setProperty({"shadows", 0., 0., 1., {"Shadow intensity"}});
    properties.setProperty({"softShadows", 0., 0., 1., {"Shadow softness"}});
    properties.setProperty({"samplingThreshold", 0.001, 0.001, 1.,
                            {"Threshold under which sampling is ignored"}});
    properties.setProperty({"volumeSpecularExponent", 20., 1., 100.,
                            {"Volume specular exponent"}});
    properties.setProperty(
        {"volumeAlphaCorrection", 0.5, 0.001, 1., {"Volume alpha correction"}});
    // clang-format on
    engine.addRenderer("advanced_simulation", properties);
}
void _addBasicSimulationRenderer(Engine& engine)
{
    PropertyMap properties;
    properties.setProperty(
        {"alphaCorrection", 0.5, 0.001, 1., {"Alpha correction"}});
    engine.addRenderer("basic_simulation", properties);
}
}

void CircuitViewer::init()
{
    auto& scene = _api->getScene();
    auto& registry = scene.getLoaderRegistry();
    _api->getParametersManager().getRenderingParameters().setCurrentRenderer(
        "basic_simulation");

    registry.registerLoader(std::make_unique<CircuitLoader>(scene));
    registry.registerLoader(std::make_unique<MorphologyLoader>(scene));

    _addAdvancedSimulationRenderer(_api->getEngine());
    _addBasicSimulationRenderer(_api->getEngine());
}
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(const int,
                                                         const char**)
{
    return new brayns::CircuitViewer();
}
