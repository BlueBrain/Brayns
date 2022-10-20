/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#include "SynapseLoader.h"

#include "colordata/BBPSynapseColorData.h"

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/geometry/types/Sphere.h>

#include <api/ModelType.h>
#include <api/synapse/SynapseCircuitBuilder.h>
#include <components/Coloring.h>

#include <brain/synapse.h>
#include <brain/synapses.h>
#include <brain/synapsesIterator.h>
#include <brain/synapsesStream.h>

#include <filesystem>

namespace
{
using IdSynapsesMap = std::map<uint64_t, std::vector<brayns::Sphere>>;

class AfferentGeometryLoader
{
public:
    static IdSynapsesMap load(const bbploader::LoadContext &context)
    {
        auto &circuit = context.circuit;
        auto &gids = context.gids;

        auto synapseGeometry = IdSynapsesMap();
        auto synapseData = brain::Synapses(circuit.getAfferentSynapses(gids));

        for (const auto &synapse : synapseData)
        {
            auto position = synapse.getPostsynapticSurfacePosition();
            auto gid = synapse.getPresynapticGID();
            auto &buffer = synapseGeometry[gid];
            buffer.push_back({position, 2.f});
        }
        return synapseGeometry;
    }
};

class EfferentGeometryLoader
{
public:
    static IdSynapsesMap load(const bbploader::LoadContext &context)
    {
        auto &circuit = context.circuit;
        auto &gids = context.gids;

        auto synapseGeometry = IdSynapsesMap();
        auto synapseData = brain::Synapses(circuit.getEfferentSynapses(gids));

        for (const auto &synapse : synapseData)
        {
            auto position = synapse.getPresynapticSurfacePosition();
            auto gid = synapse.getPostsynapticGID();
            auto &buffer = synapseGeometry[gid];
            buffer.push_back({position, 2.f});
        }
        return synapseGeometry;
    }
};

class GeometryLoader
{
public:
    static IdSynapsesMap load(const bbploader::LoadContext &context, bool post)
    {
        if (post)
        {
            return AfferentGeometryLoader::load(context);
        }
        return EfferentGeometryLoader::load(context);
    }
};

struct SynapseColorComponentFactory
{
    static std::unique_ptr<IColorData> create(const bbploader::LoadContext &context)
    {
        auto &config = context.config;
        auto circuitURI = config.getCircuitSource();
        auto path = circuitURI.getPath();
        if (!std::filesystem::exists(path))
        {
            circuitURI = config.getCellLibrarySource();
            path = circuitURI.getPath();
        }
        auto population = config.getCircuitPopulation();
        return std::make_unique<bbploader::BBPSynapseColorData>(std::move(path), std::move(population));
    }
};
}

namespace bbploader
{
std::shared_ptr<brayns::Model> SynapseLoader::load(const LoadContext &context, bool post)
{
    auto modelType = post ? ModelType::afferentSynapses : ModelType::efferentSynapses;
    auto model = std::make_shared<brayns::Model>(modelType);
    auto geometryData = GeometryLoader::load(context, post);
    auto colorData = SynapseColorComponentFactory::create(context);
    SynapseCircuitBuilder::build(*model, std::move(geometryData), std::move(colorData));
    return model;
}
} // namespace bbploader
