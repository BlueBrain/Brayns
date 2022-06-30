/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "SimulatedDTIBuilder.h"

#include "common/RowTreamlineMapReader.h"
#include "common/StreamlineComponentBuilder.h"

#include <brayns/utils/FileReader.h>

namespace
{
struct GIDRow
{
    uint64_t gid{};
    uint64_t row{};
};

std::istream &operator>>(std::istream &in, GIDRow &gr)
{
    return in >> gr.gid >> gr.row;
}
}

namespace dti
{
void SimulatedDTIBuilder::reset()
{
    _whitelistedRows.clear();
    _streamlines.clear();
}

void SimulatedDTIBuilder::readGidRowFile(const std::string &path)
{
    const auto content = brayns::FileReader::read(path);
    std::istringstream stream(content);
    std::vector<GIDRow> gidRows(std::istream_iterator<GIDRow>(stream), {});
    return gidRows;
}

void SimulatedDTIBuilder::readStreamlinesFile(const std::string &path)
{
    _streamlines = RowStreamlineMapReader::read(
        path,
        [](size_t row)
        {
            (void)row;
            return true;
        });
}

void SimulatedDTIBuilder::buildGeometry(float radius, brayns::Model &model)
{
    StreamlineComponentBuilder::build(_streamlines, radius, model);
}

void SimulatedDTIBuilder::buildSimulation(const std::string &path, float spikeDecayTime, brayns::Model &model)
{
    (void)path;
    (void)spikeDecayTime;
    (void)model;
}
