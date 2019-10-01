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
#include <fstream>

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
    properties.setProperty({"pixelAlpha", 1., 0.01, 10., {"Pixel alpha"}});
    properties.setProperty({"fogStart", 0., 0., 1e6, {"Fog start"}});
    properties.setProperty({"fogThickness", 1e6, 1e6, 1e6, {"Fog thickness"}});
    properties.setProperty(
        {"maxBounces", 10, 1, 100, {"Maximum number of ray bounces"}});
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
    properties.setProperty({"pixelAlpha", 1., 0.01, 10., {"Pixel alpha"}});
    properties.setProperty({"fogStart", 0., 0., 1e6, {"Fog start"}});
    properties.setProperty({"fogThickness", 1e6, 1e6, 1e6, {"Fog thickness"}});
    properties.setProperty(
        {"maxBounces", 10, 1, 100, {"Maximum number of ray bounces"}});
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
    properties.setProperty({"pixelAlpha", 1., 0.01, 10., {"Pixel alpha"}});
    properties.setProperty({"fogStart", 0., 0., 1e6, {"Fog start"}});
    properties.setProperty({"fogThickness", 1e6, 1e6, 1e6, {"Fog thickness"}});
    properties.setProperty(
        {"maxBounces", 10, 1, 100, {"Maximum number of ray bounces"}});
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
    properties.setProperty({"pixelAlpha", 1., 0.01, 10., {"Pixel alpha"}});
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
        {"maxBounces", 10, 1, 100, {"Maximum number of ray bounces"}});
    properties.setProperty({"pixelAlpha", 1., 0.01, 10., {"Pixel alpha"}});
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

        PLUGIN_INFO << "Registering 'add-grid' endpoint" << std::endl;
        _api->getActionInterface()->registerNotification<AddGrid>(
            "add-grid", [&](const AddGrid& payload) { _addGrid(payload); });
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

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int /*argc*/,
                                                         char** /*argv*/)
{
    PLUGIN_INFO << "Initializing circuit explorer plugin" << std::endl;
    return new CircuitExplorerPlugin();
}
