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

#include "MorphologyLoader.h"
#include "common.h"

#include <brayns/common/material/Material.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/types.h>
#include <brayns/common/utils/Utils.h>

#include <brayns/io/algorithms/MetaballsGenerator.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <boost/filesystem.hpp>

#include <unordered_map>

namespace
{
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
}

namespace brayns
{
class MorphologyLoader::Impl
{
public:
    Impl(const GeometryParameters& geometryParameters)
        : _geometryParameters(geometryParameters)
    {
    }

    /**
     * @brief importMorphology imports a single morphology from a specified URI
     * @param uri URI of the morphology
     * @param index Index of the morphology
     * @param defaultMaterialId Material to use
     * @param transformation Transformation to apply to the morphology
     * @param compartmentReport Compartment report to map to the morphology
     * @return Position of the soma
     */
    Vector3f importMorphology(
        const servus::URI& source, Model& model, const uint64_t index,
        const Matrix4f& transformation,
        const size_t defaultMaterialId = NO_MATERIAL,
        CompartmentReportPtr compartmentReport = nullptr) const
    {
        Vector3f somaPosition;
        auto materialFunc = [
            defaultMaterialId,
            colorScheme = _geometryParameters.getColorScheme(), index
        ](auto sectionType)
        {
            if (defaultMaterialId != NO_MATERIAL)
                return defaultMaterialId;

            size_t materialId = 0;
            switch (colorScheme)
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
                materialId = 0;
            }
            return materialId;
        };

        ParallelModelContainer modelContainer;
        somaPosition =
            importMorphology(source, index, materialFunc, transformation,
                             compartmentReport, modelContainer);

        modelContainer.addSpheresToModel(model);
        modelContainer.addCylindersToModel(model);
        modelContainer.addConesToModel(model);
        modelContainer.addSDFGeometriesToModel(model);

        model.createMissingMaterials();
        return somaPosition;
    }

    Vector3f importMorphology(const servus::URI& source, const uint64_t index,
                              MaterialFunc materialFunc,
                              const Matrix4f& transformation,
                              CompartmentReportPtr compartmentReport,
                              ParallelModelContainer& model) const
    {
        if (_geometryParameters.getMorphologySectionTypes() ==
            std::vector<MorphologySectionType>{MorphologySectionType::soma})
        {
            return _importMorphologyAsPoint(index, materialFunc, transformation,
                                            compartmentReport, model);
        }
        else if (_geometryParameters.useRealisticSomas())
        {
            return _createRealisticSoma(source, materialFunc, transformation,
                                        model);
        }
        else
        {
            return _importMorphologyFromURI(source, index, materialFunc,
                                            transformation, compartmentReport,
                                            model);
        }
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
     * @brief _importMorphologyAsPoint places sphere at the specified morphology
     * position
     * @param index Index of the current morphology
     * @param transformation Transformation to apply to the morphology
     * @param material Material that is forced in case geometry parameters do
     * not apply
     * @param compartmentReport Compartment report to map to the morphology
     * @param scene Scene to which the morphology should be loaded into
     * @return Position of the soma
     */
    Vector3f _importMorphologyAsPoint(const uint64_t index,
                                      MaterialFunc materialFunc,
                                      const Matrix4f& transformation,
                                      CompartmentReportPtr compartmentReport,
                                      ParallelModelContainer& model) const
    {
        uint64_t offset = 0;
        if (compartmentReport)
            offset = compartmentReport->getOffsets()[index][0];

        const auto radius = _geometryParameters.getRadiusMultiplier();
        const auto somaPosition = transformation.getTranslation();
        const auto materialId = materialFunc(brain::neuron::SectionType::soma);
        model.addSphere(materialId, {somaPosition, radius, offset});
        return somaPosition;
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
     * @return Position of the soma
     */
    Vector3f _createRealisticSoma(const servus::URI& uri,
                                  MaterialFunc materialFunc,
                                  const Matrix4f& transformation,
                                  ParallelModelContainer& model) const
    {
        Vector3f somaPosition;
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
            somaPosition = soma.getCentroid();
            const auto radius = _getCorrectedRadius(soma.getMeanRadius());
            metaballs.push_back(
                {somaPosition.x(), somaPosition.y(), somaPosition.z(), radius});
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
            }
        }

        // Generate mesh from metaballs
        const auto gridSize = _geometryParameters.getMetaballsGridSize();
        const auto threshold = _geometryParameters.getMetaballsThreshold();
        MetaballsGenerator metaballsGenerator;
        const auto materialId = materialFunc(brain::neuron::SectionType::soma);
        metaballsGenerator.generateMesh(metaballs, gridSize, threshold,
                                        materialId, model.trianglesMeshes);
        return somaPosition;
    }

    struct SDFMorphologyData
    {
        std::vector<SDFGeometry> geometries;
        std::vector<std::set<size_t>> neighbours;
        std::vector<size_t> materials;
        std::vector<size_t> localToGlobalIdx;
        std::vector<size_t> bifurcationIndices;
        std::unordered_map<size_t, int> geometrySection;
        std::unordered_map<int, std::vector<size_t>> sectionGeometries;
    };

    struct MorphologyTreeStructure
    {
        std::vector<int> sectionParent;
        std::vector<std::vector<size_t>> sectionChildren;
        std::vector<size_t> sectionTraverseOrder;
    };

    size_t _addSDFGeometry(SDFMorphologyData& sdfMorphologyData,
                           const SDFGeometry& geometry,
                           const std::set<size_t>& neighbours,
                           const size_t materialId, const int section) const
    {
        const size_t idx = sdfMorphologyData.geometries.size();
        sdfMorphologyData.geometries.push_back(geometry);
        sdfMorphologyData.neighbours.push_back(neighbours);
        sdfMorphologyData.materials.push_back(materialId);
        sdfMorphologyData.geometrySection[idx] = section;
        sdfMorphologyData.sectionGeometries[section].push_back(idx);
        return idx;
    }

    /**
     * Creates an SDF soma by adding and connecting the soma children using cone
     * pills
     */
    void _connectSDFSomaChildren(const Vector3f& somaPosition,
                                 const float somaRadius,
                                 const size_t materialId, const uint64_t offset,
                                 const brain::neuron::Sections& somaChildren,
                                 SDFMorphologyData& sdfMorphologyData) const
    {
        std::set<size_t> child_indices;
        for (const auto& child : somaChildren)
        {
            const auto& samples = child.getSamples();
            const Vector3f sample{samples[0].x(), samples[0].y(),
                                  samples[0].z()};

            // Create a sigmoid cone with half of soma radius to center of soma
            // to give it an organic look.
            const float radiusEnd = _getCorrectedRadius(samples[0].w() * 0.5f);
            const size_t geomIdx =
                _addSDFGeometry(sdfMorphologyData,
                                createSDFConePillSigmoid(somaPosition, sample,
                                                         somaRadius * 0.5f,
                                                         radiusEnd, offset),
                                {}, materialId, -1);
            child_indices.insert(geomIdx);
        }

        for (size_t c : child_indices)
            sdfMorphologyData.neighbours[c] = child_indices;
    }

    /**
     * Goes through all bifurcations and connects to all connected SDF
     * geometries it is overlapping. Every section that has a bifurcation will
     * traverse its children and blend the geometries inside the bifurcation.
     */
    void _connectSDFBifurcations(SDFMorphologyData& sdfMorphologyData,
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
                const int bifSection =
                    sdfMorphologyData.geometrySection.at(bifId);

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
            const auto connectGeometriesToBifurcation = [&](
                const std::vector<size_t>& geometries) {
                const auto& bifGeom =
                    sdfMorphologyData.geometries[bifurcationId];

                for (size_t geomIdx : geometries)
                {
                    // Do not blend yourself
                    if (geomIdx == bifurcationId)
                        continue;

                    const auto& geom = sdfMorphologyData.geometries[geomIdx];
                    const float dist0 =
                        geom.p0.squared_distance(bifGeom.center);
                    const float dist1 =
                        geom.p1.squared_distance(bifGeom.center);
                    const float radiusSum = geom.radius + bifGeom.radius;
                    const float radiusSumSq = radiusSum * radiusSum;

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

    /**
     * Calculates all neighbours and adds the geometries to the model container.
     */
    void _finalizeSDFGeometries(ParallelModelContainer& modelContainer,
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
            neighbours.erase(
                std::remove_if(neighbours.begin(), neighbours.end(),
                               [i](size_t elem) { return elem == i; }),
                neighbours.end());

            modelContainer.addSDFGeometry(sdfMorphologyData.materials[i],
                                          sdfMorphologyData.geometries[i],
                                          neighbours);
        }
    }

    /**
     * Calculates the structure of the morphology tree by finding overlapping
     * beginnings and endings of the sections.
     */
    MorphologyTreeStructure _calculateMorphologyTreeStructure(
        const brain::neuron::Sections& sections,
        const bool dampenThickness) const
    {
        const size_t numSections = sections.size();

        if (!dampenThickness)
        {
            MorphologyTreeStructure mts;
            mts.sectionTraverseOrder.resize(numSections);
            mts.sectionParent.resize(numSections, -1);
            std::iota(mts.sectionTraverseOrder.begin(),
                      mts.sectionTraverseOrder.end(), 0);
            return mts;
        }

        std::vector<std::pair<float, Vector3f>> bifurcationPosition(
            numSections,
            std::make_pair<float, Vector3f>(0.0f, Vector3f(0.f, 0.f, 0.f)));

        std::vector<std::pair<float, Vector3f>> sectionEndPosition(
            numSections,
            std::make_pair<float, Vector3f>(0.0f, Vector3f(0.f, 0.f, 0.f)));

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

                const auto radius = _getCorrectedRadius(sample.w() * 0.5f);

                const Vector3f position(sample.x(), sample.y(), sample.z());

                bifurcationPosition[sectionI].first = radius;
                bifurcationPosition[sectionI].second = position;
            }

            { // Branch end
                const auto& sample = samples.back();

                const auto radius = _getCorrectedRadius(sample.w() * 0.5f);

                const Vector3f position(sample.x(), sample.y(), sample.z());

                sectionEndPosition[sectionI].first = radius;
                sectionEndPosition[sectionI].second = position;
            }
        }

        const auto overlaps = [](const std::pair<float, Vector3f>& p0,
                                 const std::pair<float, Vector3f>& p1) {

            const float d = (p0.second - p1.second).length();
            const float r = p0.first + p1.first;

            return (d < r);
        };

        // Find overlapping section bifurcations and end positions
        for (size_t sectionI = 0; sectionI < numSections; sectionI++)
        {
            if (skipSection[sectionI])
                continue;

            for (size_t sectionJ = sectionI + 1; sectionJ < numSections;
                 sectionJ++)
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

    /**
     * Adds a Soma geometry to the model
     */
    void _addSomaGeometry(const brain::neuron::Soma& soma,
                          const Vector3f& translation, uint64_t offset,
                          bool useSDFGeometries, MaterialFunc materialFunc,
                          ParallelModelContainer& model,
                          SDFMorphologyData& sdfMorphologyData) const
    {
        const size_t materialId =
            materialFunc(brain::neuron::SectionType::soma);
        const auto somaPosition = soma.getCentroid() + translation;
        const auto somaRadius = _getCorrectedRadius(soma.getMeanRadius());
        const auto& children = soma.getChildren();

        if (useSDFGeometries)
        {
            _connectSDFSomaChildren(somaPosition, somaRadius, materialId,
                                    offset, children, sdfMorphologyData);
        }
        else
        {
            model.addSphere(materialId, {somaPosition, somaRadius, offset});

            if (_geometryParameters.getCircuitUseSimulationModel())
            {
                // When using a simulation model, parametric geometries
                // must occupy as much space as possible in the mesh.
                // This code inserts a Cone between the soma and the
                // beginning of each branch.
                for (const auto& child : children)
                {
                    const auto& samples = child.getSamples();
                    const Vector3f sample{samples[0].x(), samples[0].y(),
                                          samples[0].z()};
                    const float sampleRadius =
                        _getCorrectedRadius(samples[0].w() * 0.5f);

                    model.addCone(materialId, {somaPosition, sample, somaRadius,
                                               sampleRadius, offset});
                }
            }
        }
    }

    /**
     * Adds the sphere between the steps in the sections
     */
    void _addStepSphereGeometry(const bool useSDFGeometries, const bool isDone,
                                const Vector3f& position, const float radius,
                                const size_t materialId, const uint64_t offset,
                                ParallelModelContainer& model,
                                const size_t section,
                                SDFMorphologyData& sdfMorphologyData) const
    {
        if (useSDFGeometries)
        {
            if (isDone)
            {
                // Since our cone pills already give us a sphere
                // at the end points we don't need to add any
                // sphere between segments except at the
                // bifurcation
                const size_t idx =
                    _addSDFGeometry(sdfMorphologyData,
                                    createSDFSphere(position, radius, offset),
                                    {}, materialId, section);

                sdfMorphologyData.bifurcationIndices.push_back(idx);
            }
        }
        else
            model.addSphere(materialId, {position, radius, offset});
    }

    /**
     * Adds the cone between the steps in the sections
     */
    void _addStepConeGeometry(const bool useSDFGeometries,
                              const Vector3f& position, const float radius,
                              const Vector3f& target,
                              const float previousRadius,
                              const size_t materialId, const uint64_t offset,
                              ParallelModelContainer& model,
                              const size_t section,
                              SDFMorphologyData& sdfMorphologyData) const
    {
        if (useSDFGeometries)
        {
            const auto geom =
                (almost_equal(radius, previousRadius, 100000))
                    ? createSDFPill(position, target, radius, offset)
                    : createSDFConePill(position, target, radius,
                                        previousRadius, offset);
            _addSDFGeometry(sdfMorphologyData, geom, {}, materialId, section);
        }
        else
        {
            if (almost_equal(radius, previousRadius, 100000))
                model.addCylinder(materialId,
                                  {position, target, radius, offset});
            else
                model.addCone(materialId, {position, target, radius,
                                           previousRadius, offset});
        }
    }

    /**
       * @brief _importMorphologyFromURI imports a morphology from the specified
       * URI
       * @param uri URI of the morphology
       * @param index Index of the current morphology
       * @param materialFunc A function mapping brain::neuron::SectionType to a
       * material id
       * @param transformation Transformation to apply to the morphology
       * @param compartmentReport Compartment report to map to the morphology
       * @param model Model container to which the morphology should be loaded
       * into
       * @return Position of the soma
       */
    Vector3f _importMorphologyFromURI(const servus::URI& uri,
                                      const uint64_t index,
                                      MaterialFunc materialFunc,
                                      const Matrix4f& transformation,
                                      CompartmentReportPtr compartmentReport,
                                      ParallelModelContainer& model) const
    {
        Vector3f somaPosition;
        Vector3f translation;

        const size_t morphologySectionTypes =
            enumsToBitmask(_geometryParameters.getMorphologySectionTypes());

        const bool dampenThickness =
            _geometryParameters.getMorphologyDampenBranchThicknessChangerate();

        const bool useSDFGeometries =
            _geometryParameters.getMorphologyUseSDFGeometries();

        SDFMorphologyData sdfMorphologyData;

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
        somaPosition = morphology.getSoma().getCentroid() + translation;
        if (!_geometryParameters.useRealisticSomas() &&
            morphologySectionTypes &
                static_cast<size_t>(MorphologySectionType::soma))
        {
            _addSomaGeometry(morphology.getSoma(), translation, offset,
                             useSDFGeometries, materialFunc, model,
                             sdfMorphologyData);
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

        float previousRadius = 0;
        const auto& sections = morphology.getSections(sectionTypes);
        const auto morphologyTree =
            _calculateMorphologyTreeStructure(sections, dampenThickness);
        std::vector<float> sectionEndRadius(sections.size(), -1.0f);

        // Dendrites and axon
        for (const size_t sectionI : morphologyTree.sectionTraverseOrder)
        {
            const auto& section = sections[sectionI];

            if (section.getType() == brain::neuron::SectionType::soma)
                continue;

            const auto materialId = materialFunc(section.getType());
            const auto& samples = section.getSamples();
            if (samples.empty())
                continue;

            const size_t numSamples = samples.size();

            auto previousSample = samples[0];
            size_t step = 1;
            switch (_geometryParameters.getGeometryQuality())
            {
            case GeometryQuality::low:
                step = numSamples - 1;
                break;
            case GeometryQuality::medium:
                step = numSamples / 2;
                step = (step == 0) ? 1 : step;
                break;
            default:
                step = 1;
            }

            float segmentStep = 0.f;
            if (compartmentReport)
            {
                const auto& counts =
                    compartmentReport->getCompartmentCounts()[index];
                // Number of compartments usually differs from number of
                // samples
                segmentStep = counts[section.getID()] / float(numSamples);
            }

            const int sectionParent = morphologyTree.sectionParent[sectionI];

            bool resetRadius = false;
            if (sectionParent < 0)
            {
                resetRadius = true;
            }
            else
            {
                previousRadius = sectionEndRadius[sectionParent];
                assert(previousRadius >= 0.0f);
            }

            bool done = false;
            for (size_t i = step; !done && i < numSamples + step; i += step)
            {
                if (i >= (numSamples - 1))
                {
                    i = numSamples - 1;
                    done = true;
                }

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
                                offset = std::numeric_limits<uint64_t>::max();
                        }
                    }
                }

                const auto sample = samples[i];

                Vector3f position(sample.x(), sample.y(), sample.z());
                position += translation;
                Vector3f target(previousSample.x(), previousSample.y(),
                                previousSample.z());
                target += translation;
                float radius = _getCorrectedRadius(samples[i].w() * 0.5f);
                constexpr float maxRadiusChange = 0.1f;

                if (resetRadius)
                {
                    previousRadius =
                        _getCorrectedRadius(samples[i - step].w() * 0.5f);
                    resetRadius = false;
                }

                const float dist = (target - position).length();
                if (dist > 0.0001f && dampenThickness)
                {
                    const float radiusChange =
                        std::min(std::abs(previousRadius - radius),
                                 dist * maxRadiusChange);
                    if (radius < previousRadius)
                        radius = previousRadius - radiusChange;
                    else
                        radius = previousRadius + radiusChange;
                }

                if (radius > 0.f)
                {
                    _addStepSphereGeometry(useSDFGeometries, done, position,
                                           radius, materialId, offset, model,
                                           sectionI, sdfMorphologyData);

                    if (position != target && previousRadius > 0.f)
                    {
                        _addStepConeGeometry(useSDFGeometries, position, radius,
                                             target, previousRadius, materialId,
                                             offset, model, sectionI,
                                             sdfMorphologyData);
                    }
                }
                previousSample = sample;
                previousRadius = radius;
                sectionEndRadius[sectionI] = radius;
            }
        }

        if (useSDFGeometries)
        {
            _connectSDFBifurcations(sdfMorphologyData, morphologyTree);
            _finalizeSDFGeometries(model, sdfMorphologyData);
        }
        return somaPosition;
    }

private:
    const GeometryParameters& _geometryParameters;
};

MorphologyLoader::MorphologyLoader(Scene& scene,
                                   const GeometryParameters& geometryParameters)
    : Loader(scene)
    , _impl(new MorphologyLoader::Impl(geometryParameters))
{
}

MorphologyLoader::~MorphologyLoader()
{
}

bool MorphologyLoader::isSupported(const std::string& filename BRAYNS_UNUSED,
                                   const std::string& extension) const
{
    const std::set<std::string> types = {"h5", "swc"};
    return types.find(extension) != types.end();
}

ModelDescriptorPtr MorphologyLoader::importFromBlob(
    Blob&& /*blob*/, const LoaderProgress& /*callback*/, const size_t /*index*/,
    const size_t /*materialID*/) const
{
    throw std::runtime_error("Load morphology from memory not supported");
}

ModelDescriptorPtr MorphologyLoader::importFromFile(
    const std::string& fileName, const LoaderProgress& callback,
    const size_t index, const size_t defaultMaterialId BRAYNS_UNUSED) const
{
    const auto modelName = boost::filesystem::basename({fileName});
    callback.updateProgress("Loading " + modelName + " ...", 0.f);
    auto model = _scene.createModel();
    Vector3f somaPosition =
        importMorphology(servus::URI(fileName), *model, index, {});
    model->createMissingMaterials();
    callback.updateProgress("Loading " + modelName + " ...", 1.f);

    Transformation transformation;
    transformation.setRotationCenter(somaPosition);
    auto modelDescriptor =
        std::make_shared<ModelDescriptor>(std::move(model), fileName);
    modelDescriptor->setTransformation(transformation);
    return modelDescriptor;
}

Vector3f MorphologyLoader::importMorphology(
    const servus::URI& uri, Model& model, const size_t index,
    const Matrix4f& transformation) const
{
    return _impl->importMorphology(uri, model, index, transformation);
}

Vector3f MorphologyLoader::_importMorphology(
    const servus::URI& source, const uint64_t index, MaterialFunc materialFunc,
    const Matrix4f& transformation, CompartmentReportPtr compartmentReport,
    ParallelModelContainer& model) const
{
    return _impl->importMorphology(source, index, materialFunc, transformation,
                                   compartmentReport, model);
}
}
