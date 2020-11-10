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

#include "MorphologyLoader.h"
#include "Utils.h"
#include <common/log.h>
#include <common/types.h>
#include <plugin/meshing/MetaballsGenerator.h>

#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/common/types.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <boost/filesystem.hpp>

namespace
{
const std::string SUPPORTED_EXTENTION_H5 = "h5";
const std::string SUPPORTED_EXTENTION_SWC = "swc";

// From http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template <class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    almost_equal(T x, T y, int ulp)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x - y) <=
               std::numeric_limits<T>::epsilon() * std::abs(x + y) * ulp
           // unless the result is subnormal
           || std::abs(x - y) < std::numeric_limits<T>::min();
}

inline double _getLastSampleRadius(const brain::neuron::Section& section)
{
    return section[-1][3] * 0.5f;
}
} // namespace

MorphologyLoader::MorphologyLoader(brayns::Scene& scene,
                                   brayns::PropertyMap&& loaderParams)
    : Loader(scene)
    , _defaults(loaderParams)
{
}

std::string MorphologyLoader::getName() const
{
    return "Morphology loader";
}

std::vector<std::string> MorphologyLoader::getSupportedExtensions() const
{
    return {SUPPORTED_EXTENTION_H5, SUPPORTED_EXTENTION_SWC};
}

bool MorphologyLoader::isSupported(const std::string& /*filename*/,
                                   const std::string& extension) const
{
    const std::set<std::string> types = {SUPPORTED_EXTENTION_H5,
                                         SUPPORTED_EXTENTION_SWC};
    return types.find(extension) != types.end();
}

MorphologyInfo MorphologyLoader::importMorphology(
    const brayns::PropertyMap& properties, const brion::URI& source,
    brayns::Model& model, const uint64_t index,
    const brayns::Matrix4f& transformation, brain::Synapses* afferentSynapses,
    brain::Synapses* efferentSynapses,
    CompartmentReportPtr compartmentReport) const
{
    ParallelModelContainer modelContainer;
    _importMorphology(properties, source, index, modelContainer, transformation,
                      compartmentReport, afferentSynapses, efferentSynapses);

    modelContainer.applyTransformation(transformation);
    modelContainer.addSpheresToModel(model);
    modelContainer.addCylindersToModel(model);
    modelContainer.addConesToModel(model);
    modelContainer.addSDFGeometriesToModel(model);

    return modelContainer.morphologyInfo;
}

void MorphologyLoader::_importMorphology(
    const brayns::PropertyMap& properties, const brion::URI& source,
    const uint64_t index, ParallelModelContainer& model,
    const brayns::Matrix4f& transformation,
    CompartmentReportPtr compartmentReport, brain::Synapses* afferentSynapses,
    brain::Synapses* efferentSynapses) const
{
    const auto sectionTypes = getSectionTypesFromProperties(properties);
    const auto useRealisticSoma =
        properties.getProperty<bool>(PROP_USE_REALISTIC_SOMA.name);

    if (sectionTypes.size() == 1 &&
        sectionTypes[0] == brain::neuron::SectionType::soma)
        _importMorphologyAsPoint(properties, index, compartmentReport, model);
    else if (useRealisticSoma)
        _createRealisticSoma(properties, source, model);
    else
        _importMorphologyFromURI(properties, source, index, transformation,
                                 compartmentReport, model, afferentSynapses,
                                 efferentSynapses);
}

double MorphologyLoader::_getCorrectedRadius(
    const brayns::PropertyMap& properties, const double radius) const
{
    const double radiusCorrection =
        properties.getProperty<double>(PROP_RADIUS_CORRECTION.name);
    const double radiusMultiplier =
        properties.getProperty<double>(PROP_RADIUS_MULTIPLIER.name);
    return (radiusCorrection != 0.0 ? radiusCorrection
                                    : radius * radiusMultiplier * 0.5);
}

void MorphologyLoader::_importMorphologyAsPoint(
    const brayns::PropertyMap& properties, const uint64_t index,
    CompartmentReportPtr compartmentReport, ParallelModelContainer& model) const
{
    // If there is no compartment report, the offset in the simulation buffer is
    // the index of the morphology in the circuit
    uint64_t userDataOffset = index;
    if (compartmentReport)
        userDataOffset = compartmentReport->getOffsets()[index][0];

    const double radiusMultiplier =
        properties.getProperty<double>(PROP_RADIUS_MULTIPLIER.name);
    const size_t materialId =
        _getMaterialIdFromColorScheme(properties,
                                      brain::neuron::SectionType::soma);
    model.addSphere(materialId,
                    {model.morphologyInfo.somaPosition,
                     static_cast<float>(radiusMultiplier), userDataOffset});
}

void MorphologyLoader::_createRealisticSoma(
    const brayns::PropertyMap& properties, const brion::URI& uri,
    ParallelModelContainer& model) const
{
    brain::neuron::SectionTypes sectionTypes;
    if (properties.getProperty<bool>(PROP_SECTION_TYPE_SOMA.name))
        sectionTypes.push_back(brain::neuron::SectionType::soma);
    if (properties.getProperty<bool>(PROP_SECTION_TYPE_AXON.name))
        sectionTypes.push_back(brain::neuron::SectionType::axon);
    if (properties.getProperty<bool>(PROP_SECTION_TYPE_DENDRITE.name))
        sectionTypes.push_back(brain::neuron::SectionType::dendrite);
    if (properties.getProperty<bool>(PROP_SECTION_TYPE_APICAL_DENDRITE.name))
        sectionTypes.push_back(brain::neuron::SectionType::apicalDendrite);
    const size_t metaballsSamplesFromSoma =
        properties.getProperty<int>(PROP_METABALLS_SAMPLES_FROM_SOMA.name);
    const auto metaballsGridSize =
        properties.getProperty<int>(PROP_METABALLS_GRID_SIZE.name);
    const auto metaballsThreshold =
        properties.getProperty<double>(PROP_METABALLS_THRESHOLD.name);

    brain::neuron::Morphology morphology(uri);
    const auto& st = sectionTypes;
    const auto& sections = morphology.getSections(st);

    brayns::Vector4fs metaballs;

    if (std::find(st.begin(), st.end(), brain::neuron::SectionType::soma) !=
        st.end())
    {
        // Soma
        const auto& soma = morphology.getSoma();
        model.morphologyInfo.somaPosition = morphology.getSoma().getCentroid();
        const auto radius =
            _getCorrectedRadius(properties, soma.getMeanRadius());
        metaballs.push_back({model.morphologyInfo.somaPosition.x,
                             model.morphologyInfo.somaPosition.y,
                             model.morphologyInfo.somaPosition.z, radius});
    }

    // Dendrites and axon
    for (const auto& section : sections)
    {
        const auto hasParent = section.hasParent();
        if (hasParent)
        {
            const auto parentSectionType = section.getParent().getType();
            if (parentSectionType != brain::neuron::SectionType::soma)
                continue;
        }

        const auto& samples = section.getSamples();
        if (samples.empty())
            continue;

        const auto samplesToProcess =
            std::min(metaballsSamplesFromSoma, samples.size());
        for (size_t i = 0; i < samplesToProcess; ++i)
        {
            const auto& sample = samples[i];
            const brayns::Vector3f position(sample);
            const auto radius =
                _getCorrectedRadius(properties, sample.w * 0.5f);
            if (radius > 0.f)
                metaballs.push_back(
                    {position.x, position.y, position.z, radius});
        }
    }

    // Generate mesh from metaballs
    MetaballsGenerator metaballsGenerator;
    const auto materialId =
        _getMaterialIdFromColorScheme(properties,
                                      brain::neuron::SectionType::soma);
    metaballsGenerator.generateMesh(metaballs, metaballsGridSize,
                                    metaballsThreshold, materialId,
                                    model.trianglesMeshes);
}

size_t MorphologyLoader::_addSDFGeometry(SDFMorphologyData& sdfMorphologyData,
                                         const brayns::SDFGeometry& geometry,
                                         const std::set<size_t>& neighbours,
                                         const size_t materialId,
                                         const int section) const
{
    const size_t idx = sdfMorphologyData.geometries.size();
    sdfMorphologyData.geometries.push_back(geometry);
    sdfMorphologyData.neighbours.push_back(neighbours);
    sdfMorphologyData.materials.push_back(materialId);
    sdfMorphologyData.geometrySection[idx] = section;
    sdfMorphologyData.sectionGeometries[section].push_back(idx);
    return idx;
}

void MorphologyLoader::_connectSDFSomaChildren(
    const brayns::PropertyMap& properties, const brayns::Vector3f& somaPosition,
    const double somaRadius, const size_t materialId,
    const uint64_t& userDataOffset, const brain::neuron::Sections& somaChildren,
    SDFMorphologyData& sdfMorphologyData) const
{
    std::set<size_t> child_indices;

    for (const auto& child : somaChildren)
    {
        const auto& samples = child.getSamples();
        const brayns::Vector3f sample(samples[0]);

        // Create a sigmoid cone with half of soma radius to center of soma
        // to give it an organic look.
        const auto radiusEnd =
            _getCorrectedRadius(properties, samples[0].w * 0.5f);
        const size_t geomIdx =
            _addSDFGeometry(sdfMorphologyData,
                            brayns::createSDFConePillSigmoid(
                                somaPosition, sample, somaRadius * 0.5f,
                                radiusEnd, userDataOffset),
                            {}, materialId, -1);
        child_indices.insert(geomIdx);
    }

    for (size_t c : child_indices)
        sdfMorphologyData.neighbours[c] = child_indices;
}

void MorphologyLoader::_connectSDFBifurcations(
    SDFMorphologyData& sdfMorphologyData,
    const MorphologyTreeStructure& mts) const
{
    const size_t numSections = mts.sectionChildren.size();

    for (size_t section = 0; section < numSections; section++)
    {
        // Find the bifurction geometry id for this section
        size_t bifurcationId = 0;
        bool bifurcationIdFound = false;
        for (size_t bifId : sdfMorphologyData.bifurcationIndices)
        {
            const int bifSection = sdfMorphologyData.geometrySection.at(bifId);

            if (bifSection == static_cast<int>(section))
            {
                bifurcationId = bifId;
                bifurcationIdFound = true;
                break;
            }
        }

        if (!bifurcationIdFound)
            continue;

        // Function for connecting overlapping geometries with current
        // bifurcation
        const auto connectGeometriesToBifurcation =
            [&](const std::vector<size_t>& geometries) {
                const auto& bifGeom =
                    sdfMorphologyData.geometries[bifurcationId];

                for (size_t geomIdx : geometries)
                {
                    // Do not blend yourself
                    if (geomIdx == bifurcationId)
                        continue;

                    const auto& geom = sdfMorphologyData.geometries[geomIdx];
                    const double dist0 = glm::distance2(geom.p0, bifGeom.p0);
                    const double dist1 = glm::distance2(geom.p1, bifGeom.p0);
                    const double radiusSum = geom.r0 + bifGeom.r0;
                    const double radiusSumSq = radiusSum * radiusSum;

                    if (dist0 < radiusSumSq || dist1 < radiusSumSq)
                    {
                        sdfMorphologyData.neighbours[bifurcationId].insert(
                            geomIdx);
                        sdfMorphologyData.neighbours[geomIdx].insert(
                            bifurcationId);
                    }
                }
            };

        // Connect all child sections
        for (const size_t sectionChild : mts.sectionChildren[section])
        {
            connectGeometriesToBifurcation(
                sdfMorphologyData.sectionGeometries.at(sectionChild));
        }

        // Connect with own section
        connectGeometriesToBifurcation(
            sdfMorphologyData.sectionGeometries.at(section));
    }
}

void MorphologyLoader::_finalizeSDFGeometries(
    ParallelModelContainer& modelContainer,
    SDFMorphologyData& sdfMorphologyData) const
{
    const size_t numGeoms = sdfMorphologyData.geometries.size();
    sdfMorphologyData.localToGlobalIdx.resize(numGeoms, 0);

    // Extend neighbours to make sure smoothing is applied on all
    // closely connected geometries
    for (size_t rep = 0; rep < 4; rep++)
    {
        const size_t numNeighs = sdfMorphologyData.neighbours.size();
        auto neighsCopy = sdfMorphologyData.neighbours;
        for (size_t i = 0; i < numNeighs; i++)
        {
            for (size_t j : sdfMorphologyData.neighbours[i])
            {
                for (size_t newNei : sdfMorphologyData.neighbours[j])
                {
                    neighsCopy[i].insert(newNei);
                    neighsCopy[newNei].insert(i);
                }
            }
        }
        sdfMorphologyData.neighbours = neighsCopy;
    }

    for (size_t i = 0; i < numGeoms; i++)
    {
        // Convert neighbours from set to vector and erase itself from its
        // neighbours
        std::vector<size_t> neighbours;
        const auto& neighSet = sdfMorphologyData.neighbours[i];
        std::copy(neighSet.begin(), neighSet.end(),
                  std::back_inserter(neighbours));
        neighbours.erase(std::remove_if(neighbours.begin(), neighbours.end(),
                                        [i](size_t elem) { return elem == i; }),
                         neighbours.end());

        modelContainer.addSDFGeometry(sdfMorphologyData.materials[i],
                                      sdfMorphologyData.geometries[i],
                                      neighbours);
    }
}

MorphologyTreeStructure MorphologyLoader::_calculateMorphologyTreeStructure(
    const brayns::PropertyMap& properties,
    const brain::neuron::Sections& sections) const
{
    const size_t numSections = sections.size();

    const auto dampenBranchThicknessChangerate = properties.getProperty<bool>(
        PROP_DAMPEN_BRANCH_THICKNESS_CHANGERATE.name);
    if (!dampenBranchThicknessChangerate)
    {
        MorphologyTreeStructure mts;
        mts.sectionTraverseOrder.resize(numSections);
        mts.sectionParent.resize(numSections, -1);
        std::iota(mts.sectionTraverseOrder.begin(),
                  mts.sectionTraverseOrder.end(), 0);
        return mts;
    }

    std::vector<std::pair<double, brayns::Vector3f>> bifurcationPosition(
        numSections,
        std::make_pair<double, brayns::Vector3f>(0.0f, {0.f, 0.f, 0.f}));

    std::vector<std::pair<double, brayns::Vector3f>> sectionEndPosition(
        numSections,
        std::make_pair<double, brayns::Vector3f>(0.0f, {0.f, 0.f, 0.f}));

    std::vector<std::vector<size_t>> sectionChildren(numSections,
                                                     std::vector<size_t>());

    std::vector<int> sectionParent(numSections, -1);
    std::vector<bool> skipSection(numSections, true);
    std::vector<bool> addedSection(numSections, false);

    // Find section bifurcations and end positions
    for (size_t sectionI = 0; sectionI < numSections; sectionI++)
    {
        const auto& section = sections[sectionI];

        if (section.getType() == brain::neuron::SectionType::soma)
            continue;

        const auto& samples = section.getSamples();
        if (samples.empty())
            continue;

        skipSection[sectionI] = false;

        { // Branch beginning
            const auto& sample = samples[0];

            const auto radius =
                 _getCorrectedRadius(properties, sample.w * 0.5f);

            const brayns::Vector3f position(sample);

            bifurcationPosition[sectionI].first = radius;
            bifurcationPosition[sectionI].second = position;
        }

        { // Branch end
            const auto& sample = samples.back();
            const auto radius = _getCorrectedRadius(properties, sample.w * 0.5f);
            const brayns::Vector3f position(sample);
            sectionEndPosition[sectionI].first = radius;
            sectionEndPosition[sectionI].second = position;
        }
    }

    const auto overlaps = [](const std::pair<double, brayns::Vector3f>& p0,
                             const std::pair<double, brayns::Vector3f>& p1) {
        const double d = (p0.second - p1.second).length();
        const double r = p0.first + p1.first;

        return (d < r);
    };

    // Find overlapping section bifurcations and end positions
    for (size_t sectionI = 0; sectionI < numSections; sectionI++)
    {
        if (skipSection[sectionI])
            continue;

        for (size_t sectionJ = sectionI + 1; sectionJ < numSections; sectionJ++)
        {
            if (skipSection[sectionJ])
                continue;

            if (overlaps(bifurcationPosition[sectionJ],
                         sectionEndPosition[sectionI]))
            {
                if (sectionParent[sectionJ] == -1)
                {
                    sectionChildren[sectionI].push_back(sectionJ);
                    sectionParent[sectionJ] = static_cast<size_t>(sectionI);
                }
            }
            else if (overlaps(bifurcationPosition[sectionI],
                              sectionEndPosition[sectionJ]))
            {
                if (sectionParent[sectionI] == -1)
                {
                    sectionChildren[sectionJ].push_back(sectionI);
                    sectionParent[sectionI] = static_cast<size_t>(sectionJ);
                }
            }
        }
    }

    // Fill stack with root sections
    std::vector<size_t> sectionStack;
    for (size_t sectionI = 0; sectionI < numSections; sectionI++)
    {
        if (skipSection[sectionI])
            continue;
        else if (sectionParent[sectionI] == -1)
            sectionStack.push_back(sectionI);
    }

    // Starting from the roots fill the tree traversal order
    std::vector<size_t> sectionOrder;
    while (!sectionStack.empty())
    {
        const size_t sectionI = sectionStack.back();
        sectionStack.pop_back();
        assert(!addedSection[sectionI]);
        addedSection[sectionI] = true;

        sectionOrder.push_back(sectionI);
        for (const size_t childI : sectionChildren[sectionI])
            sectionStack.push_back(childI);
    }

    MorphologyTreeStructure mts;
    mts.sectionTraverseOrder = std::move(sectionOrder);
    mts.sectionParent = std::move(sectionParent);
    mts.sectionChildren = std::move(sectionChildren);
    return mts;
}

void MorphologyLoader::_addSomaGeometry(const brayns::PropertyMap& properties,
                                        const brain::neuron::Soma& soma,
                                        uint64_t offset,
                                        ParallelModelContainer& model,
                                        SDFMorphologyData& sdfMorphologyData,
                                        const bool /*useSimulationModel*/) const
{
    const size_t materialId =
        _getMaterialIdFromColorScheme(properties,
                                      brain::neuron::SectionType::soma);
    model.morphologyInfo.somaPosition = soma.getCentroid();
    const double somaRadius =
        _getCorrectedRadius(properties, soma.getMeanRadius());
    const auto& children = soma.getChildren();
    const bool useSDFGeometry =
        properties.getProperty<bool>(PROP_USE_SDF_GEOMETRY.name);

    if (useSDFGeometry)
        _connectSDFSomaChildren(properties, model.morphologyInfo.somaPosition,
                                somaRadius, materialId, offset, children,
                                sdfMorphologyData);
    else
    {
        model.addSphere(materialId, {model.morphologyInfo.somaPosition,
                                     static_cast<float>(somaRadius), offset});
        //        if (useSimulationModel)
        {
            // When using a simulation model, parametric geometries
            // must occupy as much space as possible in the mesh.
            // This code inserts a Cone between the soma and the
            // beginning of each branch.
            for (const auto& child : children)
            {
                const auto& samples = child.getSamples();
                const brayns::Vector3f sample(samples[0]);

                model.morphologyInfo.bounds.merge(sample);
                const double sampleRadius =
                    _getCorrectedRadius(properties, samples[0].w * 0.5f);

                model.addCone(materialId,
                              {model.morphologyInfo.somaPosition, sample,
                               static_cast<float>(somaRadius),
                               static_cast<float>(sampleRadius), offset});
            }
        }
    }
}

void MorphologyLoader::_addStepSphereGeometry(
    const bool useSDFGeometries, const bool isDone,
    const brayns::Vector3f& position, const double radius,
    const size_t materialId, const uint64_t& userDataOffset,
    ParallelModelContainer& model, const size_t section,
    SDFMorphologyData& sdfMorphologyData) const
{
    model.morphologyInfo.bounds.merge(position);
    if (useSDFGeometries)
    {
        if (isDone)
        {
            // Since our cone pills already give us a sphere at the end
            // points we don't need to add any sphere between segments
            // except at the bifurcation

            const size_t idx =
                _addSDFGeometry(sdfMorphologyData,
                                brayns::createSDFSphere(position, radius,
                                                        userDataOffset),
                                {}, materialId, section);

            sdfMorphologyData.bifurcationIndices.push_back(idx);
        }
    }
    else
        model.addSphere(materialId,
                        {position, static_cast<float>(radius), userDataOffset});
}

void MorphologyLoader::_addStepConeGeometry(
    const bool useSDFGeometries, const brayns::Vector3f& position,
    const double radius, const brayns::Vector3f& target,
    const double previousRadius, const size_t materialId,
    const uint64_t& userDataOffset, ParallelModelContainer& model,
    const size_t section, SDFMorphologyData& sdfMorphologyData) const
{
    model.morphologyInfo.bounds.merge(position);
    model.morphologyInfo.bounds.merge(target);
    if (useSDFGeometries)
    {
        const auto geom =
            (almost_equal(radius, previousRadius, 100000))
                ? brayns::createSDFPill(position, target, radius,
                                        userDataOffset)
                : brayns::createSDFConePill(position, target, radius,
                                            previousRadius, userDataOffset);
        _addSDFGeometry(sdfMorphologyData, geom, {}, materialId, section);
    }
    else if (almost_equal(radius, previousRadius, 100000))
        model.addCylinder(materialId,
                          {position, target, static_cast<float>(radius),
                           userDataOffset});
    else
        model.addCone(materialId,
                      {position, target, static_cast<float>(radius),
                       static_cast<float>(previousRadius), userDataOffset});
}

float MorphologyLoader::_distanceToSoma(const brain::neuron::Section& section,
                                        const size_t sampleId) const
{
    float distance = 0.0;
    if (sampleId > 0)
    {
        const auto& samples = section.getSamples();
        for (size_t i = 0; i < std::min(samples.size(), sampleId) - 1; ++i)
        {
            const auto& a = samples[i];
            const auto& b = samples[i + 1];
            distance = distance + (b - a).length();
        }
    }

    auto s = section;
    while (s.hasParent())
    {
        s = s.getParent();

        const auto& samples = s.getSamples();
        for (size_t i = 0; i < samples.size() - 1; ++i)
        {
            const auto& a = samples[i];
            const auto& b = samples[i + 1];
            distance = distance + (b - a).length();
        }
    }

    return distance * 10.f; // Since user data is uint64_t, we multiply by
                            // 10 to increase the precision of the growth
                            // (first decimal value will then be considered)
}

void MorphologyLoader::_importMorphologyFromURI(
    const brayns::PropertyMap& properties, const brion::URI& uri,
    const uint64_t index, const brayns::Matrix4f& /*transformation*/,
    CompartmentReportPtr compartmentReport, ParallelModelContainer& model,
    brain::Synapses* /*afferentSynapses*/,
    brain::Synapses* /*efferentSynapses*/) const
{
    SDFMorphologyData sdfMorphologyData;

    brain::neuron::Morphology morphology(uri);

    // Soma
    const auto sectionTypes = getSectionTypesFromProperties(properties);
    const auto useRealisticSoma =
        properties.getProperty<bool>(PROP_USE_REALISTIC_SOMA.name);
    const auto morphologyQuality = stringToEnum<MorphologyQuality>(
        properties.getProperty<std::string>(PROP_MORPHOLOGY_QUALITY.name));
    const auto userDataType = stringToEnum<UserDataType>(
        properties.getProperty<std::string>(PROP_USER_DATA_TYPE.name));
    const auto useSDFGeometry =
        properties.getProperty<bool>(PROP_USE_SDF_GEOMETRY.name);
    const auto dampenBranchThicknessChangerate = properties.getProperty<bool>(
        PROP_DAMPEN_BRANCH_THICKNESS_CHANGERATE.name);
    const auto maxDistanceToSoma = properties.getProperty<double>(
        PROP_MORPHOLOGY_MAX_DISTANCE_TO_SOMA.name);

    // If there is no compartment report, the offset in the simulation
    // buffer is the index of the morphology in the circuit
    uint64_t userDataOffset = index;
    if (compartmentReport)
        userDataOffset = compartmentReport->getOffsets()[index][0];

    if (!useRealisticSoma &&
        std::find(sectionTypes.begin(), sectionTypes.end(),
                  brain::neuron::SectionType::soma) != sectionTypes.end())
    {
        _addSomaGeometry(properties, morphology.getSoma(), userDataOffset,
                         model, sdfMorphologyData,
                         compartmentReport != nullptr);
    }

    // Only the first one or two axon sections are reported, so find the
    // last one and use its offset for all the other axon sections
    uint16_t lastAxon = 0;
    if (compartmentReport &&
        std::find(sectionTypes.begin(), sectionTypes.end(),
                  brain::neuron::SectionType::axon) != sectionTypes.end())
    {
        const auto& counts = compartmentReport->getCompartmentCounts()[index];
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

    const auto& sections = morphology.getSections(sectionTypes);
    const auto morphologyTree =
        _calculateMorphologyTreeStructure(properties, sections);
    std::vector<double> sectionEndRadius(sections.size(), -1.0f);

    // Dendrites and axon
    for (const size_t sectionI : morphologyTree.sectionTraverseOrder)
    {
        const auto& section = sections[sectionI];

        if (section.getType() == brain::neuron::SectionType::soma)
            continue;

        const auto materialId =
            _getMaterialIdFromColorScheme(properties, section.getType());
        const auto& samples = section.getSamples();
        if (samples.empty())
            continue;

        const size_t numSamples = samples.size();

        auto previousSample = samples[0];
        size_t step = 1;
        switch (morphologyQuality)
        {
        case MorphologyQuality::low:
            step = numSamples - 1;
            break;
        case MorphologyQuality::medium:
            step = numSamples / 2;
            step = (step == 0) ? 1 : step;
            break;
        default:
            step = 1;
        }

        double segmentStep = 0;
        if (compartmentReport)
        {
            const auto& counts =
                compartmentReport->getCompartmentCounts()[index];
            // Number of compartments usually differs from number of samples
            segmentStep = counts[section.getID()] / double(numSamples);
        }

        auto previousRadius =
            _getCorrectedRadius(properties,
                                section.hasParent()
                                    ? _getLastSampleRadius(section.getParent())
                                    : samples[0].w * 0.5f);

        bool done = false;
        for (size_t i = step; !done && i < numSamples + step; i += step)
        {
            if (i >= (numSamples - 1))
            {
                i = numSamples - 1;
                done = true;
            }

            const auto distanceToSoma = _distanceToSoma(section, i);
            if (distanceToSoma > maxDistanceToSoma)
                continue;

            model.morphologyInfo.maxDistanceToSoma =
                std::max(model.morphologyInfo.maxDistanceToSoma,
                         distanceToSoma);

            switch (userDataType)
            {
            case UserDataType::distance_to_soma:
                userDataOffset = distanceToSoma;
                break;

            case UserDataType::simulation_offset:
                if (compartmentReport)
                {
                    const auto& offsets =
                        compartmentReport->getOffsets()[index];
                    const auto& counts =
                        compartmentReport->getCompartmentCounts()[index];

                    // Update the offset if we have enough compartments aka
                    // a full compartment report. Otherwise we keep the soma
                    // offset which happens for soma reports and use this
                    // for all the sections
                    if (section.getID() < counts.size())
                    {
                        if (counts[section.getID()] > 0)
                            userDataOffset = offsets[section.getID()] +
                                             double(i - step) * segmentStep;
                        else
                        {
                            if (section.getType() ==
                                brain::neuron::SectionType::axon)
                                userDataOffset = offsets[lastAxon];
                            else
                                // This should never happen, but just in
                                // case use an invalid value to show an
                                // error color
                                userDataOffset =
                                    std::numeric_limits<uint64_t>::max();
                        }
                    }
                }
                break;
            case UserDataType::undefined:
                userDataOffset = index;
                break;
            }

            const auto sample = samples[i];
            brayns::Vector3f position(sample);
            brayns::Vector3f target(previousSample);


            model.morphologyInfo.bounds.merge(position);
            model.morphologyInfo.bounds.merge(target);

            auto radius =
                _getCorrectedRadius(properties, samples[i].w * 0.5f);
            const double maxRadiusChange = 0.1f;

            const double dist = glm::length(target - position);
            if (dist > 0.0001f && i != samples.size() - 1 &&
                dampenBranchThicknessChangerate)
            {
                const double radiusChange =
                    std::min(std::abs(previousRadius - radius),
                             dist * maxRadiusChange);
                if (radius < previousRadius)
                    radius = previousRadius - radiusChange;
                else
                    radius = previousRadius + radiusChange;
            }

            if (radius > 0.f)
            {
                _addStepSphereGeometry(useSDFGeometry, done, position, radius,
                                       materialId, userDataOffset, model,
                                       sectionI, sdfMorphologyData);

                if (position != target && previousRadius > 0.f)
                    _addStepConeGeometry(useSDFGeometry, position, radius,
                                         target, previousRadius, materialId,
                                         userDataOffset, model, sectionI,
                                         sdfMorphologyData);
            }
            previousSample = sample;
            previousRadius = radius;
            sectionEndRadius[sectionI] = radius;
        }
    }

    if (useSDFGeometry)
    {
        _connectSDFBifurcations(sdfMorphologyData, morphologyTree);
        _finalizeSDFGeometries(model, sdfMorphologyData);
    }
}

size_t MorphologyLoader::_getMaterialIdFromColorScheme(
    const brayns::PropertyMap& properties,
    const brain::neuron::SectionType& sectionType) const
{
    if (_defaultMaterialId != brayns::NO_MATERIAL)
        return _defaultMaterialId;

    size_t materialId = 0;
    const auto colorScheme = stringToEnum<MorphologyColorScheme>(
        properties.getProperty<std::string>(PROP_MORPHOLOGY_COLOR_SCHEME.name));
    switch (colorScheme)
    {
    case MorphologyColorScheme::neuron_by_segment_type:
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
        materialId = 0;
    }
    return materialId;
}

brayns::ModelDescriptorPtr MorphologyLoader::importFromBlob(
    brayns::Blob&& /*blob*/, const brayns::LoaderProgress& /*callback*/,
    const brayns::PropertyMap& /*properties*/) const
{
    throw std::runtime_error("Load morphology from memory not supported");
}

brayns::ModelDescriptorPtr MorphologyLoader::importFromFile(
    const std::string& fileName, const brayns::LoaderProgress& /*callback*/,
    const brayns::PropertyMap& properties) const
{
    // TODO: This needs to be done to work around wrong types coming from
    // the UI
    brayns::PropertyMap props = _defaults;
    props.merge(properties);
    // TODO: This needs to be done to work around wrong types coming from
    // the UI

    auto model = _scene.createModel();
    importMorphology(props, brion::URI(fileName), *model, 0);
    createMissingMaterials(*model);

    auto modelDescriptor =
        std::make_shared<brayns::ModelDescriptor>(std::move(model), fileName);
    return modelDescriptor;
}

brayns::PropertyMap MorphologyLoader::getProperties() const
{
    return _defaults;
}

brayns::PropertyMap MorphologyLoader::getCLIProperties()
{
    brayns::PropertyMap pm("MorphologyLoader");
    pm.setProperty(PROP_RADIUS_MULTIPLIER);
    pm.setProperty(PROP_RADIUS_CORRECTION);
    pm.setProperty(PROP_SECTION_TYPE_SOMA);
    pm.setProperty(PROP_SECTION_TYPE_AXON);
    pm.setProperty(PROP_SECTION_TYPE_DENDRITE);
    pm.setProperty(PROP_SECTION_TYPE_APICAL_DENDRITE);
    pm.setProperty(PROP_USE_SDF_GEOMETRY);
    pm.setProperty(PROP_DAMPEN_BRANCH_THICKNESS_CHANGERATE);
    pm.setProperty(PROP_USE_REALISTIC_SOMA);
    pm.setProperty(PROP_METABALLS_SAMPLES_FROM_SOMA);
    pm.setProperty(PROP_METABALLS_GRID_SIZE);
    pm.setProperty(PROP_METABALLS_THRESHOLD);
    pm.setProperty(PROP_USER_DATA_TYPE);
    pm.setProperty(PROP_MORPHOLOGY_COLOR_SCHEME);
    pm.setProperty(PROP_MORPHOLOGY_QUALITY);
    pm.setProperty(PROP_MORPHOLOGY_MAX_DISTANCE_TO_SOMA);
    return pm;
}

const brain::neuron::SectionTypes
    MorphologyLoader::getSectionTypesFromProperties(
        const brayns::PropertyMap& properties)
{
    brain::neuron::SectionTypes sectionTypes;
    if (properties.getProperty<bool>(PROP_SECTION_TYPE_SOMA.name))
        sectionTypes.push_back(brain::neuron::SectionType::soma);
    if (properties.getProperty<bool>(PROP_SECTION_TYPE_AXON.name))
        sectionTypes.push_back(brain::neuron::SectionType::axon);
    if (properties.getProperty<bool>(PROP_SECTION_TYPE_DENDRITE.name))
        sectionTypes.push_back(brain::neuron::SectionType::dendrite);
    if (properties.getProperty<bool>(PROP_SECTION_TYPE_APICAL_DENDRITE.name))
        sectionTypes.push_back(brain::neuron::SectionType::apicalDendrite);
    return sectionTypes;
}

void MorphologyLoader::createMissingMaterials(
    brayns::Model& model, const brayns::PropertyMap& properties)
{
    std::set<size_t> materialIds;
    for (auto& spheres : model.getSpheres())
        materialIds.insert(spheres.first);
    for (auto& cylinders : model.getCylinders())
        materialIds.insert(cylinders.first);
    for (auto& cones : model.getCones())
        materialIds.insert(cones.first);
    for (auto& meshes : model.getTriangleMeshes())
        materialIds.insert(meshes.first);
    for (auto& sdfGeometries : model.getSDFGeometryData().geometryIndices)
        materialIds.insert(sdfGeometries.first);

    auto materials = model.getMaterials();
    for (const auto materialId : materialIds)
    {
        const auto it = materials.find(materialId);
        if (it == materials.end())
            model.createMaterial(materialId, std::to_string(materialId),
                                 properties);
    }

    auto simulationHandler = model.getSimulationHandler();
    if (simulationHandler)
        for (const auto& material : materials)
            simulationHandler->bind(material.second);
}
