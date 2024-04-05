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

#include "EndFootPopulationLoader.h"

#include "common/EdgeMetadataFactory.h"

#include <brayns/circuit/api/circuit/EndfeetCircuitBuilder.h>
#include <brayns/circuit/io/sonataloader/colordata/ColorDataFactory.h>
#include <brayns/circuit/io/sonataloader/data/EndFeetReader.h>
#include <brayns/circuit/io/sonataloader/data/Names.h>
#include <brayns/circuit/io/sonataloader/data/Synapses.h>

namespace sonataloader
{
std::string_view EndFootPopulationLoader::getPopulationType() const noexcept
{
    return EdgeNames::endfoot;
}

void EndFootPopulationLoader::load(EdgeLoadContext &context) const
{
    auto &nodeSelection = context.nodeSelection;
    auto nodes = nodeSelection.flatten();
    auto &population = context.edgePopulation;
    auto populationProperties = context.config.getEdgesProperties(population.name());
    auto &endfeetMeshPath = populationProperties.endfeetMeshesFile;
    auto &edgeSelection = context.edgeSelection;
    auto flatEdges = edgeSelection.flatten();
    auto astrocyteIds = Synapses::getTargetNodes(population, edgeSelection);
    auto endFeetIds = Synapses::getEndFeetIds(population, edgeSelection);

    auto meshes = EndFeetReader::read(*endfeetMeshPath, endFeetIds);

    std::map<uint64_t, std::vector<brayns::TriangleMesh>> endfeetGeometry;
    for (size_t i = 0; i < astrocyteIds.size(); ++i)
    {
        const auto astrocyte = astrocyteIds[i];
        auto &buffer = endfeetGeometry[astrocyte];
        auto &mesh = meshes[i];
        buffer.push_back(std::move(mesh));
    }

    auto buildContext = EndfeetCircuitBuilder::Context{std::move(endfeetGeometry), ColorDataFactory::create(context)};
    EndfeetCircuitBuilder::build(context.model, std::move(buildContext));

    EdgeMetadataFactory::create(context);
}
} // namespace sonataloader
