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

#pragma once

#include <plugin/api/CircuitExplorerParams.h>

#include <brayns/common/geometry/SDFGeometry.h>
#include <brayns/common/loader/Loader.h>
#include <brayns/common/types.h>
#include <brayns/parameters/GeometryParameters.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <unordered_map>
#include <vector>

class AdvancedCircuitLoader;
struct ParallelModelContainer;
using GIDOffsets = std::vector<uint64_t>;
using CompartmentReportPtr = std::shared_ptr<brion::CompartmentReport>;

// SDF structures
struct SDFMorphologyData
{
    std::vector<brayns::SDFGeometry> geometries;
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

/** Loads morphologies from SWC and H5, and Circuit Config files */
class MorphologyLoader : public brayns::Loader
{
public:
    MorphologyLoader(brayns::Scene& scene, brayns::PropertyMap&& loaderParams);

    /** @copydoc Loader::getName */
    std::string getName() const final;

    /** @copydoc Loader::getSupportedExtensions */
    std::vector<std::string> getSupportedExtensions() const final;

    /** @copydoc Loader::isSupported */
    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;

    /** @copydoc Loader::getCLIProperties */
    static brayns::PropertyMap getCLIProperties();

    /** @copydoc Loader::getProperties */
    brayns::PropertyMap getProperties() const final;

    /** @copydoc Loader::importFromBlob */
    std::vector<brayns::ModelDescriptorPtr> importFromBlob(
        brayns::Blob&& blob, const brayns::LoaderProgress& callback,
        const brayns::PropertyMap& properties) const final;

    /** @copydoc Loader::importFromFile */
    std::vector<brayns::ModelDescriptorPtr> importFromFile(
        const std::string& filename, const brayns::LoaderProgress& callback,
        const brayns::PropertyMap& properties) const final;

    /**
     * @brief importMorphology imports a single morphology from a specified URI
     * @param uri URI of the morphology
     * @param index Index of the morphology
     * @param defaultMaterialId Material to use
     * @param compartmentReport Compartment report to map to the morphology
     * @return Information about the morphology
     */
    MorphologyInfo importMorphology(
        const brayns::PropertyMap& properties, const brion::URI& source,
        brayns::Model& model, const uint64_t index,
        const brayns::Matrix4f& transformation = brayns::Matrix4f(),
        brain::Synapses* afferentSynapses = nullptr,
        brain::Synapses* efferentSynapses = nullptr,
        CompartmentReportPtr compartmentReport = nullptr) const;

    /**
     * @brief setDefaultMaterialId Set the default material for the morphology
     * @param materialId Id of the default material for the morphology
     */
    void setDefaultMaterialId(const size_t materialId)
    {
        _defaultMaterialId = materialId;
    }

    /**
     * @brief createMissingMaterials Checks that all materials exist for
     * existing geometry in the model. Missing materials are created with the
     * default parameters
     */
    static void createMissingMaterials(
        brayns::Model& model, const brayns::PropertyMap& properties = {});

    static const brain::neuron::SectionTypes getSectionTypesFromProperties(
        const brayns::PropertyMap& properties);

private:
    /**
     * @brief _getCorrectedRadius Modifies the radius of the geometry according
     * to --radius-multiplier and --radius-correction geometry parameters
     * @param radius Radius to be corrected
     * @return Corrected value of a radius according to geometry parameters
     */
    double _getCorrectedRadius(const brayns::PropertyMap& properties,
                               const double radius) const;

    void _importMorphology(const brayns::PropertyMap& properties,
                           const brion::URI& source, const uint64_t index,
                           ParallelModelContainer& model,
                           const brayns::Matrix4f& transformation,
                           CompartmentReportPtr compartmentReport,
                           brain::Synapses* afferentSynapses = nullptr,
                           brain::Synapses* efferentSynapses = nullptr) const;

    /**
     * @brief _importMorphologyAsPoint places sphere at the specified morphology
     * position
     * @param index Index of the current morphology
     * @param material Material that is forced in case geometry parameters do
     * not apply
     * @param compartmentReport Compartment report to map to the morphology
     * @param scene Scene to which the morphology should be loaded into
     */
    void _importMorphologyAsPoint(const brayns::PropertyMap& properties,
                                  const uint64_t index,
                                  CompartmentReportPtr compartmentReport,
                                  ParallelModelContainer& model) const;

    /**
     * @brief _createRealisticSoma Creates a realistic soma using the metaballs
     * algorithm.
     * @param uri URI of the morphology for which the soma is created
     * @param index Index of the current morphology
     * @param material Material that is forced in case geometry parameters
     * do not apply
     * @param scene Scene to which the morphology should be loaded into
     */
    void _createRealisticSoma(const brayns::PropertyMap& properties,
                              const brion::URI& uri,
                              ParallelModelContainer& model) const;

    size_t _addSDFGeometry(SDFMorphologyData& sdfMorphologyData,
                           const brayns::SDFGeometry& geometry,
                           const std::set<size_t>& neighbours,
                           const size_t materialId, const int section) const;

    /**
     * Creates an SDF soma by adding and connecting the soma children using cone
     * pills
     */
    void _connectSDFSomaChildren(const brayns::PropertyMap& properties,
                                 const brayns::Vector3f& somaPosition,
                                 const double somaRadius,
                                 const size_t materialId,
                                 const uint64_t& userDataOffset,
                                 const brain::neuron::Sections& somaChildren,
                                 SDFMorphologyData& sdfMorphologyData) const;

    /**
     * Goes through all bifurcations and connects to all connected SDF
     * geometries it is overlapping. Every section that has a bifurcation will
     * traverse its children and blend the geometries inside the bifurcation.
     */
    void _connectSDFBifurcations(SDFMorphologyData& sdfMorphologyData,
                                 const MorphologyTreeStructure& mts) const;

    /**
     * Calculates all neighbours and adds the geometries to the model container.
     */
    void _finalizeSDFGeometries(ParallelModelContainer& modelContainer,
                                SDFMorphologyData& sdfMorphologyData) const;

    /**
     * Calculates the structure of the morphology tree by finding overlapping
     * beginnings and endings of the sections.
     */
    MorphologyTreeStructure _calculateMorphologyTreeStructure(
        const brayns::PropertyMap& properties,
        const brain::neuron::Sections& sections) const;

    /**
     * Adds a Soma geometry to the model
     */
    void _addSomaGeometry(const brayns::PropertyMap& properties,
                          const brain::neuron::Soma& soma, uint64_t offset,
                          ParallelModelContainer& model,
                          SDFMorphologyData& sdfMorphologyData,
                          const bool useSimulationModel) const;

    /**
     * Adds the sphere between the steps in the sections
     */
    void _addStepSphereGeometry(const bool useSDFGeometries, const bool isDone,
                                const brayns::Vector3f& position,
                                const double radius, const size_t materialId,
                                const uint64_t& userDataOffset,
                                ParallelModelContainer& model,
                                const size_t section,
                                SDFMorphologyData& sdfMorphologyData) const;

    /**
     * Adds the cone between the steps in the sections
     */
    void _addStepConeGeometry(
        const bool useSDFGeometries, const brayns::Vector3f& position,
        const double radius, const brayns::Vector3f& target,
        const double previousRadius, const size_t materialId,
        const uint64_t& userDataOffset, ParallelModelContainer& model,
        const size_t section, SDFMorphologyData& sdfMorphologyData) const;

    /**
     * @brief _importMorphologyFromURI imports a morphology from the specified
     * URI
     * @param uri URI of the morphology
     * @param index Index of the current morphology
     * @param materialFunc A function mapping brain::neuron::SectionType to a
     * material id
     * @param compartmentReport Compartment report to map to the morphology
     * @param model Model container to whichh the morphology should be loaded
     * into
     */
    void _importMorphologyFromURI(
        const brayns::PropertyMap& properties, const brion::URI& uri,
        const uint64_t index, const brayns::Matrix4f& transformation,
        CompartmentReportPtr compartmentReport, ParallelModelContainer& model,
        brain::Synapses* afferentSynapses = nullptr,
        brain::Synapses* efferentSynapses = nullptr) const;

    /**
     * @brief _getMaterialIdFromColorScheme returns the material id
     * corresponding to the morphology color scheme and the section type
     * @param sectionType Section type of the morphology
     * @return Material Id
     */
    size_t _getMaterialIdFromColorScheme(
        const brayns::PropertyMap& properties,
        const brain::neuron::SectionType& sectionType) const;

    /**
     * @brief Computes the distance of a segment to the soma
     * @param section Section containing the segment
     * @param sampleId segment index in the section
     * @return Distance to the soma
     */
    float _distanceToSoma(const brain::neuron::Section& section,
                          const size_t sampleId) const;

    size_t _defaultMaterialId{brayns::NO_MATERIAL};
    brayns::PropertyMap _defaults;
};

