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
#include <brayns/common/utils/utils.h>

#include <brayns/parameters/ApplicationParameters.h>

#include <brain/brain.h>

#if BRAYNS_USE_ASSIMP
#include <brayns/io/MeshLoader.h>
#endif

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

using namespace brayns;

namespace
{
using Property = brayns::Property;
const Property PROP_DENSITY = {"density", 100.0, {"Density"}};
const Property PROP_RANDOM_SEED = {"randomSeed", 0, {"Random seed"}};
const Property PROP_REPORT = {"report", std::string(""), {"Report"}};
const Property PROP_TARGETS = {"targets", std::string(""), {"Targets"}};
const Property PROP_MESH_FILENAME_PATTERN = {"meshFilenamePattern",
                                             std::string(""),
                                             {"Mesh filename pattern"}};
const Property PROP_MESH_FOLDER = {"meshFolder",
                                   std::string(""),
                                   {"Mesh folder"}};
const Property PROP_BOUNDING_BOX_P0 = {"boundingBoxP0",
                                       std::array<double, 3>{{0, 0, 0}},
                                       {"First bounding box boundary point"}};
const Property PROP_BOUNDING_BOX_P1 = {"boundingBoxP1",
                                       std::array<double, 3>{{0, 0, 0}},
                                       {"Second bounding box boundary point"}};
const Property PROP_USE_SIMULATION_MODEL = {"useSimulationModel",
                                            false,
                                            {"Use simulation model"}};
const Property PROP_TRANSFORM_MESHES = {"transformMeshes",
                                        false,
                                        {"Transform meshes"}};
const Property PROP_COLOR_SCHEME = {"colorScheme",
                                    brayns::enumToString(ColorScheme::none),
                                    brayns::enumNames<ColorScheme>(),
                                    {"Color scheme"}};
const Property PROP_START_SIMULATION_TIME = {"startSimulationTime",
                                             0.0,
                                             {"Start simulation time"}};
const Property PROP_END_SIMULATION_TIME = {"endSimulationTime",
                                           0.0,
                                           {"End simulation time"}};
const Property PROP_SIMULATION_STEP = {"simulationStep",
                                       0.0,
                                       {"Simulation step"}};
const Property PROP_SYNCHRONOUS_MODE = {"synchronousMode",
                                        false,
                                        {"Synchronous mode"}};
const Property PROP_GEOMETRY_QUALITY = {"geometryQuality",
                                        brayns::enumToString(
                                            GeometryQuality::high),
                                        brayns::enumNames<GeometryQuality>(),
                                       {"Geometry quality"}};
constexpr auto LOADER_NAME = "circuit";
const strings LOADER_EXTENSIONS{"BlueConfig", "BlueConfig3", "CircuitConfig",
                                ".json", "circuit"};

using SimulationHandlerPtr = std::shared_ptr<SimulationHandler>;

#if BRAYNS_USE_ASSIMP
/**
 * @brief getMeshFilenameFromGID Returns the name of the mesh file according
 * to the --circuit-mesh-folder, --circuit-mesh-filename-pattern command
 * line arguments and a GID
 * @param gid GID of the cell
 * @return A string with the full path of the mesh file
 */
std::string _getMeshFilenameFromGID(const uint64_t gid,
                                    const std::string& pattern,
                                    const std::string& folder)
{
    const std::string gidAsString = std::to_string(gid);
    const std::string GID = "{gid}";

    auto filenamePattern = pattern;
    if (!filenamePattern.empty())
        filenamePattern.replace(filenamePattern.find(GID), GID.length(),
                                gidAsString);
    else
        filenamePattern = gidAsString;
    return folder + "/" + filenamePattern;
}
#endif

/**
 * Return a vector of gids.size() elements with the layer index of each gid
 */
size_ts _getLayerIds(const brain::Circuit& circuit, const brain::GIDSet& gids)
{
    std::vector<brion::GIDSet> layers;
    try
    {
        for (auto layer : {"1", "2", "3", "4", "5", "6"})
            layers.push_back(circuit.getGIDs(std::string("Layer") + layer));
    }
    catch (const std::runtime_error& e)
    {
        // Some layer was not found by getGIDs.
        BRAYNS_ERROR << "Color scheme neuron_by_layer unavailable: " << e.what()
                     << std::endl;
        return size_ts();
    }

    std::vector<brion::GIDSet::const_iterator> iters;
    for (const auto& layer : layers)
        iters.push_back(layer.begin());

    size_ts materialIds;
    materialIds.reserve(gids.size());
    // For each GID find in which layer GID list it appears, assign
    // this index to its material id list.

    // The following code takes advantage of GID sets being sorted to iterate
    // all of them at once and assign a layer number to each gid in O(n) time.
    for (auto gid : gids)
    {
        size_t i = 0;
        for (i = 0; i != layers.size(); ++i)
        {
            while (iters[i] != layers[i].end() && *iters[i] < gid)
                ++iters[i];
            if (iters[i] != layers[i].end() && *iters[i] == gid)
            {
                materialIds.push_back(i);
                break;
            }
        }
        if (i == layers.size())
            throw std::runtime_error("Layer Id not found for GID " +
                                     std::to_string(gid));
    }
    return materialIds;
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

        setVariable(density, PROP_DENSITY.name, 100.0);
        setVariable(randomSeed, PROP_RANDOM_SEED.name, 0);
        setVariable(report, PROP_REPORT.name, "");
        setVariable(targets, PROP_TARGETS.name, "");
        setVariable(meshFilenamePattern, PROP_MESH_FILENAME_PATTERN.name, "");
        setVariable(meshFolder, PROP_MESH_FOLDER.name, "");
        setVariable(boundingBoxP0, PROP_BOUNDING_BOX_P0.name,
                    std::array<double, 3>{{0, 0, 0}});
        setVariable(boundingBoxP1, PROP_BOUNDING_BOX_P1.name,
                    std::array<double, 3>{{0, 0, 0}});
        setVariable(useSimulationModel, PROP_USE_SIMULATION_MODEL.name, false);
        setVariable(transformMeshes, PROP_TRANSFORM_MESHES.name, 0);
        setEnumVariable(colorScheme, PROP_COLOR_SCHEME.name, ColorScheme::none);
        setVariable(startSimulationTime, PROP_START_SIMULATION_TIME.name, 0.0);
        setVariable(endSimulationTime, PROP_END_SIMULATION_TIME.name, 0.0);
        setVariable(simulationStep, PROP_SIMULATION_STEP.name, 0.0);
        setVariable(synchronousMode, PROP_SYNCHRONOUS_MODE.name, false);
        setEnumVariable(geometryQuality, PROP_GEOMETRY_QUALITY.name,
                        GeometryQuality::high);

        boost::char_separator<char> separator(",");
        boost::tokenizer<boost::char_separator<char>> tokens(targets,
                                                             separator);
        for_each(tokens.begin(), tokens.end(),
                 [& targetList = targetList](const std::string& s) {
                     targetList.push_back(s);
                 });

        boundingBox = Boxd(toVmmlVec(boundingBoxP0), toVmmlVec(boundingBoxP1));
    }

    double density = 0.0;
    int32_t randomSeed = 0;
    std::string report;
    std::vector<std::string> targetList;
    std::string targets;
    std::string meshFilenamePattern;
    std::string meshFolder;
    bool useSimulationModel = false;
    bool transformMeshes = false;
    ColorScheme colorScheme = ColorScheme::none;

    double startSimulationTime = 0;
    double endSimulationTime = 0;
    double simulationStep = 0;

    bool synchronousMode = false;

    GeometryQuality geometryQuality = GeometryQuality::high;

    std::array<double, 3> boundingBoxP0;
    std::array<double, 3> boundingBoxP1;
    Boxd boundingBox;
};

brain::GIDSet _getFilteredGIDs(
    const std::function<brion::GIDSet(const std::string&)>& resolver,
    const brain::Circuit& circuit, const std::string& target,
    const CircuitProperties& properties)
{
    const auto allGIDs = resolver(target);
    const auto& aabb = properties.boundingBox;
    if (aabb.getSize() == Vector3f(0.f))
        return allGIDs;

    const Matrix4fs& transformations = circuit.getTransforms(allGIDs);
    brain::GIDSet gids;
    auto gid = allGIDs.begin();
    for (size_t i = 0; i < transformations.size(); ++i, ++gid)
        if (aabb.isIn(Vector3d(transformations[i].getTranslation())))
            gids.insert(*gid);
    return gids;
}

CompartmentReportPtr _openCompartmentReport(const brain::Simulation* simulation,
                                            const std::string& name,
                                            const brion::GIDSet& gids)
{
    if (!simulation || name.empty())
        return {};

    auto report = simulation->openCompartmentReport(name);
    return std::make_shared<brain::CompartmentReportView>(
        report.createView(gids));
}

SimulationHandlerPtr _createSimulationHandler(
    const CompartmentReportPtr& report, const CircuitProperties& properties)
{
    return std::make_shared<SimulationHandler>(report,
                                               properties.synchronousMode,
                                               properties.startSimulationTime,
                                               properties.endSimulationTime,
                                               properties.simulationStep);
}

class Impl
{
public:
    Impl(Scene& scene, const PropertyMap& properties)
        : _scene(scene)
        , _properties(properties)
        , _morphologyParams(properties)
    {
        _morphologyParams.useSimulationModel = _properties.useSimulationModel;
    }

    ModelDescriptorPtr importCircuit(const std::string& source,
                                     const LoaderProgress& callback) const
    {
        ModelDescriptorPtr modelDesc;
        // Model (one for the whole circuit)
        ModelMetadata metadata = {{"density",
                                   std::to_string(_properties.density)},
                                  {"report", _properties.report},
                                  {"targets", _properties.targets},
                                  {"mesh-filename-pattern",
                                   _properties.meshFilenamePattern},
                                  {"mesh-folder", _properties.meshFolder}};
        auto model = _scene.createModel();

        // Open circuit and simulation if possible
        brion::URI uri(source);
        std::unique_ptr<brain::Simulation> simulation;
        try
        {
            simulation = std::make_unique<brain::Simulation>(uri);
        }
        catch (std::runtime_error& error)
        {
            BRAYNS_INFO << "Cannot open " + source + " as simulation config: "
                        << error.what() << std::endl;
        }
        const auto circuit =
            simulation ? simulation->openCircuit() : brain::Circuit(uri);

        brain::GIDSet allGids;
        GIDOffsets targetGIDOffsets;
        targetGIDOffsets.push_back(0);

        const strings localTargets = _properties.targetList.empty()
                                         ? strings{{""}}
                                         : _properties.targetList;

        auto resolver = [&](const std::string& name) {
            const float fraction = _properties.density / 100.0f;
            if (simulation)
                return simulation->getGIDs(name, fraction,
                                           _properties.randomSeed);
            return circuit.getRandomGIDs(fraction, name,
                                         _properties.randomSeed);
        };

        for (const auto& target : localTargets)
        {
            const auto gids =
                _getFilteredGIDs(resolver, circuit, target, _properties);
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

        auto compartmentReport =
            _openCompartmentReport(simulation.get(), _properties.report,
                                   allGids);
        const brain::CompartmentReportMapping* reportMapping = nullptr;
        if (compartmentReport)
        {
            model->setSimulationHandler(
                _createSimulationHandler(compartmentReport, _properties));
            // Only keep GIDs from the report
            allGids = compartmentReport->getGIDs();
            reportMapping = &compartmentReport->getMapping();
        }

        const Matrix4fs& transformations = circuit.getTransforms(allGids);
        _logLoadedGIDs(allGids);

        const auto layerIds =
            _properties.colorScheme == ColorScheme::neuron_by_layer
                ? _getLayerIds(circuit, allGids)
                : size_ts();
        const auto electrophysiologyTypes =
            circuit.getElectrophysiologyTypes(allGids);
        const auto morphologyTypes = circuit.getMorphologyTypes(allGids);

        // Import meshes
        _importMeshes(callback, *model, allGids, transformations,
                      targetGIDOffsets, layerIds, morphologyTypes,
                      electrophysiologyTypes);

        // Import morphologies
        const auto useSimulationModel = _properties.useSimulationModel;
        model->useSimulationModel(useSimulationModel);
        if (_properties.meshFolder.empty() || useSimulationModel)
        {
            MorphologyLoader morphLoader(_scene);
            if (!_importMorphologies(circuit, callback, *model, allGids,
                                     transformations, targetGIDOffsets,
                                     reportMapping, morphLoader, layerIds,
                                     morphologyTypes, electrophysiologyTypes))
                return {};
        }

        // Create materials
        model->createMissingMaterials();
        // Assigning some colors to not get all white
        if (_properties.colorScheme != ColorScheme::none)
        {
            if (_properties.colorScheme == ColorScheme::neuron_by_segment_type)
                // This mode already handles sections types in a hacky way. A
                // refactoring is needed
                model->setMaterialsColorMap(MaterialsColorMap::none);
            else
                model->setMaterialsColorMap(MaterialsColorMap::gradient);
        }

        // Compute circuit center
        Boxf center;
        for (const auto& transformation : transformations)
            center.merge(transformation.getTranslation());

        Transformation transformation;
        transformation.setRotationCenter(center.getCenter());
        modelDesc =
            std::make_shared<ModelDescriptor>(std::move(model), "Circuit",
                                              source, metadata);
        modelDesc->setTransformation(transformation);

        return modelDesc;
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

        if (!isMesh && _properties.useSimulationModel)
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

    void _importMeshes(
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
        const auto meshedMorphologiesFolder = _properties.meshFolder;
        if (meshedMorphologiesFolder.empty())
            return;

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
            const auto transformation = _properties.transformMeshes
                                            ? transformations[meshIndex]
                                            : Matrix4f();
            try
            {
                meshLoader.importMesh(_getMeshFilenameFromGID(
                                          gid, _properties.meshFilenamePattern,
                                          _properties.meshFolder),
                                      callback, model, meshIndex,
                                      transformation, materialId, colorScheme,
                                      geometryQuality);
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
#else
        throw std::runtime_error(
            "assimp dependency is required to load meshes");
#endif
    }

    bool _importMorphologies(
        const brain::Circuit& circuit, const LoaderProgress& callback,
        Model& model, const brain::GIDSet& gids,
        const Matrix4fs& transformations, const GIDOffsets& targetGIDOffsets,
        const brain::CompartmentReportMapping* reportMapping,
        MorphologyLoader& morphLoader, const size_ts& layerIds,
        const size_ts& morphologyTypes,
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
                            transformations[morphologyIndex], reportMapping,
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

    for (const auto& name : LOADER_EXTENSIONS)
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
    return LOADER_EXTENSIONS;
}

PropertyMap CircuitLoader::getProperties() const
{
    PropertyMap pm;
    pm.setProperty(PROP_DENSITY);
    pm.setProperty(PROP_RANDOM_SEED);
    pm.setProperty(PROP_REPORT);
    pm.setProperty(PROP_TARGETS);
    pm.setProperty(PROP_MESH_FILENAME_PATTERN);
    pm.setProperty(PROP_MESH_FOLDER);
    pm.setProperty(PROP_BOUNDING_BOX_P0);
    pm.setProperty(PROP_BOUNDING_BOX_P1);
    pm.setProperty(PROP_USE_SIMULATION_MODEL);
    pm.setProperty(PROP_TRANSFORM_MESHES);
    pm.setProperty(PROP_COLOR_SCHEME);
    pm.setProperty(PROP_START_SIMULATION_TIME);
    pm.setProperty(PROP_END_SIMULATION_TIME);
    pm.setProperty(PROP_SIMULATION_STEP);
    pm.setProperty(PROP_SYNCHRONOUS_MODE);
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
