/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/engine/Model.h>
#include <brayns/pluginapi/PluginAPI.h>

/**
 * @brief The VasculatureRadiiSimulation controls the update of SONATA
 * Vasculature radii reports.
 *        TODO: Current engines only support color varying simulations. Radii
 * report modifies geometry radii. Add support for generic simulation types on
 * engines refactoring.
 */
class VasculatureRadiiSimulation
{
public:
    /**
     * @brief attempts to register a model into the list. If must have a
     * VasculatureRadiiHandler associated with it to succeed.
     */
    static void registerModel(brayns::ModelDescriptorPtr& model);

    /**
     * @brief unregister the model given its ID from this class (if the given
     * model id corresponds to an existing model).
     */
    static void unregisterModel(size_t modelId);

    /**
     * @brief iterates over all the registered vasculature datasets, gathering
     * the current simulation frame and updating the geometry radii with it.
     */
    static void update();

private:
    struct SimulationTracker
    {
        brayns::ModelDescriptor* model{nullptr};
        uint32_t lastFrame{std::numeric_limits<uint32_t>::max()};
    };

    static std::vector<SimulationTracker> _vasculatureModels;
    static uint32_t _lastFrame;
};
