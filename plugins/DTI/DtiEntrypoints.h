/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <entrypoints/AddStreamlinesEntrypoint.h>
#include <entrypoints/SetSpikeSimulationEntrypoint.h>
#include <entrypoints/SetSpikeSimulationFromFileEntrypoint.h>

#include "DTIPlugin.h"

namespace dti
{
class DtiEntrypoints
{
public:
    static void load(DTIPlugin& plugin)
    {
        plugin.add<AddStreamlinesEntrypoint>();
        plugin.add<SetSpikeSimulationEntrypoint>(plugin);
        plugin.add<SetSpikeSimulationFromFileEntrypoint>(plugin);
    }
};
} // namespace dti