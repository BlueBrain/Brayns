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

#include "MorphologyLoader.h"
#include <common/types.h>
#include <plugin/api/CellObjectMapper.h>
#include <plugin/api/CircuitExplorerParams.h>

#include <brayns/common/loader/Loader.h>
#include <brayns/common/types.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <set>
#include <vector>

namespace servus
{
class URI;
}

class CircuitExplorerPlugin;

using Matrix4fs = std::vector<brayns::Matrix4f>;

/**
 * Load circuit from BlueConfig or CircuitConfig file, including simulation.
 */
class AbstractCircuitLoader : public brayns::Loader
{
public:
    AbstractCircuitLoader(
        brayns::Scene &scene,
        const brayns::ApplicationParameters &applicationParameters,
        brayns::PropertyMap &&loaderParams,
        CircuitExplorerPlugin* pluginPtr);

    brayns::PropertyMap getProperties() const final;

    std::vector<std::string> getSupportedExtensions() const;

    bool isSupported(const std::string &filename,
                     const std::string &extension) const;

    brayns::ModelDescriptorPtr importFromBlob(
        brayns::Blob &&blob, const brayns::LoaderProgress &callback,
        const brayns::PropertyMap &properties) const;

    /**
     * @brief Imports morphology from a circuit for the given target name
     * @param circuitConfig URI of the Circuit Config file
     * @return ModelDescriptor if the circuit is successfully loaded, nullptr if
     * the circuit contains no cells.
     */
    brayns::ModelDescriptorPtr importCircuit(
        const std::string &circuitConfig, const brayns::PropertyMap &properties,
        const brayns::LoaderProgress &callback) const;

    /**
     * @brief _populateLayerIds populates the neuron layer IDs. This is
     * currently only supported for the MVD2 format.
     * @param blueConfig Configuration of the circuit
     * @param gids GIDs of the neurons
     * @param result SchemeItem where the ids and layer names will be
     * sotred
     */
    void _populateLayerIds(const brayns::PropertyMap &props,
                           const brion::BlueConfig &blueConfig,
                           const brain::GIDSet &gids,
                           SchemeItem& result) const;

    static void setSimulationTransferFunction(brayns::TransferFunction &tf,
                                              const float finalOpacity = 1.f);

protected:
    const brayns::ApplicationParameters &_applicationParameters;
    brayns::PropertyMap _defaults;
    brayns::PropertyMap _fixedDefaults;

private:

    std::vector<std::string> _getTargetsAsStrings(
        const std::string &targets) const;

    std::vector<uint64_t> _getGIDsAsInts(const std::string &gids) const;

    brain::GIDSet _getGids(const brayns::PropertyMap &properties,
                           const brion::BlueConfig &blueConfiguration,
                           const brain::Circuit &circuit,
                           SchemeItem& targets) const;

    std::string _getMeshFilenameFromGID(const brayns::PropertyMap &props,
                                        const uint64_t gid) const;

    float _importMorphologies(
        const brayns::PropertyMap &props, const brain::Circuit &circuit,
        brayns::Model &model, const brain::GIDSet &gids,
        const Matrix4fs &transformations, CellObjectMapper& mapper,
        CompartmentReportPtr compartmentReport,
        const brayns::LoaderProgress &callback,
        const size_t materialId = brayns::NO_MATERIAL) const;

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
    size_t _getMaterialFromCircuitAttributes(
        const brayns::PropertyMap &props, const uint64_t index,
        const size_t material, const bool forSimulationModel,
        CircuitSchemeData* data = nullptr) const;

    void _importMeshes(const brayns::PropertyMap &props, brayns::Model &model,
                       const brain::GIDSet &gids,
                       const Matrix4fs &transformations,
                       const brayns::LoaderProgress &callback,
                       CellObjectMapper& mapper) const;

    CompartmentReportPtr _attachSimulationHandler(
        const brayns::PropertyMap &properties,
        const brion::BlueConfig &blueConfiguration, brayns::Model &model,
        const ReportType &reportType, brain::GIDSet &gids) const;

    void _filterGIDsWithClippingPlanes(brain::GIDSet &gids,
                                       Matrix4fs &transformations) const;

    void _filterGIDsWithAreasOfInterest(const uint16_t areasOfInterest,
                                        brain::GIDSet &gids,
                                        Matrix4fs &transformations) const;

    bool _isClipped(const brayns::Vector3f &position) const;

    void _setDefaultCircuitColorMap(brayns::Model &model) const;

    // Synapses
    void _buildAfferentSynapses(const brain::Synapse &synapse,
                                const size_t materialId, const float radius,
                                brayns::Model &model) const;
    void _buildEfferentSynapses(const brain::Synapse &synapse,
                                const size_t materialId, const float radius,
                                brayns::Model &model) const;
    void _loadPairSynapses(const brayns::PropertyMap &properties,
                           const brain::Circuit &circuit,
                           const uint32_t &preGid, const uint32_t &postGid,
                           const float synapseRadius,
                           brayns::Model &model) const;
    void _loadAllSynapses(const brayns::PropertyMap &properties,
                          const brain::Circuit &circuit,
                          const brain::GIDSet &gids, const float synapseRadius,
                          const bool loadAfferentSynapses,
                          const bool loadEfferentSynapses,
                          brayns::Model &model) const;

    CircuitExplorerPlugin* _pluginPtr{nullptr};
};
