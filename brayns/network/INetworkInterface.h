/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 *
 * Responsible Authors: Daniel.Nachbaur@epfl.ch
 *                      Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/network/entrypoint/EntrypointRef.h>

namespace brayns
{
/**
 * Access to network API.
 *
 */
class INetworkInterface
{
public:
    virtual ~INetworkInterface() = default;

    /**
     * @brief Register an entrypoint.
     *
     * @param entrypoint Entrypoint to register.
     */
    virtual void registerEntrypoint(EntrypointRef entrypoint) = 0;

    /**
     * @brief Can be used to poll requests from an entrypoint or plugin.
     *
     * @note The network manager already calls it on each update.
     */
    virtual void poll() = 0;

    /**
     * @brief Stop the network loop.
     *
     */
    virtual void stop() = 0;
};
} // namespace brayns
