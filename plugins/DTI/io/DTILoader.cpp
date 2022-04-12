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

#include "dtiloader/DTIConfiguration.h"
#include "dtiloader/GIDRowReader.h"

#include <fstream>

namespace
{
struct NormalColorGenerator
{
    static std::vector<brayns::Vector4f> generate(const std::vector<brayns::Vector3f> &points)
    {
        std::vector<brayns::Vector4f> colors(points.size());

        colors[0] = brayns::Vector4f(0.f, 0.f, 0.f, 1.f);

        for (uint64_t i = 1; i < points.size(); ++i)
        {
            const auto &p1 = points[i - 1];
            const auto &p2 = points[i];
            const auto dir = glm::normalize(p2 - p1);
            const auto n = brayns::Vector3f(0.5f + dir.x * 0.5f, 0.5f + dir.y * 0.5f, 0.5f + dir.z * 0.5f);
            colors[i] = brayns::Vector4f(n, 1.f);
        }

        return colors;
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
    const auto config = dtiloader::DTIConfigurationReader::read(path);

    const auto &gidRowsFilePath = config.gidsToStreamlinesPath;
    const auto gidRows = dtiloader::GIDRowReader::read(gidRowsFilePath);

    std::ifstream streamlinesFile(config.streamlines, std::ios::in);
    if (!streamlinesFile.good())
        throw std::runtime_error("Could not open streamlines file " + config.streamlines);

    // Load positions
    callback.updateProgress("Loading positions ...", 0.f);

    // Rows to load
    std::set<uint64_t> rowsToLoad;
    for (const auto &gidRow : gidRows)
    {
        rowsToLoad.insert(gidRow.row);
    }

    // Load points
    std::map<uint64_t, std::vector<brayns::Vector3f>> streamlines;
    uint64_t count = 0;
    callback.updateProgress("Loading streamlines ...", 0.4f);
    while (streamlinesFile.good())
    {
        std::string line;
        std::getline(streamlinesFile, line);

        if (rowsToLoad.find(count) != rowsToLoad.end())
        {
            std::istringstream in(line);
            uint64_t nbPoints;
            in >> nbPoints;

            auto &streamline = streamlines[count];
            streamline.reserve(nbPoints);
            for (uint64_t i = 0; i < nbPoints; ++i)
            {
                brayns::Vector3f point;
                in >> point.x >> point.y >> point.z;
                streamline.push_back(point);
            }
        }
        ++count;
    }
    streamlinesFile.close();

    // Create model
    auto model = scene.createModel();
    const auto nbStreamlines = gidRows.size();
    count = 0;
    uint64_t i = 0;
    std::set<u_int16_t> streamlineAdded;
    for (const auto &gidRow : gidRows)
    {
        const auto it = streamlines.find(gidRow.row);
        if (it != streamlines.end() && streamlineAdded.find(gidRow.row) == streamlineAdded.end())
        {
            streamlineAdded.insert(gidRow.row);
            callback.updateProgress(
                "Creating " + std::to_string(count) + " streamlines ...",
                0.6f + 0.2f * float(i) / float(nbStreamlines));

            const auto &points = (*it).second;
            const auto nbPoints = points.size();
            const auto colors = getColorsFromPoints(points, input.opacity, input.color_scheme);

            std::vector<float> radii;
            radii.resize(nbPoints, input.radius);

            brayns::Streamline streamline(points, colors, radii);
            model->createMaterial(gidRow.gid, std::to_string(gidRow.gid));
            model->addStreamline(gidRow.gid, streamline);
            ++count;
        }
        ++i;
    }

    callback.updateProgress("Committing " + std::to_string(count) + " streamlines ...", 0.8f);
    brayns::ModelMetadata metadata = {{"Number of streamlines", std::to_string(count)}};
    auto modelDescriptor = std::make_shared<brayns::ModelDescriptor>(std::move(model), "DTI", metadata);
    callback.updateProgress("Done", 1.f);
    return {modelDescriptor};
}
} // namespace dti
