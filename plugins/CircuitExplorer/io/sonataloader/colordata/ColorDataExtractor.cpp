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

#include <api/neuron/NeuronColorMethods.h>

namespace
{
// I dont like std pair
struct AttributeMethodEntry
{
    NeuronColorMethods method;
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
            {NeuronColorMethods::BY_ETYPE, "etype"},
            {NeuronColorMethods::BY_HEMISPHERE, "hemisphere"},
            {NeuronColorMethods::BY_LAYER, "layer"},
            {NeuronColorMethods::BY_MORPHOLOGY, "morphology"},
            {NeuronColorMethods::BY_MORPHOLOGY_CLASS, "morph_class"},
            {NeuronColorMethods::BY_MTYPE, "mtype"},
            {NeuronColorMethods::BY_REGION, "region"},
            {NeuronColorMethods::BY_SYNAPSE_CLASS, "synapse_class"}};
    }

    static std::string getAttributeForMethod(const std::string &method)
    {
        const auto enumEntry = brayns::stringToEnum<NeuronColorMethods>(method);
        return getAttributeForMethod(enumEntry);
    }

    static std::string getAttributeForMethod(const NeuronColorMethods method)
    {
        const auto mapping = generate();
        const auto begin = mapping.begin();
        const auto end = mapping.end();
        auto it = std::find_if(begin, end, [&](const AttributeMethodEntry &entry) { return entry.method == method; });

        if (it == mapping.end())
        {
            throw std::invalid_argument("Unknown method " + brayns::enumToString<NeuronColorMethods>(method));
        }

        return it->attribute;
    }
};

/**
 * @brief Encapsulates queries for string data to the population object
 */
struct PopulationQuery
{
    static std::vector<std::string> query(
        const bbp::sonata::NodePopulation &population,
        const std::string &attribute,
        const bbp::sonata::Selection &selection)
    {
        try
        {
            return population.getAttribute<std::string>(attribute, selection);
        }
        catch (...)
        {
        }

        return {};
    }

    static std::vector<std::string> query(
        const bbp::sonata::NodePopulation &population,
        const std::string &attribute,
        const std::vector<uint64_t> &ids)
    {
        const auto selection = bbp::sonata::Selection::fromValues(ids);
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
    const auto possibleMethods = AttributeMethodMapping::generate();

    std::vector<std::string> result;
    result.reserve(possibleMethods.size());

    for (const auto &possible : possibleMethods)
    {
        const auto &attribute = possible.attribute;
        const auto method = possible.method;

        const auto query = PopulationQuery::queryAny(population, attribute);
        if (!query.empty())
        {
            auto methodName = brayns::enumToString<NeuronColorMethods>(method);
            result.push_back(std::move(methodName));
        }
    }

    return result;
}

std::vector<std::string> CellNodeColorValues::get(
    const bbp::sonata::NodePopulation &population,
    const std::string &method,
    const std::vector<uint64_t> &ids)
{
    const auto attribute = AttributeMethodMapping::getAttributeForMethod(method);
    return PopulationQuery::query(population, attribute, ids);
}

std::vector<std::string> CellNodeColorValues::getAll(
    const bbp::sonata::NodePopulation &population,
    const std::string &method)
{
    const auto attribute = AttributeMethodMapping::getAttributeForMethod(method);
    return PopulationQuery::queryAll(population, attribute);
}
}
