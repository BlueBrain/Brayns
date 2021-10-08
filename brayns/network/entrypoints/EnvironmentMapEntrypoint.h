/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/engine/Scene.h>

#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/messages/EnvironmentMapMessage.h>

namespace brayns
{
class GetEnvironmentMapEntrypoint
    : public Entrypoint<EmptyMessage, EnvironmentMapMessage>
{
public:
    virtual std::string getName() const override
    {
        return "get-environment-map";
    }

    virtual std::string getDescription() const override
    {
        return "Get the environment map from the scene";
    }

    virtual void onRequest(const Request& request) override
    {
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        auto& environmentMap = scene.getEnvironmentMap();
        request.reply({environmentMap});
    }
};

class SetEnvironmentMapEntrypoint
    : public Entrypoint<EnvironmentMapMessage, EmptyMessage>
{
public:
    virtual std::string getName() const override
    {
        return "set-environment-map";
    }

    virtual std::string getDescription() const override
    {
        return "Set an environment map in the scene";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& filename = params.filename;
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        if (!scene.setEnvironmentMap(filename))
        {
            throw EntrypointException("Failed to set environment map from: '" +
                                      filename + "'");
        }
        request.notify(params);
        request.reply(EmptyMessage());
    }
};
} // namespace brayns