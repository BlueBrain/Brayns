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

#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/meshing/PointCloudLoader.h>
#include <plugin/messages/SetConnectionsPerValueMessage.h>

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
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        _loadPoints(scene, params);
        engine.triggerRender();
        request.reply(brayns::EmptyMessage());
    }

private:
    void _loadPoints(brayns::Scene& scene, const Params& params)
    {
        PointCloudInfo info;
        info.title = "Connection for value " + std::to_string(params.value);
        info.modelId = params.model_id;
        info.frame = params.frame;
        info.value = params.value;
        info.epsilon = params.epsilon;
        PointCloudLoader::loadConvexHull(scene, info);
    }
};