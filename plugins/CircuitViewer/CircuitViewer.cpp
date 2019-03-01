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

#ifdef BRAYNS_USE_OSPRAY
#include "renderer/AdvancedSimulationRenderer.h" // enum Shading
#endif

#include <brayns/engine/Engine.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
#ifdef BRAYNS_USE_OSPRAY
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
    engine.addRendererType("advanced_simulation", properties);
}
void _addBasicSimulationRenderer(Engine& engine)
{
    PropertyMap properties;
    properties.setProperty(
        {"alphaCorrection", 0.5, 0.001, 1., {"Alpha correction"}});
    engine.addRendererType("basic_simulation", properties);
}
}
#endif

CircuitViewer::CircuitViewer(PropertyMap&& circuitParams,
                             PropertyMap&& morphologyParams)
    : _circuitParams(std::move(circuitParams))
    , _morphologyParams(std::move(morphologyParams))
{
}

void CircuitViewer::init()
{
    auto& params = _api->getParametersManager();

    // This code should eventually go away. A more verbose alternative to
    // the harcoded label is to move the PropertyMap to struct parsing code
    // out of the circuit and morphology loader implementations.
    const auto addGeometryQuality = [&params](PropertyMap& map) {
        map.setProperty(
            {"geometryQuality",
             enumToString(params.getGeometryParameters().getGeometryQuality()),
             enumNames<GeometryQuality>(),
             {"Geometry Quality"}});
    };

    addGeometryQuality(_circuitParams);
    addGeometryQuality(_morphologyParams);

    auto& scene = _api->getScene();
    auto& registry = scene.getLoaderRegistry();
    registry.registerLoader(
        std::make_unique<CircuitLoader>(scene, std::move(_circuitParams)));
    registry.registerLoader(
        std::make_unique<MorphologyLoader>(scene,
                                           std::move(_morphologyParams)));

#ifdef BRAYNS_USE_OSPRAY
    if (params.getApplicationParameters().getEngine() == "ospray")
    {
        _addAdvancedSimulationRenderer(_api->getEngine());
        _addBasicSimulationRenderer(_api->getEngine());

        params.getRenderingParameters().setCurrentRenderer("basic_simulation");
    }
#endif
}
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(const int argc,
                                                         const char** argv)
{
    using namespace brayns;

    PropertyMap circuitParams = CircuitLoader::getCLIProperties();
    PropertyMap morphologyParams = MorphologyLoader::getCLIProperties();
    // Using a single PropertyMap for command line parsing. This way the
    // UI for circuit loading inherits the properties from the morphology
    // loader as well.
    circuitParams.merge(morphologyParams);

    if (!circuitParams.parse(argc, argv))
        return nullptr;

    // Update morphology parameters with command line defaults
    morphologyParams.update(circuitParams);

    return new CircuitViewer(std::move(circuitParams),
                             std::move(morphologyParams));
}
