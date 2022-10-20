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

#include "SonataModelType.h"

#include <api/ModelType.h>
#include <io/sonataloader/data/SonataCells.h>
#include <io/sonataloader/data/SonataNames.h>
#include <io/sonataloader/data/SonataSynapses.h>

#include <unordered_map>

namespace
{
namespace sl = sonataloader;

inline static const std::unordered_map<std::string_view, std::string> nodeToType = {
    {sl::SonataNodeNames::astrocyte, ModelType::astroctyes},
    {sl::SonataNodeNames::biophysical, ModelType::neurons},
    {sl::SonataNodeNames::pointNeuron, ModelType::neurons},
    {sl::SonataNodeNames::vasculature, ModelType::vasculature}};
}

namespace sonataloader
{
const std::string &SonataModelType::fromNodes(const bbp::sonata::NodePopulation &population)
{
    auto populationType = SonataCells::getPopulationType(population);
    auto it = nodeToType.find(populationType);
    if (it == nodeToType.end())
    {
        throw std::invalid_argument("Unsupported population type");
    }

    return it->second;
}

const std::string &SonataModelType::fromEdges(const bbp::sonata::EdgePopulation &population, bool afferent)
{
    auto populationType = SonataSynapses::getPopulationType(population);
    if (populationType == SonataEdgeNames::endfoot)
    {
        return ModelType::endfeet;
    }

    return afferent ? ModelType::afferentSynapses : ModelType::efferentSynapses;
}
}
