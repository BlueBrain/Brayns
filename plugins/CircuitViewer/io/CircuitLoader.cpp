/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "CircuitLoader.h"
#include "MorphologyLoader.h"
#include "SimulationHandler.h"
#include "common.h"

#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>

#include <brayns/parameters/ApplicationParameters.h>

#include <brain/brain.h>
#include <brion/brion.h>

#if BRAYNS_USE_ASSIMP
#include <brayns/io/MeshLoader.h>
#endif

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

using namespace brayns;

namespace
{
using Property = brayns::PropertyMap::Property;
const Property PROP_CIRCUIT_DENSITY = {"circuitDensity", "Density", 100.0};
const Property PROP_CIRCUIT_RANDOM_SEED = {"circuitRandomSeed", "Random seed",
                                           0};
const Property PROP_CIRCUIT_REPORT = {"circuitReport", "Report",
                                      std::string("")};
const Property PROP_CIRCUIT_TARGETS = {"circuitTargets", "Targets",
                                       std::string("")};
const Property PROP_CIRCUIT_MESH_FILENAME_PATTERN = {
    "circuitMeshFilenamePattern", "Mesh filename pattern", std::string("")};
const Property PROP_CIRCUIT_MESH_FOLDER = {"circuitMeshFolder", "Mesh folder",
                                           std::string("")};
const Property PROP_CIRCUIT_BOUNDING_BOX_P0 = {
    "circuitBoundingBoxP0", "First bounding box boundary point",
    std::array<double, 3>{{0, 0, 0}}};
const Property PROP_CIRCUIT_BOUNDING_BOX_P1 = {
    "circuitBoundingBoxP1", "Second bounding box boundary point",
    std::array<double, 3>{{0, 0, 0}}};
const Property PROP_CIRCUIT_USE_SIMULATION_MODEL = {"circuitUseSimulationModel",
                                                    "Use simulation model",
                                                    false};
const Property PROP_CIRCUIT_MESH_TRANSFORMATION = {"circuitMeshTransformation",
                                                   "Mesh transformation",
                                                   int32_t(0)};
const Property PROP_COLOR_SCHEME = {"colorScheme", "Color scheme",
                                    brayns::enumToString(ColorScheme::none),
                                    brayns::enumNames<ColorScheme>()};
const Property PROP_CIRCUIT_START_SIMULATION_TIME = {
    "circuitStartSimulationTime", "Start simulation time", 0.0};
const Property PROP_CIRCUIT_END_SIMULATION_TIME = {"circuitEndSimulationTime",
                                                   "End simulation time", 0.0};
const Property PROP_CIRCUIT_SIMULATION_STEP = {"circuitSimulationStep",
                                               "Simulation step", 0.0};
const Property PROP_SYNCHRONOUS_MODE = {"synchronousMode", "Synchronous mode",
                                        false};
const Property PROP_LOAD_CACHE_FILE = {"loadCacheFile", "Load cache file",
                                       std::string("")};
const Property PROP_GEOMETRY_QUALITY = {"geometryQuality", "Geometry quality",
                                        brayns::enumToString(
                                            GeometryQuality::high),
                                        brayns::enumNames<GeometryQuality>()};
const auto LOADER_NAME = "circuit";

/**
 * @brief getMeshFilenameFromGID Returns the name of the mesh file according
 * to the --circuit-mesh-folder, --circuit-mesh-filename-pattern command
 * line arguments and a GID
 * @param gid GID of the cell
 * @return A string with the full path of the mesh file
 */
std::string getMeshFilenameFromGID(
    const uint64_t gid, const std::string& circuitMeshFilenamePattern,
    const std::string& circuitMeshFolder)
{
    const std::string gidAsString = std::to_string(gid);
    const std::string GID = "{gid}";

    auto meshFilenamePattern = circuitMeshFilenamePattern;
    if (!meshFilenamePattern.empty())
        meshFilenamePattern.replace(meshFilenamePattern.find(GID), GID.length(),
                                    gidAsString);
    else
        meshFilenamePattern = gidAsString;
    return circuitMeshFolder + "/" + meshFilenamePattern;
}

struct CircuitProperties
{
    CircuitProperties() = default;
    CircuitProperties(const PropertyMap& properties)
    {
        const auto setVariable = [&](auto& variable, const std::string& name,
                                     auto defaultVal) {
            using T = typename std::remove_reference<decltype(variable)>::type;
            variable = properties.getProperty<T>(name, defaultVal);
        };

        const auto setEnumVariable =
            [&](auto& variable, const std::string& name, auto defaultVal) {
                using T = decltype(defaultVal);
                const auto enumStr =
                    properties.getProperty<std::string>(name, enumToString<T>(
                                                                  defaultVal));
                variable = stringToEnum<T>(enumStr);
            };

        setVariable(circuitDensity, PROP_CIRCUIT_DENSITY.name, 100.0);
        setVariable(circuitRandomSeed, PROP_CIRCUIT_RANDOM_SEED.name, 0);
        setVariable(circuitReport, PROP_CIRCUIT_REPORT.name, "");
        setVariable(circuitTargets, PROP_CIRCUIT_TARGETS.name, "");
        setVariable(circuitMeshFilenamePattern,
                    PROP_CIRCUIT_MESH_FILENAME_PATTERN.name, "");
        setVariable(circuitMeshFolder, PROP_CIRCUIT_MESH_FOLDER.name, "");
        setVariable(circuitBoundingBoxP0, PROP_CIRCUIT_BOUNDING_BOX_P0.name,
                    std::array<double, 3>{{0, 0, 0}});
        setVariable(circuitBoundingBoxP1, PROP_CIRCUIT_BOUNDING_BOX_P1.name,
                    std::array<double, 3>{{0, 0, 0}});
        setVariable(circuitUseSimulationModel,
                    PROP_CIRCUIT_USE_SIMULATION_MODEL.name, false);
        setVariable(circuitMeshTransformation,
                    PROP_CIRCUIT_MESH_TRANSFORMATION.name, 0);
        setEnumVariable(colorScheme, PROP_COLOR_SCHEME.name, ColorScheme::none);
        setVariable(circuitStartSimulationTime,
                    PROP_CIRCUIT_START_SIMULATION_TIME.name, 0.0);
        setVariable(circuitEndSimulationTime,
                    PROP_CIRCUIT_END_SIMULATION_TIME.name, 0.0);
        setVariable(circuitSimulationStep, PROP_CIRCUIT_SIMULATION_STEP.name,
                    0.0);
        setVariable(synchronousMode, PROP_SYNCHRONOUS_MODE.name, false);
        setVariable(loadCacheFile, PROP_LOAD_CACHE_FILE.name, "");
        setEnumVariable(geometryQuality, PROP_GEOMETRY_QUALITY.name,
                        GeometryQuality::high);

        {
            strings targets;
            boost::char_separator<char> separator(",");
            boost::tokenizer<boost::char_separator<char>> tokens(circuitTargets,
                                                                 separator);
            for_each(tokens.begin(), tokens.end(),
                     [&targets](const std::string& s) {
                         targets.push_back(s);
                     });
            circuitTargetsList = std::move(targets);
        }
        circuitBoundingBox = Boxd(toVmmlVec(circuitBoundingBoxP0),
                                  toVmmlVec(circuitBoundingBoxP1));
    }

    double circuitDensity = 0.0;
    int32_t circuitRandomSeed = 0;
    std::string circuitReport;
    std::vector<std::string> circuitTargetsList;
    std::string circuitTargets;
    std::string circuitMeshFilenamePattern;
    std::string circuitMeshFolder;
    bool circuitUseSimulationModel = false;
    int32_t circuitMeshTransformation = 0;
    ColorScheme colorScheme = ColorScheme::none;

    double circuitStartSimulationTime = 0;
    double circuitEndSimulationTime = 0;
    double circuitSimulationStep = 0;

    bool synchronousMode = false;

    std::string loadCacheFile;
    GeometryQuality geometryQuality = GeometryQuality::high;

    std::array<double, 3> circuitBoundingBoxP0;
    std::array<double, 3> circuitBoundingBoxP1;

    Boxd circuitBoundingBox;
};

class Impl
{
public:
    Impl(Scene& scene, const PropertyMap& properties)
        : _scene(scene)
        , _properties(properties)
        , _morphologyParams(properties)
    {
    }

    ModelDescriptorPtr importCircuit(const std::string& source,
                                     const LoaderProgress& callback) const
    {
        const auto& targets = _properties.circuitTargetsList;
        const auto& report = _properties.circuitReport;

        bool returnValue = true;
        ModelDescriptorPtr modelDesc;
        try
        {
            // Model (one for the whole circuit)
            ModelMetadata metadata = {
                {"density", std::to_string(_properties.circuitDensity)},
                {"report", _properties.circuitReport},
                {"targets", _properties.circuitTargets},
                {"mesh-filename-pattern",
                 _properties.circuitMeshFilenamePattern},
                {"mesh-folder", _properties.circuitMeshFolder}};
            auto model = _scene.createModel();

            // Open Circuit and select GIDs according to specified target
            const brion::BlueConfig bc(source);
            const brain::Circuit circuit(bc);
            const auto circuitDensity = _properties.circuitDensity / 100.f;

            brain::GIDSet allGids;
            GIDOffsets targetGIDOffsets;
            targetGIDOffsets.push_back(0);

            strings localTargets;
            if (targets.empty())
                localTargets.push_back(bc.getCircuitTarget());
            else
                localTargets = targets;

            for (const auto& target : localTargets)
            {
                const auto targetGids =
                    circuit.getRandomGIDs(circuitDensity, target,
                                          _properties.circuitRandomSeed);
                const Matrix4fs& allTransformations =
                    circuit.getTransforms(targetGids);

                brain::GIDSet gids;
                const auto& aabb = _properties.circuitBoundingBox;
                if (aabb.getSize() == Vector3f(0.f))
                    gids = targetGids;
                else
                {
                    auto gidIterator = targetGids.begin();
                    for (size_t i = 0; i < allTransformations.size(); ++i)
                    {
                        if (aabb.isIn(Vector3d(
                                allTransformations[i].getTranslation())))
                            gids.insert(*gidIterator);
                        ++gidIterator;
                    }
                }

                if (gids.empty())
                {
                    BRAYNS_ERROR << "Target " << target
                                 << " does not contain any cells" << std::endl;
                    continue;
                }

                BRAYNS_INFO << "Target " << target << ": " << gids.size()
                            << " cells" << std::endl;
                allGids.insert(gids.begin(), gids.end());
                targetGIDOffsets.push_back(allGids.size());
            }

            if (allGids.empty())
                return {};

            // Load simulation information from compartment report
            CompartmentReportPtr compartmentReport;
            AbstractSimulationHandlerPtr simulationHandler;
            if (!report.empty())
            {
                try
                {
                    auto handler = std::make_shared<SimulationHandler>(
                        bc.getReportSource(report), allGids,
                        _properties.synchronousMode,
                        _properties.circuitStartSimulationTime,
                        _properties.circuitEndSimulationTime,
                        _properties.circuitSimulationStep);
                    compartmentReport = handler->getCompartmentReport();
                    // Only keep simulated GIDs
                    if (compartmentReport)
                    {
                        allGids = compartmentReport->getGIDs();
                        simulationHandler = handler;
                    }
                }
                catch (const std::exception& e)
                {
                    BRAYNS_ERROR << e.what() << std::endl;
                }
            }

            if (!_properties.loadCacheFile.empty())
                return {};

            const Matrix4fs& transformations = circuit.getTransforms(allGids);
            _logLoadedGIDs(allGids);

            const auto layerIds = _populateLayerIds(bc, allGids);
            const auto& electrophysiologyTypes =
                circuit.getElectrophysiologyTypes(allGids);
            const auto& morphologyTypes = circuit.getMorphologyTypes(allGids);

            // Import meshes
            returnValue =
                returnValue &&
                _importMeshes(callback, *model, allGids, transformations,
                              targetGIDOffsets, layerIds, morphologyTypes,
                              electrophysiologyTypes);

            // Import morphologies
            const auto useSimulationModel =
                _properties.circuitUseSimulationModel;
            model->useSimulationModel(useSimulationModel);
            if (_properties.circuitMeshFolder.empty() || useSimulationModel)
            {
                MorphologyLoader morphLoader(_scene);
                returnValue =
                    returnValue &&
                    _importMorphologies(circuit, callback, *model, allGids,
                                        transformations, targetGIDOffsets,
                                        compartmentReport, morphLoader,
                                        layerIds, morphologyTypes,
                                        electrophysiologyTypes);
            }

            // Attach simulation handler
            if (simulationHandler)
                model->setSimulationHandler(simulationHandler);

            // Create materials
            model->createMissingMaterials();

            // Compute circuit center
            Boxf circuitCenter;
            for (const auto& transformation : transformations)
                circuitCenter.merge(transformation.getTranslation());

            Transformation transformation;
            transformation.setRotationCenter(circuitCenter.getCenter());
            modelDesc =
                std::make_shared<ModelDescriptor>(std::move(model), "Circuit",
                                                  source, metadata);
            modelDesc->setTransformation(transformation);
        }
        catch (const std::exception& error)
        {
            BRAYNS_ERROR << "Failed to open " << source << ": " << error.what()
                         << std::endl;
            return {};
        }

        if (returnValue)
            return modelDesc;
        return {};
    }

private:
    /**
     * @brief _getMaterialFromSectionType return a material determined by the
     * --color-scheme geometry parameter
     * @param index Index of the element to which the material will attached
     * @param material Material that is forced in case geometry parameters
     * do not apply
     * @param sectionType Section type of the geometry to which the material
     * will be applied
     * @return Material ID determined by the geometry parameters
     */
    size_t _getMaterialFromGeometryParameters(
        const uint64_t index, const size_t material,
        const brain::neuron::SectionType sectionType,
        const GIDOffsets& targetGIDOffsets, const size_ts& layerIds,
        const size_ts& morphologyTypes, const size_ts& electrophysiologyTypes,
        bool isMesh) const
    {
        if (material != NO_MATERIAL)
            return material;

        if (!isMesh && _properties.circuitUseSimulationModel)
            return 0;

        size_t materialId = 0;
        switch (_properties.colorScheme)
        {
        case ColorScheme::neuron_by_id:
            materialId = index;
            break;
        case ColorScheme::neuron_by_segment_type:
            switch (sectionType)
            {
            case brain::neuron::SectionType::soma:
                materialId = 1;
                break;
            case brain::neuron::SectionType::axon:
                materialId = 2;
                break;
            case brain::neuron::SectionType::dendrite:
                materialId = 3;
                break;
            case brain::neuron::SectionType::apicalDendrite:
                materialId = 4;
                break;
            default:
                materialId = 0;
                break;
            }
            break;
        case ColorScheme::neuron_by_target:
            for (size_t i = 0; i < targetGIDOffsets.size() - 1; ++i)
                if (index >= targetGIDOffsets[i] &&
                    index < targetGIDOffsets[i + 1])
                {
                    materialId = i;
                    break;
                }
            break;
        case ColorScheme::neuron_by_etype:
            if (index < electrophysiologyTypes.size())
                materialId = electrophysiologyTypes[index];
            else
                BRAYNS_DEBUG << "Failed to get neuron E-type" << std::endl;
            break;
        case ColorScheme::neuron_by_mtype:
            if (index < morphologyTypes.size())
                materialId = morphologyTypes[index];
            else
                BRAYNS_DEBUG << "Failed to get neuron M-type" << std::endl;
            break;
        case ColorScheme::neuron_by_layer:
            if (index < layerIds.size())
                materialId = layerIds[index];
            else
                BRAYNS_DEBUG << "Failed to get neuron layer" << std::endl;
            break;
        default:
            materialId = NO_MATERIAL;
        }
        return materialId;
    }

    /**
     * @brief _populateLayerIds populates the neuron layer IDs. This is
     * currently only supported for the MVD2 format.
     * @param blueConfig Configuration of the circuit
     * @param gids GIDs of the neurons
     */
    size_ts _populateLayerIds(const brion::BlueConfig& blueConfig,
                              const brain::GIDSet& gids) const
    {
        size_ts layerIds;
        try
        {
            brion::Circuit brionCircuit(blueConfig.getCircuitSource());
            for (const auto& a : brionCircuit.get(gids, brion::NEURON_LAYER))
                layerIds.push_back(std::stoi(a[0]));
        }
        catch (...)
        {
            if (_properties.colorScheme == ColorScheme::neuron_by_layer)
                BRAYNS_ERROR
                    << "Only MVD2 format is currently supported by Brion "
                       "circuits. Color scheme by layer not available for "
                       "this circuit"
                    << std::endl;
        }
        return layerIds;
    }

    /**
     * @brief _logLoadedGIDs Logs selected GIDs for debugging purpose
     * @param gids to trace
     */
    void _logLoadedGIDs(const brain::GIDSet& gids) const
    {
        std::stringstream gidsStr;
        for (const auto& gid : gids)
            gidsStr << gid << " ";
        BRAYNS_DEBUG << "Loaded GIDs: " << gidsStr.str() << std::endl;
    }

    bool _importMeshes(
        const LoaderProgress& callback BRAYNS_UNUSED,
        Model& model BRAYNS_UNUSED, const brain::GIDSet& gids BRAYNS_UNUSED,
        const Matrix4fs& transformations BRAYNS_UNUSED,
        const GIDOffsets& targetGIDOffsets BRAYNS_UNUSED,
        const size_ts& layerIds BRAYNS_UNUSED,
        const size_ts& morphologyTypes BRAYNS_UNUSED,
        const size_ts& electrophysiologyTypes BRAYNS_UNUSED) const
    {
#if BRAYNS_USE_ASSIMP

        const auto colorScheme = _properties.colorScheme;
        const auto geometryQuality = _properties.geometryQuality;
        MeshLoader meshLoader(_scene);
        size_t loadingFailures = 0;
        const auto meshedMorphologiesFolder = _properties.circuitMeshFolder;
        if (meshedMorphologiesFolder.empty())
            return true;

        size_t meshIndex = 0;
        // Loading meshes is currently sequential. TODO: Make it parallel!!!
        std::stringstream message;
        message << "Loading " << gids.size() << " meshes...";
        for (const auto& gid : gids)
        {
            const size_t materialId = _getMaterialFromGeometryParameters(
                meshIndex, NO_MATERIAL, brain::neuron::SectionType::undefined,
                targetGIDOffsets, layerIds, morphologyTypes,
                electrophysiologyTypes, true);

            // Load mesh from file
            const auto transformation = _properties.circuitMeshTransformation
                                            ? transformations[meshIndex]
                                            : Matrix4f();
            try
            {
                meshLoader.importMesh(
                    getMeshFilenameFromGID(
                        gid, _properties.circuitMeshFilenamePattern,
                        _properties.circuitMeshFolder),
                    callback, model, meshIndex, transformation, materialId,
                    colorScheme, geometryQuality);
            }
            catch (...)
            {
                ++loadingFailures;
            }
            ++meshIndex;
            callback.updateProgress(message.str(),
                                    meshIndex /
                                        static_cast<float>(gids.size()));
        }
        if (loadingFailures != 0)
            BRAYNS_WARN << "Failed to import " << loadingFailures << " meshes"
                        << std::endl;
        return true;
#else
        throw std::runtime_error(
            "assimp dependency is required to load meshes");
#endif
    }

    bool _importMorphologies(
        const brain::Circuit& circuit, const LoaderProgress& callback,
        Model& model, const brain::GIDSet& gids,
        const Matrix4fs& transformations, const GIDOffsets& targetGIDOffsets,
        CompartmentReportPtr compartmentReport, MorphologyLoader& morphLoader,
        const size_ts& layerIds, const size_ts& morphologyTypes,
        const size_ts& electrophysiologyTypes) const
    {
        const brain::URIs& uris = circuit.getMorphologyURIs(gids);
        size_t loadingFailures = 0;
        std::stringstream message;
        message << "Loading " << uris.size() << " morphologies...";
        std::atomic_size_t current{0};
        std::exception_ptr cancelException;
#pragma omp parallel
        {
#pragma omp for nowait
            for (uint64_t morphologyIndex = 0; morphologyIndex < uris.size();
                 ++morphologyIndex)
            {
                ++current;

                try
                {
                    callback.updateProgress(message.str(),
                                            current / static_cast<float>(
                                                          uris.size()));

                    ParallelModelContainer modelContainer;
                    const auto& uri = uris[morphologyIndex];

                    auto materialFunc =
                        std::bind(&Impl::_getMaterialFromGeometryParameters,
                                  this, morphologyIndex, NO_MATERIAL,
                                  std::placeholders::_1, targetGIDOffsets,
                                  layerIds, morphologyTypes,
                                  electrophysiologyTypes, false);

                    if (!morphLoader._importMorphology(
                            uri, morphologyIndex, materialFunc,
                            transformations[morphologyIndex], compartmentReport,
                            modelContainer, _morphologyParams))
#pragma omp atomic
                        ++loadingFailures;
#pragma omp critical
                    modelContainer.addSpheresToModel(model);
#pragma omp critical
                    modelContainer.addCylindersToModel(model);
#pragma omp critical
                    modelContainer.addConesToModel(model);
#pragma omp critical
                    modelContainer.addSDFGeometriesToModel(model);
                }
                catch (...)
                {
                    cancelException = std::current_exception();
                    morphologyIndex = uris.size();
                }
            }
        }

        if (cancelException)
            std::rethrow_exception(cancelException);

        if (loadingFailures != 0)
        {
            BRAYNS_ERROR << loadingFailures << " could not be loaded"
                         << std::endl;
            return false;
        }
        return true;
    }

private:
    Scene& _scene;
    CircuitProperties _properties;
    MorphologyLoaderParams _morphologyParams;
};
}

namespace brayns
{
CircuitLoader::CircuitLoader(Scene& scene)
    : Loader(scene)
{
}

CircuitLoader::~CircuitLoader()
{
}

bool CircuitLoader::isSupported(const std::string& filename,
                                const std::string& extension
                                    BRAYNS_UNUSED) const
{
    const auto ends_with = [](const std::string& value,
                              const std::string& ending) {
        if (ending.size() > value.size())
            return false;
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    };

    const std::set<std::string> names = {"BlueConfig", "BlueConfig3",
                                         "CircuitConfig", "circuit"};

    for (const auto& name : names)
        if (ends_with(filename, name))
            return true;

    return false;
}

ModelDescriptorPtr CircuitLoader::importFromBlob(
    Blob&& /*blob*/, const LoaderProgress& /*callback*/,
    const PropertyMap& properties BRAYNS_UNUSED, const size_t /*index*/,
    const size_t /*materialID*/) const
{
    throw std::runtime_error("Loading circuit from blob is not supported");
}

ModelDescriptorPtr CircuitLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    const PropertyMap& propertiesTmp, const size_t /*index*/,
    const size_t /*materialID*/) const
{
    // Fill property map since the actual property types are known now.
    PropertyMap properties = getProperties();
    properties.merge(propertiesTmp);
    auto impl = Impl(_scene, properties);
    return impl.importCircuit(filename, callback);
}

std::string CircuitLoader::getName() const
{
    return LOADER_NAME;
}

std::vector<std::string> CircuitLoader::getSupportedExtensions() const
{
    return {"BlueConfig", "BlueConfig3", "CircuitConfig", "circuit"};
}

PropertyMap CircuitLoader::getProperties() const
{
    PropertyMap pm;
    pm.setProperty(PROP_CIRCUIT_DENSITY);
    pm.setProperty(PROP_CIRCUIT_RANDOM_SEED);
    pm.setProperty(PROP_CIRCUIT_REPORT);
    pm.setProperty(PROP_CIRCUIT_TARGETS);
    pm.setProperty(PROP_CIRCUIT_MESH_FILENAME_PATTERN);
    pm.setProperty(PROP_CIRCUIT_MESH_FOLDER);
    pm.setProperty(PROP_CIRCUIT_BOUNDING_BOX_P0);
    pm.setProperty(PROP_CIRCUIT_BOUNDING_BOX_P1);
    pm.setProperty(PROP_CIRCUIT_USE_SIMULATION_MODEL);
    pm.setProperty(PROP_CIRCUIT_MESH_TRANSFORMATION);
    pm.setProperty(PROP_COLOR_SCHEME);
    pm.setProperty(PROP_CIRCUIT_START_SIMULATION_TIME);
    pm.setProperty(PROP_CIRCUIT_END_SIMULATION_TIME);
    pm.setProperty(PROP_CIRCUIT_SIMULATION_STEP);
    pm.setProperty(PROP_SYNCHRONOUS_MODE);
    pm.setProperty(PROP_LOAD_CACHE_FILE);
    pm.setProperty(PROP_GEOMETRY_QUALITY);

    { // Add all morphology loader properties
        const auto mlpm = MorphologyLoader(_scene).getProperties();
        for (const auto& prop : mlpm.getProperties())
            if (prop && !pm.hasProperty(prop->name))
                pm.setProperty(*prop);
    }

#if BRAYNS_USE_ASSIMP
    { // Add all mesh loader properties
        const auto mlpm = MeshLoader(_scene).getProperties();
        for (const auto& prop : mlpm.getProperties())
            if (prop && !pm.hasProperty(prop->name))
                pm.setProperty(*prop);
    }
#endif

    return pm;
}
}
