/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

namespace sonataloader
{
void NeuronMetadataFactory::create(NodeLoadContext &context)
{
    auto &model = context.model;
    auto &metadata = model.getComponents().add<brayns::Metadata>();

    metadata["population_type"] = "node";
    metadata["population_name"] = context.population.name();
    metadata["loaded_neuron_count"] = std::to_string(context.selection.flatSize());

    auto &nodeSets = context.params.node_sets;
    if (!nodeSets.empty())
    {
        metadata["node_sets"] = brayns::StringJoiner::join(nodeSets, ",");
    }
}
}
