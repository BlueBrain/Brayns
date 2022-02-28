/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "GetLightsEntrypoint.h"

namespace
{
class LightsMessageBuilder
{
public:
    static std::vector<brayns::LightMessage> buildMessages(const std::map<size_t, brayns::LightPtr> &lights)
    {
        std::vector<brayns::LightMessage> messages;
        messages.reserve(lights.size());
        for (const auto &pair : lights)
        {
            auto id = pair.first;
            auto &light = pair.second;
            if (!light)
            {
                continue;
            }
            auto message = _buildMessage(id, light);
            messages.push_back(std::move(message));
        }
        return messages;
    }

private:
    static brayns::LightMessage _buildMessage(size_t id, const brayns::LightPtr &light)
    {
        brayns::LightMessage message;
        message.type = light->_type;
        message.id = id;
        message.properties = *light;
        return message;
    }
};
} // namespace

namespace brayns
{
GetLightsEntrypoint::GetLightsEntrypoint(LightManager &manager)
    : _manager(manager)
{
}

std::string GetLightsEntrypoint::getMethod() const
{
    return "get-lights";
}

std::string GetLightsEntrypoint::getDescription() const
{
    return "Get all lights";
}

void GetLightsEntrypoint::onRequest(const Request &request)
{
    auto &lights = _manager.getLights();
    auto messages = LightsMessageBuilder::buildMessages(lights);
    request.reply(messages);
}
} // namespace brayns
