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

#include <plugin/entrypoints/AddBoxEntrypoint.h>
#include <plugin/entrypoints/AddColumnEntrypoint.h>
#include <plugin/entrypoints/AddCylinderEntrypoint.h>
#include <plugin/entrypoints/AddGridEntrypoint.h>
#include <plugin/entrypoints/AddPillEntrypoint.h>
#include <plugin/entrypoints/AddSphereEntrypoint.h>
#include <plugin/entrypoints/AttachCellGrowthHandlerEntrypoint.h>
#include <plugin/entrypoints/AttachCircuitSimulationHandlerEntrypoint.h>
#include <plugin/entrypoints/ColorCellsEntrypoint.h>
#include <plugin/entrypoints/ExportFramesToDiskEntrypoint.h>
#include <plugin/entrypoints/GetExportFramesProgressEntrypoint.h>
#include <plugin/entrypoints/GetMaterialIdsEntrypoint.h>
#include <plugin/entrypoints/MakeMovieEntrypoint.h>
#include <plugin/entrypoints/MaterialEntrypoint.h>
#include <plugin/entrypoints/MirrorModelEntrypoint.h>
#include <plugin/entrypoints/OduCameraEntrypoint.h>
#include <plugin/entrypoints/RemapCircuitColorEntrypoint.h>
#include <plugin/entrypoints/SaveModelToCacheEntrypoint.h>
#include <plugin/entrypoints/SetCircuitThicknessEntrypoint.h>
#include <plugin/entrypoints/SetConnectionsPerValueEntrypoint.h>
#include <plugin/entrypoints/SetMaterialExtraAttributesEntrypoint.h>
#include <plugin/entrypoints/SetMaterialRangeEntrypoint.h>
#include <plugin/entrypoints/SetMaterialsEntrypoint.h>
#include <plugin/entrypoints/SetMetaballsPerSimulationValueEntrypoint.h>
#include <plugin/entrypoints/SetSynapsesAttributesEntrypoint.h>
#include <plugin/entrypoints/TraceAnterogradeEntrypoint.h>

#include "CircuitExplorerPlugin.h"

class CircuitExplorerEntrypoints
{
public:
    static void load(CircuitExplorerPlugin& plugin,
                     brayns::ActionInterface& interface)
    {
        interface.add<brayns::GetMaterialIdsEntrypoint>();
        interface.add<brayns::GetMaterialEntrypoint>();
        interface.add<brayns::SetMaterialEntrypoint>();
        interface.add<brayns::SetMaterialsEntrypoint>();
        interface.add<brayns::SetMaterialRangeEntrypoint>();
        interface.add<brayns::SetMaterialExtraAttributesEntrypoint>();
        interface.add<SetSynapsesAttributesEntrypoint>(plugin);
        interface.add<SaveModelToCacheEntrypoint>();
        interface.add<SetConnectionsPerValueEntrypoint>();
        interface.add<SetMetaballsPerSimulationValueEntrypoint>();
        interface.add<GetOduCameraEntrypoint>();
        interface.add<SetOduCameraEntrypoint>();
        interface.add<AttachCellGrowthHandlerEntrypoint>();
        interface.add<AttachCircuitSimulationHandlerEntrypoint>();
        interface.add<ExportFramesToDiskEntrypoint>(plugin);
        interface.add<GetExportFramesProgressEntrypoint>(plugin);
        interface.add<MakeMovieEntrypoint>();
        interface.add<TraceAnterogradeEntrypoint>(plugin);
        interface.add<AddGridEntrypoint>();
        interface.add<AddColumnEntrypoint>();
        interface.add<AddSphereEntrypoint>();
        interface.add<AddPillEntrypoint>();
        interface.add<AddCylinderEntrypoint>();
        interface.add<AddBoxEntrypoint>();
        interface.add<RemapCircuitColorEntrypoint>(plugin);
        interface.add<ColorCellsEntrypoint>(plugin);
        interface.add<MirrorModelEntrypoint>();
        interface.add<SetCircuitThicknessEntrypoint>();
    }
};