/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/model/Model.h>

#include <api/coloring/IBrainColorData.h>

#include <map>
#include <vector>

/**
 * @brief Adds the necessary components and systems to a model from the input grouped synapses to render a synapse
 * circuit
 */
class SynapseCircuitBuilder
{
public:
    struct Context
    {
        std::map<uint64_t, std::vector<brayns::Sphere>> groupedSynapses;
        std::unique_ptr<IBrainColorData> colorData;
    };

    static void build(brayns::Model &model, Context context);
};
