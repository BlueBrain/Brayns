/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#pragma once

#include <plugin/entrypoints/MaterialEntrypoint.h>
#include <plugin/entrypoints/SaveModelToCacheEntrypoint.h>
#include <plugin/entrypoints/SetConnectionsPerValueEntrypoint.h>
#include <plugin/entrypoints/SetMetaballsPerSimulationValueEntrypoint.h>
#include <plugin/entrypoints/SetSynapsesAttributesEntrypoint.h>

#include "CircuitExplorerContext.h"

class CircuitExplorerEntrypoints
{
public:
    static void load(CircuitExplorerContext& context,
                     brayns::ActionInterface& interface)
    {
        interface.add<brayns::GetMaterialIdsEntrypoint>();
        interface.add<brayns::GetMaterialEntrypoint>();
        interface.add<brayns::SetMaterialEntrypoint>();
        interface.add<brayns::SetMaterialsEntrypoint>();
        interface.add<brayns::SetMaterialRangeEntrypoint>();
        interface.add<brayns::SetMaterialExtraAttributesEntrypoint>();
        interface.add<SetSynapsesAttributesEntrypoint>(context);
        interface.add<SaveModelToCacheEntrypoint>();
        interface.add<SetConnectionsPerValueEntrypoint>();
        interface.add<SetMetaballsPerSimulationValueEntrypoint>();
    }
};