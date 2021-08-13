/* Copyright (c) 2021 EPFL/Blue Brain Project
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "PointCloudLoader.h"

#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/network/common/ExtractModel.h>

#include "PointCloudMesher.h"

namespace
{
class PointCloudBuilder
{
private:
    using Handler = brayns::AbstractSimulationHandler;

public:
    static PointCloud build(brayns::Model& model, const PointCloudInfo& info)
    {
        PointCloud cloud;
        auto data = _getFrameData(model, info.frame);
        for (const auto& pair : model.getSpheres())
        {
            auto& points = cloud[pair.first];
            for (const auto& sphere : pair.second)
            {
                auto value = data[sphere.userData];
                if (!_match(info, value))
                {
                    continue;
                }
                points.push_back(_getPoint(sphere));
            }
        }
        return cloud;
    }

private:
    static float* _getFrameData(brayns::Model& model, uint32_t frame)
    {
        auto& handler = _getSimulationHandler(model);
        auto data = handler.getFrameData(frame);
        return static_cast<float*>(data);
    }

    static Handler& _getSimulationHandler(brayns::Model& model)
    {
        auto handler = model.getSimulationHandler();
        if (!handler)
        {
            throw brayns::EntrypointException("Scene has no user data handler");
        }
        return *handler;
    }

    static bool _match(const PointCloudInfo& info, float value)
    {
        auto delta = std::abs(value - info.value);
        return delta < info.epsilon;
    }

    static brayns::Vector4f _getPoint(const brayns::Sphere& sphere)
    {
        return {sphere.center.x, sphere.center.y, sphere.center.z,
                sphere.radius};
    }
};

class PointCloudModelLoader
{
public:
    static bool loadConvexHull(brayns::Scene& scene, const PointCloud& cloud,
                               const PointCloudInfo& info)
    {
        auto model = _createConvertHull(scene, cloud);
        return _loadModel(scene, std::move(model), info.title);
    }

    static bool loadMetaballs(brayns::Scene& scene, const PointCloud& cloud,
                              const PointCloudInfo& info)
    {
        auto model = _createMetaballs(scene, cloud, info);
        return _loadModel(scene, std::move(model), info.title);
    }

private:
    static brayns::ModelPtr _createConvertHull(brayns::Scene& scene,
                                               const PointCloud& cloud)
    {
        auto model = scene.createModel();
        PointCloudMesher mesher;
        if (!mesher.toConvexHull(*model, cloud))
        {
            return nullptr;
        }
        return model;
    }

    static brayns::ModelPtr _createMetaballs(brayns::Scene& scene,
                                             const PointCloud& cloud,
                                             const PointCloudInfo& info)
    {
        auto gridSize = info.gridSize;
        auto threshold = float(info.threshold);
        auto model = scene.createModel();
        PointCloudMesher mesher;
        if (!mesher.toMetaballs(*model, cloud, gridSize, threshold))
        {
            return nullptr;
        }
        return model;
    }

    static bool _loadModel(brayns::Scene& scene, brayns::ModelPtr model,
                           const std::string& title)
    {
        if (!model)
        {
            return false;
        }
        auto descriptor =
            std::make_shared<brayns::ModelDescriptor>(std::move(model), title);
        scene.addModel(std::move(descriptor));
        scene.markModified();
        return true;
    }
};

class PointCloudHelper
{
public:
    static PointCloud createCloud(brayns::Scene& scene,
                                  const PointCloudInfo& info)
    {
        auto modelId = info.modelId;
        auto& descriptor = brayns::ExtractModel::fromId(scene, modelId);
        auto& model = descriptor.getModel();
        auto cloud = PointCloudBuilder::build(model, info);
        if (cloud.empty())
        {
            auto value = info.value;
            auto message = "No mesh added for value " + std::to_string(value);
            throw brayns::EntrypointException(std::move(message));
        }
        return cloud;
    }
};
} // namespace

void PointCloudLoader::loadConvexHull(brayns::Scene& scene,
                                      const PointCloudInfo& info)
{
    auto cloud = PointCloudHelper::createCloud(scene, info);
    if (!PointCloudModelLoader::loadConvexHull(scene, cloud, info))
    {
        throw brayns::EntrypointException("Cannot create convex hull");
    }
}

void PointCloudLoader::loadMetaballs(brayns::Scene& scene,
                                     const PointCloudInfo& info)
{
    auto cloud = PointCloudHelper::createCloud(scene, info);
    if (!PointCloudModelLoader::loadMetaballs(scene, cloud, info))
    {
        throw brayns::EntrypointException("Cannot create metaballs");
    }
}