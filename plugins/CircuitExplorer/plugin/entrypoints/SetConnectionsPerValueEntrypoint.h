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

#pragma once

#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/meshing/PointCloudMesher.h>
#include <plugin/messages/SetConnectionsPerValueMessage.h>

class PointCloudLoader
{
private:
    using Params = SetConnectionsPerValueMessage;
    using Handler = brayns::AbstractSimulationHandler;

public:
    static PointCloud load(brayns::Model& model, const Params& params)
    {
        PointCloud cloud;
        auto& handler = _getSimulationHandler(model);
        auto data = _getFrameData(handler, params);
        for (const auto& pair : model.getSpheres())
        {
            auto id = pair.first;
            auto& spheres = pair.second;
            auto& points = cloud[id];
            for (const auto& sphere : spheres)
            {
                auto value = data[sphere.userData];
                if (!_match(params, value))
                {
                    continue;
                }
                points.push_back(_getPoint(id, sphere));
            }
        }
        return cloud;
    }

private:
    static Handler& _getSimulationHandler(brayns::Model& model)
    {
        auto handler = model.getSimulationHandler();
        if (!handler)
        {
            throw brayns::EntrypointException("Scene has no user data handler");
        }
        return *handler;
    }

    static float* _getFrameData(Handler& handler, const Params& params)
    {
        auto frame = params.frame;
        auto frameData = handler.getFrameData(frame);
        return static_cast<float*>(frameData);
    }

    static bool _match(const Params& params, float value)
    {
        auto epsilon = params.epsilon;
        auto reference = params.value;
        auto delta = std::abs(value - reference);
        return delta < epsilon;
    }

    static brayns::Vector4f _getPoint(size_t id, const brayns::Sphere& sphere)
    {
        return {sphere.center.x, sphere.center.y, sphere.center.z,
                sphere.radius};
    }
};

class PointCloudMeshLoader
{
public:
    static void load(brayns::Scene& scene, const PointCloud& cloud,
                     const std::string& title)
    {
        auto model = _createModel(scene, cloud, title);
        if (!model)
        {
            return;
        }
        scene.addModel(model);
        scene.markModified();
    }

private:
    static brayns::ModelDescriptorPtr _createModel(brayns::Scene& scene,
                                                   const PointCloud& cloud,
                                                   const std::string& title)
    {
        auto mesh = scene.createModel();
        PointCloudMesher mesher;
        if (!mesher.toConvexHull(*mesh, cloud))
        {
            return nullptr;
        }
        return std::make_shared<brayns::ModelDescriptor>(std::move(mesh),
                                                         title);
    }
};

class SetConnectionsPerValueEntrypoint
    : public brayns::Entrypoint<SetConnectionsPerValueMessage,
                                brayns::EmptyMessage>
{
public:
    virtual std::string getName() const override
    {
        return "set-connections-per-value";
    }

    virtual std::string getDescription() const override
    {
        return "Draw a point cloud representing the number of "
               "connections for a given frame and simulation value";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto modelId = params.model_id;
        auto value = params.value;
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        auto& descriptor = brayns::ExtractModel::fromId(scene, modelId);
        auto& model = descriptor.getModel();
        auto cloud = _loadPoints(model, params, value);
        auto title = "Connection for value " + std::to_string(value);
        PointCloudMeshLoader::load(scene, cloud, title);
        request.reply(brayns::EmptyMessage());
    }

private:
    PointCloud _loadPoints(brayns::Model& model, const Params& params,
                           float value)
    {
        auto cloud = PointCloudLoader::load(model, params);
        if (cloud.empty())
        {
            throw brayns::EntrypointException(
                "No connections added for value " + std::to_string(value));
        }
    }
};