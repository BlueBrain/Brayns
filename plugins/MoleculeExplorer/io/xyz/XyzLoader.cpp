/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "XyzLoader.h"

#include <brayns/utils/FileReader.h>
#include <brayns/utils/Log.h>

#include <brayns/engine/colormethods/PrimitiveColorMethod.h>
#include <brayns/engine/colormethods/SolidColorMethod.h>

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GenericColorSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>

#include <brayns/engine/geometry/types/Sphere.h>

#include <brayns/utils/string/StringTrimmer.h>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace
{
class XyzReader
{
public:
    static inline constexpr float defaultRadius = 0.15f;

    static std::vector<brayns::Sphere> fromBytes(const brayns::LoaderProgress &progress, const std::string &bytes)
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
            progress(msg, i++ / static_cast<float>(lineCount));
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

class XYZColorMethods
{
public:
    static auto build(size_t primitiveCount)
    {
        auto result = std::vector<std::unique_ptr<brayns::IColorMethod>>();
        result.push_back(std::make_unique<brayns::SolidColorMethod>());
        result.push_back(std::make_unique<brayns::PrimitiveColorMethod>("vertex", primitiveCount));
        return result;
    }
};
} // namespace

std::string XyzLoader::getName() const
{
    return "xyz";
}

std::vector<std::string> XyzLoader::getExtensions() const
{
    return {"xyz"};
}

bool XyzLoader::canLoadBinary() const
{
    return true;
}

std::vector<std::shared_ptr<brayns::Model>> XyzLoader::loadBinary(const BinaryRequest &request)
{
    auto data = request.data;
    auto &progress = request.progress;

    brayns::Log::info("[ME] Loading xyz.");

    auto spheres = XyzReader::fromBytes(progress, std::string(data));
    auto sphereCount = spheres.size();

    auto model = std::make_shared<brayns::Model>("xyz");

    auto &components = model->getComponents();
    components.add<brayns::Geometries>(std::move(spheres));

    auto &systems = model->getSystems();
    systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
    systems.setDataSystem<brayns::GeometryDataSystem>();
    systems.setColorSystem<brayns::GenericColorSystem>(XYZColorMethods::build(sphereCount));

    std::vector<std::shared_ptr<brayns::Model>> result;
    result.push_back(std::move(model));
    return result;
}
