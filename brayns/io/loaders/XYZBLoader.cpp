/* Copyright 2015-2024 Blue Brain Project/EPFL
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
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/utils/Filesystem.h>
#include <brayns/utils/StringUtils.h>

#include <fstream>
#include <sstream>

namespace brayns
{
namespace
{
constexpr auto ALMOST_ZERO = 1e-7f;
constexpr auto LOADER_NAME = "xyzb";

float _computeHalfArea(const Boxf& bbox)
{
    const auto size = bbox.getSize();
    return size[0] * size[1] + size[0] * size[2] + size[1] * size[2];
}
} // namespace

std::vector<ModelDescriptorPtr> XYZBLoader::importFromBlob(
    Blob&& blob, const LoaderProgress& callback, Scene& scene) const
{
    Log::info("Loading xyz {}.", blob.name);

    std::stringstream stream(std::string(blob.data.begin(), blob.data.end()));
    size_t numlines = 0;
    {
        numlines = std::count(std::istreambuf_iterator<char>(stream),
                              std::istreambuf_iterator<char>(), '\n');
    }
    stream.seekg(0);

    auto model = scene.createModel();

    const auto name = fs::path({blob.name}).stem().string();
    const auto materialId = 0;
    model->createMaterial(materialId, name);
    auto& spheres = model->getSpheres()[materialId];

    const size_t startOffset = spheres.size();
    spheres.reserve(spheres.size() + numlines);

    Boxf bbox;
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
            bbox.merge(position);
            // The point radius used here is irrelevant as it's going to be
            // changed later.
            model->addSphere(materialId, {position, 1});
            break;
        }
        default:
            throw std::runtime_error("Invalid content in line " +
                                     std::to_string(i + 1) + ": " + line);
        }
        callback.updateProgress(msg.str(), i++ / static_cast<float>(numlines));
    }

    // Find an appropriate mean radius to avoid overlaps of the spheres, see
    // https://en.wikipedia.org/wiki/Wigner%E2%80%93Seitz_radius

    const auto volume = glm::compMul(bbox.getSize());
    const auto density4PI =
        4 * M_PI * numlines /
        (volume > ALMOST_ZERO ? volume : _computeHalfArea(bbox));

    const double meanRadius = volume > ALMOST_ZERO
                                  ? std::pow((3. / density4PI), 1. / 3.)
                                  : std::sqrt(1 / density4PI);

    // resize the spheres to the new mean radius
    for (i = 0; i < numlines; ++i)
        spheres[i + startOffset].radius = meanRadius;

    Transformation transformation;
    transformation.setRotationCenter(model->getBounds().getCenter());
    auto modelDescriptor =
        std::make_shared<ModelDescriptor>(std::move(model), blob.name);
    modelDescriptor->setTransformation(transformation);

    Property radiusProperty("radius", meanRadius, {"Point size"});
    radiusProperty.onModified([modelDesc = std::weak_ptr<ModelDescriptor>(
                                   modelDescriptor)](const Property& property) {
        if (auto modelDesc_ = modelDesc.lock())
        {
            const auto newRadius = property.as<double>();
            for (auto& sphere : modelDesc_->getModel().getSpheres()[materialId])
                sphere.radius = newRadius;
        }
    });
    PropertyMap modelProperties;
    modelProperties.add(radiusProperty);
    modelDescriptor->setProperties(modelProperties);
    return {modelDescriptor};
}

std::vector<ModelDescriptorPtr> XYZBLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    Scene& scene) const
{
    std::ifstream file(filename);
    if (!file.good())
        throw std::runtime_error("Could not open file " + filename);
    return importFromBlob({"xyz",
                           filename,
                           {std::istreambuf_iterator<char>(file),
                            std::istreambuf_iterator<char>()}},
                          callback, scene);
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
