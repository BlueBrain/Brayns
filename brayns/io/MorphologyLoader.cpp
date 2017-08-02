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

#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/log.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/simulation/CircuitSimulationHandler.h>
#include <brayns/io/algorithms/MetaballsGenerator.h>

#include <brain/brain.h>
#include <brion/brion.h>
#include <servus/types.h>

#include <algorithm>
#include <fstream>

#include <boost/filesystem.hpp>

namespace
{
const uint64_t OFFSET_MAGIC = 1e6;
}

namespace brayns
{
typedef std::unique_ptr<brion::CompartmentReport> CompartmentReportPtr;
typedef brion::CompartmentCounts* CompartmentCountsPtr;
typedef brion::SectionOffsets* SectionOffsetsPtr;

/**
 * @brief The ParallelSceneContainer class
 */
class ParallelSceneContainer
{
public:
    ParallelSceneContainer(SpheresMap& spheres, CylindersMap& cylinders,
                           ConesMap& cones, TrianglesMeshMap& trianglesMeshes,
                           MaterialsMap& materials, Boxf& worldBounds)
        : _spheres(spheres)
        , _cylinders(cylinders)
        , _cones(cones)
        , _trianglesMeshes(trianglesMeshes)
        , _materials(materials)
        , _worldBounds(worldBounds)
    {
    }

    SpheresMap& getSpheres() { return _spheres; }
    CylindersMap& getCylinders() { return _cylinders; }
    ConesMap& getCones() { return _cones; }
    TrianglesMeshMap& getTriangleMeshes() { return _trianglesMeshes; }
    MaterialsMap& getMaterials() { return _materials; }
    Boxf& getWorldBounds() { return _worldBounds; }
private:
    SpheresMap& _spheres;
    CylindersMap& _cylinders;
    ConesMap& _cones;
    TrianglesMeshMap& _trianglesMeshes;
    MaterialsMap& _materials;
    Boxf& _worldBounds;
};

/**
 * @brief The MorphologyLoader::Impl class
 */
class MorphologyLoader::Impl
{
public:
    Impl(GeometryParameters geometryParameters, Scene& scene)
        : _geometryParameters(geometryParameters)
        , _scene(scene)
        , _compartmentReport(nullptr)
    {
    }

    /**
     * @brief importMorphology
     * @param source
     * @param morphologyIndex
     * @param material
     * @param transformation
     * @return
     */
    bool importSingleMorphology(const servus::URI& source,
                                const uint64_t morphologyIndex,
                                const size_t material,
                                const Matrix4f& transformation)
    {
        ParallelSceneContainer sceneContainer(_scene.getSpheres(),
                                              _scene.getCylinders(),
                                              _scene.getCones(),
                                              _scene.getTriangleMeshes(),
                                              _scene.getMaterials(),
                                              _scene.getWorldBounds());

        return _importMorphologyInParallel(source, morphologyIndex, material,
                                           transformation, sceneContainer);
    }

    /**
     * @brief importCircuit
     * @param circuitConfig
     * @param target
     * @param report
     * @param meshLoader
     * @return
     */
    bool importCircuit(const servus::URI& circuitConfig,
                       const std::string& target, const std::string& report,
                       MeshLoader& meshLoader)
    {
        // Open Circuit and select GIDs according to specified target
        const std::string& filename = circuitConfig.getPath();
        const brion::BlueConfig bc(filename);
        const brain::Circuit circuit(bc);
        const brain::GIDSet& gids =
            (target.empty() ? circuit.getGIDs() : circuit.getGIDs(target));
        if (gids.empty())
        {
            BRAYNS_ERROR << "Circuit does not contain any cells" << std::endl;
            return false;
        }

        // Load simulation information from compartment report
        brain::GIDSet reportedGIDs = gids;
        if (!report.empty())
            try
            {
                _compartmentReport.reset(new brion::CompartmentReport(
                    brion::URI(bc.getReportSource(report).getPath()),
                    brion::MODE_READ, gids));
                reportedGIDs = _compartmentReport->getGIDs();
            }
            catch (const std::exception& e)
            {
                BRAYNS_ERROR << e.what() << std::endl;
                _compartmentReport = nullptr;
            }

        // Filter GIDs according to circuit density and selected bounding box
        const auto circuitDensity = _geometryParameters.getCircuitDensity();
        const size_t nbSkippedCells =
            reportedGIDs.size() / (reportedGIDs.size() * circuitDensity / 100);
        brain::GIDSet activeGids;
        Matrix4fs activeTransformations;
        size_t count = 0;
        const Matrix4fs& transforms = circuit.getTransforms(reportedGIDs);
        Progress progressGids("Loading GIDs...", reportedGIDs.size());
        for (const auto& gid : reportedGIDs)
        {
            ++progressGids;
            if ((nbSkippedCells != 0 && count % nbSkippedCells != 0) ||
                !_positionInCircuitBoundingBox(
                    transforms[count].getTranslation()))
            {
                ++count;
                continue;
            }
            activeGids.insert(gid);
            activeTransformations.push_back(transforms[count]);
            ++count;
        }
        _logLoadedGIDs(activeGids);

        // Read Brion circuit and pouplate neuron matrix. This is necessary
        // to identify layers, e-types, m-types, etc.
        _populateNeuronMatrix(bc, activeGids,
                              _geometryParameters.getColorScheme());

        bool returnValue = true;
        // Import meshes
        returnValue =
            _importMeshes(activeGids, activeTransformations, meshLoader);

        // Import morphologies
        if (_geometryParameters.getCircuitMeshFolder().empty() ||
            _geometryParameters.getCircuitUseSimulationModel())
            returnValue =
                _importMorphologies(circuit, activeGids, activeTransformations);
        return returnValue;
    }

private:
    /**
     * @brief _getCorrectedRadius
     * @param radius Radius to be corrected
     * @return Corrected value of a radius according to geometry parameters
     */
    float _getCorrectedRadius(const float radius)
    {
        return (_geometryParameters.getRadiusCorrection() != 0.f
                    ? _geometryParameters.getRadiusCorrection()
                    : radius * _geometryParameters.getRadiusMultiplier());
    }

    /**
     * @brief _getMaterialFromSectionType
     * @param morphologyIndex
     * @param forcedMaterial
     * @param sectionType
     * @param colorScheme
     * @return
     */
    size_t _getMaterialFromGeometryParameters(
        const uint64_t morphologyIndex, const size_t forcedMaterial,
        const brain::neuron::SectionType sectionType,
        const ColorScheme colorScheme)
    {
        if (forcedMaterial != NO_MATERIAL)
            return forcedMaterial;
        size_t material;
        switch (colorScheme)
        {
        case ColorScheme::neuron_by_id:
            material = morphologyIndex;
            break;
        case ColorScheme::neuron_by_segment_type:
            size_t s;
            switch (sectionType)
            {
            case brain::neuron::SectionType::soma:
                s = 1;
                break;
            case brain::neuron::SectionType::axon:
                s = 2;
                break;
            case brain::neuron::SectionType::dendrite:
                s = 3;
                break;
            case brain::neuron::SectionType::apicalDendrite:
                s = 4;
                break;
            default:
                s = 0;
                break;
            }
            material = s;
            break;
        default:
            material = 0;
        }
        return NB_SYSTEM_MATERIALS + material;
    }

    /**
     * @brief _getSectionTypes
     * @param morphologySectionTypes
     * @return
     */
    brain::neuron::SectionTypes _getSectionTypes(
        const size_t morphologySectionTypes)
    {
        brain::neuron::SectionTypes sectionTypes;
        if (morphologySectionTypes & size_t(MorphologySectionType::soma))
            sectionTypes.push_back(brain::neuron::SectionType::soma);
        if (morphologySectionTypes & size_t(MorphologySectionType::axon))
            sectionTypes.push_back(brain::neuron::SectionType::axon);
        if (morphologySectionTypes & size_t(MorphologySectionType::dendrite))
            sectionTypes.push_back(brain::neuron::SectionType::dendrite);
        if (morphologySectionTypes &
            size_t(MorphologySectionType::apical_dendrite))
            sectionTypes.push_back(brain::neuron::SectionType::apicalDendrite);
        return sectionTypes;
    }

    /**
     * @brief _positionInCircuitBoundingBox
     * @param position
     * @return
     */
    bool _positionInCircuitBoundingBox(const Vector3f& position) const
    {
        const auto& aabb = _geometryParameters.getCircuitBoundingBox();
        return aabb.getSize() == Vector3f(0.f) ? true : aabb.isIn(position);
    }

    /**
     * @brief _getNeuronAttributes
     * @param colorScheme
     * @return
     */
    brion::NeuronAttributes _getNeuronAttributes(const ColorScheme& colorScheme)
    {
        brion::NeuronAttributes neuronAttributes;
        switch (colorScheme)
        {
        case ColorScheme::neuron_by_layer:
            neuronAttributes = brion::NEURON_LAYER;
            break;
        case ColorScheme::neuron_by_mtype:
            neuronAttributes = brion::NEURON_MTYPE;
            break;
        case ColorScheme::neuron_by_etype:
            neuronAttributes = brion::NEURON_ETYPE;
            break;
        default:
            neuronAttributes = brion::NEURON_ALL;
            break;
        }
        return neuronAttributes;
    }

    /**
     * @brief _populateNeuronMatrix
     * @param blueConfig
     * @param gids
     * @param colorScheme
     */
    void _populateNeuronMatrix(const brion::BlueConfig& blueConfig,
                               const brain::GIDSet& gids,
                               const ColorScheme colorScheme)
    {
        _neuronMatrix.clear();
        brion::NeuronAttributes neuronAttributes =
            _getNeuronAttributes(colorScheme);
        if (neuronAttributes == brion::NEURON_ALL)
            return;
        try
        {
            brion::Circuit brionCircuit(blueConfig.getCircuitSource());
            for (const auto& a : brionCircuit.get(gids, neuronAttributes))
                _neuronMatrix.push_back(a[0]);
        }
        catch (...)
        {
            BRAYNS_WARN
                << "Only MVD2 format is currently supported by Brion "
                   "circuits. Color scheme by layer, e-type or m-type is "
                   "not available for this circuit"
                << std::endl;
        }
    }

    /**
     * @brief _getOffsetAsVector2f
     * @param offset
     * @return
     */
    Vector2f _getOffsetAsVector2f(const uint64_t offset)
    {
        Vector2f offsets;
        double y = offset / OFFSET_MAGIC;
        double x = offset - y * OFFSET_MAGIC;
        offsets.x() = x;
        offsets.y() = y;
        return offsets;
    }

    /**
     * @brief _logLoadedGIDs
     * @param gids
     */
    void _logLoadedGIDs(const brain::GIDSet& gids)
    {
        // Log loaded GIDs
        std::stringstream gidsStr;
        for (const auto& gid : gids)
            gidsStr << gid << " ";
        BRAYNS_INFO << "Loaded GIDs: " << gidsStr.str() << std::endl;
    }

    /**
     * @brief _importMorphologyAsPoint
     * @param morphologyIndex
     * @param transformation
     * @param forcedMaterial
     * @param scene
     * @return
     */
    bool _importMorphologyAsPoint(const uint64_t morphologyIndex,
                                  const size_t forcedMaterial,
                                  const Matrix4f& transformation,
                                  ParallelSceneContainer& scene)
    {
        uint64_t offset = 0;
        if (_compartmentReport)
            offset = _compartmentReport->getOffsets()[morphologyIndex][0];

        const float radius = _geometryParameters.getRadiusMultiplier();
        const auto offsets = _getOffsetAsVector2f(offset);
        const auto somaPosition = transformation.getTranslation();
        const size_t material = _getMaterialFromGeometryParameters(
            morphologyIndex, forcedMaterial, brain::neuron::SectionType::soma,
            _geometryParameters.getColorScheme());

        scene.getSpheres()[material].push_back(
            SpherePtr(new Sphere(somaPosition, radius, 0.f, offsets)));
        scene.getWorldBounds().merge(somaPosition);
        return true;
    }

    /**
     * @brief _importMorphologyAsMesh
     * @param source
     * @param morphologyIndex
     * @param transformation
     * @param forcedMaterial
     * @param scene
     * @return
     */
    bool _importMorphologyAsMesh(const servus::URI& source,
                                 const uint64_t morphologyIndex,
                                 const size_t forcedMaterial,
                                 const Matrix4f& transformation,
                                 ParallelSceneContainer& scene)
    {
        try
        {
            const size_t morphologySectionTypes =
                _geometryParameters.getMorphologySectionTypes();

            brain::neuron::Morphology morphology(source, transformation);
            brain::neuron::SectionTypes sectionTypes =
                _getSectionTypes(morphologySectionTypes);

            const brain::neuron::Sections& sections =
                morphology.getSections(sectionTypes);

            Spheres metaballs;
            if (morphologySectionTypes & size_t(MorphologySectionType::soma))
            {
                // Soma
                const auto& soma = morphology.getSoma();
                const auto center = soma.getCentroid();
                const auto radius = _getCorrectedRadius(soma.getMeanRadius());
                metaballs.push_back(SpherePtr(new Sphere(center, radius)));
                scene.getWorldBounds().merge(center);
            }

            // Dendrites and axon
            for (size_t s = 0; s < sections.size(); ++s)
            {
                const auto& section = sections[s];
                const bool hasParent = section.hasParent();
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

                const size_t samplesFromSoma =
                    _geometryParameters.getMetaballsSamplesFromSoma();
                const size_t samplesToProcess =
                    std::min(samplesFromSoma, samples.size());
                for (size_t i = 0; i < samplesToProcess; ++i)
                {
                    const auto& sample = samples[i];
                    const Vector3f position(sample.x(), sample.y(), sample.z());
                    const auto radius = _getCorrectedRadius(sample.w() * 0.5f);
                    if (radius > 0.f)
                        metaballs.push_back(
                            SpherePtr(new Sphere(position, radius)));

                    scene.getWorldBounds().merge(position);
                }
            }

            // Generate mesh
            const size_t gridSize = _geometryParameters.getMetaballsGridSize();
            const float threshold = _geometryParameters.getMetaballsThreshold();
            MetaballsGenerator metaballsGenerator;
            const size_t material = _getMaterialFromGeometryParameters(
                morphologyIndex, forcedMaterial,
                brain::neuron::SectionType::soma,
                _geometryParameters.getColorScheme());
            metaballsGenerator.generateMesh(metaballs, gridSize, threshold,
                                            scene.getMaterials(), material,
                                            scene.getTriangleMeshes());
        }
        catch (const std::runtime_error& e)
        {
            BRAYNS_ERROR << e.what() << std::endl;
            return false;
        }
        return true;
    }

    /**
     * @brief _importMorphology
     * @param source
     * @param morphologyIndex
     * @param transformation
     * @param forcedMaterial
     * @param scene
     * @return
     */
    bool _importMorphology(const servus::URI& source, uint64_t morphologyIndex,
                           const size_t forcedMaterial,
                           const Matrix4f& transformation,
                           ParallelSceneContainer& scene)
    {
        try
        {
            Vector3f translation;

            const size_t morphologySectionTypes =
                _geometryParameters.getMorphologySectionTypes();

            brain::neuron::Morphology morphology(source, transformation);
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
                        static_cast<float>(morphologyIndex % layout.nbColumns),
                    -1.f * layout.verticalSpacing *
                        static_cast<float>(morphologyIndex / layout.nbColumns),
                    0.f};
                translation = positionInGrid - morphologyAABB.getCenter();
            }

            sectionTypes = _getSectionTypes(morphologySectionTypes);

            const brain::neuron::Sections& sections =
                morphology.getSections(sectionTypes);

            size_t sectionId = 0;

            uint64_t offset = 0;
            if (_compartmentReport)
                offset = _compartmentReport->getOffsets()[morphologyIndex]
                                                         [sectionId];

            if (!_geometryParameters.useMetaballs() &&
                morphologySectionTypes & size_t(MorphologySectionType::soma))
            {
                // Soma
                const auto& soma = morphology.getSoma();
                const size_t material = _getMaterialFromGeometryParameters(
                    morphologyIndex, forcedMaterial,
                    brain::neuron::SectionType::soma,
                    _geometryParameters.getColorScheme());
                const auto somaPosition = soma.getCentroid() + translation;
                const auto radius = _getCorrectedRadius(soma.getMeanRadius());
                const auto offsets = _getOffsetAsVector2f(offset);
                scene.getSpheres()[material].push_back(
                    SpherePtr(new Sphere(somaPosition, radius, 0.f, offsets)));
                scene.getWorldBounds().merge(somaPosition);

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
                        const Vector3f sample = {samples[0].x(), samples[0].y(),
                                                 samples[0].z()};
                        scene.getCones()[material].push_back(ConePtr(
                            new Cone(somaPosition, sample, radius,
                                     _getCorrectedRadius(samples[0].w() * 0.5f),
                                     0.f, offsets)));
                        scene.getWorldBounds().merge(sample);
                    }
                }
            }

            // Dendrites and axon
            for (const auto& section : sections)
            {
                const auto material = _getMaterialFromGeometryParameters(
                    morphologyIndex, forcedMaterial, section.getType(),
                    _geometryParameters.getColorScheme());
                const auto& samples = section.getSamples();
                if (samples.empty())
                    continue;

                Vector4f previousSample = samples[0];
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
                if (_compartmentReport)
                {
                    const auto& counts =
                        _compartmentReport
                            ->getCompartmentCounts()[morphologyIndex];
                    // Number of compartments usually differs from number of
                    // samples
                    if (samples.empty() && counts[sectionId] > 1)
                        segmentStep = counts[sectionId] / float(samples.size());
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

                    const float distance = distanceToSoma + distancesToSoma[i];

                    if (_compartmentReport)
                        offset =
                            _compartmentReport->getOffsets()[morphologyIndex]
                                                            [sectionId] +
                            float(i) * segmentStep;

                    const auto sample = samples[i];
                    const auto previousRadius =
                        _getCorrectedRadius(samples[i - step].w() * 0.5f);

                    Vector3f position(sample.x(), sample.y(), sample.z());
                    position += translation;
                    Vector3f target(previousSample.x(), previousSample.y(),
                                    previousSample.z());
                    target += translation;
                    const auto offsets = _getOffsetAsVector2f(offset);
                    const auto radius =
                        _getCorrectedRadius(samples[i].w() * 0.5f);

                    if (radius > 0.f)
                    {
                        auto& spheres = scene.getSpheres()[material];
                        spheres.push_back(SpherePtr(
                            new Sphere(position, radius, distance, offsets)));
                        scene.getWorldBounds().merge(position);

                        if (position != target && previousRadius > 0.f)
                        {
                            auto& cylinders = scene.getCylinders()[material];
                            auto& cones = scene.getCones()[material];
                            if (radius == previousRadius)
                                cylinders.push_back(CylinderPtr(
                                    new Cylinder(position, target, radius,
                                                 distance, offsets)));
                            else
                                cones.push_back(
                                    ConePtr(new Cone(position, target, radius,
                                                     previousRadius, distance,
                                                     offsets)));
                            scene.getWorldBounds().merge(target);
                        }
                    }
                    previousSample = sample;
                }
                ++sectionId;
            }
        }
        catch (const std::runtime_error& e)
        {
            BRAYNS_ERROR << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool _importMeshes(const brain::GIDSet& gids,
                       const Matrix4fs& transformations, MeshLoader& meshLoader)
    {
#if (BRAYNS_USE_ASSIMP)
        size_t loadingFailures = 0;
        const auto meshedMorphologiesFolder =
            _geometryParameters.getCircuitMeshFolder();
        if (meshedMorphologiesFolder.empty())
            return true;

        uint64_t meshIndex = 0;
        // Loading meshes is currently sequential. TODO: Make it parallel!!!
        std::stringstream message;
        message << "Loading " << gids.size() << " meshes...";
        Progress progress(message.str(), gids.size());
        for (const auto& gid : gids)
        {
            ++progress;

            const size_t material =
                _neuronMatrix.empty()
                    ? _getMaterialFromGeometryParameters(
                          meshIndex, NO_MATERIAL,
                          brain::neuron::SectionType::undefined,
                          _geometryParameters.getColorScheme())
                    : NB_SYSTEM_MATERIALS +
                          boost::lexical_cast<size_t>(_neuronMatrix[meshIndex]);

            auto meshFilenamePattern =
                _geometryParameters.getCircuitMeshFilenamePattern();
            std::stringstream gidAsString;
            gidAsString << gid;
            const std::string GID = "{gid}";
            if (!meshFilenamePattern.empty())
                meshFilenamePattern.replace(meshFilenamePattern.find(GID),
                                            GID.length(), gidAsString.str());
            else
                meshFilenamePattern = gidAsString.str();

            const auto transformation =
                _geometryParameters.getCircuitMeshTransformation()
                    ? transformations[meshIndex]
                    : Matrix4f();
            auto meshFilename =
                meshedMorphologiesFolder + "/" + meshFilenamePattern;
            if (!meshLoader.importMeshFromFile(
                    meshFilename, _scene,
                    _geometryParameters.getGeometryQuality(), transformation,
                    material))
            {
                ++loadingFailures;
            }
            ++meshIndex;
        }
        if (loadingFailures != 0)
            BRAYNS_WARN << "Failed to import " << loadingFailures << " meshes"
                        << std::endl;
#endif
        return true;
    }

    /**
     * @brief _importMorphologyInParallel
     * @param source
     * @param morphologyIndex
     * @param material
     * @param transformation
     * @param scene
     * @return
     */
    bool _importMorphologyInParallel(const servus::URI& source,
                                     const uint64_t morphologyIndex,
                                     const size_t material,
                                     const Matrix4f& transformation,
                                     ParallelSceneContainer& scene)
    {
        bool returnValue = true;
        const size_t morphologySectionTypes =
            _geometryParameters.getMorphologySectionTypes();
        if (morphologySectionTypes ==
            static_cast<size_t>(MorphologySectionType::soma))
            return _importMorphologyAsPoint(morphologyIndex, material,
                                            transformation, scene);
        else if (_geometryParameters.useMetaballs())
            returnValue =
                _importMorphologyAsMesh(source, morphologyIndex, material,
                                        transformation, scene);
        returnValue =
            returnValue && _importMorphology(source, morphologyIndex, material,
                                             transformation, scene);
        return returnValue;
    }

    /**
     * @brief _importMorphologies
     * @param circuit
     * @param gids
     * @param transformations
     * @return
     */
    bool _importMorphologies(const brain::Circuit& circuit,
                             const brain::GIDSet& gids,
                             const Matrix4fs& transformations)
    {
        const brain::URIs& uris = circuit.getMorphologyURIs(gids);
        bool returnValue = true;
        std::stringstream message;
        message << "Loading " << uris.size() << " morphologies...";
        Progress progress(message.str(), uris.size());
#pragma omp parallel
        {
#pragma omp for nowait
            for (uint64_t morphologyIndex = 0; morphologyIndex < uris.size();
                 ++morphologyIndex)
            {
#pragma omp critical
                ++progress;

                SpheresMap spheres;
                CylindersMap cylinders;
                ConesMap cones;
                TrianglesMeshMap triangleMeshes;
                MaterialsMap materials;
                Boxf bounds;
                ParallelSceneContainer sceneContainer(spheres, cylinders, cones,
                                                      triangleMeshes, materials,
                                                      bounds);
                const auto& uri = uris[morphologyIndex];

                const size_t materialId =
                    _geometryParameters.getCircuitUseSimulationModel()
                        ? NB_SYSTEM_MATERIALS
                        : _neuronMatrix.empty()
                              ? NO_MATERIAL
                              : NB_SYSTEM_MATERIALS +
                                    boost::lexical_cast<size_t>(
                                        _neuronMatrix[morphologyIndex]);

                bool result = _importMorphologyInParallel(
                    uri, morphologyIndex, materialId,
                    transformations[morphologyIndex], sceneContainer);

#pragma omp atomic
                returnValue &= result;

#pragma omp critical
                for (const auto& sphere : spheres)
                {
                    const auto id = sphere.first;
                    _scene.getSpheres()[id].insert(
                        _scene.getSpheres()[id].end(),
                        sceneContainer.getSpheres()[id].begin(),
                        sceneContainer.getSpheres()[id].end());
                }

#pragma omp critical
                for (const auto& cylinder : cylinders)
                {
                    const auto id = cylinder.first;
                    _scene.getCylinders()[id].insert(
                        _scene.getCylinders()[id].end(),
                        sceneContainer.getCylinders()[id].begin(),
                        sceneContainer.getCylinders()[id].end());
                }

#pragma omp critical
                for (const auto& cone : cones)
                {
                    const auto id = cone.first;
                    _scene.getCones()[id].insert(
                        _scene.getCones()[id].end(),
                        sceneContainer.getCones()[id].begin(),
                        sceneContainer.getCones()[id].end());
                }

#pragma omp critical
                for (const auto& material : materials)
                    _scene.getMaterials()[material.first] = material.second;

#pragma omp critical
                _scene.getWorldBounds().merge(bounds);
            }
        }
        return returnValue;
    }

private:
    GeometryParameters _geometryParameters;
    Scene& _scene;
    strings _neuronMatrix;
    CompartmentReportPtr _compartmentReport;
};

MorphologyLoader::MorphologyLoader(const GeometryParameters& geometryParameters,
                                   Scene& scene)
    : _impl(new MorphologyLoader::Impl(geometryParameters, scene))
{
}

MorphologyLoader::~MorphologyLoader()
{
}

bool MorphologyLoader::importSingleMorphology(const servus::URI& source,
                                              const uint64_t morphologyIndex,
                                              const size_t material,
                                              const Matrix4f& transformation)
{
    return _impl->importSingleMorphology(source, morphologyIndex, material,
                                         transformation);
}

bool MorphologyLoader::importCircuit(const servus::URI& circuitConfig,
                                     const std::string& target,
                                     const std::string& report,
                                     MeshLoader& meshLoader)
{
    return _impl->importCircuit(circuitConfig, target, report, meshLoader);
}
}
