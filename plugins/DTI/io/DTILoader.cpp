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

#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <unordered_set>

#include <boost/property_tree/ini_parser.hpp>

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

struct RowStreamlineMapReader
{
    static std::map<uint64_t, StreamlineData>
        read(const std::string &path, const std::vector<GIDRow> &gidRowMapping, const brayns::LoaderProgress &callback)
    {
        // Load only the rows that are going to be used
        std::unordered_set<uint64_t> rowsToLoad;
        for (const auto &gidRow : gidRowMapping)
        {
            rowsToLoad.insert(gidRow.row);
        }

        std::map<uint64_t, StreamlineData> result;
        const auto content = brayns::FileReader::read(path);
        std::istringstream stream(content);
        const auto streamlineProgress = 0.2f / static_cast<float>(rowsToLoad.size());
        auto overallProgress = 0.4f;
        size_t row = 0;
        size_t index = 0;
        while (stream.good())
        {
            std::string line;
            std::getline(stream, line);

            const auto check = rowsToLoad.find(row);

            if (check != rowsToLoad.end())
            {
                std::istringstream lineStream(line);
                uint64_t nbPoints;
                lineStream >> nbPoints;

                auto &streamline = result[row];

                streamline.linealIndex = index++;

                auto &points = streamline.points;
                points.reserve(nbPoints);

                for (uint64_t i = 0; i < nbPoints; ++i)
                {
                    brayns::Vector3f point;
                    lineStream >> point.x >> point.y >> point.z;
                    points.push_back(point);
                }

                overallProgress += streamlineProgress;
                callback.updateProgress("Loading streamlines", overallProgress);
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
                const auto &end = points[i + 1];
                geometry.push_back(brayns::Primitive::cylinder(start, end, radius));
            }
        }

        auto &dti = model.addComponent<DTIComponent>();
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
    callback.updateProgress("Reading configuration", 0.f);
    const auto config = DTIConfigurationReader::read(path);

    callback.updateProgress("Reading gid to row mapping file (might take time)", 0.2f);
    const auto &gidRowsFilePath = config.gidsToStreamlinesPath;
    const auto gidRows = GIDRowReader::read(gidRowsFilePath);

    callback.updateProgress("Loading streamlines", 0.4f);
    const auto &streamlinesPath = config.streamlinesPath;
    auto rowStreamlineMap = RowStreamlineMapReader::read(streamlinesPath, gidRows, callback);

    callback.updateProgress("Generating geometry", 0.6f);
    std::vector<std::unique_ptr<brayns::Model>> result;
    result.push_back(std::make_unique<brayns::Model>());
    auto &model = *(result.back());
    const auto radius = params.radius;
    StreamlineComponentBuilder::build(rowStreamlineMap, radius, model);

    const auto &circuitPath = config.circuitPath;
    if (circuitPath.has_value())
    {
        callback.updateProgress("Loading simulation", 0.8f);
        auto gidsToStreamlineIndices = GIDsToStreamlineIndicesMapping::generate(gidRows, rowStreamlineMap);
        auto spikeDecayTime = params.spike_decay_time;
    }

    callback.updateProgress("Done", 1.f);
    return result;
}
} // namespace dti
