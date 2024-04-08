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

#include "SonataColorData.h"

namespace
{
struct AttributeMethodEntry
{
    BrainColorMethod method;
    std::string attribute;
};

/**
 * @brief Mapping between coloring methods and SONATA population attribute names
 */
class AttributeMethodMapping
{
public:
    static inline const std::vector<AttributeMethodEntry> mapping = {
        {BrainColorMethod::ByEtype, "etype"},
        {BrainColorMethod::ByHemisphere, "hemisphere"},
        {BrainColorMethod::ByLayer, "layer"},
        {BrainColorMethod::ByMorphology, "morphology"},
        {BrainColorMethod::ByMorphologyClass, "morph_class"},
        {BrainColorMethod::ByMtype, "mtype"},
        {BrainColorMethod::ByRegion, "region"},
        {BrainColorMethod::BySubregion, "subregion"},
        {BrainColorMethod::BySynapseClass, "synapse_class"}};

    static std::string getAttributeForMethod(const std::string &method)
    {
        auto enumEntry = brayns::EnumInfo::getValue<BrainColorMethod>(method);
        return getAttributeForMethod(enumEntry);
    }

    static std::string getAttributeForMethod(const BrainColorMethod method)
    {
        const auto begin = mapping.begin();
        const auto end = mapping.end();
        auto it = std::find_if(begin, end, [&](const AttributeMethodEntry &entry) { return entry.method == method; });

        if (it == mapping.end())
        {
            throw std::invalid_argument("Unsupported method " + brayns::EnumInfo::getName(method));
        }

        return it->attribute;
    }
};

/**
 * @brief Encapsulates queries for string data to the population object
 */
class PopulationQuery
{
public:
    static bool hasAttribute(const bbp::sonata::NodePopulation &population, const std::string &attribute)
    {
        auto &attributes = population.attributeNames();
        return attributes.find(attribute) != attributes.end();
    }

    static std::vector<std::string> query(
        const bbp::sonata::NodePopulation &population,
        const std::string &attribute,
        const bbp::sonata::Selection &selection)
    {
        return population.getAttribute<std::string>(attribute, selection);
    }

    static std::vector<std::string> query(
        const bbp::sonata::NodePopulation &population,
        const std::string &attribute,
        const std::vector<uint64_t> &ids)
    {
        auto selection = bbp::sonata::Selection::fromValues(ids);
        return query(population, attribute, selection);
    }
};
}

namespace sonataloader
{
SonataColorData::SonataColorData(bbp::sonata::NodePopulation population):
    _population(std::move(population))
{
}

std::vector<BrainColorMethod> SonataColorData::getMethods() const
{
    auto &possibleMethods = AttributeMethodMapping::mapping;

    auto result = std::vector<BrainColorMethod>();
    result.reserve(possibleMethods.size());

    for (auto &possible : possibleMethods)
    {
        if (!PopulationQuery::hasAttribute(_population, possible.attribute))
        {
            continue;
        }

        result.push_back(possible.method);
    }

    return result;
}

std::vector<std::string> SonataColorData::getValues(BrainColorMethod method, const std::vector<uint64_t> &ids) const
{
    auto attribute = AttributeMethodMapping::getAttributeForMethod(method);
    return PopulationQuery::query(_population, attribute, ids);
}
}
