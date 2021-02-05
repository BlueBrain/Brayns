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

#include "AbstractCircuitLoader.h"
#include "MorphologyLoader.h"
#include "SpikeSimulationHandler.h"
#include "Utils.h"
#include "VoltageSimulationHandler.h"

#include <common/commonTypes.h>
#include <common/log.h>
#include <common/types.h>
#include <plugin/CircuitExplorerPlugin.h>
#include <plugin/io/CellGrowthHandler.h>

#include <brayns/common/Timer.h>
#include <brayns/common/scene/ClipPlane.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#if BRAYNS_USE_ASSIMP
#include <brayns/io/MeshLoader.h>
#endif

#include <unordered_set>


namespace
{
const strings LOADER_KEYWORDS{"BlueConfig", "CircuitConfig"};
const strings LOADER_EXTENSIONS{"BlueConfig",    "BlueConfig3",
                                "CircuitConfig", ".json",
                                "circuit",       "CircuitConfig_nrn"};
const std::string GID_PATTERN = "{gid}";
const size_t NB_MATERIALS_PER_INSTANCE = 3;
} // namespace

AbstractCircuitLoader::AbstractCircuitLoader(
    brayns::Scene &scene,
    const brayns::ApplicationParameters &applicationParameters,
    brayns::PropertyMap &&loaderParams,
    CircuitExplorerPlugin* pluginPtr)
    : Loader(scene)
    , _applicationParameters(applicationParameters)
    , _defaults(loaderParams)
    , _pluginPtr(pluginPtr)
{
}

std::vector<std::string> AbstractCircuitLoader::getSupportedExtensions() const
{
    return LOADER_EXTENSIONS;
}

bool AbstractCircuitLoader::isSupported(const std::string &filename,
                                        const std::string & /*extension*/) const
{
    const auto ends_with = [](const std::string &value,
                              const std::string &ending) {
        if (ending.size() > value.size())
            return false;
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    };

    for (const auto &name : LOADER_EXTENSIONS)
        if (ends_with(filename, name))
            return true;

    const auto contains = [](const std::string &value,
                             const std::string &keyword) {
        if(value.size() < keyword.size())
            return false;

        const auto lastSlash = value.find_last_of("/");
        std::string compareTo = value;
        if(lastSlash != std::string::npos)
            compareTo = value.substr(lastSlash + 1);
        return compareTo.find(keyword) != std::string::npos;
    };

    for(const auto& keyw : LOADER_KEYWORDS)
        if(contains(filename, keyw))
            return true;

    return false;
}

std::vector<std::string> AbstractCircuitLoader::_getTargetsAsStrings(
    const std::string &targets) const
{
    std::vector<std::string> result;
    std::string split;
    std::istringstream ss(targets);
    while (std::getline(ss, split, ','))
        result.push_back(split);
    return result;
}

std::vector<uint64_t> AbstractCircuitLoader::_getGIDsAsInts(
    const std::string &gids) const
{
    std::vector<uint64_t> result;
    std::string split;
    std::istringstream ss(gids);
    while (std::getline(ss, split, ','))
        result.push_back(atoi(split.c_str()));
    return result;
}

brain::GIDSet AbstractCircuitLoader::_getGids(
    const brayns::PropertyMap &properties,
    const brion::BlueConfig &blueConfiguration, const brain::Circuit &circuit,
    SchemeItem& targets) const
{
    const auto circuitTargets = _getTargetsAsStrings(
        properties.getProperty<std::string>(PROP_TARGETS.name));
    const auto circuitGids =
        _getGIDsAsInts(properties.getProperty<std::string>(PROP_GIDS.name));
    const auto circuitDensity =
        properties.getProperty<double>(PROP_DENSITY.name);
    const auto randomSeed =
        properties.getProperty<double>(PROP_RANDOM_SEED.name);
    const auto preSynapticNeuron =
        properties.getProperty<std::string>(PROP_PRESYNAPTIC_NEURON_GID.name);
    const auto postSynapticNeuron =
        properties.getProperty<std::string>(PROP_POSTSYNAPTIC_NEURON_GID.name);

    brain::GIDSet gids;
    targets.ids.push_back(0);

    // Pair synapse usecase
    if (!preSynapticNeuron.empty() && !postSynapticNeuron.empty())
    {
        gids.insert(boost::lexical_cast<uint64_t>(preSynapticNeuron));
        gids.insert(boost::lexical_cast<uint64_t>(postSynapticNeuron));
        return gids;
    }

    if (!circuitGids.empty())
    {
        for (const auto gid : circuitGids)
            gids.insert(gid);
        return gids;
    }

    // Determine GIDs according to specified targets
    strings localTargets;
    if (circuitTargets.empty())
    {
        const auto defaultTarget = blueConfiguration.getCircuitTarget();
        localTargets.push_back(defaultTarget);
        PLUGIN_INFO << "No target specified. Loading default one: "
                    << defaultTarget << std::endl;
    }
    else
        localTargets = circuitTargets;

    size_t counter = 0;
    for (const auto &target : localTargets)
    {
        const auto targetGids =
            (circuitDensity == 1.f
                 ? (target.empty() ? circuit.getGIDs()
                                   : circuit.getGIDs(target))
                 : circuit.getRandomGIDs(circuitDensity, target, randomSeed));
        PLUGIN_INFO << "Target " << target << ": " << targetGids.size()
                    << " cells" << std::endl;
        gids.insert(targetGids.begin(), targetGids.end());
        targets.ids.push_back(gids.size());
        targets.nameMap[counter++] = target;
    }

    if (gids.empty())
        PLUGIN_THROW("Selection is empty");
    return gids;
}

CompartmentReportPtr AbstractCircuitLoader::_attachSimulationHandler(
    const brayns::PropertyMap &properties,
    const brion::BlueConfig &blueConfiguration, brayns::Model &model,
    const ReportType &reportType, brain::GIDSet &gids) const
{
    CompartmentReportPtr compartmentReport{nullptr};

    // Load simulation information from compartment report
    const auto report = properties.getProperty<std::string>(PROP_REPORT.name);
    const auto userDataType = stringToEnum<UserDataType>(
        properties.getProperty<std::string>(PROP_USER_DATA_TYPE.name));

    const auto dbConnectionString =
        properties.getProperty<std::string>(PROP_DB_CONNECTION_STRING.name);
    const auto synchronousMode =
        properties.getProperty<bool>(PROP_SYNCHRONOUS_MODE.name);

    brayns::AbstractSimulationHandlerPtr simulationHandler{nullptr};
    switch (reportType)
    {
    case ReportType::voltages_from_file:
    {
        if (report.empty() && (userDataType == UserDataType::simulation_offset ||
                               userDataType == UserDataType::undefined))
            return nullptr;

        PLUGIN_INFO << "Loading simulation data in "
                    << (synchronousMode ? "a" : "") << "synchronous mode"
                    << std::endl;
        const auto &voltageReport = blueConfiguration.getReportSource(report);
        PLUGIN_INFO << "Voltage report: " << voltageReport << std::endl;
        auto handler =
            std::make_shared<VoltageSimulationHandler>(voltageReport.getPath(),
                                                       gids, synchronousMode);
        compartmentReport = handler->getReport();

        // Only keep simulated GIDs
        if (!compartmentReport)
            PLUGIN_THROW("No voltage report was found");
        gids = compartmentReport->getGIDs();

        model.setSimulationHandler(handler);

        simulationHandler = handler;
        setSimulationTransferFunction(_scene.getTransferFunction());
        break;
    }
    case ReportType::spikes:
    {
        const auto &spikeReport = blueConfiguration.getSpikeSource();
        const auto transitionTime = properties.getProperty<double>
                (PROP_SPIKE_TRANSITION_TIME.name);
        PLUGIN_INFO << "Spike report: " << spikeReport << std::endl;
        auto handler =
            std::make_shared<SpikeSimulationHandler>(spikeReport.getPath(),
                                                     gids,
                                                     static_cast<float>(transitionTime));
        model.setSimulationHandler(handler);
        simulationHandler = handler;
        setSimulationTransferFunction(_scene.getTransferFunction());
        break;
    }
    case ReportType::undefined:
    {
        return compartmentReport;
    }
    default:
        if (userDataType == UserDataType::distance_to_soma)
        {
            auto handler = std::make_shared<CellGrowthHandler>(100);
            model.setSimulationHandler(handler);
            setSimulationTransferFunction(_scene.getTransferFunction(), 0.f);
        }
        else
            PLUGIN_THROW("Unknown report type. Simulation ignored");
    }
    return compartmentReport;
}

bool AbstractCircuitLoader::_isClipped(const brayns::Vector3f &position) const
{
    auto &clipPlanes = _scene.getClipPlanes();
    if (clipPlanes.empty())
        return false;

    bool visible = true;
    for (auto clipPlane : clipPlanes)
    {
        const auto &plane = clipPlane->getPlane();
        const brayns::Vector3f normal = {plane[0], plane[1], plane[2]};
        const float d = plane[3];
        const float distance = dot(normal, position) + d;
        visible &= (distance > 0.f);
    }
    return !visible;
}

void AbstractCircuitLoader::_filterGIDsWithClippingPlanes(
    brain::GIDSet &gids, Matrix4fs &transformations) const
{
    // Filter our guids according to clipping planes
    Matrix4fs clippedTransformations;
    brain::GIDSet clippedGids;
    uint64_t i = 0;
    for (const auto gid : gids)
    {
        const auto &transformation = transformations[i];
        if (!_isClipped(get_translation(transformation)))
        {
            clippedTransformations.push_back(transformation);
            clippedGids.insert(gid);
        }
        ++i;
    }
    gids = clippedGids;
    transformations = clippedTransformations;
    PLUGIN_INFO << "Clipped circuit: " << gids.size() << " cells" << std::endl;
}

void AbstractCircuitLoader::_filterGIDsWithAreasOfInterest(
    const uint16_t areasOfInterest, brain::GIDSet &gids,
    Matrix4fs &transformations) const
{
    brain::GIDSet rearrangedGids;
    Matrix4fs rearrangedTransformations;
    brayns::Boxf aabb;
    for (const auto &transformation : transformations)
        aabb.merge(get_translation(transformation));

    const auto bbMin = aabb.getMin();
    const auto bbMax = aabb.getMax();
    const float bbSize = aabb.getSize().x / areasOfInterest;
    for (int i = 0; i < areasOfInterest; ++i)
    {
        brayns::Boxf area;
        area.merge({bbMin.x + i * bbSize, bbMin.y, bbMin.z});
        area.merge({bbMin.x + (i + 1) * bbSize, bbMax.y, bbMax.z});

        uint64_t j = 0;
        for (const auto &gid : gids)
        {
            if (inBox(get_translation(transformations[j]), area))
            {
                rearrangedGids.insert(gid);
                rearrangedTransformations.push_back(transformations[j]);
                break;
            }
            ++j;
        }
    }
    gids = rearrangedGids;
    transformations = rearrangedTransformations;
    PLUGIN_INFO << "Areas of interest: " << gids.size() << " cells"
                << std::endl;
}

brayns::ModelDescriptorPtr AbstractCircuitLoader::importCircuit(
    const std::string &circuitConfiguration,
    const brayns::PropertyMap &properties,
    const brayns::LoaderProgress &callback) const
{
    const auto colorScheme = stringToEnum<CircuitColorScheme>(
        properties.getProperty<std::string>(PROP_CIRCUIT_COLOR_SCHEME.name));
    const auto morphologyScheme = stringToEnum<MorphologyColorScheme>(
        properties.getProperty<std::string>(PROP_MORPHOLOGY_COLOR_SCHEME.name));
    const auto morphologyQuality = stringToEnum<MorphologyQuality>(
        properties.getProperty<std::string>(PROP_MORPHOLOGY_QUALITY.name));
    const auto meshFolder = properties.getProperty<std::string>(PROP_MESH_FOLDER.name);
    const auto meshFilenamePattern =
        properties.getProperty<std::string>(PROP_MESH_FILENAME_PATTERN.name);
    const auto reportType = stringToEnum<ReportType>(
        properties.getProperty<std::string>(PROP_REPORT_TYPE.name));
    const auto userDataType = stringToEnum<UserDataType>(
        properties.getProperty<std::string>(PROP_USER_DATA_TYPE.name));
    const auto cellClipping = properties.getProperty<bool>(PROP_CELL_CLIPPING.name);
    const auto areasOfInterest = properties.getProperty<int>(PROP_AREAS_OF_INTEREST.name);
    const auto loadLayers = properties.getProperty<bool>(PROP_LOAD_LAYERS.name, true);
    const auto loadEtypes = properties.getProperty<bool>(PROP_LOAD_ETYPES.name, true);
    const auto loadMtypes = properties.getProperty<bool>(PROP_LOAD_MTYPES.name, true);


    // Model (one for the whole circuit)
    auto model = _scene.createModel();
    if (!model)
        PLUGIN_THROW("Failed to create model");

    // Open Circuit and select GIDs according to specified target
    callback.updateProgress("Open Brion circuit ...", 0);
    const brion::BlueConfig blueConfiguration(circuitConfiguration);
    callback.updateProgress("Open Brain circuit ...", 0);
    const brain::Circuit circuit(blueConfiguration);

    // Circuit mapper. Morphology info will be gathered during loading
    CellObjectMapper objMapper;
    // Circuit schema data will be stored here
    CircuitSchemeData& data = objMapper.getSchemeData();

    callback.updateProgress("Getting GIDs...", 0);
    GIDOffsets targetGIDOffsets;
    brain::GIDSet allGids =
        _getGids(properties, blueConfiguration, circuit, data.targets);

    callback.updateProgress("Attaching to simulation data...", 0);

    // Attach simulation handler
    const auto compartmentReport =
        _attachSimulationHandler(properties, blueConfiguration, *model,
                                 reportType, allGids);

    // Cell transformations
    Matrix4fs allTransformations = circuit.getTransforms(allGids);

    // Filter out guids according to clipping planes
    if (cellClipping)
        _filterGIDsWithClippingPlanes(allGids, allTransformations);

    // Filter out guids according to aeras of interest
    if (areasOfInterest != 0)
        _filterGIDsWithAreasOfInterest(areasOfInterest, allGids,
                                       allTransformations);

    // Import meshes and morphologies
    if(loadLayers || colorScheme == CircuitColorScheme::by_layer)
    {
        callback.updateProgress("Identifying layer ids...", 0);
        _populateLayerIds(blueConfiguration, circuit, allGids, data.layers);
    }

    if(loadEtypes || colorScheme == CircuitColorScheme::by_etype)
    {
        callback.updateProgress("Identifying electro-physiology types...", 0);
        data.etypes.ids = circuit.getElectrophysiologyTypes(allGids);
        //data.etypes.names = circuit.getElectrophysiologyTypeNames();
    }

    if(loadMtypes || colorScheme == CircuitColorScheme::by_mtype)
    {
        callback.updateProgress("Getting morphology types...", 0);
        data.mtypes.ids = circuit.getMorphologyTypes(allGids);
    }

    callback.updateProgress("Importing morphologies...", 0);
    float maxMorphologyLength = 0.f;
    if (meshFolder.empty())
        maxMorphologyLength =
            _importMorphologies(properties, circuit, *model, allGids,
                                allTransformations, objMapper,
                                compartmentReport, callback);
    else
    {
        _importMeshes(properties, *model, allGids, allTransformations,
                      callback, objMapper);
        if (compartmentReport != nullptr)
            // If meshes are loaded, and simulation is enabled, a secondary
            // model is created to store the simulation data in the 3D scene
            maxMorphologyLength =
                _importMorphologies(properties, circuit, *model, allGids,
                                    allTransformations, objMapper,
                                    compartmentReport, callback,
                                    brayns::SECONDARY_MODEL_MATERIAL_ID);
    }

    if (userDataType == UserDataType::distance_to_soma)
    {
        // Update cell growth information
        _scene.getTransferFunction().setValuesRange({0.f, maxMorphologyLength});
        const auto frameSize = uint64_t(maxMorphologyLength) + 1;
        model->getSimulationHandler()->setFrameSize(frameSize);
        model->getSimulationHandler()->setNbFrames(frameSize);
    }

    // Create custom materials
    brayns::PropertyMap materialProps;
    materialProps.setProperty(
        {MATERIAL_PROPERTY_CAST_USER_DATA,
         (compartmentReport || userDataType == UserDataType::distance_to_soma ||
          reportType == ReportType::spikes)
             ? true
             : false});
    materialProps.setProperty({MATERIAL_PROPERTY_SHADING_MODE,
                               static_cast<int>(MaterialShadingMode::diffuse)});
    materialProps.setProperty(
        {MATERIAL_PROPERTY_CLIPPING_MODE,
         static_cast<int>(MaterialClippingMode::no_clipping)});
    MorphologyLoader::createMissingMaterials(*model, materialProps);

    // Apply default colotmap
    _setDefaultCircuitColorMap(*model);

    // Compute circuit center according to soma positions
    callback.updateProgress("Computing circuit center...", 1);
    brayns::Boxf circuitCenter;
    for (const auto &transformation : allTransformations)
        circuitCenter.merge(get_translation(transformation));

    // Create model
    brayns::ModelMetadata metadata = {
        {"Report", properties.getProperty<std::string>(PROP_REPORT.name)},
        {"Report type",
         properties.getProperty<std::string>(PROP_REPORT_TYPE.name)},
        {"Targets", properties.getProperty<std::string>(PROP_TARGETS.name)},
        {"GIDs", properties.getProperty<std::string>(PROP_GIDS.name)},
        {"Color scheme", enumToString<CircuitColorScheme>(colorScheme)},
        {"Use simulation model", enumToString<bool>(!meshFolder.empty())},
        {"Mesh filename pattern", meshFilenamePattern},
        {"Mesh folder", meshFolder},
        {"Morphology quality",
         enumToString<MorphologyQuality>(morphologyQuality)},
        {"Number of neurons", std::to_string(allGids.size())},
        {"Density", std::to_string(properties.getProperty<double>(PROP_DENSITY.name))},
        {"RandomSeed", std::to_string(properties.getProperty<double>(PROP_RANDOM_SEED.name))},
        {"CircuitPath", circuitConfiguration}};

    brayns::ModelDescriptorPtr modelDescriptor;
    brayns::Transformation transformation;
    transformation.setRotationCenter(circuitCenter.getCenter());
    modelDescriptor =
        std::make_shared<brayns::ModelDescriptor>(std::move(model), "Circuit",
                                                  circuitConfiguration,
                                                  metadata);
    modelDescriptor->setTransformation(transformation);

    // Clean the circuit mapper associated with this model
    modelDescriptor->onRemoved([plptr = _pluginPtr](const brayns::ModelDescriptor& remMod)
    {
        plptr->releaseCircuitMapper(remMod.getModelID());
    });

    objMapper.setSourceModel(modelDescriptor);
    objMapper.onCircuitColorFinish(colorScheme, morphologyScheme);

    this->_pluginPtr->addCircuitMapper(std::move(objMapper));

    return modelDescriptor;
}

size_t AbstractCircuitLoader::_getMaterialFromCircuitAttributes(
    const brayns::PropertyMap &properties, const uint64_t index,
    const size_t material, const bool forSimulationModel,
    CircuitSchemeData* data) const
{
    if (material != brayns::NO_MATERIAL)
        return material;

    if (forSimulationModel)
        return 0;

    const auto colorScheme = stringToEnum<CircuitColorScheme>(
        properties.getProperty<std::string>(PROP_CIRCUIT_COLOR_SCHEME.name));

    size_t materialId = 0;
    switch (colorScheme)
    {
    case CircuitColorScheme::single_material:
        materialId = 1;
        break;
    case CircuitColorScheme::by_id:
        materialId = NB_MATERIALS_PER_INSTANCE * index;
        break;
    case CircuitColorScheme::by_target:
        if(data && !data->targets.ids.empty())
        {
            for (size_t i = 0; i < data->targets.ids.size() - 1; ++i)
                if (index >= data->targets.ids[i] && index < data->targets.ids[i + 1])
                {
                    materialId = NB_MATERIALS_PER_INSTANCE * i;
                    data->targets.materialMap[i] = materialId;
                    break;
                }
        }
        break;
    case CircuitColorScheme::by_etype:
        if (data && index < data->etypes.ids.size())
        {
            const auto etypeId = data->etypes.ids[index];
            materialId =
                NB_MATERIALS_PER_INSTANCE * etypeId;
            data->etypes.materialMap[etypeId] = materialId;
        }
        else
            PLUGIN_DEBUG << "Failed to get neuron E-type" << std::endl;
        break;
    case CircuitColorScheme::by_mtype:
        if (data && index < data->mtypes.ids.size())
        {
            const auto mtypeId = data->mtypes.ids[index];
            materialId = NB_MATERIALS_PER_INSTANCE * mtypeId;
            data->mtypes.materialMap[mtypeId] = materialId;
        }
        else
            PLUGIN_DEBUG << "Failed to get neuron M-type" << std::endl;
        break;
    case CircuitColorScheme::by_layer:
        if (data && index < data->layers.ids.size())
        {
            const auto& layerId = data->layers.ids[index];
            auto vi = data->layers.virtualIndex[layerId];
            materialId = NB_MATERIALS_PER_INSTANCE * vi;
            data->layers.materialMap[layerId] = materialId;
        }
        else
            PLUGIN_DEBUG << "Failed to get neuron layer" << std::endl;
        break;
    default:
        materialId = brayns::NO_MATERIAL;
    }
    return materialId;
}

void AbstractCircuitLoader::_populateLayerIds(const brion::BlueConfig &blueConfig,
                                              const brain::Circuit& circuit,
                                              const brain::GIDSet &gids,
                                              LayerSchemeItem& result) const
{
    const auto& tsvFile = blueConfig.get(brion::BlueConfigSection::CONFIGSECTION_RUN,
                                         "Default", "MEComboInfoFile");
    result.ids = circuit.getLayers(gids, tsvFile);
    std::unordered_set<std::string> uniqueLayers(result.ids.begin(), result.ids.end());
    size_t idx = 0;
    for(const auto& uniqueLayer : uniqueLayers)
        result.virtualIndex[uniqueLayer] = idx++;
}

std::string AbstractCircuitLoader::_getMeshFilenameFromGID(
    const brayns::PropertyMap &properties, const uint64_t gid) const
{
    const auto meshFolder =
        properties.getProperty<std::string>(PROP_MESH_FOLDER.name);
    const auto meshFilenamePattern =
        properties.getProperty<std::string>(PROP_MESH_FILENAME_PATTERN.name);
    const std::string gidAsString = std::to_string(gid);
    auto tmp = meshFilenamePattern;
    if (!tmp.empty())
        tmp.replace(tmp.find(GID_PATTERN), GID_PATTERN.length(), gidAsString);
    else
        tmp = gidAsString;
    return meshFolder + "/" + tmp;
}

#if BRAYNS_USE_ASSIMP
void AbstractCircuitLoader::_importMeshes(
    const brayns::PropertyMap &properties, brayns::Model &model,
    const brain::GIDSet &gids, const Matrix4fs &transformations,
    const brayns::LoaderProgress &callback, CellObjectMapper& mapper) const
{
    brayns::MeshLoader meshLoader(_scene);
    const auto morphologyQuality = stringToEnum<MorphologyQuality>(
        properties.getProperty<std::string>(PROP_MORPHOLOGY_QUALITY.name));
    const auto meshTransformation =
        properties.getProperty<bool>(PROP_MESH_TRANSFORMATION.name);

    size_t meshIndex = 0;
    for (const auto &gid : gids)
    {
        const size_t materialId = _getMaterialFromCircuitAttributes(
            properties, meshIndex, brayns::NO_MATERIAL, false, &mapper.getSchemeData());

        // Load mesh from file
        const auto transformation = meshTransformation
                                        ? transformations[meshIndex]
                                        : brayns::Matrix4f();

        brayns::GeometryQuality quality;
        switch (morphologyQuality)
        {
        case MorphologyQuality::low:
            quality = brayns::GeometryQuality::low;
            break;
        case MorphologyQuality::medium:
            quality = brayns::GeometryQuality::medium;
            break;
        default:
            quality = brayns::GeometryQuality::high;
            break;
        }
        try
        {
            meshLoader.importMesh(_getMeshFilenameFromGID(properties, gid),
                                  brayns::LoaderProgress(), model,
                                  transformation, materialId, quality);
            MorphologyMap newMap;
            newMap._hasMesh = true;
            newMap._triangleIndx = materialId;
            mapper.add(gid, newMap);
        }
        catch (const std::runtime_error &e)
        {
            PLUGIN_WARN << e.what() << std::endl;
        }
        ++meshIndex;
        callback.updateProgress("Loading morphologies as meshes...",
                                (float)meshIndex / (float)gids.size());
    }
    // Add custom properties to materials
    for (auto &material : model.getMaterials())
    {
        brayns::PropertyMap props;
        props.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, true});
        props.setProperty({MATERIAL_PROPERTY_SHADING_MODE,
                           static_cast<int>(MaterialShadingMode::diffuse)});
        props.setProperty(
            {MATERIAL_PROPERTY_CLIPPING_MODE,
             static_cast<int>(MaterialClippingMode::no_clipping)});
        material.second->setProperties(props);
    }
}
#else
void AbstractCircuitLoader::_importMeshes(
    brayns::Model &, const brain::GIDSet &, const brayns::Matrix4s &,
    const GIDOffsets &, const size_ts &, const size_ts &, const size_ts &,
    const CircuitColorScheme &, const brayns::LoaderProgress &) const
{
}
#endif

void AbstractCircuitLoader::_setDefaultCircuitColorMap(
    brayns::Model &model) const
{
    const std::vector<brayns::Vector3d> colors = {
        {0.8941176470588236, 0.10196078431372549, 0.10980392156862745},
        {0.21568627450980393, 0.49411764705882355, 0.7215686274509804},
        {0.30196078431372547, 0.6862745098039216, 0.2901960784313726},
        {0.596078431372549, 0.3058823529411765, 0.6392156862745098},
        {1.0, 0.4980392156862745, 0.0},
        {1.0, 1.0, 0.2},
        {0.6509803921568628, 0.33725490196078434, 0.1568627450980392},
        {0.9686274509803922, 0.5058823529411764, 0.7490196078431373},
        {0.6, 0.6, 0.6},
        {0.8941176470588236, 0.10196078431372549, 0.10980392156862745},
        {0.21568627450980393, 0.49411764705882355, 0.7215686274509804},
        {0.30196078431372547, 0.6862745098039216, 0.2901960784313726},
        {0.596078431372549, 0.3058823529411765, 0.6392156862745098},
        {1.0, 0.4980392156862745, 0.0},
        {1.0, 1.0, 0.2},
        {0.6509803921568628, 0.33725490196078434, 0.1568627450980392},
        {0.9686274509803922, 0.5058823529411764, 0.7490196078431373},
        {0.6, 0.6, 0.6},
        {0.8941176470588236, 0.10196078431372549, 0.10980392156862745},
        {0.21568627450980393, 0.49411764705882355, 0.7215686274509804}};

    uint64_t i = 0;
    auto &materials = model.getMaterials();
    for (auto &material : materials)
    {
        const auto &color = colors[i % colors.size()];
        material.second->setDiffuseColor(color);
        ++i;
    }
}

void AbstractCircuitLoader::setSimulationTransferFunction(
    brayns::TransferFunction &tf, const float finalOpacity)
{
    tf.setControlPoints({{0, 1}, {1, finalOpacity}});
    // curl https://api.colormaps.io/colormap/unipolar
    tf.setColorMap(
        {"unipolar",
         {{0.0, 0.0, 0.0},
          {0.00392156862745098, 0.00392156862745098, 0.12941176470588237},
          {0.00784313725490196, 0.00784313725490196, 0.25882352941176473},
          {0.011764705882352941, 0.011764705882352941, 0.39215686274509803},
          {0.01568627450980392, 0.01568627450980392, 0.5215686274509804},
          {0.0196078431372549, 0.0196078431372549, 0.6549019607843137},
          {0.03529411764705882, 0.0784313725490196, 0.6862745098039216},
          {0.047058823529411764, 0.13333333333333333, 0.7215686274509804},
          {0.058823529411764705, 0.18823529411764706, 0.7568627450980392},
          {0.07450980392156863, 0.24705882352941178, 0.788235294117647},
          {0.08627450980392157, 0.30196078431372547, 0.8235294117647058},
          {0.09803921568627451, 0.3607843137254902, 0.8588235294117647},
          {0.11372549019607843, 0.41568627450980394, 0.8901960784313725},
          {0.12549019607843137, 0.47058823529411764, 0.9254901960784314},
          {0.13725490196078433, 0.5294117647058824, 0.9568627450980393},
          {0.2196078431372549, 0.4666666666666667, 0.8745098039215686},
          {0.30196078431372547, 0.403921568627451, 0.796078431372549},
          {0.3843137254901961, 0.3411764705882353, 0.7137254901960784},
          {0.4823529411764706, 0.28627450980392155, 0.596078431372549},
          {0.5764705882352941, 0.22745098039215686, 0.47843137254901963},
          {0.6705882352941176, 0.16862745098039217, 0.36470588235294116},
          {0.7686274509803922, 0.11372549019607843, 0.24705882352941178},
          {0.8627450980392157, 0.054901960784313725, 0.13333333333333333},
          {0.9568627450980393, 0.0, 0.01568627450980392},
          {0.9568627450980393, 0.0196078431372549, 0.01568627450980392},
          {0.9529411764705882, 0.043137254901960784, 0.01568627450980392},
          {0.9490196078431372, 0.06666666666666667, 0.01568627450980392},
          {0.9450980392156862, 0.08627450980392157, 0.01568627450980392},
          {0.9411764705882353, 0.10980392156862745, 0.01568627450980392},
          {0.9372549019607843, 0.13333333333333333, 0.011764705882352941},
          {0.9333333333333333, 0.1568627450980392, 0.011764705882352941},
          {0.9333333333333333, 0.17647058823529413, 0.011764705882352941},
          {0.9294117647058824, 0.2, 0.011764705882352941},
          {0.9254901960784314, 0.2235294117647059, 0.011764705882352941},
          {0.9215686274509803, 0.24705882352941178, 0.011764705882352941},
          {0.9176470588235294, 0.26666666666666666, 0.00784313725490196},
          {0.9137254901960784, 0.2901960784313726, 0.00784313725490196},
          {0.9098039215686274, 0.3137254901960784, 0.00784313725490196},
          {0.9098039215686274, 0.33725490196078434, 0.00784313725490196},
          {0.9058823529411765, 0.3568627450980392, 0.00784313725490196},
          {0.9019607843137255, 0.3803921568627451, 0.00784313725490196},
          {0.8980392156862745, 0.403921568627451, 0.00392156862745098},
          {0.8941176470588236, 0.4235294117647059, 0.00392156862745098},
          {0.8901960784313725, 0.4470588235294118, 0.00392156862745098},
          {0.8862745098039215, 0.47058823529411764, 0.00392156862745098},
          {0.8823529411764706, 0.49411764705882355, 0.00392156862745098},
          {0.8823529411764706, 0.5137254901960784, 0.00392156862745098},
          {0.8784313725490196, 0.5372549019607843, 0.0},
          {0.8745098039215686, 0.5607843137254902, 0.0},
          {0.8705882352941177, 0.5843137254901961, 0.0},
          {0.8666666666666667, 0.6039215686274509, 0.0},
          {0.8627450980392157, 0.6274509803921569, 0.0},
          {0.8588235294117647, 0.6509803921568628, 0.0},
          {0.8588235294117647, 0.6745098039215687, 0.0},
          {0.8588235294117647, 0.6823529411764706, 0.01568627450980392},
          {0.8627450980392157, 0.6901960784313725, 0.03529411764705882},
          {0.8666666666666667, 0.7019607843137254, 0.050980392156862744},
          {0.8705882352941177, 0.7098039215686275, 0.07058823529411765},
          {0.8705882352941177, 0.7176470588235294, 0.08627450980392157},
          {0.8745098039215686, 0.7294117647058823, 0.10588235294117647},
          {0.8784313725490196, 0.7372549019607844, 0.12549019607843137},
          {0.8823529411764706, 0.7450980392156863, 0.1411764705882353},
          {0.8823529411764706, 0.7568627450980392, 0.1607843137254902},
          {0.8862745098039215, 0.7647058823529411, 0.17647058823529413},
          {0.8901960784313725, 0.7764705882352941, 0.19607843137254902},
          {0.8941176470588236, 0.7843137254901961, 0.21568627450980393},
          {0.8980392156862745, 0.792156862745098, 0.23137254901960785},
          {0.8980392156862745, 0.803921568627451, 0.25098039215686274},
          {0.9019607843137255, 0.8117647058823529, 0.26666666666666666},
          {0.9058823529411765, 0.8196078431372549, 0.28627450980392155},
          {0.9098039215686274, 0.8313725490196079, 0.3058823529411765},
          {0.9098039215686274, 0.8392156862745098, 0.3215686274509804},
          {0.9137254901960784, 0.8509803921568627, 0.3411764705882353},
          {0.9176470588235294, 0.8588235294117647, 0.3568627450980392},
          {0.9215686274509803, 0.8666666666666667, 0.3764705882352941},
          {0.9215686274509803, 0.8784313725490196, 0.396078431372549},
          {0.9254901960784314, 0.8862745098039215, 0.4117647058823529},
          {0.9294117647058824, 0.8941176470588236, 0.43137254901960786},
          {0.9333333333333333, 0.9058823529411765, 0.4470588235294118},
          {0.9372549019607843, 0.9137254901960784, 0.4666666666666667},
          {0.9372549019607843, 0.9254901960784314, 0.48627450980392156},
          {0.9411764705882353, 0.9333333333333333, 0.5019607843137255},
          {0.9450980392156862, 0.9411764705882353, 0.5215686274509804},
          {0.9490196078431372, 0.9529411764705882, 0.5372549019607843},
          {0.9490196078431372, 0.9607843137254902, 0.5568627450980392},
          {0.9529411764705882, 0.9686274509803922, 0.5764705882352941},
          {0.9568627450980393, 0.9803921568627451, 0.592156862745098},
          {0.9607843137254902, 0.9882352941176471, 0.611764705882353},
          {0.9647058823529412, 1.0, 0.6274509803921569},
          {0.9647058823529412, 1.0, 0.6392156862745098},
          {0.9647058823529412, 1.0, 0.6470588235294118},
          {0.9647058823529412, 1.0, 0.6588235294117647},
          {0.9647058823529412, 1.0, 0.6666666666666666},
          {0.9686274509803922, 1.0, 0.6745098039215687},
          {0.9686274509803922, 1.0, 0.6862745098039216},
          {0.9686274509803922, 1.0, 0.6941176470588235},
          {0.9686274509803922, 1.0, 0.7019607843137254},
          {0.9725490196078431, 1.0, 0.7137254901960784},
          {0.9725490196078431, 1.0, 0.7215686274509804},
          {0.9725490196078431, 1.0, 0.7294117647058823},
          {0.9725490196078431, 1.0, 0.7411764705882353},
          {0.9725490196078431, 1.0, 0.7490196078431373},
          {0.9764705882352941, 1.0, 0.7568627450980392},
          {0.9764705882352941, 1.0, 0.7686274509803922},
          {0.9764705882352941, 1.0, 0.7764705882352941},
          {0.9764705882352941, 1.0, 0.7843137254901961},
          {0.9803921568627451, 1.0, 0.796078431372549},
          {0.9803921568627451, 1.0, 0.803921568627451},
          {0.9803921568627451, 1.0, 0.8117647058823529},
          {0.9803921568627451, 1.0, 0.8235294117647058},
          {0.9803921568627451, 1.0, 0.8313725490196079},
          {0.984313725490196, 1.0, 0.8431372549019608},
          {0.984313725490196, 1.0, 0.8509803921568627},
          {0.984313725490196, 1.0, 0.8588235294117647},
          {0.984313725490196, 1.0, 0.8705882352941177},
          {0.9882352941176471, 1.0, 0.8784313725490196},
          {0.9882352941176471, 1.0, 0.8862745098039215},
          {0.9882352941176471, 1.0, 0.8980392156862745},
          {0.9882352941176471, 1.0, 0.9058823529411765},
          {0.9882352941176471, 1.0, 0.9137254901960784},
          {0.9921568627450981, 1.0, 0.9254901960784314},
          {0.9921568627450981, 1.0, 0.9333333333333333},
          {0.9921568627450981, 1.0, 0.9411764705882353},
          {0.9921568627450981, 1.0, 0.9529411764705882},
          {0.996078431372549, 1.0, 0.9607843137254902},
          {0.996078431372549, 1.0, 0.9686274509803922},
          {0.996078431372549, 1.0, 0.9803921568627451},
          {1.0, 1.0, 1.0}}});
    tf.setValuesRange({-80, -10});
}

float AbstractCircuitLoader::_importMorphologies(
    const brayns::PropertyMap &properties, const brain::Circuit &circuit,
    brayns::Model &model, const brain::GIDSet &gids,
    const Matrix4fs &transformations, CellObjectMapper& mapper,
    CompartmentReportPtr compartmentReport,
    const brayns::LoaderProgress &callback, const size_t materialId) const
{
    float maxDistanceToSoma = 0.f;
    brayns::Timer chrono;
    const auto sectionTypes =
        MorphologyLoader::getSectionTypesFromProperties(_defaults);
    const bool somasOnly =
        (sectionTypes.size() == 1 &&
         sectionTypes[0] == brain::neuron::SectionType::soma);
    brain::URIs uris;
    if (!somasOnly)
        uris = circuit.getMorphologyURIs(gids);

    brayns::PropertyMap morphologyProps(properties);
    MorphologyLoader loader(_scene, std::move(morphologyProps));

    // Function to compute shape indexes within the model and store them into
    // the morphology map
    const auto func = [](std::unordered_map<size_t, std::vector<size_t>>& storage,
                         size_t mid, size_t start, size_t end)
    {
        if(end > start)
        {
            auto& storageVector = storage[mid];
            for(size_t i = start; i < end; ++i)
                storageVector.push_back(i);
        }
    };

    size_t i = 0;
    for (auto gid : gids)
    {
        const auto uri = somasOnly ? brain::URI() : uris[i];
        const auto id =
            _getMaterialFromCircuitAttributes(properties, i, materialId,
                                              false, &mapper.getSchemeData());

        loader.setDefaultMaterialId(id);

        // Start indices before adding the morphology
        const size_t startSpheres = model.getSpheres()[id].size();
        const size_t startCones = model.getCones()[id].size();
        const size_t startCylinders = model.getCylinders()[id].size();
        const size_t startSDFGeoms = model.getSDFGeometryData().geometries.size();
        const size_t startSDFBeziers = model.getSDFBeziers()[id].size();

        MorphologyInfo morphologyInfo;
        morphologyInfo = loader.importMorphology(morphologyProps, uri, model, i,
                                                 transformations[i], nullptr,
                                                 nullptr, compartmentReport);

        // End indices after adding the morphology
        const size_t endSpheres = model.getSpheres()[id].size();
        const size_t endCones = model.getCones()[id].size();
        const size_t endCylinders = model.getCylinders()[id].size();
        const size_t endSDFGeoms = model.getSDFGeometryData().geometries.size();
        const size_t endSDFBeziers = model.getSDFBeziers()[id].size();

        // Map morphology
        MorphologyMap newMap;
        func(newMap._sphereMap,      id,    startSpheres,    endSpheres);
        func(newMap._coneMap,        id,    startCones,      endCones);
        func(newMap._cylinderMap,    id,    startCylinders,  endCylinders);
        func(newMap._sdfGeometryMap, id,    startSDFGeoms,   endSDFGeoms);
        func(newMap._sdfBezierMap,   id,    startSDFBeziers, endSDFBeziers);
        newMap._linealIndex = i;
        mapper.add(gid, newMap);

        maxDistanceToSoma =
            std::max(morphologyInfo.maxDistanceToSoma, maxDistanceToSoma);

        callback.updateProgress("Loading morphologies...",
                 static_cast<float>(i) / static_cast<float>(uris.size()));

        ++i;
    }


    // Synapses
    const bool loadAfferentSynapses =
        properties.getProperty<bool>(PROP_LOAD_AFFERENT_SYNAPSES.name);
    const bool loadEfferentSynapses =
        properties.getProperty<bool>(PROP_LOAD_EFFERENT_SYNAPSES.name);
    const double synapseRadius =
        properties.getProperty<double>(PROP_SYNAPSE_RADIUS.name);
    const std::string preSynapticGID =
        properties.getProperty<std::string>(PROP_PRESYNAPTIC_NEURON_GID.name);
    const std::string postSynapticGID =
        properties.getProperty<std::string>(PROP_POSTSYNAPTIC_NEURON_GID.name);
    if (!preSynapticGID.empty() && !postSynapticGID.empty())
        _loadPairSynapses(properties, circuit, stoi(preSynapticGID),
                          stoi(postSynapticGID), synapseRadius, model);
    else
        _loadAllSynapses(properties, circuit, gids, static_cast<float>(synapseRadius),
                         loadAfferentSynapses, loadEfferentSynapses, model, compartmentReport);

    PLUGIN_TIMER(chrono.elapsed(), "Loading of " << gids.size() << " cells");
    return maxDistanceToSoma;
}

void AbstractCircuitLoader::_loadPairSynapses(
    const brayns::PropertyMap &properties, const brain::Circuit &circuit,
    const uint32_t &preGid, const uint32_t &postGid, const float synapseRadius,
    brayns::Model &model) const
{
    PLUGIN_INFO << "Loading pair Synapses (" << preGid << " -> " << postGid
                << ")" << std::endl;
    const brain::Synapses &postAfferentSynapses(
        circuit.getAfferentSynapses({postGid}));

    size_t materialId =
        _getMaterialFromCircuitAttributes(properties, 2, brayns::NO_MATERIAL,
                                          false);
    for (const auto &synapse : postAfferentSynapses)
    {
        const auto gid = synapse.getPresynapticGID();
        if (gid == preGid)
            _buildAfferentSynapses(synapse, materialId, synapseRadius, model);
    }
}

void AbstractCircuitLoader::_loadAllSynapses(
    const brayns::PropertyMap &properties, const brain::Circuit &circuit,
    const brain::GIDSet &gids, const float synapseRadius,
    const bool loadAfferentSynapses, const bool loadEfferentSynapses,
    brayns::Model &model, CompartmentReportPtr compartmentReport) const
{
    uint64_t i = 0;

    auto uris = circuit.getMorphologyURIs(gids);
    if(compartmentReport)
    {
        for (const auto &gid : gids)
        {
            const size_t id =
                _getMaterialFromCircuitAttributes(properties, i,
                                                  brayns::NO_MATERIAL, false);

            const auto& offsets =
                compartmentReport->getOffsets()[i];
            const auto& counts =
                compartmentReport->getCompartmentCounts()[i];

            brain::neuron::Morphology morphology(uris[i]);


            if (loadAfferentSynapses)
            {
                const brain::Synapses &afferentSynapses(
                    circuit.getAfferentSynapses({gid}));
                for (const brain::Synapse &synapse : afferentSynapses)
                {
                    uint64_t userDataOffset = i;
                    auto sectionId = synapse.getPostsynapticSectionID();
                    if (sectionId < counts.size() && counts[sectionId] > 0)
                    {
                        auto sections = morphology.getSections({brain::neuron::SectionType::apicalDendrite,
                                                                brain::neuron::SectionType::dendrite,
                                                                brain::neuron::SectionType::axon});
                        brain::neuron::Section* section = nullptr;
                        for(auto& s : sections)
                        {
                            if(s.getID() == sectionId)
                            {
                                section = &s;
                                break;
                            }
                        }

                        if (section)
                        {
                            auto samples = section->getSamples();
                            uint64_t j = 0, chosen = 0;
                            double closer = 99999999.9;
                            for(const auto& sample : samples)
                            {
                                glm::vec3 v3sample (sample);
                                auto dist = glm::length(v3sample - synapse.getPostsynapticSurfacePosition());
                                if(dist < closer)
                                {
                                    closer = dist;
                                    chosen = j;
                                }
                                ++j;
                            }

                            const auto segCounts = counts[sectionId];
                            const auto alpha = static_cast<double>(chosen) / static_cast<double>(segCounts);
                            const auto extra = static_cast<uint64_t>(floor(segCounts * alpha));

                            userDataOffset = offsets[sectionId] + extra;
                        }
                    }

                    _buildAfferentSynapses(synapse, id + 1, synapseRadius, model, userDataOffset);
                }
            }

            if (loadEfferentSynapses)
            {
                const brain::Synapses &efferentSynapses(
                    circuit.getEfferentSynapses({gid}));
                for (const brain::Synapse &synapse : efferentSynapses)
                {
                    uint64_t userDataOffset = i;
                    const auto sectionId = synapse.getPresynapticSectionID();
                    if (sectionId < counts.size() && counts[sectionId] > 0)
                    {
                        auto sections = morphology.getSections({brain::neuron::SectionType::apicalDendrite,
                                                                brain::neuron::SectionType::dendrite,
                                                                brain::neuron::SectionType::axon});
                        brain::neuron::Section* section = nullptr;
                        for(auto& s : sections)
                        {
                            if(s.getID() == sectionId)
                            {
                                section = &s;
                                break;
                            }
                        }

                        if (section)
                        {
                            const auto samples = section->getSamples();
                            uint64_t j = 0, chosen = 0;
                            double closer = 99999999.9;
                            for(const auto& sample : samples)
                            {
                                glm::vec3 v3sample (sample);
                                auto dist = glm::length(v3sample - synapse.getPresynapticSurfacePosition());
                                if(dist < closer)
                                {
                                    closer = dist;
                                    chosen = j;
                                }
                                ++j;
                            }

                            const auto segCounts = counts[sectionId];
                            const auto alpha = static_cast<double>(chosen) / static_cast<double>(segCounts);
                            const auto extra = static_cast<uint64_t>(floor(segCounts * alpha));

                            userDataOffset = offsets[sectionId] + extra;
                        }
                    }
                    _buildEfferentSynapses(synapse, id + 2, synapseRadius, model, userDataOffset);
                }
            }
            ++i;
        }
    }
    else
    {
        for (const auto &gid : gids)
        {
            const size_t id =
                _getMaterialFromCircuitAttributes(properties, i,
                                                  brayns::NO_MATERIAL, false);

            if (loadAfferentSynapses)
            {
                const brain::Synapses &afferentSynapses(
                    circuit.getAfferentSynapses({gid}));
                for (const brain::Synapse &synapse : afferentSynapses)
                    _buildAfferentSynapses(synapse, id + 1, synapseRadius, model);
            }

            if (loadEfferentSynapses)
            {
                const brain::Synapses &efferentSynapses(
                    circuit.getEfferentSynapses({gid}));
                for (const brain::Synapse &synapse : efferentSynapses)
                    _buildEfferentSynapses(synapse, id + 2, synapseRadius, model);
            }
            ++i;
        }
    }
}

void AbstractCircuitLoader::_buildAfferentSynapses(
    const brain::Synapse &synapse, const size_t materialId, const float radius,
    brayns::Model &model, const uint64_t userData) const
{
    model.addSphere(materialId, {synapse.getPostsynapticSurfacePosition(), radius, userData});
}

void AbstractCircuitLoader::_buildEfferentSynapses(
    const brain::Synapse &synapse, const size_t materialId, const float radius,
    brayns::Model &model, const uint64_t userData) const
{
    model.addSphere(materialId, {synapse.getPresynapticSurfacePosition(), radius, userData});
}

std::vector<brayns::ModelDescriptorPtr> AbstractCircuitLoader::importFromBlob(
    brayns::Blob && /*blob*/, const brayns::LoaderProgress & /*callback*/,
    const brayns::PropertyMap & /*properties*/) const
{
    PLUGIN_THROW("Load circuit from memory not supported");
}

brayns::PropertyMap AbstractCircuitLoader::getProperties() const
{
    return _defaults;
}
