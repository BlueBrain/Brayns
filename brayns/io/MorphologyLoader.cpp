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

#include "MorphologyLoader.h"

#include <brayns/common/geometry/GeometryGroup.h>
#include <brayns/common/log.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/io/algorithms/MetaballsGenerator.h>
#include <brayns/io/simulation/CircuitSimulationHandler.h>

#include <brain/brain.h>
#include <brion/brion.h>
#include <servus/types.h>

#if (BRAYNS_USE_ASSIMP)
#include <brayns/io/MeshLoader.h>
#endif

#include <algorithm>
#include <fstream>

#include <boost/filesystem.hpp>

namespace
{
// needs to be the same in SimulationRenderer.ispc
const float INDEX_MAGIC = 1e6;
}

namespace brayns
{
typedef std::vector<uint64_t> GIDOffsets;

struct ParallelGeometryGroupContainer
{
public:
    ParallelGeometryGroupContainer(SpheresMap& s, CylindersMap& cy,
                                   ConesMap& co, TrianglesMeshMap& tm, Boxf& wb)
        : spheres(s)
        , cylinders(cy)
        , cones(co)
        , trianglesMeshes(tm)
        , bounds(wb)
    {
    }

    void addSphere(const size_t materialId, const Sphere& sphere)
    {
        setMaterial(materialId);
        spheres[materialId].push_back(sphere);
        bounds.merge(sphere.center);
    }

    void addCylinder(const size_t materialId, const Cylinder& cylinder)
    {
        setMaterial(materialId);
        cylinders[materialId].push_back(cylinder);
        bounds.merge(cylinder.center);
        bounds.merge(cylinder.up);
    }

    void addCone(const size_t materialId, const Cone& cone)
    {
        setMaterial(materialId);
        cones[materialId].push_back(cone);
        bounds.merge(cone.center);
        bounds.merge(cone.up);
    }

    void setMaterial(const size_t materialId)
    {
        const auto iter =
            std::find(materialIds.begin(), materialIds.end(), materialId);
        if (iter == materialIds.end())
            materialIds.push_back(materialId);
    }

    size_t materialIndex(const size_t materialId)
    {
        const auto it =
            std::find(materialIds.begin(), materialIds.end(), materialId);
        const auto distance = std::distance(materialIds.begin(), it);
        return distance;
    }

    SpheresMap& spheres;
    CylindersMap& cylinders;
    ConesMap& cones;
    TrianglesMeshMap& trianglesMeshes;
    std::vector<size_t> materialIds;
    Boxf& bounds;
};

class MorphologyLoader::Impl
{
public:
    Impl(const ApplicationParameters& applicationParameters,
         const GeometryParameters& geometryParameters, const size_t nbMaterials,
         MorphologyLoader& parent)
        : _parent(parent)
        , _applicationParameters(applicationParameters)
        , _geometryParameters(geometryParameters)
        , _materialsOffset(nbMaterials)
    {
    }

    /**
     * @brief importMorphology imports a single morphology from a specified URI
     * @param uri URI of the morphology
     * @param index Index of the morphology
     * @param material Material to use
     * @param transformation Transformation to apply to the morphology
     * @param compartmentReport Compartment report to map to the morphology
     * @return True is the morphology was successfully imported, false otherwise
     */
    bool importMorphology(const servus::URI& source, const uint64_t index,
                          const size_t material, const Matrix4f& transformation,
                          GeometryGroup& group,
                          const GIDOffsets& targetGIDOffsets,
                          CompartmentReportPtr compartmentReport = nullptr)
    {
        ParallelGeometryGroupContainer groupContainer(
            group.getSpheres(), group.getCylinders(), group.getCones(),
            group.getTrianglesMeshes(), group.getBounds());

        return _importMorphology(source, index, material, transformation,
                                 compartmentReport, targetGIDOffsets,
                                 groupContainer);
    }

    /**
     * @brief importCircuit Imports a circuit from a specified URI
     * @param uri URI of the CircuitConfig
     * @param target Target to load
     * @param report Report to load
     * @param meshLoader Mesh loader used to load meshes
     * @return True is the circuit was successfully imported, false otherwise
     */
    bool importCircuit(const servus::URI& uri, const strings& targets,
                       const std::string& report, Scene& scene,
                       MeshLoader& meshLoader)
    {
        bool returnValue = true;
        try
        {
            // Geometry group (one for the whole circuit)
            GeometryGroup group;

            // Open Circuit and select GIDs according to specified target
            const brain::Circuit circuit(uri);
            const brion::BlueConfig bc(uri.getPath());
            const auto circuitDensity =
                _geometryParameters.getCircuitDensity() / 100.f;

            brain::GIDSet allGids;
            GIDOffsets targetGIDOffsets;
            targetGIDOffsets.push_back(0);

            strings localTargets;
            if (targets.empty())
                localTargets.push_back("");
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
                returnValue && _importMeshes(allGids, transformations, group,
                                             targetGIDOffsets, meshLoader);

            // Import morphologies
            if (_geometryParameters.getCircuitMeshFolder().empty() ||
                _geometryParameters.getCircuitUseSimulationModel())
                returnValue =
                    returnValue &&
                    _importMorphologies(circuit, allGids, transformations,
                                        group, targetGIDOffsets,
                                        compartmentReport);

            scene.addGeometryGroup(group);
        }
        catch (const std::exception& error)
        {
            BRAYNS_ERROR << "Failed to open " << uri.getPath() << ": "
                         << error.what() << std::endl;
            return false;
        }

        return returnValue;
    }

private:
    /**
     * @brief _getCorrectedRadius Modifies the radius of the geometry according
     * to --radius-multiplier and --radius-correction geometry parameters
     * @param radius Radius to be corrected
     * @return Corrected value of a radius according to geometry parameters
     */
    float _getCorrectedRadius(const float radius) const
    {
        return (_geometryParameters.getRadiusCorrection() != 0.f
                    ? _geometryParameters.getRadiusCorrection()
                    : radius * _geometryParameters.getRadiusMultiplier());
    }

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
     * @brief _getSectionTypes converts Brayns section types into brain::neuron
     * section types
     * @param morphologySectionTypes Brayns section types
     * @return brain::neuron section types
     */
    brain::neuron::SectionTypes _getSectionTypes(
        const size_t morphologySectionTypes) const
    {
        brain::neuron::SectionTypes sectionTypes;
        if (morphologySectionTypes &
            static_cast<size_t>(MorphologySectionType::soma))
            sectionTypes.push_back(brain::neuron::SectionType::soma);
        if (morphologySectionTypes &
            static_cast<size_t>(MorphologySectionType::axon))
            sectionTypes.push_back(brain::neuron::SectionType::axon);
        if (morphologySectionTypes &
            static_cast<size_t>(MorphologySectionType::dendrite))
            sectionTypes.push_back(brain::neuron::SectionType::dendrite);
        if (morphologySectionTypes &
            static_cast<size_t>(MorphologySectionType::apical_dendrite))
            sectionTypes.push_back(brain::neuron::SectionType::apicalDendrite);
        return sectionTypes;
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
     * @brief _getIndexAsTextureCoordinates converts a uint64_t index into 2
     * floats so that it can be stored in the texture coordinates of the the
     * geometry to which it is attached
     * @param index Index to be stored in texture coordinates
     * @return Texture coordinates for the given index
     */
    Vector2f _getIndexAsTextureCoordinates(const uint64_t index) const
    {
        Vector2f textureCoordinates;

        // https://stackoverflow.com/questions/2810280
        float x = ((index & 0xFFFFFFFF00000000LL) >> 32) / INDEX_MAGIC;
        float y = (index & 0xFFFFFFFFLL) / INDEX_MAGIC;

        textureCoordinates.x() = x;
        textureCoordinates.y() = y;
        return textureCoordinates;
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

    /**
     * @brief _importMorphologyAsPoint places sphere at the specified morphology
     * position
     * @param index Index of the current morphology
     * @param transformation Transformation to apply to the morphology
     * @param material Material that is forced in case geometry parameters do
     * not apply
     * @param compartmentReport Compartment report to map to the morphology
     * @param scene Scene to which the morphology should be loaded into
     * @return True if the loading was successful, false otherwise
     */
    bool _importMorphologyAsPoint(const uint64_t index, const size_t material,
                                  const Matrix4f& transformation,
                                  CompartmentReportPtr compartmentReport,
                                  const GIDOffsets& targetGIDOffsets,
                                  ParallelGeometryGroupContainer& group)
    {
        uint64_t offset = 0;
        if (compartmentReport)
            offset = compartmentReport->getOffsets()[index][0];

        const auto radius = _geometryParameters.getRadiusMultiplier();
        const auto textureCoordinates = _getIndexAsTextureCoordinates(offset);
        const auto somaPosition = transformation.getTranslation();
        const auto materialId =
            _getMaterialFromGeometryParameters(index, material,
                                               brain::neuron::SectionType::soma,
                                               targetGIDOffsets);
        group.addSphere(materialId,
                        {somaPosition, radius, 0.f, textureCoordinates});
        return true;
    }

    /**
     * @brief _createRealisticSoma Creates a realistic soma using the metaballs
     * algorithm.
     * @param uri URI of the morphology for which the soma is created
     * @param index Index of the current morphology
     * @param transformation Transformation to apply to the morphology
     * @param material Material that is forced in case geometry parameters
     * do not apply
     * @param scene Scene to which the morphology should be loaded into
     * @return True if the loading was successful, false otherwise
     */
    bool _createRealisticSoma(const servus::URI& uri, const uint64_t index,
                              const size_t material,
                              const Matrix4f& transformation,
                              const GIDOffsets& targetGIDOffsets,
                              ParallelGeometryGroupContainer& group)
    {
        try
        {
            const size_t morphologySectionTypes =
                enumsToBitmask(_geometryParameters.getMorphologySectionTypes());

            brain::neuron::Morphology morphology(uri, transformation);
            const auto sectionTypes = _getSectionTypes(morphologySectionTypes);
            const auto& sections = morphology.getSections(sectionTypes);

            Vector4fs metaballs;
            if (morphologySectionTypes & size_t(MorphologySectionType::soma))
            {
                // Soma
                const auto& soma = morphology.getSoma();
                const auto center = soma.getCentroid();
                const auto radius = _getCorrectedRadius(soma.getMeanRadius());
                metaballs.push_back(
                    Vector4f(center.x(), center.y(), center.z(), radius));
                group.bounds.merge(center);
            }

            // Dendrites and axon
            for (const auto& section : sections)
            {
                const auto hasParent = section.hasParent();
                if (hasParent)
                {
                    const auto parentSectionType =
                        section.getParent().getType();
                    if (parentSectionType != brain::neuron::SectionType::soma)
                        continue;
                }

                const auto& samples = section.getSamples();
                if (samples.empty())
                    continue;

                const auto samplesFromSoma =
                    _geometryParameters.getMetaballsSamplesFromSoma();
                const auto samplesToProcess =
                    std::min(samplesFromSoma, samples.size());
                for (size_t i = 0; i < samplesToProcess; ++i)
                {
                    const auto& sample = samples[i];
                    const Vector3f position(sample.x(), sample.y(), sample.z());
                    const auto radius = _getCorrectedRadius(sample.w() * 0.5f);
                    if (radius > 0.f)
                        metaballs.push_back(Vector4f(position.x(), position.y(),
                                                     position.z(), radius));

                    group.bounds.merge(position);
                }
            }

            // Generate mesh from metaballs
            const auto gridSize = _geometryParameters.getMetaballsGridSize();
            const auto threshold = _geometryParameters.getMetaballsThreshold();
            MetaballsGenerator metaballsGenerator;
            const auto materialId = _getMaterialFromGeometryParameters(
                index, material, brain::neuron::SectionType::soma,
                targetGIDOffsets);
            metaballsGenerator.generateMesh(metaballs, gridSize, threshold,
                                            materialId, group.trianglesMeshes);
        }
        catch (const std::runtime_error& e)
        {
            BRAYNS_ERROR << e.what() << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief _importMorphologyFromURI imports a morphology from the specified
     * URI
     * @param uri URI of the morphology
     * @param index Index of the current morphology
     * @param transformation Transformation to apply to the morphology
     * @param material Material that is forced in case geometry parameters
     * do not apply
     * @param compartmentReport Compartment report to map to the morphology
     * @param scene Scene to which the morphology should be loaded into
     * @return True if the loading was successful, false otherwise
     */
    bool _importMorphologyFromURI(const servus::URI& uri, const uint64_t index,
                                  const size_t material,
                                  const Matrix4f& transformation,
                                  CompartmentReportPtr compartmentReport,
                                  const GIDOffsets& targetGIDOffsets,
                                  ParallelGeometryGroupContainer& group) const
    {
        try
        {
            Vector3f translation;

            const size_t morphologySectionTypes =
                enumsToBitmask(_geometryParameters.getMorphologySectionTypes());

            brain::neuron::Morphology morphology(uri, transformation);
            brain::neuron::SectionTypes sectionTypes;

            const MorphologyLayout& layout =
                _geometryParameters.getMorphologyLayout();

            if (layout.nbColumns != 0)
            {
                Boxf morphologyAABB;
                const auto& points = morphology.getPoints();
                for (const auto& point : points)
                    morphologyAABB.merge({point.x(), point.y(), point.z()});

                const Vector3f positionInGrid = {
                    -1.f * layout.horizontalSpacing *
                        static_cast<float>(index % layout.nbColumns),
                    -1.f * layout.verticalSpacing *
                        static_cast<float>(index / layout.nbColumns),
                    0.f};
                translation = positionInGrid - morphologyAABB.getCenter();
            }

            sectionTypes = _getSectionTypes(morphologySectionTypes);

            uint64_t offset = 0;

            if (compartmentReport)
                offset = compartmentReport->getOffsets()[index][0];

            // Soma
            if (!_geometryParameters.useRealisticSomas() &&
                morphologySectionTypes &
                    static_cast<size_t>(MorphologySectionType::soma))
            {
                const auto& soma = morphology.getSoma();
                const size_t materialId = _getMaterialFromGeometryParameters(
                    index, material, brain::neuron::SectionType::soma,
                    targetGIDOffsets);
                const auto somaPosition = soma.getCentroid() + translation;
                const auto radius = _getCorrectedRadius(soma.getMeanRadius());
                const auto textureCoordinates =
                    _getIndexAsTextureCoordinates(offset);
                group.addSphere(materialId, {somaPosition, radius, 0.f,
                                             textureCoordinates});

                if (_geometryParameters.getCircuitUseSimulationModel())
                {
                    // When using a simulation model, parametric geometries must
                    // occupy as much space as possible in the mesh. This code
                    // inserts a Cone between the soma and the beginning of each
                    // branch.
                    const auto& children = soma.getChildren();
                    for (const auto& child : children)
                    {
                        const auto& samples = child.getSamples();
                        const Vector3f sample{samples[0].x(), samples[0].y(),
                                              samples[0].z()};
                        group.addCone(materialId, {somaPosition, sample, radius,
                                                   _getCorrectedRadius(
                                                       samples[0].w() * 0.5f),
                                                   0.f, textureCoordinates});
                    }
                }
            }

            // Only the first one or two axon sections are reported, so find the
            // last one and use its offset for all the other axon sections
            uint16_t lastAxon = 0;
            if (compartmentReport &&
                (morphologySectionTypes &
                 static_cast<size_t>(MorphologySectionType::axon)))
            {
                const auto& counts =
                    compartmentReport->getCompartmentCounts()[index];
                const auto& axon =
                    morphology.getSections(brain::neuron::SectionType::axon);
                for (const auto& section : axon)
                {
                    if (counts[section.getID()] > 0)
                    {
                        lastAxon = section.getID();
                        continue;
                    }
                    break;
                }
            }

            // Dendrites and axon
            for (const auto& section : morphology.getSections(sectionTypes))
            {
                if (section.getType() == brain::neuron::SectionType::soma)
                    continue;

                const auto materialId =
                    _getMaterialFromGeometryParameters(index, material,
                                                       section.getType(),
                                                       targetGIDOffsets);
                const auto& samples = section.getSamples();
                if (samples.empty())
                    continue;

                auto previousSample = samples[0];
                size_t step = 1;
                switch (_geometryParameters.getGeometryQuality())
                {
                case GeometryQuality::low:
                    step = samples.size() - 1;
                    break;
                case GeometryQuality::medium:
                    step = samples.size() / 2;
                    step = (step == 0) ? 1 : step;
                    break;
                default:
                    step = 1;
                }

                const float distanceToSoma = section.getDistanceToSoma();
                const floats& distancesToSoma =
                    section.getSampleDistancesToSoma();

                float segmentStep = 0.f;
                if (compartmentReport)
                {
                    const auto& counts =
                        compartmentReport->getCompartmentCounts()[index];
                    // Number of compartments usually differs from number of
                    // samples
                    segmentStep =
                        counts[section.getID()] / float(samples.size());
                }

                bool done = false;
                for (size_t i = step; !done && i < samples.size() + step;
                     i += step)
                {
                    if (i >= samples.size())
                    {
                        i = samples.size() - 1;
                        done = true;
                    }

                    const auto distance = distanceToSoma + distancesToSoma[i];

                    if (compartmentReport)
                    {
                        const auto& offsets =
                            compartmentReport->getOffsets()[index];
                        const auto& counts =
                            compartmentReport->getCompartmentCounts()[index];

                        // update the offset if we have enough compartments aka
                        // a full compartment report. Otherwise we keep the soma
                        // offset which happens for soma reports and use this
                        // for all the sections
                        if (section.getID() < counts.size())
                        {
                            if (counts[section.getID()] > 0)
                                offset = offsets[section.getID()] +
                                         float(i - step) * segmentStep;
                            else
                            {
                                if (section.getType() ==
                                    brain::neuron::SectionType::axon)
                                {
                                    offset = offsets[lastAxon];
                                }
                                else
                                    // This should never happen, but just in
                                    // case use an invalid value to show an
                                    // error color
                                    offset =
                                        std::numeric_limits<uint64_t>::max();
                            }
                        }
                    }

                    const auto sample = samples[i];
                    const auto previousRadius =
                        _getCorrectedRadius(samples[i - step].w() * 0.5f);

                    Vector3f position(sample.x(), sample.y(), sample.z());
                    position += translation;
                    Vector3f target(previousSample.x(), previousSample.y(),
                                    previousSample.z());
                    target += translation;
                    const auto textureCoordinates =
                        _getIndexAsTextureCoordinates(offset);
                    const auto radius =
                        _getCorrectedRadius(samples[i].w() * 0.5f);

                    if (radius > 0.f)
                    {
                        group.addSphere(materialId, {position, radius, distance,
                                                     textureCoordinates});

                        if (position != target && previousRadius > 0.f)
                        {
                            if (radius == previousRadius)
                                group.addCylinder(materialId,
                                                  {position, target, radius,
                                                   distance,
                                                   textureCoordinates});
                            else
                                group.addCone(materialId,
                                              {position, target, radius,
                                               previousRadius, distance,
                                               textureCoordinates});
                        }
                    }
                    previousSample = sample;
                }
            }
        }
        catch (const std::runtime_error& e)
        {
            BRAYNS_ERROR << e.what() << std::endl;
            return false;
        }
        return true;
    }

#if (BRAYNS_USE_ASSIMP)
    bool _importMeshes(const brain::GIDSet& gids,
                       const Matrix4fs& transformations, GeometryGroup& group,
                       const GIDOffsets& targetGIDOffsets,
                       MeshLoader& meshLoader)
    {
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
            if (!meshLoader.importMeshFromFile(
                    meshLoader.getMeshFilenameFromGID(gid), group,
                    transformation, materialId))
                ++loadingFailures;
            ++meshIndex;
            _parent.updateProgress(message.str(), meshIndex, gids.size());
        }
        if (loadingFailures != 0)
            BRAYNS_WARN << "Failed to import " << loadingFailures << " meshes"
                        << std::endl;
        return true;
    }
#else
    bool _importMeshes(const brain::GIDSet&, const Matrix4fs&,
                       const GIDOffsets&, MeshLoader&)
    {
        BRAYNS_ERROR << "assimp dependency is required to load meshes"
                     << std::endl;
        return false;
    }
#endif

    bool _importMorphology(const servus::URI& source, const uint64_t index,
                           const size_t material,
                           const Matrix4f& transformation,
                           CompartmentReportPtr compartmentReport,
                           const GIDOffsets& targetGIDOffsets,
                           ParallelGeometryGroupContainer& group)
    {
        bool returnValue = true;
        const size_t morphologySectionTypes =
            enumsToBitmask(_geometryParameters.getMorphologySectionTypes());
        if (morphologySectionTypes ==
            static_cast<size_t>(MorphologySectionType::soma))
            return _importMorphologyAsPoint(index, material, transformation,
                                            compartmentReport, targetGIDOffsets,
                                            group);
        else if (_geometryParameters.useRealisticSomas())
            returnValue =
                _createRealisticSoma(source, index, material, transformation,
                                     targetGIDOffsets, group);
        returnValue =
            returnValue &&
            _importMorphologyFromURI(source, index, material, transformation,
                                     compartmentReport, targetGIDOffsets,
                                     group);
        return returnValue;
    }

    bool _importMorphologies(const brain::Circuit& circuit,
                             const brain::GIDSet& gids,
                             const Matrix4fs& transformations,
                             GeometryGroup& group,
                             const GIDOffsets& targetGIDOffsets,
                             CompartmentReportPtr compartmentReport)
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
                Boxf bounds;
                ParallelGeometryGroupContainer groupContainer(spheres,
                                                              cylinders, cones,
                                                              triangleMeshes,
                                                              bounds);
                const auto& uri = uris[morphologyIndex];

                const size_t materialId = _getMaterialFromGeometryParameters(
                    morphologyIndex, NO_MATERIAL,
                    brain::neuron::SectionType::undefined, targetGIDOffsets);

                if (!_importMorphology(uri, morphologyIndex, materialId,
                                       transformations[morphologyIndex],
                                       compartmentReport, targetGIDOffsets,
                                       groupContainer))
#pragma omp atomic
                    ++loadingFailures;

#pragma omp critical
                for (const auto& material : groupContainer.materialIds)
                    group.getMaterialManager().set(material);
#pragma omp critical
                for (const auto& sphere : spheres)
                {
                    const auto index =
                        group.getMaterialManager().position(sphere.first);
                    group.getSpheres()[index].insert(
                        group.getSpheres()[index].end(), sphere.second.begin(),
                        sphere.second.end());
                }
#pragma omp critical
                for (const auto& cylinder : cylinders)
                {
                    const auto index =
                        group.getMaterialManager().position(cylinder.first);
                    group.getCylinders()[index].insert(
                        group.getCylinders()[index].end(),
                        cylinder.second.begin(), cylinder.second.end());
                }
#pragma omp critical
                for (const auto& cone : cones)
                {
                    const auto index =
                        group.getMaterialManager().position(cone.first);
                    group.getCones()[index].insert(
                        group.getCones()[index].end(), cone.second.begin(),
                        cone.second.end());
                }
#pragma omp critical
                group.getBounds().merge(bounds);
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
    MorphologyLoader& _parent;
    const ApplicationParameters& _applicationParameters;
    const GeometryParameters& _geometryParameters;
    size_ts _layerIds;
    size_ts _electrophysiologyTypes;
    size_ts _morphologyTypes;
    size_t _materialsOffset;
};

MorphologyLoader::MorphologyLoader(
    const ApplicationParameters& applicationParameters,
    const GeometryParameters& geometryParameters, const size_t nbMaterials)
    : _impl(new MorphologyLoader::Impl(applicationParameters,
                                       geometryParameters, nbMaterials, *this))
{
}

MorphologyLoader::~MorphologyLoader()
{
}

bool MorphologyLoader::importMorphology(const servus::URI& uri,
                                        const uint64_t index,
                                        const size_t material,
                                        GeometryGroup& group,
                                        const Matrix4f& transformation)
{
    const GIDOffsets targetGIDOffsets;
    return _impl->importMorphology(uri, index, material, transformation, group,
                                   targetGIDOffsets);
}

bool MorphologyLoader::importCircuit(const servus::URI& uri,
                                     const strings& targets,
                                     const std::string& report, Scene& scene,
                                     MeshLoader& meshLoader)
{
    return _impl->importCircuit(uri, targets, report, scene, meshLoader);
}
}
