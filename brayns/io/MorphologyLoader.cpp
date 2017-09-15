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
#include <brayns/io/algorithms/MetaballsGenerator.h>
#include <brayns/io/simulation/CircuitSimulationHandler.h>

#include <brain/brain.h>
#include <brion/brion.h>
#include <servus/types.h>

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
struct ParallelSceneContainer
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

    SpheresMap& _spheres;
    CylindersMap& _cylinders;
    ConesMap& _cones;
    TrianglesMeshMap& _trianglesMeshes;
    MaterialsMap& _materials;
    Boxf& _worldBounds;
};

class MorphologyLoader::Impl
{
public:
    Impl(GeometryParameters geometryParameters, Scene& scene)
        : _geometryParameters(geometryParameters)
        , _scene(scene)
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
    bool importMorphology(const servus::URI& uri, const uint64_t index,
                          const size_t material, const Matrix4f& transformation,
                          CompartmentReportPtr compartmentReport = nullptr)
    {
        ParallelSceneContainer sceneContainer(_scene.getSpheres(),
                                              _scene.getCylinders(),
                                              _scene.getCones(),
                                              _scene.getTriangleMeshes(),
                                              _scene.getMaterials(),
                                              _scene.getWorldBounds());

        return _importMorphologyFromURI(uri, index, material, transformation,
                                        compartmentReport, sceneContainer);
    }

    /**
     * @brief importCircuit Imports a circuit from a specified URI
     * @param uri URI of the CircuitConfig
     * @param target Target to load
     * @param report Report to load
     * @param meshLoader Mesh loader used to load meshes
     * @return True is the circuit was successfully imported, false otherwise
     */
    bool importCircuit(const servus::URI& uri, const std::string& target,
                       const std::string& report, Scene& scene,
                       MeshLoader& meshLoader)
    {
        // Open Circuit and select GIDs according to specified target
        const brion::BlueConfig bc(uri.getPath());
        const brain::Circuit circuit(bc);
        const auto circuitDensity =
            _geometryParameters.getCircuitDensity() / 100.f;
        const auto allGids =
            (target.empty() ? circuit.getRandomGIDs(circuitDensity)
                            : circuit.getRandomGIDs(circuitDensity, target));
        const Matrix4fs& allTransformations = circuit.getTransforms(allGids);

        brain::GIDSet gids;
        const auto& aabb = _geometryParameters.getCircuitBoundingBox();
        if (aabb.getSize() == Vector3f(0.f))
            gids = allGids;
        else
        {
            auto gidIterator = allGids.begin();
            for (size_t i = 0; i < allTransformations.size(); ++i)
            {
                if (aabb.isIn(allTransformations[i].getTranslation()))
                    gids.insert(*gidIterator);
                ++gidIterator;
            }
        }

        if (gids.empty())
        {
            BRAYNS_ERROR << "Circuit does not contain any cells" << std::endl;
            return false;
        }

        // Load simulation information from compartment report
        CompartmentReportPtr compartmentReport = nullptr;
        if (!report.empty())
            try
            {
                CircuitSimulationHandlerPtr simulationHandler(
                    new CircuitSimulationHandler(
                        _geometryParameters,
                        bc.getReportSource(report).getPath(), gids));
                compartmentReport = simulationHandler->getCompartmentReport();
                // Attach simulation handler
                scene.setSimulationHandler(simulationHandler);
            }
            catch (const std::exception& e)
            {
                BRAYNS_ERROR << e.what() << std::endl;
            }

        const Matrix4fs& transformations = circuit.getTransforms(gids);
        _logLoadedGIDs(gids);

        // Read Brion circuit and pouplate neuron matrix. This is necessary
        // to identify layers, e-types, m-types, etc.
        _populateNeuronMatrix(bc, gids);

        // Import meshes
        bool returnValue = _importMeshes(gids, transformations, meshLoader);

        // Import morphologies
        if (_geometryParameters.getCircuitMeshFolder().empty() ||
            _geometryParameters.getCircuitUseSimulationModel())
            returnValue = returnValue &&
                          _importMorphologies(circuit, gids, transformations,
                                              compartmentReport);
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
        const brain::neuron::SectionType sectionType) const
    {
        if (material != NO_MATERIAL)
            return material;
        size_t materialId;
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
        default:
            materialId = NO_MATERIAL;
        }
        return NB_SYSTEM_MATERIALS + materialId;
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
     * @brief _getNeuronAttributes converts the --color-scheme geometry
     * parameters into brion::NeuronAttributes is applicable
     * @return brion::NeuronAttributes corresponding to the selected color
     * scheme
     */
    brion::NeuronAttributes _getNeuronAttributes() const
    {
        brion::NeuronAttributes neuronAttributes;
        switch (_geometryParameters.getColorScheme())
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
     * @brief _populateNeuronMatrix populates the neuron matrix according to the
     * --color-scheme geometry parameter. This is currently only supported for
     * the MVD2 format.
     * @param blueConfig Configuration of the circuit
     * @param gids GIDs of the neurons for which the matrix is required
     */
    void _populateNeuronMatrix(const brion::BlueConfig& blueConfig,
                               const brain::GIDSet& gids)
    {
        _neuronMatrix.clear();
        brion::NeuronAttributes neuronAttributes = _getNeuronAttributes();
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
        BRAYNS_INFO << "Loaded GIDs: " << gidsStr.str() << std::endl;
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
     * @return True if the loading was successfull, false otherwise
     */
    bool _importMorphologyAsPoint(const uint64_t index, const size_t material,
                                  const Matrix4f& transformation,
                                  CompartmentReportPtr compartmentReport,
                                  ParallelSceneContainer& scene)
    {
        uint64_t offset = 0;
        if (compartmentReport)
            offset = compartmentReport->getOffsets()[index][0];

        const auto radius = _geometryParameters.getRadiusMultiplier();
        const auto textureCoordinates = _getIndexAsTextureCoordinates(offset);
        const auto somaPosition = transformation.getTranslation();
        const auto materialId = _getMaterialFromGeometryParameters(
            index, material, brain::neuron::SectionType::soma);
        scene._spheres[materialId].push_back(SpherePtr(
            new Sphere(somaPosition, radius, 0.f, textureCoordinates)));
        scene._worldBounds.merge(somaPosition);
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
     * @return True if the loading was successfull, false otherwise
     */
    bool _createRealisticSoma(const servus::URI& uri, const uint64_t index,
                              const size_t material,
                              const Matrix4f& transformation,
                              ParallelSceneContainer& scene)
    {
        try
        {
            const size_t morphologySectionTypes =
                _geometryParameters.getMorphologySectionTypes();

            brain::neuron::Morphology morphology(uri, transformation);
            const auto sectionTypes = _getSectionTypes(morphologySectionTypes);
            const auto& sections = morphology.getSections(sectionTypes);

            Spheres metaballs;
            if (morphologySectionTypes & size_t(MorphologySectionType::soma))
            {
                // Soma
                const auto& soma = morphology.getSoma();
                const auto center = soma.getCentroid();
                const auto radius = _getCorrectedRadius(soma.getMeanRadius());
                metaballs.push_back(SpherePtr(new Sphere(center, radius)));
                scene._worldBounds.merge(center);
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
                        metaballs.push_back(
                            SpherePtr(new Sphere(position, radius)));

                    scene._worldBounds.merge(position);
                }
            }

            // Generate mesh from metaballs
            const auto gridSize = _geometryParameters.getMetaballsGridSize();
            const auto threshold = _geometryParameters.getMetaballsThreshold();
            MetaballsGenerator metaballsGenerator;
            const auto materialId = _getMaterialFromGeometryParameters(
                index, material, brain::neuron::SectionType::soma);
            metaballsGenerator.generateMesh(metaballs, gridSize, threshold,
                                            scene._materials, materialId,
                                            scene._trianglesMeshes);
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
     * @return True if the loading was successfull, false otherwise
     */
    bool _importMorphologyFromURI(const servus::URI& uri, const uint64_t index,
                                  const size_t material,
                                  const Matrix4f& transformation,
                                  CompartmentReportPtr compartmentReport,
                                  ParallelSceneContainer& scene)
    {
        try
        {
            Vector3f translation;

            const size_t morphologySectionTypes =
                _geometryParameters.getMorphologySectionTypes();

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
                    index, material, brain::neuron::SectionType::soma);
                const auto somaPosition = soma.getCentroid() + translation;
                const auto radius = _getCorrectedRadius(soma.getMeanRadius());
                const auto textureCoordinates =
                    _getIndexAsTextureCoordinates(offset);
                scene._spheres[materialId].push_back(SpherePtr(
                    new Sphere(somaPosition, radius, 0.f, textureCoordinates)));
                scene._worldBounds.merge(somaPosition);

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
                        scene._cones[materialId].push_back(ConePtr(
                            new Cone(somaPosition, sample, radius,
                                     _getCorrectedRadius(samples[0].w() * 0.5f),
                                     0.f, textureCoordinates)));
                        scene._worldBounds.merge(sample);
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
                                                       section.getType());
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
                                // This should never happen, but just in case
                                // use an invalid value to show an error color
                                offset = std::numeric_limits<uint64_t>::max();
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
                        auto& spheres = scene._spheres[materialId];
                        spheres.push_back(
                            SpherePtr(new Sphere(position, radius, distance,
                                                 textureCoordinates)));
                        scene._worldBounds.merge(position);

                        if (position != target && previousRadius > 0.f)
                        {
                            auto& cylinders = scene._cylinders[materialId];
                            auto& cones = scene._cones[materialId];
                            if (radius == previousRadius)
                                cylinders.push_back(CylinderPtr(
                                    new Cylinder(position, target, radius,
                                                 distance,
                                                 textureCoordinates)));
                            else
                                cones.push_back(
                                    ConePtr(new Cone(position, target, radius,
                                                     previousRadius, distance,
                                                     textureCoordinates)));
                            scene._worldBounds.merge(target);
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
                       const Matrix4fs& transformations, MeshLoader& meshLoader)
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
        Progress progress(message.str(), gids.size());
        for (const auto& gid : gids)
        {
            ++progress;

            const size_t materialId =
                _neuronMatrix.empty()
                    ? _getMaterialFromGeometryParameters(
                          meshIndex, NO_MATERIAL,
                          brain::neuron::SectionType::undefined)
                    : NB_SYSTEM_MATERIALS +
                          boost::lexical_cast<size_t>(_neuronMatrix[meshIndex]);

            // Define mesh filename according to file pattern and GID
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
            auto meshFilename =
                meshedMorphologiesFolder + "/" + meshFilenamePattern;

            // Load mesh from file
            const auto transformation =
                _geometryParameters.getCircuitMeshTransformation()
                    ? transformations[meshIndex]
                    : Matrix4f();
            if (!meshLoader.importMeshFromFile(
                    meshFilename, _scene,
                    _geometryParameters.getGeometryQuality(), transformation,
                    materialId))
            {
                ++loadingFailures;
            }
            ++meshIndex;
        }
        if (loadingFailures != 0)
            BRAYNS_WARN << "Failed to import " << loadingFailures << " meshes"
                        << std::endl;
        return true;
    }
#else
    bool _importMeshes(const brain::GIDSet&, const Matrix4fs&, MeshLoader&)
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
                           ParallelSceneContainer& scene)
    {
        bool returnValue = true;
        const size_t morphologySectionTypes =
            _geometryParameters.getMorphologySectionTypes();
        if (morphologySectionTypes ==
            static_cast<size_t>(MorphologySectionType::soma))
            return _importMorphologyAsPoint(index, material, transformation,
                                            compartmentReport, scene);
        else if (_geometryParameters.useRealisticSomas())
            returnValue = _createRealisticSoma(source, index, material,
                                               transformation, scene);
        returnValue =
            returnValue &&
            _importMorphologyFromURI(source, index, material, transformation,
                                     compartmentReport, scene);
        return returnValue;
    }

    bool _importMorphologies(const brain::Circuit& circuit,
                             const brain::GIDSet& gids,
                             const Matrix4fs& transformations,
                             CompartmentReportPtr compartmentReport)
    {
        const brain::URIs& uris = circuit.getMorphologyURIs(gids);
        size_t loadingFailures = 0;
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

                if (!_importMorphology(uri, morphologyIndex, materialId,
                                       transformations[morphologyIndex],
                                       compartmentReport, sceneContainer))
#pragma omp atomic
                    ++loadingFailures;

#pragma omp critical
                for (const auto& sphere : spheres)
                {
                    const auto id = sphere.first;
                    _scene.getSpheres()[id].insert(
                        _scene.getSpheres()[id].end(),
                        sceneContainer._spheres[id].begin(),
                        sceneContainer._spheres[id].end());
                }

#pragma omp critical
                for (const auto& cylinder : cylinders)
                {
                    const auto id = cylinder.first;
                    _scene.getCylinders()[id].insert(
                        _scene.getCylinders()[id].end(),
                        sceneContainer._cylinders[id].begin(),
                        sceneContainer._cylinders[id].end());
                }

#pragma omp critical
                for (const auto& cone : cones)
                {
                    const auto id = cone.first;
                    _scene.getCones()[id].insert(
                        _scene.getCones()[id].end(),
                        sceneContainer._cones[id].begin(),
                        sceneContainer._cones[id].end());
                }

#pragma omp critical
                for (const auto& material : materials)
                    _scene.getMaterials()[material.first] = material.second;

#pragma omp critical
                _scene.getWorldBounds().merge(bounds);
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
    GeometryParameters _geometryParameters;
    Scene& _scene;
    strings _neuronMatrix;
};

MorphologyLoader::MorphologyLoader(const GeometryParameters& geometryParameters,
                                   Scene& scene)
    : _impl(new MorphologyLoader::Impl(geometryParameters, scene))
{
}

MorphologyLoader::~MorphologyLoader()
{
}

bool MorphologyLoader::importMorphology(const servus::URI& uri,
                                        const uint64_t index,
                                        const size_t material,
                                        const Matrix4f& transformation)
{
    return _impl->importMorphology(uri, index, material, transformation);
}

bool MorphologyLoader::importCircuit(const servus::URI& uri,
                                     const std::string& target,
                                     const std::string& report, Scene& scene,
                                     MeshLoader& meshLoader)
{
    return _impl->importCircuit(uri, target, report, scene, meshLoader);
}
}
