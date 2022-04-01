/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#pragma once

#include <brayns/io/Loader.h>

#include <io/bbploader/LoadContext.h>
#include <io/simulation/SimulationMapping.h>
#include <io/util/ProgressUpdater.h>

namespace bbploader
{
/**
 * @brief The CellLoader class is in charge of load the needed data from BBP's
 * internal format files provided by a CircuitConfig/BlueConfig file, and
 * transform them into a list of MorphologyInstances that can be added to the
 * Brayns scene
 */
class CellLoader
{
public:
    /**
     * @brief load
     * @param context
     * @param updater
     * @param model
     * @return
     */
    static std::vector<CompartmentStructure> load(
            const LoadContext &context, ProgressUpdater &updater, brayns::Model &model);
};
} // namespace bbploader
