/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/utils/MathTypes.h>

#include <brayns/engine/model/Model.h>

#include <api/coloring/IBrainColorData.h>
#include <api/reports/ReportMapping.h>
#include <io/NeuronMorphologyLoaderParameters.h>
#include <io/util/ProgressUpdater.h>

/**
 * @brief The MorphologyCircuitLoader struct loads a morphology circuit into a MorphologyCircuitComponent
 */
class MorphologyCircuitBuilder
{
public:
    struct Context
    {
        Context(
            const std::vector<uint64_t> &ids,
            const std::vector<std::string> &morphologyPaths,
            const std::vector<brayns::Vector3f> &positions,
            const std::vector<brayns::Quaternion> &rotations,
            const NeuronMorphologyLoaderParameters &morphologyParams);

        const std::vector<uint64_t> &ids;
        const std::vector<std::string> &morphologyPaths;
        const std::vector<brayns::Vector3f> &positions;
        const std::vector<brayns::Quaternion> &rotations;
        const NeuronMorphologyLoaderParameters &morphologyParams;
    };

    static std::vector<CellCompartments> load(
        const Context &context,
        brayns::Model &model,
        ProgressUpdater &cb,
        std::unique_ptr<IBrainColorData> colorData);
};
