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

#include "SimulatedDTIBuilder.h"

#include "common/RowTreamlineMapReader.h"
#include "common/StreamlineComponentBuilder.h"

#include <components/SpikeReportData.h>
#include <systems/SpikeReportSystem.h>

#include <brayns/engine/components/SimulationInfo.h>
#include <brayns/utils/FileReader.h>

#include <brain/spikeReportReader.h>
#include <brion/blueConfig.h>

#include <unordered_set>

namespace
{
class GIDsToFibersMapping
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

class SpikeReader
{
public:
    explicit SpikeReader(const std::string &blueConfigPath):
        _reader(brion::BlueConfig(blueConfigPath).getSpikeSource())
    {
    }

    std::vector<dti::SpikeReportData::Spike> readAll()
    {
        auto allSpikes = _reader.getSpikes(0.f, _reader.getEndTime());
        auto result = std::vector<dti::SpikeReportData::Spike>();
        result.reserve(allSpikes.size());
        for (auto &spike : allSpikes)
        {
            result.push_back({spike.first, spike.second});
        }
        return result;
    }

    float getEndTime()
    {
        return _reader.getEndTime();
    }

private:
    brain::SpikeReportReader _reader;
};
}

namespace dti
{
std::istream &operator>>(std::istream &in, SimulatedDTIBuilder::GIDRow &gr)
{
    return in >> gr.gid >> gr.row;
}

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
    auto spikeReader = SpikeReader(path);
    auto spikes = spikeReader.readAll();
    auto endTime = spikeReader.getEndTime();

    auto &components = model.getComponents();
    auto &spikeData = components.add<SpikeReportData>();
    spikeData.numStreamlines = _streamlines.size();
    spikeData.spikes = std::move(spikes);
    spikeData.gidToFibers = GIDsToFibersMapping::generate(_gidRows, _streamlines);
    spikeData.decayTime = spikeDecayTime;

    components.add<brayns::SimulationInfo>(0.f, endTime, 0.01f);

    auto &systems = model.getSystems();
    systems.setUpdateSystem<SpikeReportSystem>();
}
}
