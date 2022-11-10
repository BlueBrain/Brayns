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

#include <api/vasculature/VasculatureSection.h>
#include <io/util/ProgressUpdater.h>

#include <vector>

class VasculatureCircuitBuilder
{
public:
    struct Context
    {
        std::vector<uint64_t> ids;
        std::vector<brayns::Vector3f> startPos;
        std::vector<float> startRadii;
        std::vector<brayns::Vector3f> endPos;
        std::vector<float> endRadii;
        std::vector<VasculatureSection> sections;
    };

    static void build(brayns::Model &model, Context context);
};
