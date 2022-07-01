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

#include <components/SpikeReportComponent.h>

#include <brayns/utils/FileReader.h>

#include <brain/spikeReportReader.h>
#include <brion/blueConfig.h>

#include <unordered_set>

namespace
{
std::istream &operator>>(std::istream &in, dti::SimulatedDTIBuilder::GIDRow &gr)
{
    return in >> gr.gid >> gr.row;
}

class GIDsToStreamlineIndicesMapping
{
public:
    static std::unordered_map<uint64_t, std::vector<size_t>> generate(
        const std::vector<dti::SimulatedDTIBuilder::GIDRow> &gidRows,
        const std::map<uint64_t, dti::StreamlineData> &streamlinesMap)
    {
        std::unordered_map<uint64_t, std::vector<size_t>> result;
        for (const auto &gidRow : gidRows)
        {
            const auto gid = gidRow.gid;
            const auto row = gidRow.row;

            const auto &streamline = streamlinesMap.at(row);
            const auto streamlineIndex = streamline.linealIndex;

            auto &gidIndexList = result[gid];
            gidIndexList.push_back(streamlineIndex);
        }

        return result;
    }
};
}

namespace dti
{
void SimulatedDTIBuilder::reset()
{
    _gidRows.clear();
    _streamlines.clear();
}

void SimulatedDTIBuilder::readGidRowFile(const std::string &path)
{
    const auto content = brayns::FileReader::read(path);
    std::istringstream stream(content);
    _gidRows = std::vector<GIDRow>(std::istream_iterator<GIDRow>(stream), {});
}

void SimulatedDTIBuilder::readStreamlinesFile(const std::string &path)
{
    std::unordered_set<size_t> whitelistedRows;
    for (const auto &entry : _gidRows)
    {
        const auto row = entry.row;
        whitelistedRows.insert(row);
    }

    _streamlines = RowStreamlineMapReader::read(
        path,
        [&](size_t row) { return whitelistedRows.find(row) != whitelistedRows.end(); });
}

void SimulatedDTIBuilder::buildGeometry(float radius, brayns::Model &model)
{
    StreamlineComponentBuilder::build(_streamlines, radius, model);
}

void SimulatedDTIBuilder::buildSimulation(const std::string &path, float spikeDecayTime, brayns::Model &model)
{
    auto gidStreamlineMap = GIDsToStreamlineIndicesMapping::generate(_gidRows, _streamlines);
    const auto blueConfig = brion::BlueConfig(path);
    const auto spikesURI = blueConfig.getSpikeSource();
    auto spikeReport = std::make_unique<brain::SpikeReportReader>(spikesURI);
    model.addComponent<dti::SpikeReportComponent>(std::move(spikeReport), std::move(gidStreamlineMap), spikeDecayTime);
}
}
