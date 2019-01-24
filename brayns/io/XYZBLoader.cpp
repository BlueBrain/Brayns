/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include <brayns/common/log.h>
#include <brayns/common/utils/utils.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <fstream>

#include <boost/filesystem.hpp>

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
}

XYZBLoader::XYZBLoader(Scene& scene)
    : Loader(scene)
{
}

bool XYZBLoader::isSupported(const std::string& filename BRAYNS_UNUSED,
                             const std::string& extension) const
{
    const std::set<std::string> types = {"xyz"};
    return types.find(extension) != types.end();
}

ModelDescriptorPtr XYZBLoader::importFromBlob(
    Blob&& blob, const LoaderProgress& callback,
    const PropertyMap& properties BRAYNS_UNUSED,
    const size_t index BRAYNS_UNUSED,
    const size_t defaultMaterialId BRAYNS_UNUSED) const
{
    BRAYNS_INFO << "Loading xyz " << blob.name << std::endl;

    std::stringstream stream(blob.data);
    size_t numlines = 0;
    {
        numlines = std::count(std::istreambuf_iterator<char>(stream),
                              std::istreambuf_iterator<char>(), '\n');
    }
    stream.seekg(0);

    auto model = _scene.createModel();

    const auto name = boost::filesystem::basename({blob.name});
    const auto materialId =
        (defaultMaterialId == NO_MATERIAL ? 0 : defaultMaterialId);
    model->createMaterial(materialId, name);
    auto& spheres = model->getSpheres()[materialId];

    const size_t startOffset = spheres.size();
    spheres.reserve(spheres.size() + numlines);

    Boxf bbox;
    size_t i = 0;
    std::string line;
    std::stringstream msg;
    msg << "Loading " << shortenString(blob.name) << " ...";
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

    const auto volume = bbox.getSize().product();
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

    Property radiusProperty("radius", meanRadius, 0., meanRadius * 2.,
                            {"Point size"});
    radiusProperty.onModified([
        materialId, modelDesc = std::weak_ptr<ModelDescriptor>(modelDescriptor)
    ](const auto& property) {
        if (auto modelDesc_ = modelDesc.lock())
        {
            const auto newRadius = property.template get<double>();
            for (auto& sphere : modelDesc_->getModel().getSpheres()[materialId])
                sphere.radius = newRadius;
        }
    });
    PropertyMap modelProperties;
    modelProperties.setProperty(radiusProperty);
    modelDescriptor->setProperties(modelProperties);
    return modelDescriptor;
}

ModelDescriptorPtr XYZBLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    const PropertyMap& properties, const size_t index,
    const size_t defaultMaterialId) const
{
    std::ifstream file(filename);
    if (!file.good())
        throw std::runtime_error("Could not open file " + filename);
    return importFromBlob({"xyz",
                           filename,
                           {std::istreambuf_iterator<char>(file),
                            std::istreambuf_iterator<char>()}},
                          callback, properties, index, defaultMaterialId);
}

std::string XYZBLoader::getName() const
{
    return LOADER_NAME;
}

std::vector<std::string> XYZBLoader::getSupportedExtensions() const
{
    return {"xyz"};
}
}
