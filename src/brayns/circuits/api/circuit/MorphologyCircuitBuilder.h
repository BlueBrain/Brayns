/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/core/engine/model/Model.h>
#include <brayns/core/utils/MathTypes.h>

#include <brayns/circuits/api/coloring/IBrainColorData.h>
#include <brayns/circuits/api/reports/ReportMapping.h>
#include <brayns/circuits/io/NeuronMorphologyLoaderParameters.h>
#include <brayns/circuits/io/util/ProgressUpdater.h>

/**
 * @brief The MorphologyCircuitLoader struct loads a morphology circuit into a MorphologyCircuitComponent
 */
class MorphologyCircuitBuilder
{
public:
    struct Context
    {
        std::vector<uint64_t> ids;
        std::vector<std::string> morphologyPaths;
        std::vector<brayns::Vector3f> positions;
        std::vector<brayns::Quaternion> rotations;
        NeuronMorphologyLoaderParameters morphologyParams;
        std::unique_ptr<IBrainColorData> colorData;
    };

    static std::vector<CellCompartments> build(brayns::Model &model, Context context, ProgressUpdater &cb);
};
