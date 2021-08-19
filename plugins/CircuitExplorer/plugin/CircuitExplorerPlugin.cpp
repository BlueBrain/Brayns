/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
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
#include <common/commonTypes.h>
#include <common/log.h>

#include <plugin/io/AdvancedCircuitLoader.h>
#include <plugin/io/AstrocyteLoader.h>
#include <plugin/io/BrickLoader.h>
#include <plugin/io/CellGrowthHandler.h>
#include <plugin/io/MeshCircuitLoader.h>
#include <plugin/io/MorphologyCollageLoader.h>
#include <plugin/io/MorphologyLoader.h>
#include <plugin/io/PairSynapsesLoader.h>
#include <plugin/io/SonataLoader.h>
#include <plugin/io/SynapseCircuitLoader.h>
#include <plugin/io/SynapseJSONLoader.h>
#include <plugin/io/VoltageSimulationHandler.h>
#include <plugin/meshing/PointCloudMesher.h>
#include <plugin/movie/MovieMaker.h>

#include <brayns/common/Progress.h>
#include <brayns/common/Timer.h>
#include <brayns/common/geometry/Streamline.h>
#include <brayns/common/utils/base64/base64.h>
#include <brayns/common/utils/imageUtils.h>
#include <brayns/common/utils/stringUtils.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/network/interface/ActionInterface.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/PluginAPI.h>

#include <brion/brion.h>

#include <algorithm>
#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <random>
#include <regex>
#include <unistd.h>
#include <unordered_set>

#include <sys/types.h>
#include <sys/wait.h>

#include <boost/filesystem.hpp>

#include "CircuitExplorerEntrypoints.h"

#define REGISTER_LOADER(LOADER, FUNC) \
    registry.registerLoader({std::bind(&LOADER::getSupportedDataTypes), FUNC});

const std::string ANTEROGRADE_TYPE_AFFERENT = "afferent";
const std::string ANTEROGRADE_TYPE_AFFERENTEXTERNAL = "projection";
const std::string ANTEROGRADE_TYPE_EFFERENT = "efferent";

void _addAdvancedSimulationRenderer(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering advanced renderer" << std::endl;
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
    PLUGIN_INFO << "Registering basic renderer" << std::endl;

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

void _addVoxelizedSimulationRenderer(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering voxelized Simulation renderer" << std::endl;

    brayns::PropertyMap properties;
    properties.add({"alphaCorrection", 0.5, {"Alpha correction"}});
    properties.add({"simulationThreshold", 0., {"Simulation threshold"}});
    properties.add({"exposure", 1., {"Exposure"}});
    properties.add({"fogStart", 0., {"Fog start"}});
    properties.add({"fogThickness", 1e6, {"Fog thickness"}});
    properties.add({"maxBounces", 3, {"Maximum number of ray bounces"}});
    properties.add({"useHardwareRandomizer",
                    false,
                    {"Use hardware accelerated randomizer"}});
    engine.addRendererType("circuit_explorer_voxelized_simulation", properties);
}

void _addGrowthRenderer(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering cell growth renderer" << std::endl;

    brayns::PropertyMap properties;
    properties.add({"alphaCorrection", 0.5, {"Alpha correction"}});
    properties.add({"simulationThreshold", 0., {"Simulation threshold"}});
    properties.add({"exposure", 1., {"Exposure"}});
    properties.add({"fogStart", 0., {"Fog start"}});
    properties.add({"fogThickness", 1e6, {"Fog thickness"}});
    properties.add({"tfColor", false, {"Use transfer function color"}});
    properties.add({"shadows", 0., {"Shadow intensity"}});
    properties.add({"softShadows", 0., {"Shadow softness"}});
    properties.add({"shadowDistance", 1e4, {"Shadow distance"}});
    properties.add({"useHardwareRandomizer",
                    false,
                    {"Use hardware accelerated randomizer"}});
    engine.addRendererType("circuit_explorer_cell_growth", properties);
}

void _addProximityRenderer(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering proximity detection renderer" << std::endl;

    brayns::PropertyMap properties;
    properties.add({"alphaCorrection", 0.5, {"Alpha correction"}});
    properties.add({"detectionDistance", 1., {"Detection distance"}});
    properties.add({"detectionFarColor",
                    brayns::Vector3d{1., 0., 0.},
                    {"Detection far color"}});
    properties.add({"detectionNearColor",
                    brayns::Vector3d{0., 1., 0.},
                    {"Detection near color"}});
    properties.add({"detectionOnDifferentMaterial",
                    false,
                    {"Detection on different material"}});
    properties.add({"surfaceShadingEnabled", true, {"Surface shading"}});
    properties.add({"maxBounces", 3, {"Maximum number of ray bounces"}});
    properties.add({"exposure", 1., {"Exposure"}});
    properties.add({"useHardwareRandomizer",
                    false,
                    {"Use hardware accelerated randomizer"}});
    engine.addRendererType("circuit_explorer_proximity_detection", properties);
}

void _addDOFPerspectiveCamera(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering DOF perspective camera" << std::endl;

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
    PLUGIN_INFO << "Registering sphere clipping perspective camera"
                << std::endl;

    brayns::PropertyMap properties;
    properties.add({"fovy", 45., {"Field of view"}});
    properties.add({"aspect", 1., {"Aspect ratio"}});
    properties.add({"apertureRadius", 0., {"Aperture radius"}});
    properties.add({"focusDistance", 1., {"Focus Distance"}});
    properties.add({"enableClippingPlanes", true, {"Clipping"}});
    engine.addCameraType("circuit_explorer_sphere_clipping", properties);
}

void CircuitExplorerPlugin::init()
{
    auto& scene = _api->getScene();
    auto& registry = scene.getLoaderRegistry();
    auto& pm = _api->getParametersManager();

    // Store the current accumulation settings
    _prevAccumulationSetting = _api->getParametersManager()
                                   .getRenderingParameters()
                                   .getMaxAccumFrames();

    registry.registerLoader(
        std::make_unique<BrickLoader>(scene, BrickLoader::getCLIProperties()));

    registry.registerLoader(
        std::make_unique<SynapseJSONLoader>(scene, _synapseAttributes));

    registry.registerLoader(std::make_unique<SynapseCircuitLoader>(
        scene, pm.getApplicationParameters(),
        SynapseCircuitLoader::getCLIProperties(), this));

    registry.registerLoader(std::make_unique<MorphologyLoader>(
        scene, MorphologyLoader::getCLIProperties()));

    registry.registerLoader(std::make_unique<AdvancedCircuitLoader>(
        scene, pm.getApplicationParameters(),
        AdvancedCircuitLoader::getCLIProperties(), this));

    registry.registerLoader(std::make_unique<MorphologyCollageLoader>(
        scene, pm.getApplicationParameters(),
        MorphologyCollageLoader::getCLIProperties(), this));

    registry.registerLoader(std::make_unique<MeshCircuitLoader>(
        scene, pm.getApplicationParameters(),
        MeshCircuitLoader::getCLIProperties(), this));

    registry.registerLoader(std::make_unique<PairSynapsesLoader>(
        scene, pm.getApplicationParameters(),
        PairSynapsesLoader::getCLIProperties(), this));

    registry.registerLoader(
        std::make_unique<AstrocyteLoader>(scene, pm.getApplicationParameters(),
                                          AstrocyteLoader::getCLIProperties(),
                                          this));

    registry.registerLoader(
        std::make_unique<SonataLoader>(scene, pm.getApplicationParameters(),
                                       SonataLoader::getCLIProperties(), this));

    auto actionInterface = _api->getActionInterface();
    if (actionInterface)
    {
        CircuitExplorerEntrypoints::load(*this, *actionInterface);
    }

    auto& engine = _api->getEngine();
    _addAdvancedSimulationRenderer(engine);
    _addBasicSimulationRenderer(engine);
    _addVoxelizedSimulationRenderer(engine);
    _addGrowthRenderer(engine);
    _addProximityRenderer(engine);
    _addDOFPerspectiveCamera(engine);
    _addSphereClippingPerspectiveCamera(engine);

    _api->getParametersManager().getRenderingParameters().setCurrentRenderer(
        "circuit_explorer_advanced");
}

void CircuitExplorerPlugin::preRender()
{
    if (_dirty)
        _api->getScene().markModified();
    _dirty = false;

    // If there was an error during the last frame export, stop the export
    // process
    if (_exportFrameError)
    {
        _exportFramesToDiskDirty = false;
        _exportFramesToDiskStartFlag = false;
        // Re-establish the old accumulation settings after we are done with the
        // frame export
        _api->getParametersManager().getRenderingParameters().setMaxAccumFrames(
            _prevAccumulationSetting);
    }
    else if (_exportFramesToDiskDirty && _accumulationFrameNumber == 0)
    {
        const auto& ai = _exportFramesToDiskPayload.animationInformation;
        if (_frameNumber >= ai.size())
        {
            _exportFramesToDiskDirty = false;
            _exportFramesToDiskStartFlag = false;
            // Re-establish the old accumulation settings after we are done with
            // the frame export
            _api->getParametersManager()
                .getRenderingParameters()
                .setMaxAccumFrames(_prevAccumulationSetting);
        }
        else
        {
            _exportFramesToDiskStartFlag = true;
            const uint64_t i = 11 * _frameNumber;
            // Camera position
            CameraDefinition cd;
            const auto& ci = _exportFramesToDiskPayload.cameraInformation;
            cd.origin = {ci[i], ci[i + 1], ci[i + 2]};
            cd.direction = {ci[i + 3], ci[i + 4], ci[i + 5]};
            cd.up = {ci[i + 6], ci[i + 7], ci[i + 8]};
            cd.apertureRadius = ci[i + 9];
            cd.focusDistance = ci[i + 10];
            _setCamera(cd);

            // Animation parameters
            _api->getParametersManager().getAnimationParameters().setFrame(
                ai[_frameNumber]);
        }
    }
}

void CircuitExplorerPlugin::postRender()
{
    if (_exportFramesToDiskDirty && _exportFramesToDiskStartFlag)
    {
        ++_accumulationFrameNumber;
        if (_accumulationFrameNumber == _exportFramesToDiskPayload.spp)
        {
            _doExportFrameToDisk();
            ++_frameNumber;
            _accumulationFrameNumber = 0;
        }
    }
}

void CircuitExplorerPlugin::releaseCircuitMapper(const size_t modelId)
{
    _mappers.erase(
        std::remove_if(_mappers.begin(), _mappers.end(),
                       [mid = modelId](
                           const std::unique_ptr<CellObjectMapper>& mapper)
                       { return mapper->getSourceModelId() == mid; }),
        _mappers.end());
}

brayns::Message CircuitExplorerPlugin::exportFramesToDisk(
    const ExportFramesToDisk& payload)
{
    brayns::Message result;

    // Store the current accumulation settings
    if (!_exportFramesToDiskDirty)
        _prevAccumulationSetting = _api->getParametersManager()
                                       .getRenderingParameters()
                                       .getMaxAccumFrames();

    _exportFramesToDiskPayload = payload;
    _exportFramesToDiskDirty = true;
    _exportFramesToDiskStartFlag = false;
    _exportFrameError = false;
    _exportFrameErrorMessage = "";
    _frameNumber = payload.startFrame;
    _accumulationFrameNumber = 0;
    auto& frameBuffer = _api->getEngine().getFrameBuffer();
    frameBuffer.clear();

    // Store the current accumulation settings
    _api->getParametersManager().getRenderingParameters().setMaxAccumFrames(
        static_cast<size_t>(payload.spp) + 1);

    PLUGIN_INFO << "-----------------------------------------------------------"
                   "---------------------"
                << std::endl;
    PLUGIN_INFO << "Movie settings     :" << std::endl;
    PLUGIN_INFO << "- Number of frames : "
                << payload.animationInformation.size() - payload.startFrame
                << std::endl;
    PLUGIN_INFO << "- Samples per pixel: " << payload.spp << std::endl;
    PLUGIN_INFO << "- Frame size       : " << frameBuffer.getSize()
                << std::endl;
    PLUGIN_INFO << "- Export folder    : " << payload.path << std::endl;
    PLUGIN_INFO << "- Start frame      : " << payload.startFrame << std::endl;
    PLUGIN_INFO << "-----------------------------------------------------------"
                   "---------------------"
                << std::endl;

    return result;
}

brayns::Message CircuitExplorerPlugin::_setCamera(
    const CameraDefinition& payload)
{
    brayns::Message result;

    auto& camera = _api->getCamera();

    // Origin
    const auto& o = payload.origin;
    brayns::Vector3f origin{o[0], o[1], o[2]};
    camera.setPosition(origin);

    // Target
    const auto& d = payload.direction;
    brayns::Vector3f direction{d[0], d[1], d[2]};
    camera.setTarget(origin + direction);

    // Up
    const auto& u = payload.up;
    brayns::Vector3f up{u[0], u[1], u[2]};

    // Orientation
    const glm::quat q = glm::inverse(
        glm::lookAt(origin, origin + direction,
                    up)); // Not quite sure why this should be inverted?!?
    camera.setOrientation(q);

    // Aperture
    camera.updateProperty("apertureRadius", payload.apertureRadius);

    // Focus distance
    camera.updateProperty("focusDistance", payload.focusDistance);

    _api->getCamera().markModified();

    PLUGIN_DEBUG << "SET: " << origin << ", " << direction << ", " << up << ", "
                 << glm::inverse(q) << "," << payload.apertureRadius << ","
                 << payload.focusDistance << std::endl;

    return result;
}

void CircuitExplorerPlugin::_doExportFrameToDisk()
{
    auto& frameBuffer = _api->getEngine().getFrameBuffer();
    auto image = frameBuffer.getImage();
    auto fif = _exportFramesToDiskPayload.format == "jpg"
                   ? FIF_JPEG
                   : FreeImage_GetFIFFromFormat(
                         _exportFramesToDiskPayload.format.c_str());
    if (fif == FIF_JPEG)
        image.reset(FreeImage_ConvertTo24Bits(image.get()));
    else if (fif == FIF_UNKNOWN)
        throw std::runtime_error("Unknown format: " +
                                 _exportFramesToDiskPayload.format);

    int flags = _exportFramesToDiskPayload.quality;
    if (fif == FIF_TIFF)
        flags = TIFF_NONE;

    brayns::freeimage::MemoryPtr memory(FreeImage_OpenMemory());

    FreeImage_SaveToMemory(fif, image.get(), memory.get(), flags);

    BYTE* pixels = nullptr;
    DWORD numPixels = 0;
    FreeImage_AcquireMemory(memory.get(), &pixels, &numPixels);

    auto fn = _frameNumber;
    if (_exportFramesToDiskPayload.nameAfterStep)
        fn = _exportFramesToDiskPayload.animationInformation[_frameNumber];
    char frame[7];
    sprintf(frame, "%05d", static_cast<int32_t>(fn));

    std::string filename = _exportFramesToDiskPayload.path + '/' + frame + "." +
                           _exportFramesToDiskPayload.format;
    std::ofstream file;

    try
    {
        file.open(filename, std::ios_base::binary);
    }
    catch (const std::exception& e)
    {
        frameBuffer.clear();
        _exportFrameError = true;
        _exportFrameErrorMessage = std::string(e.what());
        return;
    }

    if (!file.is_open())
    {
        frameBuffer.clear();
        _exportFrameError = true;
        _exportFrameErrorMessage = "Failed to create " + filename;
        return;
    }

    file.write((char*)pixels, numPixels);
    file.close();

    frameBuffer.clear();

    PLUGIN_INFO << "Frame saved to " << filename << std::endl;
}

FrameExportProgress CircuitExplorerPlugin::getFrameExportProgress()
{
    FrameExportProgress result;

    const size_t totalNumberOfFrames =
        (_exportFramesToDiskPayload.animationInformation.size() -
         _exportFramesToDiskPayload.startFrame) *
        _exportFramesToDiskPayload.spp;
    const float currentProgress =
        _frameNumber * _exportFramesToDiskPayload.spp +
        _accumulationFrameNumber;

    result.progress = std::min(static_cast<double>(currentProgress /
                                                   float(totalNumberOfFrames)),
                               1.0);

    if (_exportFrameError)
        result.setError(1, _exportFrameErrorMessage);
    else
        result.setError(0, "");
    return result;
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int /*argc*/,
                                                         char** /*argv*/)
{
    PLUGIN_INFO << "Initializing circuit explorer plugin" << std::endl;
    return new CircuitExplorerPlugin();
}
