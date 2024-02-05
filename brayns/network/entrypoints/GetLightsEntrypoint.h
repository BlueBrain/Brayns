/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <vector>

#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/messages/LightMessage.h>

namespace brayns
{
class GetLightsEntrypoint
    : public Entrypoint<EmptyMessage, std::vector<LightMessage>>
{
public:
    virtual std::string getName() const override { return "get-lights"; }

    virtual std::string getDescription() const override
    {
        return "Get all lights";
    }

    virtual void onRequest(const Request& request) override
    {
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        auto& lightManager = scene.getLightManager();
        auto& lights = lightManager.getLights();
        auto messages = _buildMessages(lights);
        request.reply(messages);
    }

private:
    std::vector<LightMessage> _buildMessages(
        const std::map<size_t, LightPtr>& lights)
    {
        std::vector<LightMessage> messages;
        messages.reserve(lights.size());
        for (const auto& pair : lights)
        {
            auto id = pair.first;
            auto& light = pair.second;
            if (!light)
            {
                continue;
            }
            messages.push_back(_buildMessage(id, light));
        }
        return messages;
    }

    LightMessage _buildMessage(size_t id, const LightPtr& light)
    {
        LightMessage message;
        message.type = light->_type;
        message.id = id;
        message.properties = *light;
        return message;
    }
};
} // namespace brayns