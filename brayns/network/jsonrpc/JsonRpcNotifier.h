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

#pragma once

#include <brayns/network/common/RateLimiter.h>
#include <brayns/network/interface/INetworkInterface.h>

#include "JsonRpcFactory.h"

namespace brayns
{
/**
 * @brief Helper class wrapping network interface to send notifications.
 *
 */
class JsonRpcNotifier
{
public:
    /**
     * @brief Construct a notifier.
     *
     * @param interface Interface to send the message.
     * @param period Optional min period between two notifications.
     */
    JsonRpcNotifier(INetworkInterface &interface, Duration period = Duration(0))
        : _interface(interface)
        , _limiter(period)
    {
    }

    template<typename ParamsType>
    void notify(const std::string &method, const ParamsType &params)
    {
        auto json = Json::serialize(params);
        auto message = JsonRpcFactory::notification(method, json);
        _interface.notify(message);
    }

    template<typename RequestType, typename ParamsType>
    void notify(const RequestType &request, const ParamsType &params)
    {
        auto &client = request.getClient();
        auto &method = request.getMethod();
        auto json = Json::serialize(params);
        auto message = JsonRpcFactory::notification(method, json);
        _interface.notify(message, client);
    }

private:
    INetworkInterface &_interface;
    RateLimiter _limiter;
};
} // namespace brayns
