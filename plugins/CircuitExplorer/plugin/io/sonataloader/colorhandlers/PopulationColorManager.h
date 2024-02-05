/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <plugin/api/CircuitColorHandler.h>

#include <plugin/io/SonataLoaderParameters.h>
#include <plugin/io/sonataloader/data/SonataConfig.h>

namespace sonataloader
{
/**
 * @brief The PopulationColorManager class is in charge of creating the
 * appropiate circuit color handlers for each node and edge population types
 */
class PopulationColorManager
{
public:
    /**
     * @brief creates a CircuitColorHandler based on the node population type
     */
    static CircuitColorHandler::Ptr createNodeColorHandler(
        const SonataConfig::Data& network,
        const SonataNodePopulationParameters& lc);

    /**
     * @brief creates a CircuitColorHandler based on the edge population type
     */
    static CircuitColorHandler::Ptr createEdgeColorHandler(
        const SonataConfig::Data& network,
        const SonataEdgePopulationParameters& lc);
};
} // namespace sonataloader
