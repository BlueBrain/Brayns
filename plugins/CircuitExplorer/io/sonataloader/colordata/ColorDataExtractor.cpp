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

#include "ColorDataExtractor.h"

#include <api/neuron/NeuronColorMethod.h>

namespace
{
// I dont like std pair
struct AttributeMethodEntry
{
    NeuronColorMethod method;
    std::string attribute;
};

/**
 * @brief Mapping between coloring methods and SONATA population attribute names
 */
struct AttributeMethodMapping
{
    static std::vector<AttributeMethodEntry> generate()
    {
        return {
            {NeuronColorMethod::ByEtype, "etype"},
            {NeuronColorMethod::ByHemisphere, "hemisphere"},
            {NeuronColorMethod::ByLayer, "layer"},
            {NeuronColorMethod::ByMorphology, "morphology"},
            {NeuronColorMethod::ByMorphologyClass, "morph_class"},
            {NeuronColorMethod::ByMtype, "mtype"},
            {NeuronColorMethod::ByRegion, "region"},
            {NeuronColorMethod::BySynapseClass, "synapse_class"}};
    }

    static std::string getAttributeForMethod(const std::string &method)
    {
        auto enumEntry = brayns::EnumInfo::getValue<NeuronColorMethod>(method);
        return getAttributeForMethod(enumEntry);
    }

    static std::string getAttributeForMethod(const NeuronColorMethod method)
    {
        const auto mapping = generate();
        const auto begin = mapping.begin();
        const auto end = mapping.end();
        auto it = std::find_if(begin, end, [&](const AttributeMethodEntry &entry) { return entry.method == method; });

        if (it == mapping.end())
        {
            throw std::invalid_argument("Unknown method " + brayns::EnumInfo::getName(method));
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

    static std::vector<std::string> queryAny(
        const bbp::sonata::NodePopulation &population,
        const std::string &attribute)
    {
        return query(population, attribute, std::vector<uint64_t>{0});
    }

    static std::vector<std::string> queryAll(
        const bbp::sonata::NodePopulation &population,
        const std::string &attribute)
    {
        const auto selection = population.selectAll();
        return query(population, attribute, selection);
    }
};
}

namespace sonataloader
{
std::vector<std::string> CellNodeColorMethods::get(const bbp::sonata::NodePopulation &population)
{
    auto possibleMethods = AttributeMethodMapping::generate();

    auto result = std::vector<std::string>();
    result.reserve(possibleMethods.size());

    for (auto &possible : possibleMethods)
    {
        auto &attribute = possible.attribute;
        auto method = possible.method;

        if (!PopulationQuery::hasAttribute(population, attribute))
        {
            continue;
        }
        auto methodName = brayns::EnumInfo::getName(method);
        result.push_back(std::move(methodName));
    }

    return result;
}

std::vector<std::string> CellNodeColorValues::get(
    const bbp::sonata::NodePopulation &population,
    const std::string &method,
    const std::vector<uint64_t> &ids)
{
    auto attribute = AttributeMethodMapping::getAttributeForMethod(method);
    return PopulationQuery::query(population, attribute, ids);
}

std::vector<std::string> CellNodeColorValues::getAll(
    const bbp::sonata::NodePopulation &population,
    const std::string &method)
{
    auto attribute = AttributeMethodMapping::getAttributeForMethod(method);

    if (!PopulationQuery::hasAttribute(population, attribute))
    {
        throw std::invalid_argument("The attribute " + attribute + " is not available");
    }

    return PopulationQuery::queryAll(population, attribute);
}
}
