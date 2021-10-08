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

#pragma once

#include <plugin/api/CircuitExplorerParams.h>

#include <brayns/common/loader/Loader.h>
#include <brayns/common/types.h>
#include <brayns/parameters/GeometryParameters.h>

#include <brain/brain.h>

#include <vector>

/**
 * Load circuit from BlueConfig or CircuitConfig file, including simulation.
 */
class SynapseJSONLoader : public brayns::Loader
{
public:
    SynapseJSONLoader(brayns::Scene& scene,
                      const SynapseAttributes& synapseAttributes);

    std::string getName() const final;

    std::vector<std::string> getSupportedExtensions() const final;

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;

    std::vector<brayns::ModelDescriptorPtr> importFromBlob(
        brayns::Blob&& blob, const brayns::LoaderProgress& callback,
        const brayns::PropertyMap& properties) const final;

    std::vector<brayns::ModelDescriptorPtr> importFromFile(
        const std::string& filename, const brayns::LoaderProgress& callback,
        const brayns::PropertyMap& properties) const final;

    /**
     * @brief Imports synapses from a circuit for the given target name
     * @param circuitConfig URI of the Circuit Config file
     * @param gid Gid of the neuron
     * @param scene Scene into which the circuit is imported
     * @return True if the circuit is successfully loaded, false if the circuit
     * contains no cells.
     */
    brayns::ModelDescriptorPtr importSynapsesFromGIDs(
        const SynapseAttributes& synapseAttributes,
        const brayns::Vector3fs& colors);

private:
    const SynapseAttributes& _synapseAttributes;
};
