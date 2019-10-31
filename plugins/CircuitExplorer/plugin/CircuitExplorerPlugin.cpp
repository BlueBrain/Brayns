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
#include <brayns/common/utils/imageUtils.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/PluginAPI.h>

#include <brion/brion.h>

#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <regex>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#define REGISTER_LOADER(LOADER, FUNC) \
    registry.registerLoader({std::bind(&LOADER::getSupportedDataTypes), FUNC});

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
    engine.addRendererType("circuit_explorer_proximity_detection", properties);
}

void _addPerspectiveCamera(brayns::Engine& engine)
{
    PLUGIN_INFO << "Registering DOF perspective camera" << std::endl;

    brayns::PropertyMap properties;
    properties.setProperty({"fovy", 45., .1, 360., {"Field of view"}});
    properties.setProperty({"aspect", 1., {"Aspect ratio"}});
    properties.setProperty({"apertureRadius", 0., {"Aperture radius"}});
    properties.setProperty({"focusDistance", 1., {"Focus Distance"}});
    properties.setProperty({"enableClippingPlanes", true, {"Clipping"}});
    engine.addCameraType("circuit_explorer_perspective", properties);
}

std::string _sanitizeString(const std::string& input)
{
    static const std::vector<std::string> sanitetizeItems = {"\"", "\\", "'", ";", "&", "|", "`"};

    std::string result = "";

    for(size_t i = 0; i < input.size(); i++)
    {
        bool found = false;
        for(const auto & token : sanitetizeItems)
        {
            if(std::string(1, input[i]) == token)
            {
                result += "\\" + token;
                found = true;
                break;
            }
        }
        if(!found)
        {
            result += std::string(1, input[i]);
        }
    }
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

    registry.registerLoader(
        std::make_unique<BrickLoader>(scene, BrickLoader::getCLIProperties()));

    registry.registerLoader(
        std::make_unique<SynapseJSONLoader>(scene,
                                            std::move(_synapseAttributes)));

    registry.registerLoader(std::make_unique<SynapseCircuitLoader>(
        scene, pm.getApplicationParameters(),
        SynapseCircuitLoader::getCLIProperties()));

    registry.registerLoader(std::make_unique<MorphologyLoader>(
        scene, MorphologyLoader::getCLIProperties()));

    registry.registerLoader(std::make_unique<AdvancedCircuitLoader>(
        scene, pm.getApplicationParameters(),
        AdvancedCircuitLoader::getCLIProperties()));

    registry.registerLoader(std::make_unique<MorphologyCollageLoader>(
        scene, pm.getApplicationParameters(),
        MorphologyCollageLoader::getCLIProperties()));

    registry.registerLoader(std::make_unique<MeshCircuitLoader>(
        scene, pm.getApplicationParameters(),
        MeshCircuitLoader::getCLIProperties()));

    registry.registerLoader(std::make_unique<PairSynapsesLoader>(
        scene, pm.getApplicationParameters(),
        PairSynapsesLoader::getCLIProperties()));

    registry.registerLoader(
        std::make_unique<AstrocyteLoader>(scene, pm.getApplicationParameters(),
                                          AstrocyteLoader::getCLIProperties()));

    auto actionInterface = _api->getActionInterface();
    if (actionInterface)
    {
        PLUGIN_INFO << "Registering 'set-material' endpoint" << std::endl;
        actionInterface->registerNotification<MaterialDescriptor>(
            "set-material",
            [&](const MaterialDescriptor& param) { _setMaterial(param); });

        PLUGIN_INFO << "Registering 'set-materials' endpoint" << std::endl;
        actionInterface->registerNotification<MaterialsDescriptor>(
            "set-materials",
            [&](const MaterialsDescriptor& param) { _setMaterials(param); });

        PLUGIN_INFO << "Registering 'get-material-ids' endpoint" << std::endl;
        actionInterface->registerRequest<ModelId, MaterialIds>(
            "get-material-ids", [&](const ModelId& modelId) -> MaterialIds {
                return _getMaterialIds(modelId);
            });

        PLUGIN_INFO << "Registering 'set-material-extra-attributes' endpoint"
                    << std::endl;
        actionInterface->registerNotification<MaterialExtraAttributes>(
            "set-material-extra-attributes",
            [&](const MaterialExtraAttributes& param) {
                _setMaterialExtraAttributes(param);
            });

        PLUGIN_INFO << "Registering 'set-synapses-attributes' endpoint"
                    << std::endl;
        actionInterface->registerNotification<SynapseAttributes>(
            "set-synapses-attributes", [&](const SynapseAttributes& param) {
                _setSynapseAttributes(param);
            });

        PLUGIN_INFO << "Registering 'save-model-to-cache' endpoint"
                    << std::endl;
        actionInterface->registerNotification<SaveModelToCache>(
            "save-model-to-cache",
            [&](const SaveModelToCache& param) { _saveModelToCache(param); });

        PLUGIN_INFO << "Registering 'set-connections-per-value' endpoint"
                    << std::endl;
        actionInterface->registerNotification<ConnectionsPerValue>(
            "set-connections-per-value", [&](const ConnectionsPerValue& param) {
                _setConnectionsPerValue(param);
            });

        PLUGIN_INFO
            << "Registering 'set-metaballs-per-simulation-value' endpoint"
            << std::endl;
        actionInterface->registerNotification<MetaballsFromSimulationValue>(
            "set-metaballs-per-simulation-value",
            [&](const MetaballsFromSimulationValue& param) {
                _setMetaballsPerSimulationValue(param);
            });

        PLUGIN_INFO << "Registering 'set-odu-camera' endpoint" << std::endl;
        _api->getActionInterface()->registerNotification<CameraDefinition>(
            "set-odu-camera",
            [&](const CameraDefinition& s) { _setCamera(s); });

        PLUGIN_INFO << "Registering 'get-odu-camera' endpoint" << std::endl;
        _api->getActionInterface()->registerRequest<CameraDefinition>(
            "get-odu-camera",
            [&]() -> CameraDefinition { return _getCamera(); });

        PLUGIN_INFO << "Registering 'attach-cell-growth-handler' endpoint"
                    << std::endl;
        _api->getActionInterface()
            ->registerNotification<AttachCellGrowthHandler>(
                "attach-cell-growth-handler",
                [&](const AttachCellGrowthHandler& s) {
                    _attachCellGrowthHandler(s);
                });

        PLUGIN_INFO
            << "Registering 'attach-circuit-simulation-handler' endpoint"
            << std::endl;
        _api->getActionInterface()
            ->registerNotification<AttachCircuitSimulationHandler>(
                "attach-circuit-simulation-handler",
                [&](const AttachCircuitSimulationHandler& s) {
                    _attachCircuitSimulationHandler(s);
                });

        PLUGIN_INFO << "Registering 'export-frames-to-disk' endpoint"
                    << std::endl;
        _api->getActionInterface()->registerNotification<ExportFramesToDisk>(
            "export-frames-to-disk",
            [&](const ExportFramesToDisk& s) { _exportFramesToDisk(s); });

        PLUGIN_INFO << "Registering 'get-export-frames-progress' endpoint"
                    << std::endl;
        actionInterface->registerRequest<FrameExportProgress>(
            "get-export-frames-progress", [&](void) -> FrameExportProgress {
                return _getFrameExportProgress();
            });

        PLUGIN_INFO << "Registering 'make-movie' endpoint" << std::endl;
        actionInterface->registerNotification<MakeMovieParameters>(
             "make-movie", [&](const MakeMovieParameters& params) {
                _makeMovie(params);
            });

        PLUGIN_INFO << "Registering 'add-grid' endpoint" << std::endl;
        _api->getActionInterface()->registerNotification<AddGrid>(
            "add-grid", [&](const AddGrid& payload) { _addGrid(payload); });

        PLUGIN_INFO << "Registering 'add-column' endpoint" << std::endl;
        _api->getActionInterface()->registerNotification<AddColumn>(
            "add-column",
            [&](const AddColumn& payload) { _addColumn(payload); });

        PLUGIN_INFO << "Registering 'add-sphere' endpoint" << std::endl;
        _api->getActionInterface()->registerRequest<AddSphere, AddShapeResult>(
            "add-sphere",
            [&](const AddSphere& payload) { return _addSphere(payload); });

        PLUGIN_INFO << "Registering 'add-pill' endpoint" << std::endl;
        _api->getActionInterface()->registerRequest<AddPill, AddShapeResult>(
            "add-pill",
            [&](const AddPill& payload) { return _addPill(payload); });

        PLUGIN_INFO << "Registering 'add-cylinder' endpoint" << std::endl;
        _api->getActionInterface()->registerRequest<AddCylinder, AddShapeResult>(
            "add-cylinder",
            [&](const AddCylinder& payload) { return _addCylinder(payload); });

        PLUGIN_INFO << "Registering 'add-box' endpoint" << std::endl;
        _api->getActionInterface()->registerRequest<AddBox, AddShapeResult>(
            "add-box",
            [&](const AddBox& payload) { return _addBox(payload); });
    }

    auto& engine = _api->getEngine();
    _addAdvancedSimulationRenderer(engine);
    _addBasicSimulationRenderer(engine);
    _addVoxelizedSimulationRenderer(engine);
    _addGrowthRenderer(engine);
    _addProximityRenderer(engine);
    _addPerspectiveCamera(engine);

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
        auto& ai = _exportFramesToDiskPayload.animationInformation;
        if (_frameNumber >= ai.size())
        {
            _exportFramesToDiskDirty = false;
        }
        else
        {
            uint64_t i = 11 * _frameNumber;
            // Camera position
            CameraDefinition cd;
            auto& ci = _exportFramesToDiskPayload.cameraInformation;
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
    ++_accumulationFrameNumber;
    if (_exportFramesToDiskDirty &&
        _accumulationFrameNumber == _exportFramesToDiskPayload.spp - 1)
    {
        _doExportFrameToDisk();
        ++_frameNumber;
        _accumulationFrameNumber = 0;
    }
}

void CircuitExplorerPlugin::_setMaterialExtraAttributes(
    const MaterialExtraAttributes& mea)
{
    auto modelDescriptor = _api->getScene().getModel(mea.modelId);
    if (modelDescriptor)
        try
        {
            auto materials = modelDescriptor->getModel().getMaterials();
            for (auto& material : materials)
            {
                brayns::PropertyMap props;
                props.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, 0});
                props.setProperty(
                    {MATERIAL_PROPERTY_SHADING_MODE,
                     static_cast<int>(MaterialShadingMode::diffuse)});
                props.setProperty({MATERIAL_PROPERTY_CLIPPED, 0});
                material.second->updateProperties(props);
            }
        }
        catch (const std::runtime_error& e)
        {
            PLUGIN_INFO << e.what() << std::endl;
        }
    else
        PLUGIN_INFO << "Model " << mea.modelId << " is not registered"
                    << std::endl;
}

void CircuitExplorerPlugin::_setMaterial(const MaterialDescriptor& md)
{
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
                                         md.simulationDataCast ? 1 : 0);
                material->updateProperty(MATERIAL_PROPERTY_SHADING_MODE,
                                         md.shadingMode);
                material->updateProperty(MATERIAL_PROPERTY_CLIPPED,
                                         md.clipped ? 1 : 0);
                material->markModified(); // This is needed to apply
                                          // propery modifications
                material->commit();

                _dirty = true;
            }
            else
                PLUGIN_INFO << "Material " << md.materialId
                            << " is not registered in model " << md.modelId
                            << std::endl;
        }
        catch (const std::runtime_error& e)
        {
            PLUGIN_INFO << e.what() << std::endl;
        }
    else
        PLUGIN_INFO << "Model " << md.modelId << " is not registered"
                    << std::endl;
}

void CircuitExplorerPlugin::_setMaterials(const MaterialsDescriptor& md)
{
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
                                md.simulationDataCasts[id] ? 1 : 0);
                        if (!md.shadingModes.empty())
                            material->updateProperty(
                                MATERIAL_PROPERTY_SHADING_MODE,
                                md.shadingModes[id]);
                        if (!md.clips.empty())
                            material->updateProperty(MATERIAL_PROPERTY_CLIPPED,
                                                     md.clips[id] ? 1 : 0);
                        material->markModified(); // This is needed to apply
                                                  // propery modifications
                        material->commit();
                    }
                }
                catch (const std::runtime_error& e)
                {
                    PLUGIN_INFO << e.what() << std::endl;
                }
                ++id;
            }
            _dirty = true;
        }
        else
            PLUGIN_INFO << "Model " << modelId << " is not registered"
                        << std::endl;
    }
}

MaterialIds CircuitExplorerPlugin::_getMaterialIds(const ModelId& modelId)
{
    MaterialIds materialIds;
    auto modelDescriptor = _api->getScene().getModel(modelId.id);
    if (modelDescriptor)
    {
        for (const auto& material : modelDescriptor->getModel().getMaterials())
            if (material.first != brayns::BOUNDINGBOX_MATERIAL_ID &&
                material.first != brayns::SECONDARY_MODEL_MATERIAL_ID)
                materialIds.ids.push_back(material.first);
    }
    else
        PLUGIN_THROW("Invalid model ID");
    return materialIds;
}

void CircuitExplorerPlugin::_setSynapseAttributes(
    const SynapseAttributes& param)
{
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
    }
    catch (...)
    {
        PLUGIN_ERROR
            << "Unexpected exception occured in _updateMaterialFromJson"
            << std::endl;
    }
}

void CircuitExplorerPlugin::_saveModelToCache(const SaveModelToCache& saveModel)
{
    auto modelDescriptor = _api->getScene().getModel(saveModel.modelId);
    if (modelDescriptor)
    {
        BrickLoader brickLoader(_api->getScene());
        brickLoader.exportToFile(modelDescriptor, saveModel.path);
    }
    else
        PLUGIN_ERROR << "Model " << saveModel.modelId << " is not registered"
                     << std::endl;
}

void CircuitExplorerPlugin::_setConnectionsPerValue(
    const ConnectionsPerValue& cpv)
{
    PointCloud pointCloud;

    auto modelDescriptor = _api->getScene().getModel(cpv.modelId);
    if (modelDescriptor)
    {
        auto simulationHandler =
            modelDescriptor->getModel().getSimulationHandler();
        if (!simulationHandler)
        {
            BRAYNS_ERROR << "Scene has not user data handler" << std::endl;
            return;
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
            PLUGIN_INFO << "No connections added for value "
                        << std::to_string(cpv.value) << std::endl;
    }
    else
        PLUGIN_INFO << "Model " << cpv.modelId << " is not registered"
                    << std::endl;
}

void CircuitExplorerPlugin::_setMetaballsPerSimulationValue(
    const MetaballsFromSimulationValue& mpsv)
{
    PointCloud pointCloud;

    auto modelDescriptor = _api->getScene().getModel(mpsv.modelId);
    if (modelDescriptor)
    {
        auto simulationHandler =
            modelDescriptor->getModel().getSimulationHandler();
        if (!simulationHandler)
        {
            BRAYNS_ERROR << "Scene has not user data handler" << std::endl;
            return;
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
                PLUGIN_INFO << "No mesh was created for value "
                            << std::to_string(mpsv.value) << std::endl;
        }
        else
            PLUGIN_INFO << "No connections added for value "
                        << std::to_string(mpsv.value) << std::endl;
    }
    else
        PLUGIN_INFO << "Model " << mpsv.modelId << " is not registered"
                    << std::endl;
}

void CircuitExplorerPlugin::_setCamera(const CameraDefinition& payload)
{
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

void CircuitExplorerPlugin::_attachCellGrowthHandler(
    const AttachCellGrowthHandler& payload)
{
    PLUGIN_INFO << "Attaching Cell Growth Handler to model " << payload.modelId
                << std::endl;
    auto modelDescriptor = _api->getScene().getModel(payload.modelId);
    if (modelDescriptor)
    {
        auto handler = std::make_shared<CellGrowthHandler>(payload.nbFrames);
        modelDescriptor->getModel().setSimulationHandler(handler);
    }
}

void CircuitExplorerPlugin::_attachCircuitSimulationHandler(
    const AttachCircuitSimulationHandler& payload)
{
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
    }
}

void CircuitExplorerPlugin::_exportFramesToDisk(
    const ExportFramesToDisk& payload)
{
    _exportFramesToDiskPayload = payload;
    _exportFramesToDiskDirty = true;
    _frameNumber = payload.startFrame;
    _accumulationFrameNumber = -1;
    auto& frameBuffer = _api->getEngine().getFrameBuffer();
    frameBuffer.clear();
    PLUGIN_INFO << "-----------------------------------------------------------"
                   "---------------------"
                << std::endl;
    PLUGIN_INFO << "Setting movie: " << std::endl;
    PLUGIN_INFO << "- Number of frames : "
                << payload.animationInformation.size() - payload.startFrame
                << std::endl;
    PLUGIN_INFO << "- Samples per pixel: " << payload.spp << std::endl;
    PLUGIN_INFO << "- Export folder    : " << payload.path << std::endl;
    PLUGIN_INFO << "- Start frame      : " << payload.startFrame << std::endl;
    PLUGIN_INFO << "-----------------------------------------------------------"
                   "---------------------"
                << std::endl;
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
    sprintf(frame, "%05d", _frameNumber);
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
    result.frameNumber = _frameNumber;
    result.done = !_exportFramesToDiskDirty;
    return result;
}

void CircuitExplorerPlugin::_makeMovie(const MakeMovieParameters& params)
{
    //static const std::string root = "/gpfs/bbp.cscs.ch/project";

    // Find ffmpeg executable path
    std::array<char, 256> buffer;
    std::string ffmpegPath;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("which ffmpeg", "r"), pclose);
    if (!pipe) {
        PLUGIN_ERROR << "Could not launch movie creation: ffmpeg not found" << std::endl;
        return;
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        ffmpegPath += buffer.data();
    }

    // Remove new lines
    size_t pos = std::string::npos;
    do
    {
        pos = ffmpegPath.find("\n");
        if(pos != std::string::npos)
        {
            ffmpegPath.replace(pos, pos + 2, "");
        }
    } while(pos != std::string::npos);

    // Get sanitetized string inputs
    std::string sanitizedFramesFolder = params.framesFolderPath;
    const std::string slash = sanitizedFramesFolder[sanitizedFramesFolder.length() - 1] == '/'? "" : "/";

    std::string sanitizedFramesExt = params.framesFileExtension;
    std::string sanitizedOutputPath = params.outputMoviePath;

    sanitizedFramesFolder = _sanitizeString(sanitizedFramesFolder);
    sanitizedFramesExt = _sanitizeString(sanitizedFramesExt);
    sanitizedOutputPath = _sanitizeString(sanitizedOutputPath);

    const std::string inputParam = sanitizedFramesFolder+slash+"%05d."+sanitizedFramesExt;
    const std::string filterParam = "scale="
                                    +std::to_string(static_cast<int>(params.dimensions[0]))
                                    +":"
                                    +std::to_string(static_cast<int>(params.dimensions[1]))
                                    +",format=yuv420p";
    const pid_t pid = fork();

    if(pid == 0)
    {
        execl(ffmpegPath.c_str(),
              "ffmpeg",
              "-y",
              "-hide_banner",
              "-loglevel",
              "0",
              "-r",
              std::to_string(params.fpsRate).c_str(),
              "-i",
              inputParam.c_str(),
              "-vf",
              filterParam.c_str(),
              "-crf",
              "0",
              "-codec:v",
              "libx264",
              sanitizedOutputPath.c_str(),
              static_cast<char*>(nullptr));
    }
    else
    {
        int status = 0;
        wait(&status);
        if(status != 0)
        {
            // If we could not make the movie, inform and stop execution
            PLUGIN_ERROR << "Could not create media video file. FFMPEG returned with error " << status << std::endl;
            return;
        }
    }

    if(params.eraseFrames)
    {
        DIR *dir;
        struct dirent *ent;
        const std::regex fileNameRegex ("[0-9]{5}." + params.framesFileExtension);
        if ((dir = opendir (params.framesFolderPath.c_str())) != nullptr)
        {
          while ((ent = readdir (dir)) != nullptr)
          {
            std::string fileName(ent->d_name);
            if(std::regex_match(fileName, fileNameRegex))
            {
                const std::string fullPath = sanitizedFramesFolder + slash + fileName;
                PLUGIN_INFO << "Cleaning frame " << fullPath << std::endl;
                remove(fullPath.c_str());
            }
          }
          closedir (dir);
        }
        else
        {
          PLUGIN_ERROR << "make-movie: Could not clean up frames" << std::endl;
        }
    }
}

void CircuitExplorerPlugin::_createShapeMaterial(brayns::ModelPtr& model,
                                                 const size_t id,
                                                 const brayns::Vector3d& color,
                                                 const double& opacity)
{
   brayns::MaterialPtr mptr = model->createMaterial(id, std::to_string(id));
   mptr->setDiffuseColor(color);
   mptr->setOpacity(opacity);
   mptr->setSpecularExponent(0.0);

   brayns::PropertyMap props;
   props.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, 0});
   props.setProperty(
       {MATERIAL_PROPERTY_SHADING_MODE,
        static_cast<int>(MaterialShadingMode::diffuse_transparency)});
   props.setProperty({MATERIAL_PROPERTY_CLIPPED, 0});

   mptr->updateProperties(props);

   mptr->markModified();
   mptr->commit();
}

AddShapeResult CircuitExplorerPlugin::_addSphere(const AddSphere &payload)
{
    AddShapeResult result;
    result.error = 0;
    result.message = "";

    if(payload.center.size() < 3)
    {
        result.error = 1;
        result.message = "Sphere center has the wrong number of parameters (3 necessary)";
        return result;
    }

    if(payload.color.size() < 4)
    {
        result.error = 2;
        result.message = "Sphere color has the wrong number of parameters (RGBA, 4 necessary)";
        return result;
    }

    if(payload.radius < 0.0f)
    {
        result.error = 3;
        result.message = "Negative radius passed for sphere creation";
        return result;
    }

    brayns::ModelPtr modelptr = _api->getScene().createModel();

    const size_t matId = 1;
    const brayns::Vector3d color (payload.color[0], payload.color[1], payload.color[2]);
    const double opacity = payload.color[3];
    _createShapeMaterial(modelptr, matId, color, opacity);

    const brayns::Vector3f center (payload.center[0], payload.center[1], payload.center[2]);
    modelptr->addSphere(matId, {center, payload.radius});

    size_t numModels = _api->getScene().getNumModels();
    const std::string name = payload.name.empty()? "sphere_" + std::to_string(numModels) : payload.name;
    result.id = _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelptr), name));
    _api->getScene().markModified();

    _api->getEngine().triggerRender();

    _dirty = true;
    return result;
}

AddShapeResult CircuitExplorerPlugin::_addPill(const AddPill &payload)
{
    AddShapeResult result;
    result.error = 0;
    result.message = "";

    if(payload.p1.size() < 3)
    {
        result.error = 1;
        result.message = "Pill point 1 has the wrong number of parameters (3 necessary)";
        return result;
    }
    if(payload.p2.size() < 3)
    {
        result.error = 2;
        result.message = "Pill point 2 has the wrong number of parameters (3 necessary)";
        return result;
    }
    if(payload.color.size() < 4)
    {
        result.error = 3;
        result.message = "Pill color has the wrong number of parameters (RGBA, 4 necessary)";
        return result;
    }
    if(payload.type != "pill" && payload.type != "conepill" && payload.type != "sigmoidpill")
    {
        result.error = 4;
        result.message = "Unknown pill type parameter. Must be either \"pill\", \"conepill\", or \"sigmoidpill\"";
        return result;
    }
    if(payload.radius1 < 0.0f || payload.radius2 < 0.0f)
    {
        result.error = 5;
        result.message = "Negative radius passed for the pill creation";
        return result;
    }

    brayns::ModelPtr modelptr = _api->getScene().createModel();

    size_t matId = 1;
    const brayns::Vector3d color (payload.color[0], payload.color[1], payload.color[2]);
    const double opacity = payload.color[3];
    _createShapeMaterial(modelptr, matId, color, opacity);

    const brayns::Vector3f p0 (payload.p1[0], payload.p1[1], payload.p1[2]);
    const brayns::Vector3f p1 (payload.p2[0], payload.p2[1], payload.p2[2]);
    brayns::SDFGeometry sdf;
    if(payload.type == "pill")
    {
        sdf = brayns::createSDFPill(p0, p1, payload.radius1);
    }
    else if(payload.type == "conepill")
    {
        sdf = brayns::createSDFConePill(p0, p1, payload.radius1, payload.radius2);
    }
    else if(payload.type == "sigmoidpill")
    {
        sdf = brayns::createSDFConePillSigmoid(p0, p1, payload.radius1, payload.radius2);
    }

    modelptr->addSDFGeometry(matId, sdf, {});

    size_t numModels = _api->getScene().getNumModels();
    const std::string name = payload.name.empty()? payload.type + "_" + std::to_string(numModels) : payload.name;
    result.id = _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelptr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    _dirty = true;

    return result;
}

AddShapeResult CircuitExplorerPlugin::_addCylinder(const AddCylinder &payload)
{
    AddShapeResult result;
    result.error = 0;
    result.message = "";

    if(payload.center.size() < 3)
    {
        result.error = 1;
        result.message = "Cylinder center has the wrong number of parameters (3 necessary)";
        return result;;
    }
    if(payload.up.size() < 3)
    {
        result.error = 2;
        result.message = "Cylinder up has the wrong number of parameters (3 necessary)";
        return result;
    }
    if(payload.color.size() < 4)
    {
        result.error = 3;
        result.message = "Cylinder color has the wrong number of parameters (RGBA, 4 necessary)";
        return result;
    }
    if(payload.radius < 0.0f)
    {
        result.error = 4;
        result.message = "Negative radius passed for cylinder creation";
        return result;
    }

    brayns::ModelPtr modelptr = _api->getScene().createModel();

    const size_t matId = 1;
    const brayns::Vector3d color (payload.color[0], payload.color[1], payload.color[2]);
    const double opacity = payload.color[3];
    _createShapeMaterial(modelptr, matId, color, opacity);

    const brayns::Vector3f center (payload.center[0], payload.center[1], payload.center[2]);
    const brayns::Vector3f up (payload.up[0], payload.up[1], payload.up[2]);
    modelptr->addCylinder(matId, {center, up, payload.radius});

    size_t numModels = _api->getScene().getNumModels();
    const std::string name = payload.name.empty()? "cylinder_" + std::to_string(numModels) : payload.name;
    result.id = _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelptr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    _dirty = true;

    return result;
}

AddShapeResult CircuitExplorerPlugin::_addBox(const AddBox &payload)
{
    AddShapeResult result;
    result.error = 0;
    result.message = "";

    if(payload.minCorner.size() < 3)
    {
        result.error = 1;
        result.message = "Box minCorner has the wrong number of parameters (3 necessary)";
        return result;
    }
    if(payload.maxCorner.size() < 3)
    {
        result.error = 2;
        result.message = "Box maxCorner has the wrong number of parameters (3 necesary)";
        return result;
    }
    if(payload.color.size() < 4)
    {
        result.error = 3;
        result.message = "Box color has the wrong number of parameters (RGBA, 4 necesary)";
        return result;
    }

    brayns::ModelPtr modelptr = _api->getScene().createModel();

    const size_t matId = 1;
    const brayns::Vector3d color (payload.color[0], payload.color[1], payload.color[2]);
    const double opacity = payload.color[3];
    _createShapeMaterial(modelptr, matId, color, opacity);

    const brayns::Vector3f minCorner (payload.minCorner[0], payload.minCorner[1], payload.minCorner[2]);
    const brayns::Vector3f maxCorner (payload.maxCorner[0], payload.maxCorner[1], payload.maxCorner[2]);

    brayns::TriangleMesh mesh = brayns::createBox(minCorner, maxCorner);

    modelptr->getTriangleMeshes()[matId] = mesh;
    modelptr->markInstancesDirty();

    size_t numModels = _api->getScene().getNumModels();
    const std::string name = payload.name.empty()? "box_" + std::to_string(numModels) : payload.name;
    result.id = _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelptr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    _dirty = true;

    return result;
}

void CircuitExplorerPlugin::_addGrid(const AddGrid& payload)
{
    BRAYNS_INFO << "Building Grid scene" << std::endl;

    auto& scene = _api->getScene();
    auto model = scene.createModel();

    const brayns::Vector3f red = {1, 0, 0};
    const brayns::Vector3f green = {0, 1, 0};
    const brayns::Vector3f blue = {0, 0, 1};
    const brayns::Vector3f grey = {0.5, 0.5, 0.5};

    brayns::PropertyMap props;
    props.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, 0});
    props.setProperty({MATERIAL_PROPERTY_SHADING_MODE,
                       static_cast<int>(MaterialShadingMode::none)});

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
        diffuseProps.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, 0});
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
}

void CircuitExplorerPlugin::_addColumn(const AddColumn& payload)
{
    BRAYNS_INFO << "Building Column model" << std::endl;

    auto& scene = _api->getScene();
    auto model = scene.createModel();

    //    const brayns::Vector3f grey = {0.5, 0.5, 0.5};
    const brayns::Vector3f white = {1.f, 1.f, 1.F};

    brayns::PropertyMap props;
    props.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, 0});
    props.setProperty({MATERIAL_PROPERTY_SHADING_MODE,
                       static_cast<int>(MaterialShadingMode::diffuse)});

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

    const auto r = payload.radius;
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
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int /*argc*/,
                                                         char** /*argv*/)
{
    PLUGIN_INFO << "Initializing circuit explorer plugin" << std::endl;
    return new CircuitExplorerPlugin();
}
