/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/network/INetworkInterface.h>

namespace brayns
{
/**
 * @brief Used to cancel a request processing.
 *
 */
class CancellationToken
{
public:
    /**
     * @brief Construct a token with network access.
     *
     * @param interface Used to poll requests to receive cancellation.
     */
    explicit CancellationToken(INetworkInterface &interface);

    /**
     * @brief Poll requests and check if the cancel flag is set.
     *
     * @throw TaskCancelledException Cancel flag was set.
     */
    void poll() const;

    /**
     * @brief Set the cancel flag.
     *
     */
    void cancel();

    /**
     * @brief Reset the cancel flag.
     *
     */
    void reset();

private:
    INetworkInterface &_interface;
    bool _cancelled = false;
};
} // namespace brayns
