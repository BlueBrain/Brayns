/* Copyright 2018-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#include "CircuitExplorerPlugin.h"

#include <brayns/network/interface/ActionInterface.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/PluginAPI.h>

#include <plugin/api/CircuitColorManager.h>
#include <plugin/api/VasculatureRadiiSimulation.h>

#include <plugin/io/BBPLoader.h>
#include <plugin/io/NeuronMorphologyLoader.h>
#include <plugin/io/SonataLoader.h>
#include <plugin/io/SonataNGVLoader.h>

#include <plugin/network/entrypoints/AddBoxEntrypoint.h>
#include <plugin/network/entrypoints/AddColumnEntrypoint.h>
#include <plugin/network/entrypoints/AddCylinderEntrypoint.h>
#include <plugin/network/entrypoints/AddGridEntrypoint.h>
#include <plugin/network/entrypoints/AddPillEntrypoint.h>
#include <plugin/network/entrypoints/AddSphereEntrypoint.h>
#include <plugin/network/entrypoints/ColorCircuitEntryPoint.h>
#include <plugin/network/entrypoints/GetMaterialIdsEntrypoint.h>
#include <plugin/network/entrypoints/MakeMovieEntrypoint.h>
#include <plugin/network/entrypoints/MaterialEntrypoint.h>
#include <plugin/network/entrypoints/MirrorModelEntrypoint.h>
#include <plugin/network/entrypoints/SetCircuitThicknessEntrypoint.h>
#include <plugin/network/entrypoints/SetMaterialExtraAttributesEntrypoint.h>
#include <plugin/network/entrypoints/SetMaterialRangeEntrypoint.h>
#include <plugin/network/entrypoints/SetMaterialsEntrypoint.h>
#include <plugin/network/entrypoints/SimulationColorEntryPoint.h>
#include <plugin/network/entrypoints/TraceAnterogradeEntrypoint.h>

namespace
{
void _addAdvancedSimulationRenderer(brayns::Engine& engine)
{
    brayns::Log::info("[CE] Registering advanced renderer.");
    brayns::PropertyMap properties;
    properties.add({"giDistance", 10000., {"Global illumination distance"}});
    properties.add({"giWeight", 0., {"Global illumination weight"}});
    properties.add({"giSamples", 0, {"Global illumination samples"}});
    properties.add({"shadows", 0., {"Shadow intensity"}});
    properties.add({"softShadows", 0., {"Shadow softness"}});
    properties.add({"softShadowsSamples", 1, {"Soft shadow samples"}});
    properties.add({"epsilonFactor", 1., {"Epsilon factor"}});
    properties.add({"samplingThreshold",
                    0.001,
                    {"Threshold under which sampling is ignored"}});
    properties.add(
        {"volumeSpecularExponent", 20., {"Volume specular exponent"}});
    properties.add({"volumeAlphaCorrection", 0.5, {"Volume alpha correction"}});
    properties.add({"maxDistanceToSecondaryModel",
                    30.,
                    {"Maximum distance to secondary model"}});
    properties.add({"exposure", 1., {"Exposure"}});
    properties.add({"fogStart", 0., {"Fog start"}});
    properties.add({"fogThickness", 1e6, {"Fog thickness"}});
    properties.add({"maxBounces", 3, {"Maximum number of ray bounces"}});
    properties.add({"useHardwareRandomizer",
                    false,
                    {"Use hardware accelerated randomizer"}});
    engine.addRendererType("circuit_explorer_advanced", properties);
}

void _addBasicSimulationRenderer(brayns::Engine& engine)
{
    brayns::Log::info("[CE] Registering basic renderer.");

    brayns::PropertyMap properties;
    properties.add({"alphaCorrection", 0.5, {"Alpha correction"}});
    properties.add({"simulationThreshold", 0., {"Simulation threshold"}});
    properties.add({"maxDistanceToSecondaryModel",
                    30.,
                    {"Maximum distance to secondary model"}});
    properties.add({"exposure", 1., {"Exposure"}});
    properties.add({"maxBounces", 3, {"Maximum number of ray bounces"}});
    properties.add({"useHardwareRandomizer",
                    false,
                    {"Use hardware accelerated randomizer"}});
    engine.addRendererType("circuit_explorer_basic", properties);
}

void _addDOFPerspectiveCamera(brayns::Engine& engine)
{
    brayns::Log::info("[CE] Registering DOF perspective camera.");

    brayns::PropertyMap properties;
    properties.add({"fovy", 45., {"Field of view"}});
    properties.add({"aspect", 1., {"Aspect ratio"}});
    properties.add({"apertureRadius", 0., {"Aperture radius"}});
    properties.add({"focusDistance", 1., {"Focus Distance"}});
    properties.add({"enableClippingPlanes", true, {"Clipping"}});
    engine.addCameraType("circuit_explorer_dof_perspective", properties);
}

void _addSphereClippingPerspectiveCamera(brayns::Engine& engine)
{
    brayns::Log::info("[CE] Registering sphere clipping perspective camera.");

    brayns::PropertyMap properties;
    properties.add({"fovy", 45., {"Field of view"}});
    properties.add({"aspect", 1., {"Aspect ratio"}});
    properties.add({"apertureRadius", 0., {"Aperture radius"}});
    properties.add({"focusDistance", 1., {"Focus Distance"}});
    properties.add({"enableClippingPlanes", true, {"Clipping"}});
    engine.addCameraType("circuit_explorer_sphere_clipping", properties);
}
} // namespace

CircuitExplorerPlugin::CircuitExplorerPlugin()
    : brayns::ExtensionPlugin("Circuit Explorer")
{
}

void CircuitExplorerPlugin::init()
{
    // LOADERS ADDED BY THIS PLUGIN
    auto& registry = _api->getLoaderRegistry();
    registry.registerLoader(std::make_unique<BBPLoader>(_colorManager));
    registry.registerLoader(std::make_unique<NeuronMorphologyLoader>());
    registry.registerLoader(std::make_unique<SonataLoader>(_colorManager));
    registry.registerLoader(std::make_unique<SonataNGVLoader>(_colorManager));

    // ENTRY POINTS ADDED BY THIS PLUGIN
    add<brayns::GetMaterialIdsEntrypoint>();
    add<brayns::GetMaterialEntrypoint>();
    add<brayns::SetMaterialEntrypoint>();
    add<brayns::SetMaterialsEntrypoint>();
    add<brayns::SetMaterialRangeEntrypoint>();
    add<brayns::SetMaterialExtraAttributesEntrypoint>();
    add<MakeMovieEntrypoint>();
    add<TraceAnterogradeEntrypoint>(_colorManager);
    add<AddGridEntrypoint>();
    add<AddColumnEntrypoint>();
    add<AddSphereEntrypoint>();
    add<AddPillEntrypoint>();
    add<AddCylinderEntrypoint>();
    add<AddBoxEntrypoint>();
    add<MirrorModelEntrypoint>();
    add<SetCircuitThicknessEntrypoint>();
    add<ColorCircuitByIdEntrypoint>(_colorManager);
    add<ColorCircuitBySingleColorEntrypoint>(_colorManager);
    add<AvailableColorMethodsEntrypoint>(_colorManager);
    add<AvailableColorMethodVariablesEntrypoint>(_colorManager);
    add<ColorCircuitByMethodEntrypoint>(_colorManager);
    add<SimulationColorEntryPoint>();

    // RENDERERS ADDED BY THIS PLUGIN
    auto& engine = _api->getEngine();
    _addAdvancedSimulationRenderer(engine);
    _addBasicSimulationRenderer(engine);
    _addDOFPerspectiveCamera(engine);
    _addSphereClippingPerspectiveCamera(engine);

    _api->getParametersManager().getRenderingParameters().setCurrentRenderer(
        "circuit_explorer_advanced");
}

void CircuitExplorerPlugin::preRender()
{
    auto& scene = _api->getScene();
    const auto frame =
        _api->getParametersManager().getAnimationParameters().getFrame();
    VasculatureRadiiSimulation::update(frame, scene);
}

void CircuitExplorerPlugin::postRender() {}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int, char**)
{
    brayns::Log::info("[CE] Initializing circuit explorer plugin.");
    return new CircuitExplorerPlugin();
}
