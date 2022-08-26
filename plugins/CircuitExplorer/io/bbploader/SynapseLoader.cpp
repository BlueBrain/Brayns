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

#include <brayns/engine/geometry/types/Sphere.h>

#include <api/synapse/SynapseColorHandler.h>
#include <components/CircuitColorComponent.h>
#include <components/SynapseComponent.h>
#include <io/bbploader/colordata/BBPSynapseColorData.h>

#include <brain/synapse.h>
#include <brain/synapses.h>
#include <brain/synapsesIterator.h>
#include <brain/synapsesStream.h>

#include <filesystem>

namespace
{
struct AfferentLoader
{
    static void load(const bbploader::LoadContext &context, brayns::Model &model)
    {
        const auto &circuit = context.circuit;
        const auto &gids = context.gids;

        std::map<uint64_t, std::vector<brayns::Sphere>> synapseGeometry;
        const brain::Synapses synapseData(circuit.getAfferentSynapses(gids));
        for (const auto &synapse : synapseData)
        {
            const auto position = synapse.getPostsynapticSurfacePosition();
            const auto gid = synapse.getPresynapticGID();
            auto &buffer = synapseGeometry[gid];
            buffer.push_back({position, 2.f});
        }

        model.addComponent<SynapseComponent>(synapseGeometry);
    }
};

struct EfferentLoader
{
    static void load(const bbploader::LoadContext &context, brayns::Model &model)
    {
        const auto &circuit = context.circuit;
        const auto &gids = context.gids;

        std::map<uint64_t, std::vector<brayns::Sphere>> synapseGeometry;
        const brain::Synapses synapseData(circuit.getEfferentSynapses(gids));
        for (const auto &synapse : synapseData)
        {
            const auto position = synapse.getPresynapticSurfacePosition();
            const auto gid = synapse.getPostsynapticGID();
            auto &buffer = synapseGeometry[gid];
            buffer.push_back({position, 2.f});
        }

        model.addComponent<SynapseComponent>(synapseGeometry);
    }
};

struct SynapseColorComponentFactory
{
    static void create(const bbploader::LoadContext &context, brayns::Model &model)
    {
        auto &synapses = model.getComponent<SynapseComponent>();
        const auto &config = context.config;
        auto circuitURI = config.getCircuitSource();
        auto path = circuitURI.getPath();
        if (!std::filesystem::exists(path))
        {
            circuitURI = config.getCellLibrarySource();
            path = circuitURI.getPath();
        }
        auto population = config.getCircuitPopulation();
        auto colorData = std::make_unique<bbploader::BBPSynapseColorData>(std::move(path), std::move(population));
        auto colorHandler = std::make_unique<SynapseColorHandler>(synapses);
        model.addComponent<CircuitColorComponent>(std::move(colorData), std::move(colorHandler));
    }
};
}

namespace bbploader
{
void SynapseLoader::load(const LoadContext &context, const bool post, brayns::Model &model)
{
    if (post)
    {
        AfferentLoader::load(context, model);
    }
    else
    {
        EfferentLoader::load(context, model);
    }

    SynapseColorComponentFactory::create(context, model);
}
} // namespace bbploader
