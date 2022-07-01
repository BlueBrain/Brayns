/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "XYZBLoader.h"

#include <brayns/common/Log.h>

#include <brayns/engine/components/GeometryRendererComponent.h>
#include <brayns/engine/geometry/types/Sphere.h>

#include <brayns/utils/StringUtils.h>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace brayns
{
std::vector<std::unique_ptr<Model>> XYZBLoader::importFromBlob(const Blob &blob, const LoaderProgress &callback) const
{
    Log::info("Loading xyz {}.", blob.name);

    std::stringstream stream(std::string(blob.data.begin(), blob.data.end()));

    // Apprimately compute how much memory we will need
    size_t numlines = 0;
    {
        numlines = std::count(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>(), '\n');
    }
    stream.seekg(0);

    const auto name = std::filesystem::path({blob.name}).stem().string();
    const auto msg = "Loading " + name + " ...";

    std::vector<Sphere> spheres;
    spheres.reserve(numlines);

    size_t i = 0;
    std::string line;
    while (std::getline(stream, line))
    {
        // Handle comments
        string_utils::trim(line);
        if (line[0] == '#')
        {
            continue;
        }

        std::vector<float> lineData;
        std::stringstream lineStream(line);

        float value;
        while (lineStream >> value)
        {
            lineData.push_back(value);
        }

        switch (lineData.size())
        {
        case 3:
        {
            const Vector3f position(lineData[0], lineData[1], lineData[2]);
            spheres.push_back({position, 0.15f});
            break;
        }
        default:
        {
            throw std::runtime_error("Invalid content in line " + std::to_string(i + 1) + ": " + line);
        }
        }
        callback.updateProgress(msg, i++ / static_cast<float>(numlines));
    }

    auto model = std::make_unique<Model>();
    model->addComponent<GeometryRendererComponent<Sphere>>(std::move(spheres));

    std::vector<std::unique_ptr<Model>> result;
    result.push_back(std::move(model));
    return result;
}

std::vector<std::unique_ptr<Model>> XYZBLoader::importFromFile(
    const std::string &filename,
    const LoaderProgress &callback) const
{
    std::ifstream file(filename);
    if (!file.good())
    {
        throw std::runtime_error("XYZBLoader: Could not open file " + filename);
    }

    auto begin = std::istreambuf_iterator<char>(file);
    auto end = std::istreambuf_iterator<char>();
    return importFromBlob({"xyz", filename, {begin, end}}, callback);
}

std::string XYZBLoader::getName() const
{
    return "xyzb";
}

std::vector<std::string> XYZBLoader::getSupportedExtensions() const
{
    return {"xyz"};
}
} // namespace brayns
