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

#include "LoaderTable.h"

#include <brayns/circuit/io/sonataloader/populations/edges/ChemicalPopulationLoader.h>
#include <brayns/circuit/io/sonataloader/populations/edges/ElectricalSynapsePopulationLoader.h>
#include <brayns/circuit/io/sonataloader/populations/edges/EndFootPopulationLoader.h>
#include <brayns/circuit/io/sonataloader/populations/edges/GlialGlialPopulationLoader.h>
#include <brayns/circuit/io/sonataloader/populations/edges/SynapseAstrocytePopulationLoader.h>

#include <brayns/circuit/io/sonataloader/populations/nodes/AstrocytePopulationLoader.h>
#include <brayns/circuit/io/sonataloader/populations/nodes/BiophysicalPopulationLoader.h>
#include <brayns/circuit/io/sonataloader/populations/nodes/PointNeuronPopulationLoader.h>
#include <brayns/circuit/io/sonataloader/populations/nodes/VasculaturePopulationLoader.h>

namespace sonataloader
{
LoaderTable<NodePopulationLoader> NodeLoaderTable::create() noexcept
{
    LoaderTable<NodePopulationLoader> result;
    result.registerLoader<AstrocytePopulationLoader>();
    result.registerLoader<BiophysicalPopulationLoader>();
    result.registerLoader<PointNeuronPopulationLoader>();
    result.registerLoader<VasculaturePopulationLoader>();
    return result;
}

LoaderTable<EdgePopulationLoader> EdgeLoaderTable::create() noexcept
{
    LoaderTable<EdgePopulationLoader> result;
    result.registerLoader<ChemicalSynapsePopulationLoader>();
    result.registerLoader<ElectricalSynapsePopulationLoader>();
    result.registerLoader<EndFootPopulationLoader>();
    result.registerLoader<GlialGlialPopulationLoader>();
    result.registerLoader<SynapseAstrocytePopulationLoader>();
    return result;
}
}
