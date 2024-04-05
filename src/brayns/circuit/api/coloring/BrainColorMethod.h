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

#include <brayns/core/utils/EnumInfo.h>

/**
 * @brief Methods availables to color a brain circuit by (if corresponding data is available as well)
 */
enum class BrainColorMethod
{
    ByEtype,
    ByMtype,
    ByLayer,
    ByRegion,
    ByHemisphere,
    ByMorphology,
    ByMorphologyClass,
    BySubregion,
    BySynapseClass
};

namespace brayns
{
template<>
struct EnumReflector<BrainColorMethod>
{
    static EnumMap<BrainColorMethod> reflect()
    {
        return {
            {"etype", BrainColorMethod::ByEtype},
            {"mtype", BrainColorMethod::ByMtype},
            {"layer", BrainColorMethod::ByLayer},
            {"region", BrainColorMethod::ByRegion},
            {"hemisphere", BrainColorMethod::ByHemisphere},
            {"morphology", BrainColorMethod::ByMorphology},
            {"morphology class", BrainColorMethod::ByMorphologyClass},
            {"subregion", BrainColorMethod::BySubregion},
            {"synapse class", BrainColorMethod::BySynapseClass}};
    }
};
}
