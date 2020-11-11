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
#include <plugin/io/SynapseCircuitLoader.h>
#include <plugin/io/SynapseJSONLoader.h>
#include <plugin/io/VoltageSimulationHandler.h>
#include <plugin/meshing/PointCloudMesher.h>

#include <brayns/common/ActionInterface.h>
#include <brayns/common/Progress.h>
#include <brayns/common/Timer.h>
#include <brayns/common/geometry/Streamline.h>
#include <brayns/common/utils/base64/base64.h>
#include <brayns/common/utils/imageUtils.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/PluginAPI.h>

#include <brion/brion.h>

#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <random>
#include <regex>
#include <unistd.h>
#include <unordered_set>

#include <sys/types.h>
#include <sys/wait.h>

#define REGISTER_LOADER(LOADER, FUNC) \
    registry.registerLoader({std::bind(&LOADER::getSupportedDataTypes), FUNC});

const std::string ANTEROGRADE_TYPE_AFFERENT = "afferent";
const std::string ANTEROGRADE_TYPE_AFFERENTEXTERNAL = "projection";
const std::string ANTEROGRADE_TYPE_EFFERENT = "efferent";

void _addAdvancedSimulationRenderer(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering advanced renderer" << std::endl;
    brayns::PropertyMap properties;
    properties.setProperty(
        {"giDistance", 10000., {"Global illumination distance"}});
    properties.setProperty(
        {"giWeight", 0., 1., 1., {"Global illumination weight"}});
    properties.setProperty(
        {"giSamples", 0, 0, 64, {"Global illumination samples"}});
    properties.setProperty({"shadows", 0., 0., 1., {"Shadow intensity"}});
    properties.setProperty({"softShadows", 0., 0., 1., {"Shadow softness"}});
    properties.setProperty(
        {"softShadowsSamples", 1, 1, 64, {"Soft shadow samples"}});
    properties.setProperty(
        {"epsilonFactor", 1., 1., 1000., {"Epsilon factor"}});
    properties.setProperty({"samplingThreshold",
                            0.001,
                            0.001,
                            1.,
                            {"Threshold under which sampling is ignored"}});
    properties.setProperty({"volumeSpecularExponent",
                            20.,
                            1.,
                            100.,
                            {"Volume specular exponent"}});
    properties.setProperty(
        {"volumeAlphaCorrection", 0.5, 0.001, 1., {"Volume alpha correction"}});
    properties.setProperty({"maxDistanceToSecondaryModel",
                            30.,
                            0.1,
                            100.,
                            {"Maximum distance to secondary model"}});
    properties.setProperty({"exposure", 1., 0.01, 10., {"Exposure"}});
    properties.setProperty({"fogStart", 0., 0., 1e6, {"Fog start"}});
    properties.setProperty({"fogThickness", 1e6, 1e6, 1e6, {"Fog thickness"}});
    properties.setProperty(
        {"maxBounces", 3, 1, 100, {"Maximum number of ray bounces"}});
    properties.setProperty({"useHardwareRandomizer",
                            false,
                            {"Use hardware accelerated randomizer"}});
    engine.addRendererType("circuit_explorer_advanced", properties);
}

void _addBasicSimulationRenderer(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering basic renderer" << std::endl;

    brayns::PropertyMap properties;
    properties.setProperty(
        {"alphaCorrection", 0.5, 0.001, 1., {"Alpha correction"}});
    properties.setProperty(
        {"simulationThreshold", 0., 0., 1., {"Simulation threshold"}});
    properties.setProperty({"maxDistanceToSecondaryModel",
                            30.,
                            0.1,
                            100.,
                            {"Maximum distance to secondary model"}});
    properties.setProperty({"exposure", 1., 0.01, 10., {"Exposure"}});
    properties.setProperty(
        {"maxBounces", 3, 1, 100, {"Maximum number of ray bounces"}});
    properties.setProperty({"useHardwareRandomizer",
                            false,
                            {"Use hardware accelerated randomizer"}});
    engine.addRendererType("circuit_explorer_basic", properties);
}

void _addVoxelizedSimulationRenderer(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering voxelized Simulation renderer" << std::endl;

    brayns::PropertyMap properties;
    properties.setProperty(
        {"alphaCorrection", 0.5, 0.001, 1., {"Alpha correction"}});
    properties.setProperty(
        {"simulationThreshold", 0., 0., 1., {"Simulation threshold"}});
    properties.setProperty({"exposure", 1., 0.01, 10., {"Exposure"}});
    properties.setProperty({"fogStart", 0., 0., 1e6, {"Fog start"}});
    properties.setProperty({"fogThickness", 1e6, 1e6, 1e6, {"Fog thickness"}});
    properties.setProperty(
        {"maxBounces", 3, 1, 100, {"Maximum number of ray bounces"}});
    properties.setProperty({"useHardwareRandomizer",
                            false,
                            {"Use hardware accelerated randomizer"}});
    engine.addRendererType("circuit_explorer_voxelized_simulation", properties);
}

void _addGrowthRenderer(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering cell growth renderer" << std::endl;

    brayns::PropertyMap properties;
    properties.setProperty(
        {"alphaCorrection", 0.5, 0.001, 1., {"Alpha correction"}});
    properties.setProperty(
        {"simulationThreshold", 0., 0., 1., {"Simulation threshold"}});
    properties.setProperty({"exposure", 1., 0.01, 10., {"Exposure"}});
    properties.setProperty({"fogStart", 0., 0., 1e6, {"Fog start"}});
    properties.setProperty({"fogThickness", 1e6, 1e6, 1e6, {"Fog thickness"}});
    properties.setProperty({"tfColor", false, {"Use transfer function color"}});
    properties.setProperty({"shadows", 0., 0., 1., {"Shadow intensity"}});
    properties.setProperty({"softShadows", 0., 0., 1., {"Shadow softness"}});
    properties.setProperty(
        {"shadowDistance", 1e4, 0., 1e4, {"Shadow distance"}});
    properties.setProperty({"useHardwareRandomizer",
                            false,
                            {"Use hardware accelerated randomizer"}});
    engine.addRendererType("circuit_explorer_cell_growth", properties);
}

void _addProximityRenderer(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering proximity detection renderer" << std::endl;

    brayns::PropertyMap properties;
    properties.setProperty(
        {"alphaCorrection", 0.5, 0.001, 1., {"Alpha correction"}});
    properties.setProperty({"detectionDistance", 1., {"Detection distance"}});
    properties.setProperty({"detectionFarColor",
                            std::array<double, 3>{{1., 0., 0.}},
                            {"Detection far color"}});
    properties.setProperty({"detectionNearColor",
                            std::array<double, 3>{{0., 1., 0.}},
                            {"Detection near color"}});
    properties.setProperty({"detectionOnDifferentMaterial",
                            false,
                            {"Detection on different material"}});
    properties.setProperty(
        {"surfaceShadingEnabled", true, {"Surface shading"}});
    properties.setProperty(
        {"maxBounces", 3, 1, 100, {"Maximum number of ray bounces"}});
    properties.setProperty({"exposure", 1., 0.01, 10., {"Exposure"}});
    properties.setProperty({"useHardwareRandomizer",
                            false,
                            {"Use hardware accelerated randomizer"}});
    engine.addRendererType("circuit_explorer_proximity_detection", properties);
}

void _addDOFPerspectiveCamera(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering DOF perspective camera" << std::endl;

    brayns::PropertyMap properties;
    properties.setProperty({"fovy", 45., .1, 360., {"Field of view"}});
    properties.setProperty({"aspect", 1., {"Aspect ratio"}});
    properties.setProperty({"apertureRadius", 0., {"Aperture radius"}});
    properties.setProperty({"focusDistance", 1., {"Focus Distance"}});
    properties.setProperty({"enableClippingPlanes", true, {"Clipping"}});
    engine.addCameraType("circuit_explorer_dof_perspective", properties);
}

void _addSphereClippingPerspectiveCamera(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering sphere clipping perspective camera"
                << std::endl;

    brayns::PropertyMap properties;
    properties.setProperty({"fovy", 45., .1, 360., {"Field of view"}});
    properties.setProperty({"aspect", 1., {"Aspect ratio"}});
    properties.setProperty({"apertureRadius", 0., {"Aperture radius"}});
    properties.setProperty({"focusDistance", 1., {"Focus Distance"}});
    properties.setProperty({"enableClippingPlanes", true, {"Clipping"}});
    engine.addCameraType("circuit_explorer_sphere_clipping", properties);
}

std::string _sanitizeString(const std::string& input)
{
    static const std::vector<std::string> sanitetizeItems = {"\"", "\\", "'",
                                                             ";",  "&",  "|",
                                                             "`"};

    std::string result = "";

    for (size_t i = 0; i < input.size(); i++)
    {
        bool found = false;
        for (const auto& token : sanitetizeItems)
        {
            if (std::string(1, input[i]) == token)
            {
                result += "\\" + token;
                found = true;
                break;
            }
        }
        if (!found)
        {
            result += std::string(1, input[i]);
        }
    }
    return result;
}

std::vector<std::string> _splitString(const std::string& source, const char token)
{
    std::vector<std::string> result;
    std::string split;
    std::istringstream ss(source);
    while (std::getline(ss, split, token))
        result.push_back(split);

    return result;
}

CircuitExplorerPlugin::CircuitExplorerPlugin()
    : ExtensionPlugin()
{
}

void CircuitExplorerPlugin::init()
{
    auto& scene = _api->getScene();
    auto& registry = scene.getLoaderRegistry();
    auto& pm = _api->getParametersManager();

    // Store the current accumulation settings
    _prevAccumulationSetting = _api->getParametersManager().getRenderingParameters()
                                                           .getMaxAccumFrames();

    registry.registerLoader(
        std::make_unique<BrickLoader>(scene, BrickLoader::getCLIProperties()));

    registry.registerLoader(
        std::make_unique<SynapseJSONLoader>(scene,
                                            std::move(_synapseAttributes)));

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
                                          AstrocyteLoader::getCLIProperties(), this));

    auto actionInterface = _api->getActionInterface();
        if (actionInterface)
        {
            actionInterface->registerRequest<MaterialDescriptor, brayns::Message>(
                {"set-material",
                 "Modifies a specific material",
                 "MaterialDescriptor",
                 "The data to identify and modify the material"},
                [&](const MaterialDescriptor& param) {
                    return _setMaterial(param);
            });

            actionInterface->registerRequest<MaterialsDescriptor, brayns::Message>(
                {"set-materials",
                 "Set a set of materials from one or more models",
                 "MaterialsDescriptor",
                 "The data to identify and modify the materials of the given models"},
                [&](const MaterialsDescriptor& param) {
                    return _setMaterials(param);
            });

            actionInterface->registerRequest<MaterialRangeDescriptor, brayns::Message>(
                {"set-material-range",
                 "Sets a set of materials of a single model with common material data",
                 "MaterialRangeDescriptor",
                 "The common data with which to update the material"},
                [&](const MaterialRangeDescriptor& param) {
                    return _setMaterialRange(param);
            });

            actionInterface->registerRequest<MaterialProperties>(
                {"get-material-properties",
                 "Returns the properties of the current camera"},
                [&]() -> MaterialProperties { return _getMaterialProperties(); });

            actionInterface->registerRequest<UpdateMaterialProperties, brayns::Message>(
                {"update-material-properties",
                 "Update selected properties of a set of materials of a single model with "
                 "common material data",
                 "UpdateMaterialProperties",
                 "The properties and values to update in the materials"},
                [&](const UpdateMaterialProperties& param) {
                    return _updateMaterialProperties(param);
            });

            actionInterface->registerRequest<ModelId, MaterialIds>(
                {"get-material-ids",
                 "Returns all the material IDs of a given model",
                 "ModelId",
                 "The ID of the model to query"},
                [&](const ModelId& modelId) -> MaterialIds {
                    return _getMaterialIds(modelId);
                });

            actionInterface->registerRequest<ModelMaterialId, MaterialDescriptor>(
                {"get-material",
                  "Returns the properties from the given model and material",
                  "ModelMaterialId",
                  "The ID of the model and of the material to be queried"},
                [&](const ModelMaterialId& modelId) -> MaterialDescriptor {
                    return _getMaterial(modelId);
                });

            actionInterface->registerRequest<MaterialExtraAttributes, brayns::Message>(
                {"set-material-extra-attributes",
                 "Sets the extra material attributes necessary for the Circuit Explorer renderer",
                 "MaterialExtraAttributes",
                 "Id of the model to which the material attributes should be extended"},
                [&](const MaterialExtraAttributes& param) {
                    return _setMaterialExtraAttributes(param);
                });

            actionInterface->registerRequest<SynapseAttributes, brayns::Message>(
                {"set-synapses-attributes",
                 "Sets sypnapse specific attributes for a given model",
                 "SynapseAttributes",
                 "The model and synapse attributes to modify"},
                [&](const SynapseAttributes& param) {
                    return _setSynapseAttributes(param);
                });

            actionInterface->registerRequest<SaveModelToCache, brayns::Message>(
                {"save-model-to-cache",
                 "Builds and saves a Brayns cache model from a given loaded model",
                 "SaveModelToCache",
                 "Model to be saved and parameters for the build of the cache file"},
                [&](const SaveModelToCache& param) {
                    return _saveModelToCache(param);
            });

            actionInterface->registerRequest<ConnectionsPerValue, brayns::Message>(
                {"set-connections-per-value",
                 "Draws a point cloud representing the number of connections for a given frame"
                 " and simulation value",
                 "ConnectionsPerValue",
                 "Model, frame, and value to build the point cloude"},
                [&](const ConnectionsPerValue& param) {
                    return _setConnectionsPerValue(param);
                });

            actionInterface->registerRequest<MetaballsFromSimulationValue, brayns::Message>(
                {"set-metaballs-per-simulation-value",
                 "Adds a metaballs model representing the number of connections for a given frame"
                 " and simulation value",
                 "MetaballsFromSimulationValue",
                 "Model, frame, and value to build the metaballs"},
                [&](const MetaballsFromSimulationValue& param) {
                    return _setMetaballsPerSimulationValue(param);
                });

            actionInterface->registerRequest<CameraDefinition, brayns::Message>(
                {"set-odu-camera",
                  "Set the camera in a position and with an specific orientation towards the scene",
                  "CameraDefinition",
                  "Camera data to modify the current camera"},
                [&](const CameraDefinition& s) { return _setCamera(s); });

            actionInterface->registerRequest<CameraDefinition>(
                {"get-odu-camera",
                 "Returns the properties of the current camera"},
                [&]() -> CameraDefinition { return _getCamera(); });

            actionInterface->registerRequest<AttachCellGrowthHandler, brayns::Message>(
                {"attach-cell-growth-handler",
                 "Attach a dynamic cell growing rendering system for a given model",
                 "AttachCellGrowthHandler",
                 "Model to which to attach the handler, and number of frames the growth should span"},
                [&](const AttachCellGrowthHandler& s) {
                    return _attachCellGrowthHandler(s);
                });

            actionInterface->registerRequest<AttachCircuitSimulationHandler, brayns::Message>(
                    {"attach-circuit-simulation-handler",
                     "Dynamically loads and attach a simulation to a loaded model",
                     "AttachCircuitSimulationHandler",
                     "Model to which attach, and simulation information to fetch"},
                    [&](const AttachCircuitSimulationHandler& s) {
                        return _attachCircuitSimulationHandler(s);
                    });

            actionInterface->registerRequest<ExportFramesToDisk, brayns::Message>(
                {"export-frames-to-disk",
                 "Export a set of frames from a simulation as images written to disk",
                 "ExportFramesToDisk",
                 "Configuration of the simulation to render and image store specifications"},
                [&](const ExportFramesToDisk& s) { return _exportFramesToDisk(s); });

            actionInterface->registerRequest<FrameExportProgress>(
                {"get-export-frames-progress",
                 "Returns the progress of the last issued export frames to disk request"},
                [&](void) -> FrameExportProgress {
                    return _getFrameExportProgress();
                });

            actionInterface->registerRequest<ExportLayerToDisk, ExportLayerToDiskResult>(
                {"export-layer-to-disk",
                 "Export 1 or various layers to disk to be used in composition when generating a"
                 " movie",
                 "ExportLayerToDisk",
                 "Information to store the layer on disk, and image data to store in base64 format"},
                [&](const ExportLayerToDisk& s) {
                    return _exportLayerToDisk(s);
                });

            actionInterface->registerRequest<MakeMovieParameters, brayns::Message>(
                {"make-movie",
                 "Builds a movie file from a set of frames stored on disk",
                 "MakeMovieParameters",
                 "Information to find the frames, and how to compose them into a media file"},
                [&](const MakeMovieParameters& params) { return _makeMovie(params); });

            actionInterface->registerRequest<AnterogradeTracing, brayns::Message>(
                {"trace-anterograde",
                 "Performs neuronal tracing; Showing efferent and afferent synapse relationship "
                 " between cells (including projections)",
                 "AnterogradeTracing",
                 "Data in which to base the cell highlight"},
                [&](const AnterogradeTracing& payload) {
                    return _traceAnterogrades(payload);
                });

            actionInterface->registerRequest<AddGrid, brayns::Message>(
                {"add-grid",
                 "Adds a visual 3D grid to the scene",
                 "AddGrid",
                 "Data necessary to build and render the grid"},
                [&](const AddGrid& payload) { return _addGrid(payload); });

            actionInterface->registerRequest<AddColumn, brayns::Message>(
                {"add-column",
                 "Adds a visual 3D column as a cylinder to the scene",
                 "AddColumn",
                 "Parameters to build and add the column to the scene"},
                [&](const AddColumn& payload) { return _addColumn(payload); });

            actionInterface->registerRequest<AddSphere, AddShapeResult>(
                {"add-sphere",
                 "Adds a visual 3D sphere to the scene",
                 "AddSphere",
                 "Data to build, place and color the sphere"},
                [&](const AddSphere& payload) { return _addSphere(payload); });

            actionInterface->registerRequest<AddPill, AddShapeResult>(
                {"add-pill",
                 "Adds a pill shape to the scene",
                 "AddPill",
                 "The data to choose and build a type of pill"},
                [&](const AddPill& payload) { return _addPill(payload); });

            actionInterface->registerRequest<AddCylinder, AddShapeResult>(
                {"add-cylinder",
                 "Adds a cylinder to the scene",
                 "AddCylinder",
                 "The data to build and color the cylinder"},
                [&](const AddCylinder& payload) { return _addCylinder(payload); });

            actionInterface->registerRequest<AddBox, AddShapeResult>(
                {"add-box",
                 "Adds an axis algined box to the scene",
                 "AddBox",
                 "The data to build and color the box"},
                [&](const AddBox& payload) { return _addBox(payload); });

            actionInterface->registerRequest<RemapCircuit, brayns::Message>(
                {"remap-circuit-color",
                 "Remap the circuit colors to the specified scheme",
                 "RemapCircuit",
                 "Scheme to which remap the circuits colors to"},
                [&](const RemapCircuit& s) { return _remapCircuitToScheme(s); });

            actionInterface->registerRequest<ColorCells, brayns::Message>(
                {"color-cells",
                 "Color specific cells, given by GID, with specific "
                 "colors given in RGB",
                 "ColorCells",
                 "Information about the model, cells and color to "
                 "update"},
                [&](const ColorCells& cc) { return _colorCells(cc); });

        } // if (actionInterface)

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

    if (_exportFramesToDiskDirty && _accumulationFrameNumber == 0)
    {
        const auto& ai = _exportFramesToDiskPayload.animationInformation;
        if (_frameNumber >= ai.size())
        {
            _exportFramesToDiskDirty = false;
            _exportFramesToDiskStartFlag = false;
            // Re-establish the old accumulation settings after we are done with the
            // frame export
            _api->getParametersManager().getRenderingParameters()
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
    if(_exportFramesToDiskDirty && _exportFramesToDiskStartFlag)
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
    _mappers.erase(std::remove_if(_mappers.begin(),
                                  _mappers.end(),
                                  [mid = modelId](const std::unique_ptr<CellObjectMapper>& mapper)
    {
        return mapper->getSourceModelId() == mid;
    }), _mappers.end());
}

brayns::Message CircuitExplorerPlugin::_setMaterialExtraAttributes(
    const MaterialExtraAttributes& mea)
{
    brayns::Message result;

    auto modelDescriptor = _api->getScene().getModel(mea.modelId);
    if (modelDescriptor)
        try
        {
            auto materials = modelDescriptor->getModel().getMaterials();
            for (auto& material : materials)
            {
                brayns::PropertyMap props;
                props.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, false});
                props.setProperty(
                    {MATERIAL_PROPERTY_SHADING_MODE,
                     static_cast<int>(MaterialShadingMode::diffuse)});
                props.setProperty(
                    {MATERIAL_PROPERTY_CLIPPING_MODE,
                     static_cast<int>(MaterialClippingMode::no_clipping)});
                props.setProperty({MATERIAL_PROPERTY_USER_PARAMETER, 1.0});
                material.second->updateProperties(props);
            }
        }
        catch (const std::runtime_error& e)
        {
            PLUGIN_INFO << e.what() << std::endl;
            result.setError(3, e.what());
        }
    else
    {
        PLUGIN_INFO << "Model " << mea.modelId << " is not registered"
                    << std::endl;
        result.setError(2, "Model " + std::to_string(mea.modelId)
                           + " is not registered");
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_setMaterial(const MaterialDescriptor& md)
{
    brayns::Message result;

    auto modelDescriptor = _api->getScene().getModel(md.modelId);
    if (modelDescriptor)
        try
        {
            auto material =
                modelDescriptor->getModel().getMaterial(md.materialId);
            if (material)
            {
                material->setDiffuseColor({md.diffuseColor[0],
                                           md.diffuseColor[1],
                                           md.diffuseColor[2]});
                material->setSpecularColor({md.specularColor[0],
                                            md.specularColor[1],
                                            md.specularColor[2]});

                material->setSpecularExponent(md.specularExponent);
                material->setReflectionIndex(md.reflectionIndex);
                material->setOpacity(md.opacity);
                material->setRefractionIndex(md.refractionIndex);
                material->setEmission(md.emission);
                material->setGlossiness(md.glossiness);
                material->updateProperty(MATERIAL_PROPERTY_CAST_USER_DATA,
                                         md.simulationDataCast);
                material->updateProperty(MATERIAL_PROPERTY_SHADING_MODE,
                                         md.shadingMode);
                material->updateProperty(MATERIAL_PROPERTY_CLIPPING_MODE,
                                         md.clippingMode);
                material->updateProperty(MATERIAL_PROPERTY_USER_PARAMETER,
                                         static_cast<double>(md.userParameter));
                material->markModified(); // This is needed to apply
                                          // propery modifications
                material->commit();

                _dirty = true;
            }
            else
            {
                PLUGIN_INFO << "Material " << md.materialId
                            << " is not registered in model " << md.modelId
                            << std::endl;
                result.setError(4, "Material " + std::to_string(md.materialId)
                                   + " is not registered in model "
                                   + std::to_string(md.modelId));
            }
        }
        catch (const std::runtime_error& e)
        {
            PLUGIN_INFO << e.what() << std::endl;
            result.setError(3, e.what());
        }
    else
    {
        PLUGIN_INFO << "Model " << md.modelId << " is not registered"
                    << std::endl;
        result.setError(2, "Model " + std::to_string(md.modelId)
                           + " is not registered");
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_setMaterials(const MaterialsDescriptor& md)
{
    brayns::Message result;

    for (const auto modelId : md.modelIds)
    {
        auto modelDescriptor = _api->getScene().getModel(modelId);
        if (modelDescriptor)
        {
            size_t id = 0;
            for (const auto materialId : md.materialIds)
            {
                try
                {
                    auto material =
                        modelDescriptor->getModel().getMaterial(materialId);
                    if (material)
                    {
                        if (!md.diffuseColors.empty())
                        {
                            const size_t index = id * 3;
                            material->setDiffuseColor(
                                {md.diffuseColors[index],
                                 md.diffuseColors[index + 1],
                                 md.diffuseColors[index + 2]});
                            material->setSpecularColor(
                                {md.specularColors[index],
                                 md.specularColors[index + 1],
                                 md.specularColors[index + 2]});
                        }

                        if (!md.specularExponents.empty())
                            material->setSpecularExponent(
                                md.specularExponents[id]);
                        if (!md.reflectionIndices.empty())
                            material->setReflectionIndex(
                                md.reflectionIndices[id]);
                        if (!md.opacities.empty())
                            material->setOpacity(md.opacities[id]);
                        if (!md.refractionIndices.empty())
                            material->setRefractionIndex(
                                md.refractionIndices[id]);
                        if (!md.emissions.empty())
                            material->setEmission(md.emissions[id]);
                        if (!md.glossinesses.empty())
                            material->setGlossiness(md.glossinesses[id]);
                        if (!md.simulationDataCasts.empty())
                            material->updateProperty(
                                MATERIAL_PROPERTY_CAST_USER_DATA,
                                md.simulationDataCasts[id]);
                        if (!md.shadingModes.empty())
                            material->updateProperty(
                                MATERIAL_PROPERTY_SHADING_MODE,
                                md.shadingModes[id]);
                        if (!md.clippingModes.empty())
                            material->updateProperty(
                                MATERIAL_PROPERTY_CLIPPING_MODE,
                                md.clippingModes[id]);
                        if (!md.userParameters.empty())
                            material->updateProperty(
                                MATERIAL_PROPERTY_USER_PARAMETER,
                                static_cast<double>(md.userParameters[id]));
                        material->markModified(); // This is needed to apply
                                                  // propery modifications
                        material->commit();
                    }
                }
                catch (const std::runtime_error& e)
                {
                    PLUGIN_INFO << e.what() << std::endl;
                    result.setError(2, e.what());
                }
                ++id;
            }
            _dirty = true;
        }
        else
        {
            PLUGIN_INFO << "Model " << modelId << " is not registered"
                        << std::endl;
            result.setError(1, "Model " + std::to_string(modelId) + "is not "
                               + "registered");
        }
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_setMaterialRange(const MaterialRangeDescriptor& mrd)
{
    brayns::Message result;

    auto modelDescriptor = _api->getScene().getModel(mrd.modelId);
    if (modelDescriptor)
    {
        std::vector<size_t> matIds;
        if(mrd.materialIds.empty())
        {
            matIds.reserve(modelDescriptor->getModel().getMaterials().size());
            for(const auto& mat : modelDescriptor->getModel().getMaterials())
                matIds.push_back(mat.first);
        }
        else
        {
            matIds.reserve(mrd.materialIds.size());
            for(const auto& id : mrd.materialIds)
                matIds.push_back(static_cast<size_t>(id));
        }

        if(mrd.diffuseColor.size() % 3 != 0)
        {
            PLUGIN_ERROR << "set-material-range: The diffuse colors "
                         << "component is not a multiple of 3" << std::endl;
            result.setError(3, "The diffuse colors component size is not multiple of 3");
            return result;
        }

        const size_t numColors = mrd.diffuseColor.size() / 3;

        for (const auto materialId : matIds)
        {
            try
            {
                auto material =
                    modelDescriptor->getModel().getMaterial(materialId);
                if (material)
                {
                    const size_t randomIndex = (rand() % numColors) * 3;
                    material->setDiffuseColor({mrd.diffuseColor[randomIndex],
                                               mrd.diffuseColor[randomIndex + 1],
                                               mrd.diffuseColor[randomIndex + 2]});
                    material->setSpecularColor({mrd.specularColor[0],
                                                mrd.specularColor[1],
                                                mrd.specularColor[2]});

                    material->setSpecularExponent(mrd.specularExponent);
                    material->setReflectionIndex(mrd.reflectionIndex);
                    material->setOpacity(mrd.opacity);
                    material->setRefractionIndex(mrd.refractionIndex);
                    material->setEmission(mrd.emission);
                    material->setGlossiness(mrd.glossiness);
                    material->updateProperty(MATERIAL_PROPERTY_CAST_USER_DATA,
                                             mrd.simulationDataCast);
                    material->updateProperty(MATERIAL_PROPERTY_SHADING_MODE,
                                             mrd.shadingMode);
                    material->updateProperty(MATERIAL_PROPERTY_CLIPPING_MODE,
                                             mrd.clippingMode);
                    material->updateProperty(MATERIAL_PROPERTY_USER_PARAMETER,
                                             static_cast<double>(mrd.userParameter));
                    material->markModified(); // This is needed to apply
                                              // propery modifications
                    material->commit();

                }
            }
            catch (const std::runtime_error& e)
            {
                PLUGIN_INFO << e.what() << std::endl;
                result.setError(2, e.what());
            }
        }
        _dirty = true;
    }
    else
    {
        PLUGIN_INFO << "Model " << mrd.modelId << " is not registered"
                    << std::endl;
        result.setError(1, "Model " + std::to_string(mrd.modelId)
                           + " is not registered");
    }

    return result;
}

MaterialProperties CircuitExplorerPlugin::_getMaterialProperties()
{
    // diffuseColor, specularColor, specularExponent, reflectionIndex, refractionIndex,
    // Opacity, Emission, Glossiness, simulationDataCast, shadingMode, clippingMode, userParameter
    MaterialProperties result;
    result.properties = { "diffuse_color", "specular_color", "specular_exponent",
                          "reflection_index", "refraction_index", "opacity", "emission",
                          "glossiness", "simulation_data_cast", "shading_mode",
                          "clipping_mode", "user_parameter"};
    result.propertyTypes = { "array", "array", "float",
                             "float", "float", "float", "float",
                             "float", "bool", "int", "int", "float"};

    return result;
}

brayns::Vector3d arrayFromString(const std::string& str)
{
    brayns::Vector3d result;

    std::string s (str);
    size_t index = 0;
    static const std::regex regexExpr("([0-9]+\\.?[0-9]*)");
    std::smatch pieces;
    while(std::regex_search(s, pieces, regexExpr) && index < 3)
    {
        result[index] = std::stod(pieces.str());
        s = pieces.suffix();
        ++index;
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_updateMaterialProperties(const UpdateMaterialProperties& r)
{
    brayns::Message result;

    auto model = _api->getScene().getModel(r.modelId);
    if(!model)
    {
        result.setError(1, "Model ID " + std::to_string(r.modelId) + " does not exists");
        return result;
    }

    if(!r.materialIds.empty())
    {
        const auto& materials = model->getModel().getMaterials();
        for(const auto matId : r.materialIds)
        {
            if(materials.find(matId) == materials.end())
            {
                result.setError(2, "One or more material IDs does not belong to the given model");
                return result;
            }
        }
    }

    if(r.propertyNames.size() != r.propertyValues.size())
    {
        result.setError(3, "Property list length does not match property value list");
        return result;
    }

    MaterialExtraAttributes mea;
    mea.modelId = r.modelId;
    _setMaterialExtraAttributes(mea);

    std::vector<std::function<void(brayns::MaterialPtr& m)>> updates;
    for(size_t i = 0; i < r.propertyNames.size(); ++i)
    {
        const auto& prop = r.propertyNames[i];
        const auto& strVal = r.propertyValues[i];

        if(prop == "diffuse_color")
        {
            updates.push_back([arr = arrayFromString(strVal)](brayns::MaterialPtr& m)
            {
                m->setDiffuseColor(arr);
            });
        }
        else if(prop == "specular_color")
        {
            updates.push_back([arr = arrayFromString(strVal)](brayns::MaterialPtr& m)
            {
                m->setSpecularColor(arr);
            });
        }
        else if(prop == "specular_exponent")
        {
            updates.push_back([v = std::stod(strVal)](brayns::MaterialPtr& m)
            {
                m->setSpecularExponent(v);
            });
        }
        else if(prop == "refraction_index")
        {
            updates.push_back([v = std::stod(strVal)](brayns::MaterialPtr& m)
            {
                m->setRefractionIndex(v);
            });
        }
        else if(prop == "reflection_index")
        {
            updates.push_back([v = std::stod(strVal)](brayns::MaterialPtr& m)
            {
                m->setReflectionIndex(v);
            });
        }
        else if(prop == "opacity")
        {
            updates.push_back([v = std::stod(strVal)](brayns::MaterialPtr& m)
            {
                m->setOpacity(v);
            });
        }
        else if(prop == "emission")
        {
            updates.push_back([v = std::stod(strVal)](brayns::MaterialPtr& m)
            {
                m->setEmission(v);
            });
        }
        else if(prop == "glossiness")
        {
            updates.push_back([v = std::stod(strVal)](brayns::MaterialPtr& m)
            {
                m->setGlossiness(v);
            });
        }
        else if(prop == "simulation_data_cast")
        {
            auto copyVal = strVal;
            std::transform(copyVal.begin(), copyVal.end(), copyVal.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            updates.push_back([v = copyVal](brayns::MaterialPtr& m)
            {
                m->updateProperty(MATERIAL_PROPERTY_CAST_USER_DATA,
                                  v == "true");
            });
        }
        else if(prop == "shading_mode")
        {
            updates.push_back([v = std::stoi(strVal)](brayns::MaterialPtr& m)
            {
                m->updateProperty(MATERIAL_PROPERTY_SHADING_MODE,
                                  v);
            });
        }
        else if(prop == "clipping_mode")
        {
            updates.push_back([v = std::stoi(strVal)](brayns::MaterialPtr& m)
            {
                m->updateProperty(MATERIAL_PROPERTY_CLIPPING_MODE,
                                  v);
            });
        }
        else if(prop == "user_parameter")
        {
            updates.push_back([v = std::stod(strVal)](brayns::MaterialPtr& m)
            {
                m->updateProperty(MATERIAL_PROPERTY_USER_PARAMETER,
                                  v);
            });
        }
    }

    std::vector<size_t> matIds;
    if(r.materialIds.empty())
    {
        matIds.reserve(model->getModel().getMaterials().size());
        for(const auto& mat : model->getModel().getMaterials())
            matIds.push_back(mat.first);
    }
    else
    {
        matIds.reserve(r.materialIds.size());
        for(const auto& id : r.materialIds)
            matIds.push_back(static_cast<size_t>(id));
    }

    for (const auto materialId : matIds)
    {
        auto material =
            model->getModel().getMaterial(materialId);
        if (material)
        {
            for(const auto& update : updates)
                update(material);
            material->markModified();
            material->commit();
        }
    }
    model->markModified();
    _api->getScene().markModified();
    _api->triggerRender();

    return result;
}

MaterialIds CircuitExplorerPlugin::_getMaterialIds(const ModelId& modelId)
{
    MaterialIds materialIds;

    auto modelDescriptor = _api->getScene().getModel(modelId.modelId);
    if (modelDescriptor)
    {
        for (const auto& material : modelDescriptor->getModel().getMaterials())
            if (material.first != brayns::BOUNDINGBOX_MATERIAL_ID &&
                material.first != brayns::SECONDARY_MODEL_MATERIAL_ID)
                materialIds.ids.push_back(material.first);
    }
    else
    {
       materialIds.setError(1, "Model " + std::to_string(modelId.modelId)
                               + " is not registered");
    }
    return materialIds;
}

MaterialDescriptor CircuitExplorerPlugin::_getMaterial(const ModelMaterialId& mmId)
{
    MaterialDescriptor result;

    auto modelDescriptor = _api->getScene().getModel(mmId.modelId);
    if (modelDescriptor)
        try
        {
            const auto material =
                modelDescriptor->getModel().getMaterial(mmId.materialId);
            if (material)
            {
                result.modelId = static_cast<int32_t>(mmId.modelId);
                result.materialId = static_cast<int32_t>(mmId.materialId);

                const auto& dc = material->getDiffuseColor();
                result.diffuseColor = {static_cast<float>(dc.r),
                                       static_cast<float>(dc.g),
                                       static_cast<float>(dc.b)};


                const auto& sc = material->getSpecularColor();
                result.specularColor = {static_cast<float>(sc.r),
                                        static_cast<float>(sc.g),
                                        static_cast<float>(sc.b)};

                result.specularExponent = static_cast<float>(material->getSpecularExponent());
                result.reflectionIndex = static_cast<float>(material->getReflectionIndex());
                result.opacity = static_cast<float>(material->getOpacity());
                result.refractionIndex = static_cast<float>(material->getRefractionIndex());
                result.emission = static_cast<float>(material->getEmission());
                result.glossiness = static_cast<float>(material->getGlossiness());
                result.simulationDataCast = material->getProperty<bool>(MATERIAL_PROPERTY_CAST_USER_DATA);
                result.shadingMode = material->getProperty<int32_t>(MATERIAL_PROPERTY_SHADING_MODE);
                result.clippingMode = material->getProperty<int32_t>(MATERIAL_PROPERTY_CLIPPING_MODE);
                result.userParameter = static_cast<float>(
                            material->getProperty<double>(MATERIAL_PROPERTY_USER_PARAMETER));
            }
            else
            {
                PLUGIN_INFO << "Material " << mmId.materialId
                            << " is not registered in model " << mmId.modelId
                            << std::endl;
                result.setError(3, "Material " + std::to_string(mmId.materialId)
                                   + " is not registered in model "
                                   + std::to_string(mmId.modelId));
            }
        }
        catch (const std::runtime_error& e)
        {
            PLUGIN_INFO << e.what() << std::endl;
            result.setError(2, e.what());
        }
    else
    {
        PLUGIN_INFO << "Model " << mmId.modelId << " is not registered"
                    << std::endl;
        result.setError(1, "Model " + std::to_string(mmId.modelId) + " is not"
                           + " registered");
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_remapCircuitToScheme(const RemapCircuit& payload)
{
    brayns::Message result;

    auto modelDescriptor = _api->getScene().getModel(payload.modelId);
    if(modelDescriptor)
    {
        CellObjectMapper* mapper = getMapperForCircuit(payload.modelId);
        if(mapper)
        {
            const auto schemeEnum =
                    stringToEnum<CircuitColorScheme>(payload.scheme);
            auto remapResult = mapper->remapCircuitColors(schemeEnum, _api->getScene());
            result.setError(remapResult.error, remapResult.message);
            _dirty = true;
            _api->getEngine().triggerRender();
        }
    }
    else
    {
        result.setError(1, "The model with ID " + std::to_string(payload.modelId)
                           + " does not exists");
    }
    return result;
}

brayns::Message CircuitExplorerPlugin::_colorCells(const ColorCells& payload)
{
    brayns::Message result;

    if(payload.gids.size() * 3 != payload.colors.size())
    {
        result.setError(1, "There must be 3 color components for each GID");
        return result;
    }

    const auto updateMatColor = [](brayns::ModelDescriptorPtr& model,
                                   size_t matId,
                                   const brayns::Vector3d& color)
    {
        auto mat = model->getModel().getMaterial(matId);
        if(mat)
        {
            mat->setDiffuseColor(color);
            mat->markModified();
            mat->commit();
        }

    };

    auto modelDescriptor = _api->getScene().getModel(payload.modelId);
    if(!modelDescriptor)
    {
        result.setError(2, "The model with ID " + std::to_string(payload.modelId)
                           + " does not exists");
        return result;
    }

    CellObjectMapper* mapper = getMapperForCircuit(payload.modelId);
    if(mapper)
    {
        const auto& mapping = mapper->getMapping();
        for(size_t i = 0; i < payload.gids.size(); ++i)
        {
            const auto cellGID = payload.gids[i];
            const auto colorIndx = i * 3;
            const brayns::Vector3d color {payload.colors[colorIndx],
                                          payload.colors[colorIndx + 1],
                                          payload.colors[colorIndx + 2]};
            auto it = mapping.find(cellGID);
            if(it != mapping.end())
            {
                const MorphologyMap& mmap = it->second;
                for(const auto& kv : mmap._coneMap)
                    updateMatColor(modelDescriptor, kv.first, color);
                for(const auto& kv : mmap._cylinderMap)
                    updateMatColor(modelDescriptor, kv.first, color);
                for(const auto& kv : mmap._sdfBezierMap)
                    updateMatColor(modelDescriptor, kv.first, color);
                for(const auto& kv : mmap._sdfGeometryMap)
                    updateMatColor(modelDescriptor, kv.first, color);
                for(const auto& kv : mmap._sphereMap)
                    updateMatColor(modelDescriptor, kv.first, color);
            }
        }

        modelDescriptor->markModified();
        _api->getScene().markModified();
        _api->triggerRender();
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_setSynapseAttributes(
    const SynapseAttributes& param)
{
    brayns::Message result;

    try
    {
        _synapseAttributes = param;
        SynapseJSONLoader loader(_api->getScene(), _synapseAttributes);
        brayns::Vector3fs colors;
        for (const auto& htmlColor : _synapseAttributes.htmlColors)
        {
            auto hexCode = htmlColor;
            if (hexCode.at(0) == '#')
            {
                hexCode = hexCode.erase(0, 1);
            }
            int r, g, b;
            std::istringstream(hexCode.substr(0, 2)) >> std::hex >> r;
            std::istringstream(hexCode.substr(2, 2)) >> std::hex >> g;
            std::istringstream(hexCode.substr(4, 2)) >> std::hex >> b;

            brayns::Vector3f color{r / 255.f, g / 255.f, b / 255.f};
            colors.push_back(color);
        }
        const auto modelDescriptor =
            loader.importSynapsesFromGIDs(_synapseAttributes, colors);

        _api->getScene().addModel(modelDescriptor);

        PLUGIN_INFO << "Synapses successfully added for GID "
                    << _synapseAttributes.gid << std::endl;
        _dirty = true;
    }
    catch (const std::runtime_error& e)
    {
        PLUGIN_ERROR << e.what() << std::endl;
        result.setError(1, e.what());
    }
    catch (...)
    {
        PLUGIN_ERROR
            << "Unexpected exception occured in _updateMaterialFromJson"
            << std::endl;
        result.setError(2, "Unexpected exception occured in _setSynapseAttribute");
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_saveModelToCache(const SaveModelToCache& saveModel)
{
    brayns::Message result;

    auto modelDescriptor = _api->getScene().getModel(saveModel.modelId);
    if (modelDescriptor)
    {
        BrickLoader brickLoader(_api->getScene());
        brickLoader.exportToFile(modelDescriptor, saveModel.path);
    }
    else
    {
        PLUGIN_ERROR << "Model " << saveModel.modelId << " is not registered"
                     << std::endl;
        result.setError(1, "Model " + std::to_string(saveModel.modelId) + " is"
                           + " not registered");
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_setConnectionsPerValue(
    const ConnectionsPerValue& cpv)
{
    brayns::Message result;

    PointCloud pointCloud;

    auto modelDescriptor = _api->getScene().getModel(cpv.modelId);
    if (modelDescriptor)
    {
        auto simulationHandler =
            modelDescriptor->getModel().getSimulationHandler();
        if (!simulationHandler)
        {
            BRAYNS_ERROR << "Scene has not user data handler" << std::endl;
            result.setError(1, "Scene has no user data handler");
            return result;
        }

        auto& model = modelDescriptor->getModel();
        for (const auto& spheres : model.getSpheres())
        {
            for (const auto& s : spheres.second)
            {
                const float* data = static_cast<float*>(
                    simulationHandler->getFrameData(cpv.frame));

                const float value = data[s.userData];
                if (abs(value - cpv.value) < cpv.epsilon)
                    pointCloud[spheres.first].push_back(
                        {s.center.x, s.center.y, s.center.z, s.radius});
            }
        }

        if (!pointCloud.empty())
        {
            auto meshModel = _api->getScene().createModel();
            PointCloudMesher mesher;
            if (mesher.toConvexHull(*meshModel, pointCloud))
            {
                auto modelDesc = std::make_shared<brayns::ModelDescriptor>(
                    std::move(meshModel),
                    "Connection for value " + std::to_string(cpv.value));

                _api->getScene().addModel(modelDesc);
                _dirty = true;
            }
        }
        else
        {
            PLUGIN_INFO << "No connections added for value "
                        << std::to_string(cpv.value) << std::endl;
            result.setError(2, "No connections added for value "
                               + std::to_string(cpv.value));
        }
    }
    else
    {
        PLUGIN_INFO << "Model " << cpv.modelId << " is not registered"
                    << std::endl;
        result.setError(3, "Model " + std::to_string(cpv.modelId) + " is"
                           + " not registered");
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_setMetaballsPerSimulationValue(
    const MetaballsFromSimulationValue& mpsv)
{
    brayns::Message result;

    PointCloud pointCloud;

    auto modelDescriptor = _api->getScene().getModel(mpsv.modelId);
    if (modelDescriptor)
    {
        auto simulationHandler =
            modelDescriptor->getModel().getSimulationHandler();
        if (!simulationHandler)
        {
            BRAYNS_ERROR << "Scene has not user data handler" << std::endl;
            result.setError(1, "Scene has no user data handler");
            return result;
        }

        auto& model = modelDescriptor->getModel();
        for (const auto& spheres : model.getSpheres())
        {
            for (const auto& s : spheres.second)
            {
                const float* data = static_cast<float*>(
                    simulationHandler->getFrameData(mpsv.frame));

                const float value = data[s.userData];
                if (abs(value - mpsv.value) < mpsv.epsilon)
                    pointCloud[spheres.first].push_back(
                        {s.center.x, s.center.y, s.center.z, s.radius});
            }
        }

        if (!pointCloud.empty())
        {
            auto meshModel = _api->getScene().createModel();
            PointCloudMesher mesher;
            if (mesher.toMetaballs(*meshModel, pointCloud, mpsv.gridSize,
                                   mpsv.threshold))
            {
                auto modelDesc = std::make_shared<brayns::ModelDescriptor>(
                    std::move(meshModel),
                    "Connection for value " + std::to_string(mpsv.value));

                _api->getScene().addModel(modelDesc);
                PLUGIN_INFO << "Metaballs successfully added to the scene"
                            << std::endl;

                _dirty = true;
            }
            else
            {
                PLUGIN_INFO << "No mesh was created for value "
                            << std::to_string(mpsv.value) << std::endl;
                result.setError(2, "No mesh was created for value "
                                   + std::to_string(mpsv.value));
            }
        }
        else
        {
            PLUGIN_INFO << "No connections added for value "
                        << std::to_string(mpsv.value) << std::endl;
            result.setError(3, "No connections added for value " + std::to_string(mpsv.value));
        }
    }
    else
    {
        PLUGIN_INFO << "Model " << mpsv.modelId << " is not registered" << std::endl;
        result.setError(4, "Model " + std::to_string(mpsv.modelId) + " is not registered");
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_setCamera(const CameraDefinition& payload)
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

CameraDefinition CircuitExplorerPlugin::_getCamera()
{
    const auto& camera = _api->getCamera();

    CameraDefinition cd;
    const auto& p = camera.getPosition();
    cd.origin = {p.x, p.y, p.z};
    const auto d =
        glm::rotate(camera.getOrientation(), brayns::Vector3d(0., 0., -1.));
    cd.direction = {d.x, d.y, d.z};
    const auto u =
        glm::rotate(camera.getOrientation(), brayns::Vector3d(0., 1., 0.));
    cd.up = {u.x, u.y, u.z};
    PLUGIN_DEBUG << "GET: " << p << ", " << d << ", " << u << ", "
                 << camera.getOrientation() << std::endl;
    return cd;
}

brayns::Message CircuitExplorerPlugin::_attachCellGrowthHandler(
    const AttachCellGrowthHandler& payload)
{
    brayns::Message result;

    PLUGIN_INFO << "Attaching Cell Growth Handler to model " << payload.modelId
                << std::endl;
    auto modelDescriptor = _api->getScene().getModel(payload.modelId);
    if (modelDescriptor)
    {
        auto handler = std::make_shared<CellGrowthHandler>(payload.nbFrames);
        modelDescriptor->getModel().setSimulationHandler(handler);
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_attachCircuitSimulationHandler(
    const AttachCircuitSimulationHandler& payload)
{
    brayns::Message result;

    PLUGIN_INFO << "Attaching Circuit Simulation Handler to model "
                << payload.modelId << std::endl;
    auto modelDescriptor = _api->getScene().getModel(payload.modelId);
    if (modelDescriptor)
    {
        const brion::BlueConfig blueConfiguration(payload.circuitConfiguration);
        const brain::Circuit circuit(blueConfiguration);
        auto gids = circuit.getGIDs();
        auto handler = std::make_shared<VoltageSimulationHandler>(
            blueConfiguration.getReportSource(payload.reportName).getPath(),
            gids, payload.synchronousMode);
        auto& model = modelDescriptor->getModel();
        model.setSimulationHandler(handler);
        AdvancedCircuitLoader::setSimulationTransferFunction(
            model.getTransferFunction());
    }
    else
    {
        PLUGIN_ERROR << "Model " << payload.modelId << " does not exist"
                     << std::endl;
        result.setError(1, "Model " + std::to_string(payload.modelId) + " does not exist");
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_exportFramesToDisk(
    const ExportFramesToDisk& payload)
{
    brayns::Message result;

    // Store the current accumulation settings
    if(!_exportFramesToDiskDirty)
        _prevAccumulationSetting = _api->getParametersManager().getRenderingParameters()
                                                               .getMaxAccumFrames();

    _exportFramesToDiskPayload = payload;
    _exportFramesToDiskDirty = true;
    _exportFramesToDiskStartFlag = false;
    _frameNumber = payload.startFrame;
    _accumulationFrameNumber = 0;
    auto& frameBuffer = _api->getEngine().getFrameBuffer();
    frameBuffer.clear();

    // Store the current accumulation settings
    _api->getParametersManager().getRenderingParameters()
                                .setMaxAccumFrames(static_cast<size_t>(payload.spp) + 1);

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

    char frame[7];
    sprintf(frame, "%05d",
            static_cast<int32_t>(_exportFramesToDiskPayload.animationInformation[_frameNumber]));

    std::string filename = _exportFramesToDiskPayload.path + '/' + frame + "." +
                           _exportFramesToDiskPayload.format;
    std::ofstream file;
    file.open(filename, std::ios_base::binary);
    if (!file.is_open())
        PLUGIN_THROW("Failed to create " + filename);

    file.write((char*)pixels, numPixels);
    file.close();

    frameBuffer.clear();

    PLUGIN_INFO << "Frame saved to " << filename << std::endl;
}

FrameExportProgress CircuitExplorerPlugin::_getFrameExportProgress()
{
    FrameExportProgress result;
    const size_t totalNumberOfFrames =
        (_exportFramesToDiskPayload.animationInformation.size() -
         _exportFramesToDiskPayload.startFrame) *
        _exportFramesToDiskPayload.spp;
    const float currentProgress =
        _frameNumber * _exportFramesToDiskPayload.spp +
        _accumulationFrameNumber;

    result.progress = currentProgress / float(totalNumberOfFrames);
    return result;
}

ExportLayerToDiskResult CircuitExplorerPlugin::_exportLayerToDisk(const ExportLayerToDisk& payload)
{
    ExportLayerToDiskResult result;

    const uint32_t end = payload.startFrame + payload.framesCount;
    for(uint32_t i = payload.startFrame; i < end; i++)
    {
        char frame[7];
        sprintf(frame, "%05d", i);
        const std::string frameFileName(frame);
        const std::string slash = payload.path.at(payload.path.length() - 1) == '/'? "" : "/";
        const std::string srcFramePath = payload.path + slash + frameFileName + ".png";

        std::ifstream testSourceFrame (srcFramePath);
        const bool state = testSourceFrame.good();
        testSourceFrame.close();

        // Do not write layer if the corresponding frame does not exists
        if(!state) continue;

        const std::string layerPath = payload.path + slash + payload.name + frameFileName + ".png";
        const std::string decodedImage = base64_decode(payload.data);

        std::ofstream outFileWriter (layerPath, std::ofstream::out | std::ofstream::trunc);
        outFileWriter << decodedImage;
        outFileWriter.flush();
        outFileWriter.close();

        result.frames.push_back(i);
    }

    return result;
}

inline bool _createMediaFile(const MakeMovieParameters& params, brayns::Message& result)
{
    std::set<std::string> uniqueLayers;
    uniqueLayers.insert(params.layers.begin(), params.layers.end());

    auto defaultIt = uniqueLayers.find("movie");
    if(defaultIt == uniqueLayers.end())
    {
        PLUGIN_ERROR << "MakeMovie: Default layer \"movie\" not present. Aborting."
                     << std::endl;
        result.setError(1, "Default layer \"movie\" not present");
        return false;
    }

    // Find ffmpeg executable path
    std::array<char, 256> buffer;
    std::string ffmpegPath;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("which ffmpeg", "r"),
                                                  pclose);
    if (!pipe)
    {
        PLUGIN_ERROR << "Could not launch movie creation: ffmpeg not found"
                     << std::endl;
        result.setError(2, "Could not launch movie creation: ffmpeg not found");
        return false;
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        ffmpegPath += buffer.data();
    }

    // Remove new lines
    size_t pos = std::string::npos;
    do
    {
        pos = ffmpegPath.find("\n");
        if (pos != std::string::npos)
        {
            ffmpegPath.replace(pos, pos + 2, "");
        }
    } while (pos != std::string::npos);

    // Get sanitetized string inputs
    std::string sanitizedFramesFolder = params.framesFolderPath;
    const std::string slash =
        sanitizedFramesFolder[sanitizedFramesFolder.length() - 1] == '/' ? ""
                                                                         : "/";

    std::string sanitizedFramesExt = params.framesFileExtension;
    std::string sanitizedOutputPath = params.outputMoviePath;

    sanitizedFramesFolder = _sanitizeString(sanitizedFramesFolder);
    sanitizedFramesExt = _sanitizeString(sanitizedFramesExt);
    sanitizedOutputPath = _sanitizeString(sanitizedOutputPath);

    const std::string inputParam =
        sanitizedFramesFolder + slash + "%05d." + sanitizedFramesExt;

    std::string inputLayer = "";
    if(params.layers.size() > 1)
    {
        std::string layerName;
        // Pick the first layer whose name is not the default one
        for(const auto& ln : uniqueLayers)
        {
            if(ln != "movie")
            {
                layerName = ln;
                break;
            }
        }

        inputLayer = sanitizedFramesFolder + slash + layerName + "%05d." + sanitizedFramesExt;
    }

    const std::string filterParam =
        "scale=" + std::to_string(static_cast<int>(params.dimensions[0])) +
        ":" + std::to_string(static_cast<int>(params.dimensions[1])) +
        ",format=yuv420p";
    const pid_t pid = fork();

    if (pid == 0)
    {
        if(inputLayer.empty())
            execl(ffmpegPath.c_str(), "ffmpeg", "-y", "-hide_banner", "-loglevel",
                  "0", "-r", std::to_string(params.fpsRate).c_str(), "-i",
                  inputParam.c_str(), "-vf", filterParam.c_str(), "-crf", "0",
                  "-codec:v", "libx264", sanitizedOutputPath.c_str(),
                  static_cast<char*>(nullptr));
        else
            execl(ffmpegPath.c_str(), "ffmpeg", "-y", "-hide_banner", "-loglevel",
                  "0", "-r", std::to_string(params.fpsRate).c_str(), "-i",
                  inputParam.c_str(), "-i", inputLayer.c_str(), "-filter_complex",
                  "'[0:v][1:v]hstack[vid]'", "-map", "[vid]", "-vf", filterParam.c_str(),
                  "-crf", "0", "-codec:v", "libx264", sanitizedOutputPath.c_str(),
                  static_cast<char*>(nullptr));
    }
    else
    {
        int status = 0;
        if (waitpid(pid, &status, 0) > 0)
        {
            // If we could not make the movie, inform and stop execution
            if(WIFEXITED(status) && WEXITSTATUS(status))
            {
                if(WEXITSTATUS(status) == 127)
                {
                    PLUGIN_ERROR << "Could not create media video file. FFMPEG "
                                    "returned with error "
                                 << status << std::endl;
                    result.setError(3, "Could not create media file. FFMPEG returned "
                                       + std::to_string(status));
                    return false;
                }
            }
        }
        else
        {
            PLUGIN_ERROR << "Could not create media video file. "
                            "Could not launch FFMPEG."
                         << std::endl;
            result.setError(4, "Could not create media video file. Could not launch FFMPEG");
            return false;
        }
    }

    return true;
}

brayns::Message CircuitExplorerPlugin::_makeMovie(const MakeMovieParameters& params)
{
    brayns::Message result;

    bool ffmpegSucessful = _createMediaFile(params, result);
    
    if (ffmpegSucessful && params.eraseFrames)
    {
        std::string sanitizedFramesFolder = params.framesFolderPath;
        const std::string slash =
        sanitizedFramesFolder[sanitizedFramesFolder.length() - 1] == '/' ? ""
                                                                         : "/";
        sanitizedFramesFolder = _sanitizeString(sanitizedFramesFolder);
        DIR* dir;
        struct dirent* ent;
        const std::regex fileNameRegex("[0-9]{5}." +
                                       params.framesFileExtension);
        if ((dir = opendir(params.framesFolderPath.c_str())) != nullptr)
        {
            while ((ent = readdir(dir)) != nullptr)
            {
                std::string fileName(ent->d_name);
                if (std::regex_match(fileName, fileNameRegex))
                {
                    const std::string fullPath =
                        sanitizedFramesFolder + slash + fileName;
                    PLUGIN_INFO << "Cleaning frame " << fullPath << std::endl;
                    remove(fullPath.c_str());
                }
            }
            closedir(dir);
        }
        else
        {
            PLUGIN_ERROR << "make-movie: Could not clean up frames"
                         << std::endl;
            result.setError(5, "Could not clean up frames");
        }
    }

    return result;
}

brayns::Message CircuitExplorerPlugin::_traceAnterogrades(const AnterogradeTracing &payload)
{
    brayns::Message result;

    if(payload.cellGIDs.empty())
    {
        result.setError(1, "No input cell GIDs specified");
        return result;
    }
    if(payload.sourceCellColor.size() < 4)
    {
        result.setError(2, "Source cell stain color must have 4 components (RGBA)");
        return result;
    }
    if(payload.connectedCellsColor.size() < 4)
    {
        result.setError(3, "Connected cell stain color must have 4 components (RGBA)");
        return result;
    }
    if(payload.nonConnectedCellsColor.size() < 4)
    {
        result.setError(4, "Non connected cell stain color must have 4 components (RGBA)");
        return result;
    }

    auto modelDescriptor = _api->getScene().getModel(static_cast<size_t>(payload.modelId));
    if(!modelDescriptor)
    {
        result.setError(5, "The given model ID does not correspond to any existing scene model");
        return result;
    }

    auto cellMapper = getMapperForCircuit(payload.modelId);
    if(!cellMapper)
    {
        result.setError(6, "There is not cell mapping information for the given circuit");
        return result;
    }

    const auto& cellMaterialMap = cellMapper->getMapping();

    // Function to search for material ids based on cell GIDs using the mapper
    const std::function<void(std::unordered_set<int32_t>&,
                             const std::vector<uint32_t>&,
                             const std::unordered_map<size_t, MorphologyMap>&)> searchFunc =
    [](std::unordered_set<int32_t>& buffer,
       const std::vector<uint32_t>& src,
       const std::unordered_map<size_t, MorphologyMap>& m)
    {
        for(const auto& cellId : src)
        {
            auto morphologyMapIt = m.find(cellId);
            if(morphologyMapIt != m.end())
            {
                const auto& morphologyMap = morphologyMapIt->second;
                if(morphologyMap._hasMesh)
                    buffer.insert(static_cast<int32_t>(morphologyMap._triangleIndx));
                for(const auto& kvp : morphologyMap._coneMap)
                    buffer.insert(static_cast<int32_t>(kvp.first));
                for(const auto& kvp : morphologyMap._sphereMap)
                    buffer.insert(static_cast<int32_t>(kvp.first));
                for(const auto& kvp : morphologyMap._cylinderMap)
                    buffer.insert(static_cast<int32_t>(kvp.first));
                for(const auto& kvp : morphologyMap._sdfGeometryMap)
                    buffer.insert(static_cast<int32_t>(kvp.first));
            }
        }
    };

    // Gather material ids for the source and target cells
    std::unordered_set<int32_t> sourceCellMaterialIds;
    searchFunc(sourceCellMaterialIds, payload.cellGIDs, cellMaterialMap);
    std::unordered_set<int32_t> targetCellMaterialIds;
    searchFunc(targetCellMaterialIds, payload.targetCellGIDs, cellMaterialMap);

    // Enable extra attributes on materials
    MaterialExtraAttributes mea;
    mea.modelId = payload.modelId;
    _setMaterialExtraAttributes(mea);

    // Reset all cells to non-stained color
    MaterialRangeDescriptor mrd;
    mrd.modelId = payload.modelId;
    mrd.diffuseColor =
    {
        static_cast<float>(payload.nonConnectedCellsColor[0]),
        static_cast<float>(payload.nonConnectedCellsColor[1]),
        static_cast<float>(payload.nonConnectedCellsColor[2])
    };

    mrd.specularColor = {1.f, 1.f, 1.f};
    mrd.specularExponent = 20.f;
    mrd.glossiness = 1.f;
    mrd.reflectionIndex = 0.f;
    mrd.refractionIndex = 0.f;
    mrd.opacity = std::min<float>(std::max<float>(payload.nonConnectedCellsColor[4], 0.1f), 1.f);
    mrd.shadingMode = static_cast<int>(MaterialShadingMode::none);
    mrd.clippingMode = static_cast<int>(MaterialClippingMode::no_clipping);
    mrd.simulationDataCast = true;
    _setMaterialRange(mrd);

    // Stain (if any) source cell
    if(sourceCellMaterialIds.size() > 0)
    {
        MaterialRangeDescriptor sourcesMrd = mrd;
        sourcesMrd.diffuseColor =
        {
            static_cast<float>(payload.sourceCellColor[0]),
            static_cast<float>(payload.sourceCellColor[1]),
            static_cast<float>(payload.sourceCellColor[2])
        };
        sourcesMrd.opacity = 1.f;
        sourcesMrd.materialIds.insert(sourcesMrd.materialIds.end(),
                               sourceCellMaterialIds.begin(),
                               sourceCellMaterialIds.end());
        _setMaterialRange(sourcesMrd);
    }

    if(targetCellMaterialIds.size() > 0)
    {
        MaterialRangeDescriptor targetsMrd = mrd;
        targetsMrd.diffuseColor =
        {
            static_cast<float>(payload.connectedCellsColor[0]),
            static_cast<float>(payload.connectedCellsColor[1]),
            static_cast<float>(payload.connectedCellsColor[2])
        };
        targetsMrd.opacity = 1.f;
        targetsMrd.materialIds.insert(targetsMrd.materialIds.end(),
                               targetCellMaterialIds.begin(),
                               targetCellMaterialIds.end());
        _setMaterialRange(targetsMrd);
    }

    modelDescriptor->markModified();
    _api->getScene().markModified();
    _api->triggerRender();

    return result;
}

void CircuitExplorerPlugin::_createShapeMaterial(brayns::ModelPtr& model,
                                                 const size_t id,
                                                 const brayns::Vector3d& color,
                                                 const double& opacity)
{
    brayns::MaterialPtr mptr = model->createMaterial(id, std::to_string(id));
    mptr->setDiffuseColor(color);
    mptr->setOpacity(opacity);
    mptr->setSpecularExponent(20.0);

    brayns::PropertyMap props;
    props.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, false});
    props.setProperty(
        {MATERIAL_PROPERTY_SHADING_MODE,
         static_cast<int>(MaterialShadingMode::diffuse)});
    props.setProperty({MATERIAL_PROPERTY_CLIPPING_MODE,
                       static_cast<int>(MaterialClippingMode::no_clipping)});

    mptr->updateProperties(props);

    mptr->markModified();
    mptr->commit();
}

AddShapeResult CircuitExplorerPlugin::_addSphere(const AddSphere& payload)
{
    AddShapeResult result;

    if (payload.center.size() < 3)
    {
        result.setError(1, "Sphere center has the wrong number of parameters (3 necessary)");
        return result;
    }

    if (payload.color.size() < 4)
    {
        result.setError(2, "Sphere color has the wrong number of coefficents (RGBA, 4 necessary)");
        return result;
    }

    if (payload.radius < 0.0f)
    {
        result.setError(3, "Negative radius passed for sphere creation");
        return result;
    }

    brayns::ModelPtr modelptr = _api->getScene().createModel();

    const size_t matId = 1;
    const brayns::Vector3d color(payload.color[0], payload.color[1],
                                 payload.color[2]);
    const double opacity = payload.color[3];
    _createShapeMaterial(modelptr, matId, color, opacity);

    const brayns::Vector3f center(payload.center[0], payload.center[1],
                                  payload.center[2]);
    modelptr->addSphere(matId, {center, static_cast<float>(payload.radius)});

    size_t numModels = _api->getScene().getNumModels();
    const std::string name = payload.name.empty()
                                 ? "sphere_" + std::to_string(numModels)
                                 : payload.name;
    result.id = _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelptr), name));
    _api->getScene().markModified();

    _api->getEngine().triggerRender();

    _dirty = true;
    return result;
}

AddShapeResult CircuitExplorerPlugin::_addPill(const AddPill& payload)
{
    AddShapeResult result;

    if (payload.p1.size() < 3)
    {
        result.setError(1, "Pill point 1 has the wrong number of parameters (3 necessary)");
        return result;
    }
    if (payload.p2.size() < 3)
    {
        result.setError(2, "Pill point 2 has the wrong number of parameters (3 necessary)");
        return result;
    }
    if (payload.color.size() < 4)
    {
        result.setError(3, "Pill color has the wrong number of parameters (RGBA, 4 necessary)");
        return result;
    }
    if (payload.type != "pill" && payload.type != "conepill" &&
        payload.type != "sigmoidpill")
    {
        result.setError(4, "Unknown pill type parameter. Must be either \"pill\", "
                           "\"conepill\", or \"sigmoidpill\"");
        return result;
    }
    if (payload.radius1 < 0.0f || payload.radius2 < 0.0f)
    {
        result.setError(5, "Negative radius passed for the pill creation");
        return result;
    }

    brayns::ModelPtr modelptr = _api->getScene().createModel();

    size_t matId = 1;
    const brayns::Vector3d color(payload.color[0], payload.color[1],
                                 payload.color[2]);
    const double opacity = payload.color[3];
    _createShapeMaterial(modelptr, matId, color, opacity);

    const brayns::Vector3f p0(payload.p1[0], payload.p1[1], payload.p1[2]);
    const brayns::Vector3f p1(payload.p2[0], payload.p2[1], payload.p2[2]);
    brayns::SDFGeometry sdf;
    if (payload.type == "pill")
        sdf = brayns::createSDFPill(p0, p1, payload.radius1);
    else if (payload.type == "conepill")
        sdf =
            brayns::createSDFConePill(p0, p1, payload.radius1, payload.radius2);
    else if (payload.type == "sigmoidpill")
        sdf = brayns::createSDFConePillSigmoid(p0, p1, payload.radius1,
                                               payload.radius2);

    modelptr->addSDFGeometry(matId, sdf, {});

    size_t numModels = _api->getScene().getNumModels();
    const std::string name =
        payload.name.empty() ? payload.type + "_" + std::to_string(numModels)
                             : payload.name;
    result.id = _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelptr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    _dirty = true;

    return result;
}

AddShapeResult CircuitExplorerPlugin::_addCylinder(const AddCylinder& payload)
{
    AddShapeResult result;

    if (payload.center.size() < 3)
    {
        result.setError(1, "Cylinder center has the wrong number of parameters (3 necessary)");
        return result;
        ;
    }
    if (payload.up.size() < 3)
    {
        result.setError(2, "Cylinder up has the wrong number of parameters (3 necessary)");
        return result;
    }
    if (payload.color.size() < 4)
    {
        result.setError(3, "Cylinder color has the wrong number of parameters (RGBA, 4 necessary)");
        return result;
    }
    if (payload.radius < 0.0f)
    {
        result.setError(4, "Negative radius passed for cylinder creation");
        return result;
    }

    brayns::ModelPtr modelptr = _api->getScene().createModel();

    const size_t matId = 1;
    const brayns::Vector3d color(payload.color[0], payload.color[1],
                                 payload.color[2]);
    const double opacity = payload.color[3];
    _createShapeMaterial(modelptr, matId, color, opacity);

    const brayns::Vector3f center(payload.center[0], payload.center[1],
                                  payload.center[2]);
    const brayns::Vector3f up(payload.up[0], payload.up[1], payload.up[2]);
    modelptr->addCylinder(matId, {center, up, static_cast<float>(payload.radius)});

    size_t numModels = _api->getScene().getNumModels();
    const std::string name = payload.name.empty()
                                 ? "cylinder_" + std::to_string(numModels)
                                 : payload.name;
    result.id = _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelptr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    _dirty = true;

    return result;
}

AddShapeResult CircuitExplorerPlugin::_addBox(const AddBox& payload)
{
    AddShapeResult result;

    if (payload.minCorner.size() < 3)
    {
        result.setError(1, "Box minCorner has the wrong number of parameters (3 necessary)");
        return result;
    }
    if (payload.maxCorner.size() < 3)
    {
        result.setError(2, "Box maxCorner has the wrong number of parameters (3 necesary)");
        return result;
    }
    if (payload.color.size() < 4)
    {
        result.setError(3, "Box color has the wrong number of parameters (RGBA, 4 "
                           "necesary)");
        return result;
    }

    brayns::ModelPtr modelptr = _api->getScene().createModel();

    const size_t matId = 1;
    const brayns::Vector3d color(payload.color[0], payload.color[1],
                                 payload.color[2]);
    const double opacity = payload.color[3];
    _createShapeMaterial(modelptr, matId, color, opacity);

    const brayns::Vector3f minCorner(payload.minCorner[0], payload.minCorner[1],
                                     payload.minCorner[2]);
    const brayns::Vector3f maxCorner(payload.maxCorner[0], payload.maxCorner[1],
                                     payload.maxCorner[2]);

    brayns::TriangleMesh mesh = brayns::createBox(minCorner, maxCorner);

    modelptr->getTriangleMeshes()[matId] = mesh;
    modelptr->markInstancesDirty();

    size_t numModels = _api->getScene().getNumModels();
    const std::string name = payload.name.empty()
                                 ? "box_" + std::to_string(numModels)
                                 : payload.name;
    result.id = _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelptr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    _dirty = true;

    return result;
}

brayns::Message CircuitExplorerPlugin::_addGrid(const AddGrid& payload)
{
    brayns::Message result;

    BRAYNS_INFO << "Building Grid scene" << std::endl;

    auto& scene = _api->getScene();
    auto model = scene.createModel();

    const brayns::Vector3f red = {1, 0, 0};
    const brayns::Vector3f green = {0, 1, 0};
    const brayns::Vector3f blue = {0, 0, 1};
    const brayns::Vector3f grey = {0.5, 0.5, 0.5};

    brayns::PropertyMap props;
    props.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, false});
    props.setProperty({MATERIAL_PROPERTY_SHADING_MODE,
                       static_cast<int>(MaterialShadingMode::none)});
    props.setProperty({MATERIAL_PROPERTY_CLIPPING_MODE,
                       static_cast<int>(MaterialClippingMode::no_clipping)});

    auto material = model->createMaterial(0, "x");
    material->setDiffuseColor(grey);
    material->setProperties(props);

    const float m = payload.minValue;
    const float M = payload.maxValue;
    const float s = payload.steps;
    const float r = payload.radius;
    for (float x = m; x <= M; x += s)
        for (float y = m; y <= M; y += s)
            if (fabs(x) < 0.001f || fabs(y) < 0.001f)
            {
                model->addCylinder(0, {{x, y, m}, {x, y, M}, r});
                model->addCylinder(0, {{m, x, y}, {M, x, y}, r});
                model->addCylinder(0, {{x, m, y}, {x, M, y}, r});
            }

    material = model->createMaterial(1, "plane_x");
    material->setDiffuseColor(payload.useColors ? red : grey);
    material->setOpacity(payload.planeOpacity);
    material->setProperties(props);
    auto& tmx = model->getTriangleMeshes()[1];
    tmx.vertices.push_back({m, 0, m});
    tmx.vertices.push_back({M, 0, m});
    tmx.vertices.push_back({M, 0, M});
    tmx.vertices.push_back({m, 0, M});
    tmx.indices.push_back(brayns::Vector3ui(0, 1, 2));
    tmx.indices.push_back(brayns::Vector3ui(2, 3, 0));

    material = model->createMaterial(2, "plane_y");
    material->setDiffuseColor(payload.useColors ? green : grey);
    material->setOpacity(payload.planeOpacity);
    material->setProperties(props);
    auto& tmy = model->getTriangleMeshes()[2];
    tmy.vertices.push_back({m, m, 0});
    tmy.vertices.push_back({M, m, 0});
    tmy.vertices.push_back({M, M, 0});
    tmy.vertices.push_back({m, M, 0});
    tmy.indices.push_back(brayns::Vector3ui(0, 1, 2));
    tmy.indices.push_back(brayns::Vector3ui(2, 3, 0));

    material = model->createMaterial(3, "plane_z");
    material->setDiffuseColor(payload.useColors ? blue : grey);
    material->setOpacity(payload.planeOpacity);
    material->setProperties(props);
    auto& tmz = model->getTriangleMeshes()[3];
    tmz.vertices.push_back({0, m, m});
    tmz.vertices.push_back({0, m, M});
    tmz.vertices.push_back({0, M, M});
    tmz.vertices.push_back({0, M, m});
    tmz.indices.push_back(brayns::Vector3ui(0, 1, 2));
    tmz.indices.push_back(brayns::Vector3ui(2, 3, 0));

    if (payload.showAxis)
    {
        const float l = M;
        const float smallRadius = payload.radius * 25.0;
        const float largeRadius = payload.radius * 50.0;
        const float l1 = l * 0.89;
        const float l2 = l * 0.90;

        brayns::PropertyMap diffuseProps;
        diffuseProps.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, false});
        diffuseProps.setProperty(
            {MATERIAL_PROPERTY_SHADING_MODE,
             static_cast<int>(MaterialShadingMode::diffuse)});

        // X
        material = model->createMaterial(4, "x_axis");
        material->setDiffuseColor({1, 0, 0});
        material->setProperties(diffuseProps);

        model->addCylinder(4, {{0, 0, 0}, {l1, 0, 0}, smallRadius});
        model->addCone(4, {{l1, 0, 0}, {l2, 0, 0}, smallRadius, largeRadius});
        model->addCone(4, {{l2, 0, 0}, {M, 0, 0}, largeRadius, 0});

        // Y
        material = model->createMaterial(5, "y_axis");
        material->setDiffuseColor({0, 1, 0});
        material->setProperties(diffuseProps);

        model->addCylinder(5, {{0, 0, 0}, {0, l1, 0}, smallRadius});
        model->addCone(5, {{0, l1, 0}, {0, l2, 0}, smallRadius, largeRadius});
        model->addCone(5, {{0, l2, 0}, {0, M, 0}, largeRadius, 0});

        // Z
        material = model->createMaterial(6, "z_axis");
        material->setDiffuseColor({0, 0, 1});
        material->setProperties(diffuseProps);

        model->addCylinder(6, {{0, 0, 0}, {0, 0, l1}, smallRadius});
        model->addCone(6, {{0, 0, l1}, {0, 0, l2}, smallRadius, largeRadius});
        model->addCone(6, {{0, 0, l2}, {0, 0, M}, largeRadius, 0});

        // Origin
        model->addSphere(0, {{0, 0, 0}, smallRadius});
    }

    scene.addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(model), "Grid"));

    return result;
}

brayns::Message CircuitExplorerPlugin::_addColumn(const AddColumn& payload)
{
    brayns::Message result;

    BRAYNS_INFO << "Building Column model" << std::endl;

    auto& scene = _api->getScene();
    auto model = scene.createModel();

    //    const brayns::Vector3f grey = {0.5, 0.5, 0.5};
    const brayns::Vector3f white = {1.f, 1.f, 1.F};

    brayns::PropertyMap props;
    props.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, false});
    props.setProperty({MATERIAL_PROPERTY_SHADING_MODE,
                       static_cast<int>(MaterialShadingMode::diffuse)});
    props.setProperty({MATERIAL_PROPERTY_CLIPPING_MODE,
                       static_cast<int>(MaterialClippingMode::no_clipping)});

    auto material = model->createMaterial(0, "column");
    material->setDiffuseColor(white);
    material->setProperties(props);

    const brayns::Vector3fs verticesBottom = {
        {-0.25f, -1.0f, -0.5f}, {0.25f, -1.0f, -0.5f}, {0.5f, -1.0f, -0.25f},
        {0.5f, -1.0f, 0.25f},   {0.5f, -1.0f, -0.25f}, {0.5f, -1.0f, 0.25f},
        {0.25f, -1.0f, 0.5f},   {-0.25f, -1.0f, 0.5f}, {-0.5f, -1.0f, 0.25f},
        {-0.5f, -1.0f, -0.25f}};
    const brayns::Vector3fs verticesTop = {
        {-0.25f, 1.f, -0.5f}, {0.25f, 1.f, -0.5f}, {0.5f, 1.f, -0.25f},
        {0.5f, 1.f, 0.25f},   {0.5f, 1.f, -0.25f}, {0.5f, 1.f, 0.25f},
        {0.25f, 1.f, 0.5f},   {-0.25f, 1.f, 0.5f}, {-0.5f, 1.f, 0.25f},
        {-0.5f, 1.f, -0.25f}};

    const auto r = static_cast<float>(payload.radius);
    for (size_t i = 0; i < verticesBottom.size(); ++i)
    {
        model->addCylinder(0, {verticesBottom[i],
                               verticesBottom[(i + 1) % verticesBottom.size()],
                               r / 2.f});
        model->addSphere(0, {verticesBottom[i], r});
    }

    for (size_t i = 0; i < verticesTop.size(); ++i)
    {
        model->addCylinder(0, {verticesTop[i],
                               verticesTop[(i + 1) % verticesTop.size()],
                               r / 2.f});
        model->addSphere(0, {verticesTop[i], r});
    }

    for (size_t i = 0; i < verticesTop.size(); ++i)
        model->addCylinder(0, {verticesBottom[i], verticesTop[i], r / 2.f});

    scene.addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(model), "Column"));

    return result;
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int /*argc*/,
                                                         char** /*argv*/)
{
    PLUGIN_INFO << "Initializing circuit explorer plugin" << std::endl;
    return new CircuitExplorerPlugin();
}
