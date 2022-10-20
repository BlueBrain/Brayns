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

#include <brayns/utils/Log.h>

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>

#include <brayns/engine/geometry/types/Sphere.h>

#include <brayns/utils/string/StringTrimmer.h>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace
{
class XYZBReader
{
public:
    inline static constexpr float defaultRadius = 0.15f;

    static std::vector<brayns::Sphere> fromBytes(const brayns::LoaderProgress &callback, const std::string &bytes)
    {
        std::vector<brayns::Sphere> spheres;

        std::stringstream stream(bytes);
        auto lineCount = _computeSphereCount(stream);

        spheres.reserve(lineCount);

        size_t i = 0;
        std::string line;
        while (std::getline(stream, line))
        {
            // Handle comments
            line = brayns::StringTrimmer::trim(line);
            if (_filterLine(line))
            {
                continue;
            }

            auto sphere = _parseSphere(line, i + 1);
            spheres.push_back(sphere);

            const std::string msg = "Loading...";
            callback.updateProgress(msg, i++ / static_cast<float>(lineCount));
        }

        return spheres;
    }

private:
    static size_t _computeSphereCount(std::stringstream &stream)
    {
        size_t numlines = std::count(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>(), '\n');
        stream.seekg(0);
        return numlines;
    }

    static bool _filterLine(const std::string &line)
    {
        return line[0] == '#';
    }

    static std::vector<float> _tokenizeLine(const std::string &line)
    {
        std::vector<float> lineData;
        std::stringstream lineStream(line);

        float value;
        while (lineStream >> value)
        {
            lineData.push_back(value);
        }
        return lineData;
    }

    static brayns::Sphere _parseSphere(const std::string &line, const size_t lineNumber)
    {
        auto lineData = _tokenizeLine(line);
        if (lineData.size() != 3)
        {
            throw std::runtime_error("Invalid content in line " + std::to_string(lineNumber + 1) + ": " + line);
        }

        return {{lineData[0], lineData[1], lineData[2]}, defaultRadius};
    }
};
}

namespace brayns
{
std::vector<std::unique_ptr<Model>> XYZBLoader::importFromBlob(const Blob &blob, const LoaderProgress &callback) const
{
    Log::info("Loading xyz {}.", blob.name);

    auto spheres = XYZBReader::fromBytes(callback, std::string(blob.data.begin(), blob.data.end()));

    auto model = std::make_unique<Model>("xyz");

    auto &components = model->getComponents();
    auto &geometries = components.add<Geometries>();
    geometries.elements.emplace_back(std::move(spheres));

    auto &systems = model->getSystems();
    systems.setBoundsSystem<GenericBoundsSystem<Geometries>>();
    systems.setInitSystem<GeometryInitSystem>();
    systems.setCommitSystem<GeometryCommitSystem>();

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
