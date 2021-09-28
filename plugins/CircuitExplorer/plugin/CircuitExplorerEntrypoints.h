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
    static void load(CircuitExplorerPlugin& plugin)
    {
        plugin.add<brayns::GetMaterialIdsEntrypoint>();
        plugin.add<brayns::GetMaterialEntrypoint>();
        plugin.add<brayns::SetMaterialEntrypoint>();
        plugin.add<brayns::SetMaterialsEntrypoint>();
        plugin.add<brayns::SetMaterialRangeEntrypoint>();
        plugin.add<brayns::SetMaterialExtraAttributesEntrypoint>();
        plugin.add<SetSynapsesAttributesEntrypoint>(plugin);
        plugin.add<SaveModelToCacheEntrypoint>();
        plugin.add<SetConnectionsPerValueEntrypoint>();
        plugin.add<SetMetaballsPerSimulationValueEntrypoint>();
        plugin.add<GetOduCameraEntrypoint>();
        plugin.add<SetOduCameraEntrypoint>();
        plugin.add<AttachCellGrowthHandlerEntrypoint>();
        plugin.add<AttachCircuitSimulationHandlerEntrypoint>();
        plugin.add<ExportFramesToDiskEntrypoint>(plugin);
        plugin.add<GetExportFramesProgressEntrypoint>(plugin);
        plugin.add<MakeMovieEntrypoint>();
        plugin.add<TraceAnterogradeEntrypoint>(plugin);
        plugin.add<AddGridEntrypoint>();
        plugin.add<AddColumnEntrypoint>();
        plugin.add<AddSphereEntrypoint>();
        plugin.add<AddPillEntrypoint>();
        plugin.add<AddCylinderEntrypoint>();
        plugin.add<AddBoxEntrypoint>();
        plugin.add<RemapCircuitColorEntrypoint>(plugin);
        plugin.add<ColorCellsEntrypoint>(plugin);
        plugin.add<MirrorModelEntrypoint>();
        plugin.add<SetCircuitThicknessEntrypoint>();
    }
};