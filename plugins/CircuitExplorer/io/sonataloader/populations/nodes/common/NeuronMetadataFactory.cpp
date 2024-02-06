/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "NeuronMetadataFactory.h"

#include <brayns/engine/components/Metadata.h>

#include <brayns/utils/string/StringJoiner.h>

namespace
{
struct SonataNeuronMetadataKey
{
    static inline const std::string type = "population_type";
    static inline const std::string name = "population_name";
    static inline const std::string neuronCount = "loaded_neuron_count";
    static inline const std::string nodeSets = "node_sets";
};
}

namespace sonataloader
{
void NeuronMetadataFactory::create(NodeLoadContext &context)
{
    auto &model = context.model;
    auto &metadata = model.getComponents().add<brayns::Metadata>();

    metadata[SonataNeuronMetadataKey::type] = "node";
    metadata[SonataNeuronMetadataKey::name] = context.population.name();
    metadata[SonataNeuronMetadataKey::neuronCount] = std::to_string(context.selection.flatSize());

    auto &nodeSets = context.params.node_sets;
    if (!nodeSets.empty())
    {
        metadata[SonataNeuronMetadataKey::nodeSets] = brayns::StringJoiner::join(nodeSets, ",");
    }
}

void NeuronMetadataFactory::create(brayns::Model &model, const std::string &populationName)
{
    auto &metadata = model.getComponents().add<brayns::Metadata>();
    metadata[SonataNeuronMetadataKey::type] = "node";
    metadata[SonataNeuronMetadataKey::name] = populationName;
}
}
