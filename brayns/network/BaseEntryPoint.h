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

#include <functional>
#include <string>

#include "Json.h"
#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
class BaseEntryPoint
{
public:
    using NotificationCallback = std::function<void(const JsonValue&)>;

    PluginApi& getApi() const { return *api; }

    void setApi(PluginAPI& api) { _api = &api; };

    void notify(const JsonValue& message) { _callback(message); }

    void setNotificationCallback(const NotificationCallback& callback)
    {
        _callback = callback;
    }

    virtual void onCreate() {}

    virtual void onDestroy() {}

    virtual std::string getName() const = 0;

    virtual std::string getDescription() const = 0;

    virtual JsonValue getRequestSchema() const = 0;

    virtual JsonValue getReplySchema() const = 0;

    virtual JsonValue run(const JsonValue& request) const = 0;

    template <typename MessageType>
    void notify(const MessageType& message)
    {
        notify(Json::serialize(message));
    }

private:
    PluginApi* _api = nullptr;
    NotificationCallback _callback;
};
} // namespace brayns