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

#include <algorithm>
#include <fstream>

#include <boost/filesystem.hpp>

#if (BRAYNS_USE_BRION)
#include <brain/brain.h>
#include <brion/brion.h>
#endif

namespace
{
const float NO_OFFSET = -1.f;
}

namespace brayns
{
MorphologyLoader::MorphologyLoader(const GeometryParameters& geometryParameters)
    : _geometryParameters(geometryParameters)
{
}

#if (BRAYNS_USE_BRION)
size_t _getMaterialFromSectionType(const size_t morphologyIndex,
                                   const size_t forcedMaterial,
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
    if (morphologySectionTypes & size_t(MorphologySectionType::apical_dendrite))
        sectionTypes.push_back(brain::neuron::SectionType::apicalDendrite);
    return sectionTypes;
}

bool MorphologyLoader::_importMorphologyAsMesh(
    const servus::URI& source, const size_t morphologyIndex,
    MaterialsMap& materials, const Matrix4f& transformation,
    TrianglesMeshMap& meshes, Boxf& bounds, const size_t forcedMaterial)
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
            const brain::neuron::Soma& soma = morphology.getSoma();
            const Vector3f center = soma.getCentroid();

            const float radius =
                (_geometryParameters.getRadiusCorrection() != 0.f
                     ? _geometryParameters.getRadiusCorrection()
                     : soma.getMeanRadius() *
                           _geometryParameters.getRadiusMultiplier());

            metaballs.push_back(
                SpherePtr(new Sphere(center, radius, 0.f, 0.f)));
            bounds.merge(center);
        }

        // Dendrites and axon
        for (size_t s = 0; s < sections.size(); ++s)
        {
            const auto& section = sections[s];
            const bool hasParent = section.hasParent();
            if (hasParent)
            {
                const auto parentSectionType = section.getParent().getType();
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
                const auto radius =
                    (_geometryParameters.getRadiusCorrection() != 0.f
                         ? _geometryParameters.getRadiusCorrection()
                         : sample.w() * 0.5f *
                               _geometryParameters.getRadiusMultiplier());

                if (radius > 0.f)
                    metaballs.push_back(
                        SpherePtr(new Sphere(position, radius, 0.f, 0.f)));

                bounds.merge(position);
            }
        }

        // Generate mesh
        const size_t gridSize = _geometryParameters.getMetaballsGridSize();
        const float threshold = _geometryParameters.getMetaballsThreshold();
        MetaballsGenerator metaballsGenerator;
        const size_t material =
            _getMaterialFromSectionType(morphologyIndex, forcedMaterial,
                                        brain::neuron::SectionType::soma,
                                        _geometryParameters.getColorScheme());
        metaballsGenerator.generateMesh(metaballs, gridSize, threshold,
                                        materials, material, meshes);
    }
    catch (const std::runtime_error& e)
    {
        BRAYNS_ERROR << e.what() << std::endl;
        return false;
    }
    return true;
}

bool MorphologyLoader::importMorphology(const servus::URI& uri,
                                        const int morphologyIndex, Scene& scene)
{
    bool returnValue = true;
    if (_geometryParameters.useMetaballs())
    {
        returnValue =
            _importMorphologyAsMesh(uri, morphologyIndex, scene.getMaterials(),
                                    Matrix4f(), scene.getTriangleMeshes(),
                                    scene.getWorldBounds());
    }
    float maxDistanceToSoma;
    returnValue = returnValue &&
                  _importMorphology(uri, morphologyIndex, Matrix4f(), nullptr,
                                    scene.getSpheres(), scene.getCylinders(),
                                    scene.getCones(), scene.getWorldBounds(),
                                    NO_OFFSET, maxDistanceToSoma);
    return returnValue;
}

bool MorphologyLoader::_importMorphology(
    const servus::URI& source, const size_t morphologyIndex,
    const Matrix4f& transformation,
    SimulationInformation* simulationInformation, SpheresMap& spheres,
    CylindersMap& cylinders, ConesMap& cones, Boxf& bounds,
    const float simulationOffset, float& maxDistanceToSoma,
    const size_t forcedMaterial)
{
    maxDistanceToSoma = 0.f;
    try
    {
        Vector3f translation = {0.f, 0.f, 0.f};

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

        const size_t morphologySectionTypes =
            _geometryParameters.getMorphologySectionTypes();

        sectionTypes = _getSectionTypes(morphologySectionTypes);

        const brain::neuron::Sections& sections =
            morphology.getSections(sectionTypes);

        size_t sectionId = 0;

        float offset = NO_OFFSET;
        if (simulationInformation)
            offset = simulationInformation->getCompartmentOffsets(sectionId);
        else if (simulationOffset != NO_OFFSET)
            offset = simulationOffset;

        if (!_geometryParameters.useMetaballs() &&
            morphologySectionTypes & size_t(MorphologySectionType::soma))
        {
            // Soma
            const brain::neuron::Soma& soma = morphology.getSoma();
            const size_t material = _getMaterialFromSectionType(
                morphologyIndex, forcedMaterial,
                brain::neuron::SectionType::soma,
                _geometryParameters.getColorScheme());
            const Vector3f somaPosition = soma.getCentroid() + translation;

            const float radius =
                (_geometryParameters.getRadiusCorrection() != 0.f
                     ? _geometryParameters.getRadiusCorrection()
                     : soma.getMeanRadius() *
                           _geometryParameters.getRadiusMultiplier());

            spheres[material].push_back(
                SpherePtr(new Sphere(somaPosition, radius, 0.f, offset)));
            bounds.merge(somaPosition);

            if (_geometryParameters.getUseSimulationModel())
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
                    cones[material].push_back(ConePtr(
                        new Cone(somaPosition, sample, radius,
                                 samples[0].w() * 0.5f *
                                     _geometryParameters.getRadiusMultiplier(),
                                 0.f, offset)));
                    bounds.merge(sample);
                }
            }
        }

        // Dendrites and axon
        for (const auto& section : sections)
        {
            const size_t material = _getMaterialFromSectionType(
                morphologyIndex, forcedMaterial, section.getType(),
                _geometryParameters.getColorScheme());
            const Vector4fs& samples = section.getSamples();
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
            const floats& distancesToSoma = section.getSampleDistancesToSoma();

            float segmentStep = 0.f;
            if (simulationInformation)
            {
                const auto& counts =
                    simulationInformation->getCompartmentCounts();
                // Number of compartments usually differs from number of samples
                if (samples.empty() && counts[sectionId] > 1)
                    segmentStep = counts[sectionId] / float(samples.size());
            }

            bool done = false;
            for (size_t i = step; !done && i < samples.size() + step; i += step)
            {
                if (i >= samples.size())
                {
                    i = samples.size() - 1;
                    done = true;
                }

                const float distance = distanceToSoma + distancesToSoma[i];

                maxDistanceToSoma = std::max(maxDistanceToSoma, distance);

                if (simulationInformation)
                    offset = simulationInformation->getCompartmentOffsets(
                                 sectionId) +
                             float(i) * segmentStep;
                else if (simulationOffset != NO_OFFSET)
                    offset = simulationOffset + distance;

                Vector4f sample = samples[i];
                const float previousRadius =
                    (_geometryParameters.getRadiusCorrection() != 0.f
                         ? _geometryParameters.getRadiusCorrection()
                         : samples[i - step].w() * 0.5f *
                               _geometryParameters.getRadiusMultiplier());

                Vector3f position(sample.x(), sample.y(), sample.z());
                position += translation;
                Vector3f target(previousSample.x(), previousSample.y(),
                                previousSample.z());
                target += translation;
                const float radius =
                    (_geometryParameters.getRadiusCorrection() != 0.f
                         ? _geometryParameters.getRadiusCorrection()
                         : samples[i].w() * 0.5f *
                               _geometryParameters.getRadiusMultiplier());

                if (radius > 0.f)
                    spheres[material].push_back(SpherePtr(
                        new Sphere(position, radius, distance, offset)));

                bounds.merge(position);
                if (position != target && radius > 0.f && previousRadius > 0.f)
                {
                    if (radius == previousRadius)
                        cylinders[material].push_back(
                            CylinderPtr(new Cylinder(position, target, radius,
                                                     distance, offset)));
                    else
                        cones[material].push_back(ConePtr(
                            new Cone(position, target, radius, previousRadius,
                                     distance, offset)));
                    bounds.merge(target);
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

brion::NeuronAttributes getNeuronAttributes(const ColorScheme& colorScheme)
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

bool getNeuronMatrix(const brion::BlueConfig& bc, const brain::GIDSet& gids,
                     const ColorScheme colorScheme, strings& neuronMatrix)
{
    brion::NeuronAttributes neuronAttributes = getNeuronAttributes(colorScheme);
    if (neuronAttributes == brion::NEURON_ALL)
        return false;
    try
    {
        brion::Circuit brionCircuit(bc.getCircuitSource());
        for (const auto& a : brionCircuit.get(gids, neuronAttributes))
            neuronMatrix.push_back(a[0]);
        return true;
    }
    catch (...)
    {
        BRAYNS_WARN << "Only MVD2 format is currently supported by Brion "
                       "circuits. Color scheme by layer, e-type or m-type is "
                       "not available for this circuit"
                    << std::endl;
    }
    return false;
}

bool MorphologyLoader::importCircuit(const servus::URI& circuitConfig,
                                     const std::string& target,
                                     const std::string& report, Scene& scene
#if (BRAYNS_USE_ASSIMP)
                                     ,
                                     MeshLoader& meshLoader
#endif
                                     )
{
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
    const Matrix4fs& transforms = circuit.getTransforms(gids);

    const brain::URIs& uris = circuit.getMorphologyURIs(gids);

    brain::GIDSet cr_gids;

    // Load simulation information from compartment reports
    std::unique_ptr<brion::CompartmentReport> compartmentReport = nullptr;
    if (!report.empty())
        compartmentReport.reset(new brion::CompartmentReport(
            brion::URI(bc.getReportSource(report).getPath()), brion::MODE_READ,
            gids));

    const brion::CompartmentCounts& compartmentCounts =
        compartmentReport ? compartmentReport->getCompartmentCounts()
                          : brion::CompartmentCounts();
    const brion::SectionOffsets& compartmentOffsets =
        compartmentReport ? compartmentReport->getOffsets()
                          : brion::SectionOffsets();
    cr_gids = compartmentReport ? compartmentReport->getGIDs() : gids;

    BRAYNS_INFO << "Loading " << cr_gids.size() << " simulated cells"
                << std::endl;
    brain::URIs cr_uris;
    if (compartmentReport)
        for (const auto cr_gid : cr_gids)
        {
            const auto it = std::find(gids.begin(), gids.end(), cr_gid);
            const auto index = std::distance(gids.begin(), it);
            cr_uris.push_back(uris[index]);
        }
    else
        cr_uris = uris;

    // Read Brion circuit
    strings neuronMatrix;
    bool mvd3Support =
        getNeuronMatrix(bc, gids, _geometryParameters.getColorScheme(),
                        neuronMatrix);

    const auto circuitDensity = _geometryParameters.getCircuitDensity();
    const size_t nbSkippedCells =
        uris.size() / (uris.size() * circuitDensity / 100);

    const auto meshedMorphologiesFolder =
        _geometryParameters.getMeshedMorphologiesFolder();

    size_t morphologyCount = 0;
    bool loadParametricGeometry = true;
#if (BRAYNS_USE_ASSIMP)
    if (!meshedMorphologiesFolder.empty())
    {
        // Loading meshes is currently sequential. TODO: Make it parallel!!!
        Progress progress("Loading meshes...", uris.size());
        brion::GIDSet::const_iterator gid = cr_gids.begin();
        for (size_t i = 0; i < cr_gids.size(); ++i)
        {
            ++progress;

            if ((nbSkippedCells != 0 && morphologyCount % nbSkippedCells != 0))
            {
                ++gid;
                continue;
            }

            if (!_positionInCircuitBoundingBox(transforms[i].getTranslation()))
            {
                ++gid;
                continue;
            }

            const size_t material =
                mvd3Support ? boost::lexical_cast<size_t>(neuronMatrix[i])
                            : _getMaterialFromSectionType(
                                  morphologyCount, NO_MATERIAL,
                                  brain::neuron::SectionType::undefined,
                                  _geometryParameters.getColorScheme());

            auto meshFilenamePattern =
                _geometryParameters.getMeshFilenamePattern();
            std::stringstream gidAsString;
            gidAsString << (*gid);
            const std::string GID = "{gid}";
            if (!meshFilenamePattern.empty())
                meshFilenamePattern.replace(meshFilenamePattern.find(GID),
                                            GID.length(), gidAsString.str());
            else
                meshFilenamePattern = gidAsString.str();

            auto meshFilename =
                meshedMorphologiesFolder + "/" + meshFilenamePattern;
            meshLoader.importMeshFromFile(
                meshFilename, scene, _geometryParameters.getGeometryQuality(),
                transforms[i], material);

            ++gid;
            ++morphologyCount;
        }
        loadParametricGeometry = _geometryParameters.getUseSimulationModel();
    }
#endif

    if (loadParametricGeometry)
    {
        Progress progress("Loading geometries...", cr_uris.size());
        morphologyCount = 0;
#pragma omp parallel
        {
#pragma omp for nowait
            for (size_t i = 0; i < cr_uris.size(); ++i)
            {
#pragma omp critical
                ++progress;

                SpheresMap private_spheres;
                CylindersMap private_cylinders;
                ConesMap private_cones;
                Boxf private_bounds;

                if (nbSkippedCells != 0 && i % nbSkippedCells != 0)
                    continue;

                if (!_positionInCircuitBoundingBox(
                        transforms[i].getTranslation()))
                {
                    continue;
                }

                const auto& uri = cr_uris[i];

                std::unique_ptr<SimulationInformation> simulationInformation =
                    nullptr;
                if (compartmentReport)
                    simulationInformation.reset(new SimulationInformation(
                        (uint16_ts&)compartmentCounts[i],
                        (uint64_ts&)compartmentOffsets[i]));
                const size_t material =
                    mvd3Support
                        ? NB_SYSTEM_MATERIALS +
                              boost::lexical_cast<size_t>(neuronMatrix[i])
                        : NO_MATERIAL;

                if (_geometryParameters.useMetaballs())
                {
                    _importMorphologyAsMesh(uri, morphologyCount,
                                            scene.getMaterials(), transforms[i],
                                            scene.getTriangleMeshes(),
                                            scene.getWorldBounds(), material);
                }

                float maxDistanceToSoma;
                if (_importMorphology(uri, morphologyCount, transforms[i],
                                      simulationInformation.get(),
                                      private_spheres, private_cylinders,
                                      private_cones, private_bounds, -1.f,
                                      maxDistanceToSoma, material))
#pragma omp atomic
                    ++morphologyCount;

#pragma omp critical
                for (const auto& p : private_spheres)
                {
                    const auto m = p.first;
                    scene.getSpheres()[m].insert(scene.getSpheres()[m].end(),
                                                 private_spheres[m].begin(),
                                                 private_spheres[m].end());
                }

#pragma omp critical
                for (const auto& p : private_cylinders)
                {
                    const auto m = p.first;
                    scene.getCylinders()[m].insert(
                        scene.getCylinders()[m].end(),
                        private_cylinders[m].begin(),
                        private_cylinders[m].end());
                }

#pragma omp critical
                for (const auto& p : private_cones)
                {
                    const auto m = p.first;
                    scene.getCones()[m].insert(scene.getCones()[m].end(),
                                               private_cones[m].begin(),
                                               private_cones[m].end());
                }

#pragma omp critical
                scene.getWorldBounds().merge(private_bounds);
            }
        }
    }

    size_t nonSimulatedCells = _geometryParameters.getNonSimulatedCells();
    if (nonSimulatedCells != 0)
    {
        // Non simulated cells
        const brain::GIDSet& allGids = circuit.getGIDs();
        const brain::URIs& allUris = circuit.getMorphologyURIs(allGids);
        const Matrix4fs& allTransforms = circuit.getTransforms(allGids);

        cr_uris.clear();
        size_t index = 0;
        for (const auto gid : allGids)
        {
            auto it = std::find(cr_gids.begin(), cr_gids.end(), gid);
            if (it == cr_gids.end())
                cr_uris.push_back(allUris[index]);
            ++index;
        }

        if (cr_uris.size() < nonSimulatedCells)
            nonSimulatedCells = cr_uris.size();

        std::stringstream msg;
        msg << "Loading " << nonSimulatedCells << " non-simulated cells";

        neuronMatrix.clear();
        if (mvd3Support)
            getNeuronMatrix(bc, allGids, _geometryParameters.getColorScheme(),
                            neuronMatrix);

        Progress progress(msg.str(), allUris.size());
        morphologyCount = 0;
#pragma omp parallel
        {
#pragma omp for nowait
            for (size_t i = 0; i < nonSimulatedCells; ++i)
            {
#pragma omp critical
                ++progress;

                SpheresMap private_spheres;
                CylindersMap private_cylinders;
                ConesMap private_cones;
                Boxf private_bounds;

                if (nbSkippedCells != 0 && i % nbSkippedCells != 0)
                    continue;

                if (!_positionInCircuitBoundingBox(
                        transforms[i].getTranslation()))
                {
                    continue;
                }

                float maxDistanceToSoma;
                const auto& uri = allUris[i];
                size_t material = NO_MATERIAL;
                if (mvd3Support)
                    try
                    {
                        material =
                            NB_SYSTEM_MATERIALS +
                            boost::lexical_cast<size_t>(neuronMatrix[i][0]);
                    }
                    catch (const boost::bad_lexical_cast&)
                    {
                    }

#if (BRAYNS_USE_ASSIMP)
                if (!meshedMorphologiesFolder.empty())
                {
                    const auto filenameNoExt =
                        boost::filesystem::path(uri.getPath()).stem().string();
                    const std::string meshFilename = meshedMorphologiesFolder +
                                                     filenameNoExt.c_str() +
                                                     ".off";
                    meshLoader.importMeshFromFile(
                        meshFilename, scene,
                        _geometryParameters.getGeometryQuality(), transforms[i],
                        material);
                }
                else
                {
#endif
                    if (_importMorphology(uri, morphologyCount,
                                          allTransforms[i], nullptr,
                                          private_spheres, private_cylinders,
                                          private_cones, private_bounds, 0,
                                          maxDistanceToSoma, material))
                    {
#pragma omp atomic
                        ++morphologyCount;
                    }
                }
#pragma omp critical
                for (const auto& p : private_spheres)
                {
                    const size_t m = p.first;
                    scene.getSpheres()[m].insert(scene.getSpheres()[m].end(),
                                                 private_spheres[m].begin(),
                                                 private_spheres[m].end());
                }

#pragma omp critical
                for (const auto& p : private_cylinders)
                {
                    const size_t m = p.first;
                    scene.getCylinders()[m].insert(
                        scene.getCylinders()[m].end(),
                        private_cylinders[m].begin(),
                        private_cylinders[m].end());
                }

#pragma omp critical
                for (const auto& p : private_cones)
                {
                    const size_t m = p.first;
                    scene.getCones()[m].insert(scene.getCones()[m].end(),
                                               private_cones[m].begin(),
                                               private_cones[m].end());
                }

#pragma omp critical
                scene.getWorldBounds().merge(private_bounds);
            }
        }
    }
    return true;
}

bool MorphologyLoader::importSimulationData(const servus::URI& circuitConfig,
                                            const std::string& target,
                                            const std::string& report,
                                            Scene& scene)
{
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

    // Load simulation information from compartment reports
    brion::CompartmentReport compartmentReport(
        brion::URI(bc.getReportSource(report).getPath()), brion::MODE_READ,
        gids);

    CircuitSimulationHandlerPtr simulationHandler(
        new CircuitSimulationHandler(_geometryParameters));
    scene.setSimulationHandler(simulationHandler);
    const std::string& cacheFile = _geometryParameters.getSimulationCacheFile();
    if (simulationHandler->attachSimulationToCacheFile(cacheFile))
        // Cache already exists, no need to create it.
        return true;

    BRAYNS_INFO << "Cache file does not exist, creating it" << std::endl;
    std::ofstream file(cacheFile, std::ios::out | std::ios::binary);

    if (!file.is_open())
    {
        BRAYNS_ERROR << "Failed to create cache file" << std::endl;
        return false;
    }

    // Load simulation information from compartment reports
    const float start = compartmentReport.getStartTime();
    const float end = compartmentReport.getEndTime();
    const float step = compartmentReport.getTimestep();

    const float firstFrame =
        std::max(start, _geometryParameters.getStartSimulationTime());
    const float lastFrame =
        std::min(end, _geometryParameters.getEndSimulationTime());
    const uint64_t frameSize = compartmentReport.getFrameSize();

    const uint64_t nbFrames = (lastFrame - firstFrame) / step;

    // Write header
    simulationHandler->setNbFrames(nbFrames);
    simulationHandler->setFrameSize(frameSize);
    simulationHandler->writeHeader(file);

    // Write body
    Progress progress(
        "Loading values from compartment report and saving them to cache",
        nbFrames);
    for (uint64_t frame = 0; frame < nbFrames; ++frame)
    {
        ++progress;
        const float frameTime = firstFrame + step * frame;
        const brion::floatsPtr& valuesPtr =
            compartmentReport.loadFrame(frameTime).get();
        const floats& values = *valuesPtr;
        simulationHandler->writeFrame(file, values);
    }
    file.close();

    BRAYNS_INFO << "----------------------------------------" << std::endl;
    BRAYNS_INFO << "Cache file successfully created" << std::endl;
    BRAYNS_INFO << "Number of frames: " << nbFrames << std::endl;
    BRAYNS_INFO << "Frame size      : " << frameSize << std::endl;
    BRAYNS_INFO << "----------------------------------------" << std::endl;
    return true;
}

bool MorphologyLoader::_positionInCircuitBoundingBox(
    const Vector3f& position) const
{
    const auto& aabb = _geometryParameters.getCircuitBoundingBox();
    return aabb.getSize() == Vector3f(0.f) ? true : aabb.isIn(position);
}

#else

bool MorphologyLoader::importMorphology(const servus::URI&, const int, Scene&)
{
    BRAYNS_ERROR << "Brion is required to load morphologies" << std::endl;
    return false;
}

bool MorphologyLoader::importCircuit(const servus::URI&, const std::string&,
                                     const std::string&, Scene&
#if (BRAYNS_USE_ASSIMP)
                                     ,
                                     MeshLoader&
#endif
                                     )
{
    BRAYNS_ERROR << "Brion is required to load circuits" << std::endl;
    return false;
}

bool MorphologyLoader::importSimulationData(const servus::URI&,
                                            const std::string&,
                                            const std::string&, Scene&)
{
    BRAYNS_ERROR << "Brion is required to load circuits" << std::endl;
    return false;
}

#endif
}
