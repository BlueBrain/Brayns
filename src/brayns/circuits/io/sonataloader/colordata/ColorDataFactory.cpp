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

#include "ColorDataFactory.h"

#include "SonataColorData.h"

namespace sonataloader
{
std::unique_ptr<IBrainColorData> ColorDataFactory::create(const EdgeLoadContext &context)
{
    auto &params = context.params;
    auto afferent = params.load_afferent;
    auto &edgePopulation = context.edgePopulation;
    auto targetPopulation = afferent ? edgePopulation.target() : edgePopulation.source();

    auto &config = context.config;
    auto nodePopulation = config.getNodes(targetPopulation);

    return std::make_unique<SonataColorData>(std::move(nodePopulation));
}

std::unique_ptr<IBrainColorData> ColorDataFactory::create(const NodeLoadContext &ctxt)
{
    auto &config = ctxt.config;
    auto &nodePopulation = ctxt.population;
    auto populationName = nodePopulation.name();
    return std::make_unique<SonataColorData>(config.getNodes(populationName));
}
}
