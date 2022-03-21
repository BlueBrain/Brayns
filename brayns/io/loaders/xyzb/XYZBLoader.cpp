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

#include <brayns/io/loaders/xyzb/XYZBLoader.h>

#include <brayns/common/Log.h>

#include <brayns/engine/defaultcomponents/GeometryRendererComponent.h>
#include <brayns/engine/defaultcomponents/MaterialComponent.h>
#include <brayns/engine/geometries/Sphere.h>

#include <brayns/utils/StringUtils.h>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace brayns
{
namespace
{
constexpr auto ALMOST_ZERO = 1e-7f;
constexpr auto LOADER_NAME = "xyzb";

float _computeHalfArea(const Bounds &bbox)
{
    const auto size = bbox.getMax() - bbox.getMin();
    return size[0] * size[1] + size[0] * size[2] + size[1] * size[2];
}
} // namespace

std::vector<std::unique_ptr<Model>> XYZBLoader::importFromBlob(Blob &&blob, const LoaderProgress &callback) const
{
    Log::info("Loading xyz {}.", blob.name);

    std::stringstream stream(std::string(blob.data.begin(), blob.data.end()));
    size_t numlines = 0;
    {
        numlines = std::count(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>(), '\n');
    }
    stream.seekg(0);

    const auto name = std::filesystem::path({blob.name}).stem().string();

    std::vector<Sphere> spheres;

    const size_t startOffset = spheres.size();
    spheres.reserve(spheres.size() + numlines);

    Bounds bbox;
    size_t i = 0;
    std::string line;
    std::stringstream msg;
    msg << "Loading " << string_utils::shortenString(blob.name) << " ...";
    while (std::getline(stream, line))
    {
        std::vector<float> lineData;
        std::stringstream lineStream(line);

        float value;
        while (lineStream >> value)
            lineData.push_back(value);

        switch (lineData.size())
        {
        case 3:
        {
            const Vector3f position(lineData[0], lineData[1], lineData[2]);
            bbox.expand(position);
            // The point radius used here is irrelevant as it's going to be
            // changed later.
            spheres.push_back({position, 1});
            break;
        }
        default:
            throw std::runtime_error("Invalid content in line " + std::to_string(i + 1) + ": " + line);
        }
        callback.updateProgress(msg.str(), i++ / static_cast<float>(numlines));
    }

    // Find an appropriate mean radius to avoid overlaps of the spheres, see
    // https://en.wikipedia.org/wiki/Wigner%E2%80%93Seitz_radius

    const auto size = bbox.getMax() - bbox.getMin();
    const auto volume = glm::compMul(size);
    const auto density4PI = 4 * M_PI * numlines / (volume > ALMOST_ZERO ? volume : _computeHalfArea(bbox));

    const double meanRadius = volume > ALMOST_ZERO ? std::pow((3. / density4PI), 1. / 3.) : std::sqrt(1 / density4PI);

    // resize the spheres to the new mean radius
    for (i = 0; i < numlines; ++i)
        spheres[i + startOffset].radius = meanRadius;

    auto model = std::make_unique<Model>();
    model->addComponent<MaterialComponent>();
    model->addComponent<GeometryRendererComponent<Sphere>>(spheres);

    std::vector<std::unique_ptr<Model>> result;
    result.push_back(std::move(model));
    return result;
}

std::vector<std::unique_ptr<Model>> XYZBLoader::importFromFile(const std::string &filename, const LoaderProgress &callback) const
{
    std::ifstream file(filename);
    if (!file.good())
        throw std::runtime_error("XYZBLoader: Could not open file " + filename);
    return importFromBlob(
        {"xyz", filename, {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()}}, callback);
}

std::string XYZBLoader::getName() const
{
    return LOADER_NAME;
}

std::vector<std::string> XYZBLoader::getSupportedExtensions() const
{
    return {"xyz"};
}
} // namespace brayns
