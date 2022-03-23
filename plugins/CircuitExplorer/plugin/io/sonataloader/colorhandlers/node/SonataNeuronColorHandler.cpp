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

#include "SonataNeuronColorHandler.h"

#include <brayns/common/Log.h>

#include <set>

namespace
{
constexpr char methodByMorphology[] = "morphology";
constexpr char methodByLayer[] = "layer";
constexpr char methodByMorphClass[] = "morphology class";
constexpr char methodByEType[] = "etype";
constexpr char methodByMType[] = "mtype";
constexpr char methodBySynapseClass[] = "synapse class";
constexpr char methodByRegion[] = "region";
constexpr char methodByHemisphere[] = "hemisphere";

std::vector<std::string> getAttributeValues(
    const bbp::sonata::NodePopulation &population,
    const std::string &attribute,
    const std::vector<uint64_t> &nodeIds)
{
    const auto selection = bbp::sonata::Selection::fromValues(nodeIds);
    try
    {
        const auto values = population.getAttribute<std::string>(attribute, selection);
    }
    catch (const std::exception &e)
    {
        const auto popName = population.name();
        brayns::Log::warn(
            "Could not read sonata node attribute from population {}, attribute {}: {}",
            popName,
            attribute,
            e.what());
    }

    return {};
}
} // namespace

namespace sonataloader
{
SonataNeuronColorHandler::SonataNeuronColorHandler(bbp::sonata::CircuitConfig config, const std::string &population)
    : _config(std::move(config))
    , _population(population)
{
}

std::vector<std::string> SonataNeuronColorHandler::_getExtraMethods() const
{
    const auto population = _config.getNodePopulation(_population);
    const auto &attributes = population.attributeNames();
    const std::vector<std::string> possibleMethods = {
        methodByMorphology,
        methodByLayer,
        methodByMorphClass,
        methodByEType,
        methodByMType,
        methodBySynapseClass,
        methodByRegion,
        methodByHemisphere};
    std::vector<std::string> result;
    result.reserve(possibleMethods.size());

    for (const auto &possible : possibleMethods)
    {
        if (attributes.find(possible) != attributes.end())
        {
            // Check we can actually read the data
            const auto dataTest = getAttributeValues(population, possible, {0});
            if (!dataTest.empty())
                result.push_back(possible);
        }
    }

    result.shrink_to_fit();
    return result;
}

std::vector<std::string> SonataNeuronColorHandler::_getValuesForMethod(const std::string &method) const
{
    const auto population = _config.getNodePopulation(_population);
    return getAttributeValues(population, method, _ids);
}
} // namespace sonataloader
