/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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
#include "circuitLoaderCommon.h"

#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/io/simulation/CircuitSimulationHandler.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <brayns/io/MorphologyLoader.h>
#if BRAYNS_USE_ASSIMP
#include <brayns/io/MeshLoader.h>
#endif

namespace brayns
{
class CircuitLoader::Impl
{
public:
    Impl(const ApplicationParameters& applicationParameters,
         const GeometryParameters& geometryParameters, CircuitLoader& parent)
        : _parent(parent)
        , _applicationParameters(applicationParameters)
        , _geometryParameters(geometryParameters)
    {
    }

    ModelDescriptorPtr importCircuit(const std::string& source,
                                     const LoaderProgress& callback,
                                     const strings& targets,
                                     const std::string& report) const
    {
        bool returnValue = true;
        ModelDescriptorPtr modelDesc;
        try
        {
            // Model (one for the whole circuit)
            ModelMetadata metadata = {
                {"density",
                 std::to_string(_geometryParameters.getCircuitDensity())},
                {"report", _geometryParameters.getCircuitReport()},
                {"targets", _geometryParameters.getCircuitTargets()},
                {"mesh-filename-pattern",
                 _geometryParameters.getCircuitMeshFilenamePattern()},
                {"mesh-folder", _geometryParameters.getCircuitMeshFolder()}};
            auto model = _parent._scene.createModel();

            // Open Circuit and select GIDs according to specified target
            const brion::BlueConfig bc(source);
            const brain::Circuit circuit(bc);
            const auto circuitDensity =
                _geometryParameters.getCircuitDensity() / 100.f;

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
                const auto targetGids = circuit.getRandomGIDs(
                    circuitDensity, target,
                    _geometryParameters.getCircuitRandomSeed());
                const Matrix4fs& allTransformations =
                    circuit.getTransforms(targetGids);

                brain::GIDSet gids;
                const auto& aabb = _geometryParameters.getCircuitBoundingBox();
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
                    auto handler = std::make_shared<CircuitSimulationHandler>(
                        _applicationParameters, _geometryParameters,
                        bc.getReportSource(report), allGids);
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

            if (!_geometryParameters.getLoadCacheFile().empty())
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
                _geometryParameters.getCircuitUseSimulationModel();
            model->useSimulationModel(useSimulationModel);
            if (_geometryParameters.getCircuitMeshFolder().empty() ||
                useSimulationModel)
            {
                MorphologyLoader morphLoader(_parent._scene,
                                             _geometryParameters);
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

    const GeometryParameters& geometryParameters() const
    {
        return _geometryParameters;
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
        bool isMesh = false) const
    {
        if (material != NO_MATERIAL)
            return material;

        if (!isMesh && _geometryParameters.getCircuitUseSimulationModel())
            return 0;

        size_t materialId = 0;
        switch (_geometryParameters.getColorScheme())
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
            if (_geometryParameters.getColorScheme() ==
                ColorScheme::neuron_by_layer)
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
        MeshLoader meshLoader(_parent._scene, _geometryParameters);
        size_t loadingFailures = 0;
        const auto meshedMorphologiesFolder =
            _geometryParameters.getCircuitMeshFolder();
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
            const auto transformation =
                _geometryParameters.getCircuitMeshTransformation()
                    ? transformations[meshIndex]
                    : Matrix4f();
            try
            {
                meshLoader.importMesh(meshLoader.getMeshFilenameFromGID(gid),
                                      callback, model, meshIndex,
                                      transformation, materialId);
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

                    if (!morphLoader._importMorphology(
                            uri, morphologyIndex,
                            std::bind(&Impl::_getMaterialFromGeometryParameters,
                                      this, morphologyIndex, NO_MATERIAL,
                                      std::placeholders::_1, targetGIDOffsets,
                                      layerIds, morphologyTypes,
                                      electrophysiologyTypes, false),
                            transformations[morphologyIndex], compartmentReport,
                            modelContainer))
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
    CircuitLoader& _parent;
    const ApplicationParameters& _applicationParameters;
    const GeometryParameters& _geometryParameters;
};

CircuitLoader::CircuitLoader(Scene& scene,
                             const ApplicationParameters& applicationParameters,
                             const GeometryParameters& geometryParameters)
    : Loader(scene)
    , _impl(new CircuitLoader::Impl(applicationParameters, geometryParameters,
                                    *this))
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
    Blob&& /*blob*/, const LoaderProgress& /*callback*/, const size_t /*index*/,
    const size_t /*materialID*/) const
{
    throw std::runtime_error("Loading circuit from blob is not supported");
}

ModelDescriptorPtr CircuitLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    const size_t /*index*/, const size_t /*materialID*/) const
{
    return _impl->importCircuit(
        filename, callback,
        _impl->geometryParameters().getCircuitTargetsAsStrings(),
        _impl->geometryParameters().getCircuitReport());
}

ModelDescriptorPtr CircuitLoader::importCircuit(const servus::URI& uri,
                                                const LoaderProgress& callback,
                                                const strings& targets,
                                                const std::string& report)
{
    return _impl->importCircuit(uri.getPath(), callback, targets, report);
}
}
