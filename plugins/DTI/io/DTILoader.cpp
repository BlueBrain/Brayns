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

#include "DTILoader.h"

#include <brayns/common/Log.h>
#include <brayns/json/JsonObjectMacro.h>
#include <brayns/json/JsonSchemaValidator.h>
#include <brayns/utils/FileReader.h>
#include <brayns/utils/StringUtils.h>

#include <components/DTIComponent.h>
#include <components/SpikeReportComponent.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <unordered_set>

#include <brain/spikeReportReader.h>
#include <brion/blueConfig.h>

namespace
{
BRAYNS_JSON_OBJECT_BEGIN(DTIConfiguration)
BRAYNS_JSON_OBJECT_ENTRY(std::string, streamlines_path, "Path to the streamlines file")
BRAYNS_JSON_OBJECT_ENTRY(std::string, gids_to_streamlines_path, "Path to the gid-streamline mapping file")
BRAYNS_JSON_OBJECT_ENTRY(
    std::string,
    circuit_path,
    "Path to the source circuit for spike simulation",
    brayns::Default(""))
BRAYNS_JSON_OBJECT_END()

class DTIConfigurationReader
{
public:
    static DTIConfiguration read(const std::string &path)
    {
        auto jsonString = brayns::FileReader::read(path);
        auto json = brayns::Json::parse(jsonString);
        auto schema = brayns::Json::getSchema<DTIConfiguration>();
        auto jsonErrors = brayns::JsonSchemaValidator::validate(json, schema);

        if (!jsonErrors.empty())
        {
            auto errorString = brayns::string_utils::join(jsonErrors, "\n");
            throw std::invalid_argument("Ill-formed dti config file: " + errorString);
        }

        return brayns::Json::deserialize<DTIConfiguration>(json);
    }
};

struct GIDRow
{
    uint64_t gid{};
    uint64_t row{};
};

std::istream &operator>>(std::istream &in, GIDRow &gr)
{
    return in >> gr.gid >> gr.row;
}
class GIDRowReader
{
public:
    static std::vector<GIDRow> read(const std::string &path)
    {
        const auto content = brayns::FileReader::read(path);
        std::istringstream stream(content);
        std::vector<GIDRow> gidRows(std::istream_iterator<GIDRow>(stream), {});
        return gidRows;
    }
};

struct StreamlineData
{
    size_t linealIndex{};
    std::vector<brayns::Vector3f> points;
};

/**
 * @brief Used to filter which rows to load in RowStreamlineMapReader
 */
class StreamlineRowFilter
{
public:
    virtual bool filter(const size_t row) const noexcept = 0;
};

// When loading without gid->row file (in other words, without simulation)
class NoopStreamlineRowFilter final : public StreamlineRowFilter
{
public:
    bool filter(const size_t row) const noexcept override
    {
        return true;
    }
};

// When loading with gid->row file (in other words, with simulation)
class GIDRowStreamlineRowFilter final : public StreamlineRowFilter
{
public:
    GIDRowStreamlineRowFilter(const std::vector<GIDRow> &gidRows)
    {
        for (const auto &entry : gidRows)
        {
            const auto row = entry.row;
            _whitelistedRows.insert(row);
        }
    }

    bool filter(const size_t row) const noexcept override
    {
        auto it = _whitelistedRows.find(row);
        return it != _whitelistedRows.end();
    }

private:
    std::unordered_set<size_t> _whitelistedRows;
};

class RowStreamlineMapReader
{
public:
    static std::map<uint64_t, StreamlineData> read(const std::string &path, const StreamlineRowFilter &filter)
    {
        std::map<uint64_t, StreamlineData> result;

        const auto content = brayns::FileReader::read(path);
        std::istringstream stream(content);

        size_t row = 0;
        size_t index = 0;

        while (stream.good())
        {
            std::string line;
            std::getline(stream, line);

            if (!line.empty() && filter.filter(row))
            {
                std::istringstream lineStream(line);
                uint64_t nbPoints;
                lineStream >> nbPoints;

                if (!lineStream.good() || nbPoints == 0)
                {
                    throw std::runtime_error("Row " + std::to_string(row));
                }

                auto &streamline = result[row];

                streamline.linealIndex = index++;

                auto &points = streamline.points;
                points.reserve(nbPoints);

                for (uint64_t i = 0; i < nbPoints && lineStream.good(); ++i)
                {
                    brayns::Vector3f point;
                    lineStream >> point.x >> point.y >> point.z;
                    points.push_back(point);
                }
            }

            ++row;
        }

        return result;
    }
};

class StreamlineComponentBuilder
{
public:
    static void build(const std::map<uint64_t, StreamlineData> &streamlines, const float radius, brayns::Model &model)
    {
        std::vector<std::vector<brayns::Primitive>> geometries;
        geometries.reserve(streamlines.size());

        for (const auto &[row, streamline] : streamlines)
        {
            const auto &points = streamline.points;

            auto &geometry = geometries.emplace_back();
            geometry.reserve(points.size() - 1);

            for (size_t i = 1; i < points.size(); ++i)
            {
                const auto &start = points[i - 1];
                const auto &end = points[i];
                geometry.push_back(brayns::Primitive::cylinder(start, end, radius));
            }
        }
        model.addComponent<dti::DTIComponent>(std::move(geometries));
    }
};

class GIDsToStreamlineIndicesMapping
{
public:
    static std::unordered_map<uint64_t, std::vector<size_t>> generate(
        const std::vector<GIDRow> &gidRows,
        const std::map<uint64_t, StreamlineData> &streamlinesMap)
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

class SpikeReportComponentBuilder
{
public:
    static void build(
        std::unordered_map<uint64_t, std::vector<size_t>> gidStreamlineMap,
        float decayTime,
        const std::string &circuitPath,
        brayns::Model &model)
    {
        const auto blueConfig = brion::BlueConfig(circuitPath);
        const auto spikesURI = blueConfig.getSpikeSource();
        auto spikeReport = std::make_unique<brain::SpikeReportReader>(spikesURI);
        model.addComponent<dti::SpikeReportComponent>(std::move(spikeReport), std::move(gidStreamlineMap), decayTime);
    }
};

} // namespace

namespace dti
{
std::string DTILoader::getName() const
{
    return "DTI loader";
}

std::vector<std::string> DTILoader::getSupportedExtensions() const
{
    return {"dti"};
}

std::vector<std::unique_ptr<brayns::Model>> DTILoader::importFromBlob(
    const brayns::Blob &blob,
    const brayns::LoaderProgress &callback,
    const DTILoaderParameters &params) const
{
    (void)blob;
    (void)callback;
    (void)params;
    throw std::runtime_error("Loading DTI from blob is not supported");
}

std::vector<std::unique_ptr<brayns::Model>> DTILoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback,
    const DTILoaderParameters &params) const
{
    std::vector<std::unique_ptr<brayns::Model>> result;
    result.push_back(std::make_unique<brayns::Model>());
    auto &model = *(result.back());

    callback.updateProgress("Reading configuration", 0.f);
    const auto config = DTIConfigurationReader::read(path);

    const auto &circuitPath = config.circuitPath;

    if (circuitPath.empty())
    {
        callback.updateProgress("Loading streamlines", 0.33f);
        const NoopStreamlineRowFilter filter;
        const auto &streamlinesPath = config.streamlinesPath;
        auto rowStreamlineMap = RowStreamlineMapReader::read(streamlinesPath, filter);

        callback.updateProgress("Generating geometry", 0.66f);
        const auto radius = params.radius;
        StreamlineComponentBuilder::build(rowStreamlineMap, radius, model);

        callback.updateProgress("Done", 1.f);
        return result;
    }
    else
    {
        callback.updateProgress("Reading gid to row mapping file (might take time)", 0.2f);
        const auto &gidRowsFilePath = config.gidsToStreamlinesPath;
        const auto gidRows = GIDRowReader::read(gidRowsFilePath);

        callback.updateProgress("Loading streamlines", 0.4f);
        const GIDRowStreamlineRowFilter filter(gidRows);
        const auto &streamlinesPath = config.streamlinesPath;
        auto rowStreamlineMap = RowStreamlineMapReader::read(streamlinesPath, filter);

        callback.updateProgress("Generating geometry", 0.6f);
        const auto radius = params.radius;
        StreamlineComponentBuilder::build(rowStreamlineMap, radius, model);

        callback.updateProgress("Loading simulation", 0.8f);
        auto gidsToStreamlineIndices = GIDsToStreamlineIndicesMapping::generate(gidRows, rowStreamlineMap);
        auto spikeDecayTime = params.spike_decay_time;
        SpikeReportComponentBuilder::build(std::move(gidsToStreamlineIndices), spikeDecayTime, circuitPath, model);
    }

    callback.updateProgress("Done", 1.f);
    return result;
}
} // namespace dti
