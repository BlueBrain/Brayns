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

#include <brain/spikeReportReader.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace dti
{
struct SpikeReportData
{
    struct Spike
    {
        float time;
        uint32_t gid;
    };

    size_t numStreamlines;
    std::vector<Spike> spikes;
    std::unordered_map<uint64_t, std::vector<size_t>> gidToFibers;
    float decayTime = 1.f;
    bool lastEnabledFlag = true;
};
}
