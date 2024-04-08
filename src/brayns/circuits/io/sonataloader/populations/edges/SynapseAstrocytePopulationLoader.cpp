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

#include "SynapseAstrocytePopulationLoader.h"

#include <brayns/circuits/io/sonataloader/data/Names.h>
#include <brayns/circuits/io/sonataloader/data/Synapses.h>
#include <brayns/circuits/io/sonataloader/populations/edges/common/SynapseImporter.h>

namespace sonataloader
{
std::string_view SynapseAstrocytePopulationLoader::getPopulationType() const noexcept
{
    return EdgeNames::synapseAstrocyte;
}

void SynapseAstrocytePopulationLoader::load(EdgeLoadContext &context) const
{
    auto &edgePopulation = context.edgePopulation;
    auto &edgeSelection = context.edgeSelection;

    auto srcNodes = Synapses::getSourceNodes(edgePopulation, edgeSelection);
    auto centerPositions = Synapses::getEfferentAstrocyteCenterPos(edgePopulation, edgeSelection);

    SynapseImporter::fromData(context, srcNodes, centerPositions);
}
} // namespace sonataloader
