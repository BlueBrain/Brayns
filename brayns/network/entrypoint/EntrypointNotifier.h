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

#include <brayns/json/Json.h>

#include <brayns/network/common/RateLimiter.h>
#include <brayns/network/interface/INetworkInterface.h>

#include "IEntrypoint.h"

namespace brayns
{
/**
 * @brief Helper class to send notifications from an entrypoint.
 *
 */
class EntrypointNotifier
{
public:
    /**
     * @brief Construct a notifier.
     *
     * @param entrypoint Entrypoint which notifies.
     * @param interface Interface to send the message.
     * @param period Optional rate limit.
     */
    EntrypointNotifier(IEntrypoint &entrypoint, INetworkInterface &interface, Duration period = Duration(0));

    /**
     * @brief Send notification with JSON params.
     *
     * @param json "params" field.
     */
    void notify(const JsonValue &json);

    /**
     * @brief Send notification with params.
     *
     * @tparam T Params type, must be serializable.
     * @param params "params" field.
     */
    template<typename T>
    void notify(const T &params)
    {
        auto json = Json::serialize(params);
        notify(json);
    }

private:
    IEntrypoint &_entrypoint;
    INetworkInterface &_interface;
    RateLimiter _limiter;
};
} // namespace brayns
