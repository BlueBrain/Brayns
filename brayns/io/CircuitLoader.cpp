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

#include <brayns/common/scene/Scene.h>
#include <brayns/io/simulation/CircuitSimulationHandler.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <brayns/io/MorphologyLoader.h>
#if (BRAYNS_USE_ASSIMP)
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

    bool importCircuit(const std::string& source, const strings& targets,
                       const std::string& report, Scene& scene)
    {
        _materialsOffset = scene.getMaterials().size();

        bool returnValue = true;
        try
        {
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
                const auto targetGids =
                    (target.empty()
                         ? circuit.getRandomGIDs(circuitDensity)
                         : circuit.getRandomGIDs(circuitDensity, target));
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
                        if (aabb.isIn(allTransformations[i].getTranslation()))
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
                return false;

            // Load simulation information from compartment report
            CompartmentReportPtr compartmentReport = nullptr;
            if (!report.empty())
                try
                {
                    CircuitSimulationHandlerPtr simulationHandler(
                        new CircuitSimulationHandler(_applicationParameters,
                                                     _geometryParameters,
                                                     bc.getReportSource(report),
                                                     allGids));
                    compartmentReport =
                        simulationHandler->getCompartmentReport();
                    // Only keep simulated GIDs
                    if (compartmentReport)
                        allGids = compartmentReport->getGIDs();
                    // Attach simulation handler
                    scene.setSimulationHandler(simulationHandler);
                }
                catch (const std::exception& e)
                {
                    BRAYNS_ERROR << e.what() << std::endl;
                }

            if (!_geometryParameters.getLoadCacheFile().empty())
                return true;

            const Matrix4fs& transformations = circuit.getTransforms(allGids);
            _logLoadedGIDs(allGids);

            _populateLayerIds(bc, allGids);
            _electrophysiologyTypes =
                circuit.getElectrophysiologyTypes(allGids);
            _morphologyTypes = circuit.getMorphologyTypes(allGids);

            // Import meshes
            returnValue =
                returnValue && _importMeshes(allGids, scene, transformations,
                                             targetGIDOffsets);

            // Import morphologies
            if (_geometryParameters.getCircuitMeshFolder().empty() ||
                _geometryParameters.getCircuitUseSimulationModel())
            {
                MorphologyLoader morphLoader(_geometryParameters);
                returnValue =
                    returnValue &&
                    _importMorphologies(circuit, scene, allGids,
                                        transformations, targetGIDOffsets,
                                        compartmentReport, morphLoader);
            }
        }
        catch (const std::exception& error)
        {
            BRAYNS_ERROR << "Failed to open " << source << ": " << error.what()
                         << std::endl;
            return false;
        }

        return returnValue;
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
        const GIDOffsets& targetGIDOffsets, bool isMesh = false) const
    {
        if (material != NO_MATERIAL)
            return _materialsOffset + material;

        if (!isMesh && _geometryParameters.getCircuitUseSimulationModel())
            return _materialsOffset;

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
            if (index < _electrophysiologyTypes.size())
                materialId = _electrophysiologyTypes[index];
            else
                BRAYNS_DEBUG << "Failed to get neuron E-type" << std::endl;
            break;
        case ColorScheme::neuron_by_mtype:
            if (index < _morphologyTypes.size())
                materialId = _morphologyTypes[index];
            else
                BRAYNS_DEBUG << "Failed to get neuron M-type" << std::endl;
            break;
        case ColorScheme::neuron_by_layer:
            if (index < _layerIds.size())
                materialId = _layerIds[index];
            else
                BRAYNS_DEBUG << "Failed to get neuron layer" << std::endl;
            break;
        default:
            materialId = NO_MATERIAL;
        }
        return _materialsOffset + materialId;
    }

    /**
     * @brief _populateLayerIds populates the neuron layer IDs. This is
     * currently only supported for the MVD2 format.
     * @param blueConfig Configuration of the circuit
     * @param gids GIDs of the neurons
     */
    void _populateLayerIds(const brion::BlueConfig& blueConfig,
                           const brain::GIDSet& gids)
    {
        _layerIds.clear();
        try
        {
            brion::Circuit brionCircuit(blueConfig.getCircuitSource());
            for (const auto& a : brionCircuit.get(gids, brion::NEURON_LAYER))
                _layerIds.push_back(std::stoi(a[0]));
        }
        catch (...)
        {
            BRAYNS_WARN << "Only MVD2 format is currently supported by Brion "
                           "circuits. Color scheme by layer not available for "
                           "this circuit"
                        << std::endl;
        }
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

#if (BRAYNS_USE_ASSIMP)
    bool _importMeshes(const brain::GIDSet& gids, Scene& scene,
                       const Matrix4fs& transformations,
                       const GIDOffsets& targetGIDOffsets)
    {
        MeshLoader meshLoader(_geometryParameters);
        size_t loadingFailures = 0;
        const auto meshedMorphologiesFolder =
            _geometryParameters.getCircuitMeshFolder();
        if (meshedMorphologiesFolder.empty())
            return true;

        uint64_t meshIndex = 0;
        // Loading meshes is currently sequential. TODO: Make it parallel!!!
        std::stringstream message;
        message << "Loading " << gids.size() << " meshes...";
        for (const auto& gid : gids)
        {
            const size_t materialId = _getMaterialFromGeometryParameters(
                meshIndex, NO_MATERIAL, brain::neuron::SectionType::undefined,
                targetGIDOffsets, true);

            // Load mesh from file
            const auto transformation =
                _geometryParameters.getCircuitMeshTransformation()
                    ? transformations[meshIndex]
                    : Matrix4f();
            try
            {
                meshLoader.importFromFile(meshLoader.getMeshFilenameFromGID(
                                              gid),
                                          scene, transformation, materialId);
            }
            catch (...)
            {
                ++loadingFailures;
            }
            ++meshIndex;
            _parent.updateProgress(message.str(), meshIndex, gids.size());
        }
        if (loadingFailures != 0)
            BRAYNS_WARN << "Failed to import " << loadingFailures << " meshes"
                        << std::endl;
        return true;
    }
#else
    bool _importMeshes(const brain::GIDSet&, Scene&, const Matrix4fs&,
                       const GIDOffsets&)
    {
        BRAYNS_ERROR << "assimp dependency is required to load meshes"
                     << std::endl;
        return false;
    }
#endif

    bool _importMorphologies(const brain::Circuit& circuit, Scene& scene,
                             const brain::GIDSet& gids,
                             const Matrix4fs& transformations,
                             const GIDOffsets& targetGIDOffsets,
                             CompartmentReportPtr compartmentReport,
                             MorphologyLoader& morphLoader)
    {
        const brain::URIs& uris = circuit.getMorphologyURIs(gids);
        size_t loadingFailures = 0;
        std::stringstream message;
        message << "Loading " << uris.size() << " morphologies...";
        std::atomic_size_t current{0};
#pragma omp parallel
        {
#pragma omp for nowait
            for (uint64_t morphologyIndex = 0; morphologyIndex < uris.size();
                 ++morphologyIndex)
            {
                ++current;
                _parent.updateProgress(message.str(), current, uris.size());

                SpheresMap spheres;
                CylindersMap cylinders;
                ConesMap cones;
                TrianglesMeshMap triangleMeshes;
                Materials materials;
                Boxf bounds;
                ParallelSceneContainer sceneContainer(spheres, cylinders, cones,
                                                      triangleMeshes, materials,
                                                      bounds);
                const auto& uri = uris[morphologyIndex];

                if (!morphLoader._importMorphology(
                        uri, morphologyIndex,
                        std::bind(&Impl::_getMaterialFromGeometryParameters,
                                  this, morphologyIndex, NO_MATERIAL,
                                  std::placeholders::_1, targetGIDOffsets,
                                  false),
                        transformations[morphologyIndex], compartmentReport,
                        sceneContainer))
#pragma omp atomic
                    ++loadingFailures;

#pragma omp critical
                for (size_t i = 0; i < materials.size(); ++i)
                    scene.setMaterial(i, materials[i]);

#pragma omp critical
                for (const auto& sphere : spheres)
                {
                    const auto id = sphere.first;
                    scene.getSpheres()[id].insert(
                        scene.getSpheres()[id].end(),
                        sceneContainer.spheres[id].begin(),
                        sceneContainer.spheres[id].end());
                }

#pragma omp critical
                for (const auto& cylinder : cylinders)
                {
                    const auto id = cylinder.first;
                    scene.getCylinders()[id].insert(
                        scene.getCylinders()[id].end(),
                        sceneContainer.cylinders[id].begin(),
                        sceneContainer.cylinders[id].end());
                }

#pragma omp critical
                for (const auto& cone : cones)
                {
                    const auto id = cone.first;
                    scene.getCones()[id].insert(
                        scene.getCones()[id].end(),
                        sceneContainer.cones[id].begin(),
                        sceneContainer.cones[id].end());
                }

#pragma omp critical
                scene.getWorldBounds().merge(bounds);
            }
        }

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
    size_ts _layerIds;
    size_ts _electrophysiologyTypes;
    size_ts _morphologyTypes;
    size_t _materialsOffset;
};

CircuitLoader::CircuitLoader(const ApplicationParameters& applicationParameters,
                             const GeometryParameters& geometryParameters)
    : _impl(new CircuitLoader::Impl(applicationParameters, geometryParameters,
                                    *this))
{
}

CircuitLoader::~CircuitLoader()
{
}

std::set<std::string> CircuitLoader::getSupportedDataTypes()
{
    return {"BlueConfig", "BlueConfig3", "CircuitConfig", "circuit"};
}

void CircuitLoader::importFromBlob(Blob&& /*blob*/, Scene& /*scene*/,
                                   const Matrix4f& /*transformation*/,
                                   const size_t /*materialID*/)
{
    throw std::runtime_error("Loading circuit from blob is not supported");
}

void CircuitLoader::importFromFile(const std::string& filename, Scene& scene,
                                   const Matrix4f& /*transformation*/,
                                   const size_t /*materialID*/)
{
    _impl->importCircuit(filename, {}, "", scene);
}

bool CircuitLoader::importCircuit(const servus::URI& uri,
                                  const strings& targets,
                                  const std::string& report, Scene& scene)
{
    return _impl->importCircuit(uri.getPath(), targets, report, scene);
}
}
