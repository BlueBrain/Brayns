/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "color/ColorDataFactory.h"

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/geometry/types/Sphere.h>

#include <api/ModelType.h>
#include <api/circuit/SynapseCircuitBuilder.h>

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
            buffer.push_back({{position.x, position.y, position.z}, 2.f});
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
            buffer.push_back({{position.x, position.y, position.z}, 2.f});
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
}

namespace bbploader
{
std::shared_ptr<brayns::Model> SynapseLoader::load(const LoadContext &context, bool post)
{
    auto modelType = post ? ModelType::afferentSynapses : ModelType::efferentSynapses;
    auto model = std::make_shared<brayns::Model>(modelType);
    auto geometryData = GeometryLoader::load(context, post);
    auto colorData = ColorDataFactory::create(context);

    auto buildContext = SynapseCircuitBuilder::Context{std::move(geometryData), std::move(colorData)};
    SynapseCircuitBuilder::build(*model, std::move(buildContext));

    return model;
}
} // namespace bbploader
