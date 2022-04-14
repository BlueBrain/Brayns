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
#include <brayns/utils/FileReader.h>

#include <components/DTIComponent.h>
#include <components/SpikeReportComponent.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <unordered_set>

#include <boost/property_tree/ini_parser.hpp>

#include <brain/spikeReportReader.h>
#include <brion/blueConfig.h>

namespace
{
struct DTIConfiguration
{
    // Streamline points (1 streamline per row)
    std::filesystem::path streamlinesPath;
    // Multimap Gid -> streamline it affects
    std::filesystem::path gidsToStreamlinesPath;
    // Optional path to a Blueconfig to load a spike report
    std::optional<std::filesystem::path> circuitPath;
};

struct DTIConfigurationReader
{
    static DTIConfiguration read(const std::string &path)
    {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(path, pt);

        DTIConfiguration configuration;

        configuration.streamlinesPath = pt.get<std::string>("streamlines");
        configuration.gidsToStreamlinesPath = pt.get<std::string>("gids_to_streamline_row");

        const auto circuitFile = pt.get_optional<std::string>("circuit");
        if (circuitFile.has_value())
        {
            configuration.circuitPath = *circuitFile;
        }

        return configuration;
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
struct GIDRowReader
{
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
struct StreamlineRowFilter
{
    virtual bool filter(const size_t row) const noexcept = 0;
};

// When loading without gid->row file (in other words, without simulation)
struct NoopStreamlineRowFilter final : public StreamlineRowFilter
{
    bool filter(const size_t row) const noexcept override
    {
        return true;
    }
};

// When loading with gid->row file (in other words, with simulation)
struct GIDRowStreamlineRowFilter final : public StreamlineRowFilter
{
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

struct RowStreamlineMapReader
{
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

struct StreamlineComponentBuilder
{
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

        auto &dti = model.addComponent<dti::DTIComponent>();
        dti.setStreamlines(geometries);
    }
};

struct GIDsToStreamlineIndicesMapping
{
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

struct SpikeReportComponentBuilder
{
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
    brayns::Blob &&blob,
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

    const auto &circuitPathEntry = config.circuitPath;

    if (!circuitPathEntry.has_value())
    {
        callback.updateProgress("Loading streamlines", 0.33f);
        const NoopStreamlineRowFilter filter;
        const auto &streamlinesPath = config.streamlinesPath;
        auto rowStreamlineMap = RowStreamlineMapReader::read(streamlinesPath, filter);

        callback.updateProgress("Generating geometry", 0.66f);
        const auto radius = params.radius;
        StreamlineComponentBuilder::build(rowStreamlineMap, radius, model);
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
        const auto &circuitPath = *circuitPathEntry;
        auto gidsToStreamlineIndices = GIDsToStreamlineIndicesMapping::generate(gidRows, rowStreamlineMap);
        auto spikeDecayTime = params.spike_decay_time;
        SpikeReportComponentBuilder::build(std::move(gidsToStreamlineIndices), spikeDecayTime, circuitPath, model);
    }

    callback.updateProgress("Done", 1.f);
    return result;
}
} // namespace dti
