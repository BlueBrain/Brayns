/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/network/entrypoint/EntrypointRegistry.h>
#include <brayns/network/socket/ISocket.h>

#include "INetworkInterface.h"

namespace brayns
{
/**
 * @brief Implementation of the network interface.
 *
 */
class NetworkInterface : public INetworkInterface
{
public:
    /**
     * @brief Construct with dependencies.
     *
     * @param entrypoints Register entrypoints.
     * @param socket Poll requests.
     */
    NetworkInterface(EntrypointRegistry &entrypoints, ISocket &socket);

    /**
     * @brief Register an entrypoint.
     *
     * @param entrypoint Entrypoint to register.
     */
    virtual void add(EntrypointRef entrypoint) override;

    /**
     * @brief Poll incoming requests.
     *
     */
    virtual void poll() override;

private:
    EntrypointRegistry &_entrypoints;
    ISocket &_socket;
};
} // namespace brayns
