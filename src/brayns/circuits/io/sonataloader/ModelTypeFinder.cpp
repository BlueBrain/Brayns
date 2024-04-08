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

#include "ModelTypeFinder.h"

#include <brayns/circuits/api/ModelType.h>
#include <brayns/circuits/io/sonataloader/data/Names.h>
#include <brayns/circuits/io/sonataloader/data/PopulationType.h>

#include <unordered_map>

namespace
{
static inline const std::unordered_map<std::string_view, std::string> nodeToType = {
    {sonataloader::NodeNames::astrocyte, ModelType::astroctyes},
    {sonataloader::NodeNames::biophysical, ModelType::neurons},
    {sonataloader::NodeNames::pointNeuron, ModelType::neurons},
    {sonataloader::NodeNames::vasculature, ModelType::vasculature}};
}

namespace sonataloader
{
const std::string &ModelTypeFinder::fromNodes(const bbp::sonata::NodePopulation &nodes, const Config &config)
{
    auto populationType = PopulationType::getNodeType(nodes, config);
    auto it = nodeToType.find(populationType);
    if (it == nodeToType.end())
    {
        throw std::invalid_argument("Unsupported population type");
    }

    return it->second;
}

const std::string &ModelTypeFinder::fromEdges(
    const bbp::sonata::EdgePopulation &edges,
    bool afferent,
    const Config &config)
{
    auto populationType = PopulationType::getEdgeType(edges, config);
    if (populationType == EdgeNames::endfoot)
    {
        return ModelType::endfeet;
    }

    return afferent ? ModelType::afferentSynapses : ModelType::efferentSynapses;
}
}
