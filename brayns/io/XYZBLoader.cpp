/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include <brayns/common/log.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>
#include <fstream>

#include "XYZBLoader.h"

namespace brayns
{
XYZBLoader::XYZBLoader(const GeometryParameters& geometryParameters)
    : _geometryParameters(geometryParameters)
{
}

std::set<std::string> XYZBLoader::getSupportedDataTypes()
{
    return {"xyz"};
}

void XYZBLoader::importFromBlob(Blob&& blob, Scene& scene,
                                const size_t index BRAYNS_UNUSED,
                                const Matrix4f& transformation,
                                const size_t defaultMaterialId BRAYNS_UNUSED)
{
    BRAYNS_INFO << "Loading xyz " << blob.name << std::endl;

    std::stringstream stream(blob.data);
    size_t numlines = 0;
    {
        numlines = std::count(std::istreambuf_iterator<char>(stream),
                              std::istreambuf_iterator<char>(), '\n');
    }
    stream.seekg(0);

    const std::string name = "MemoryPointCloud";
    auto model = scene.createModel(name);

    try
    {
        const auto materialId = 0;
        model->createMaterial(materialId, name);
        auto& spheres = model->getSpheres()[materialId];

        const size_t startOffset = spheres.size();
        spheres.reserve(spheres.size() + numlines);

        size_t i = 0;
        std::string line;
        std::stringstream msg;
        msg << "Loading " << shortenString(blob.name) << " ..." << std::endl;
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
                const Vector4f position(lineData[0], lineData[1], lineData[2],
                                        1.f);
                model->addSphere(materialId,
                                 {transformation * position,
                                  _geometryParameters.getRadiusMultiplier()});
                break;
            }
            default:
                throw std::runtime_error("Invalid content in line " +
                                         std::to_string(i + 1) + ": " + line);
            }
            updateProgress(msg.str(), i++, numlines);
        }

        const float maxDim = scene.getBounds().getSize().find_max();
        if (maxDim < 100 * _geometryParameters.getRadiusMultiplier())
        {
            const float newRadius = maxDim / 100.f;
            BRAYNS_WARN << "Given radius "
                        << _geometryParameters.getRadiusMultiplier()
                        << " is too big for this scene, using radius "
                        << newRadius << " now" << std::endl;

            for (i = 0; i < numlines; ++i)
                spheres[i + startOffset].radius = newRadius;
        }
    }
    catch (const std::runtime_error&)
    {
        // Failed to load point cloud. Removing last created model
        scene.removeModel(scene.getModelDescriptors().size() - 1);
        throw;
    }
}

void XYZBLoader::importFromFile(const std::string& filename, Scene& scene,
                                const size_t index,
                                const Matrix4f& transformation,
                                const size_t defaultMaterialId)
{
    std::ifstream file(filename);
    if (!file.good())
        throw std::runtime_error("Could not open file " + filename);
    importFromBlob({"xyz",
                    filename,
                    {std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>()}},
                   scene, index, transformation, defaultMaterialId);
}
}
