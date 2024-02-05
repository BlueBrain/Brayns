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

#include <memory>

#include <brayns/network/interface/ActionInterface.h>
#include <brayns/pluginapi/ExtensionPlugin.h>

namespace brayns
{
class NetworkContext;

/**
 * @brief Network manager plugin providing the network action interface.
 *
 */
class NetworkManager : public ExtensionPlugin
{
public:
    /**
     * @brief Construct the object.
     *
     */
    NetworkManager();

    /**
     * @brief Remove the action interface if still the active one.
     *
     */
    virtual ~NetworkManager();

    /**
     * @brief Register base entrypoints and setup ActionInterface.
     *
     */
    virtual void init() override;

    /**
     * @brief Process request buffer.
     *
     */
    virtual void preRender() override;

    /**
     * @brief Broadcast images.
     *
     */
    virtual void postRender() override;

private:
    std::unique_ptr<NetworkContext> _context;
    std::shared_ptr<ActionInterface> _interface;
};
} // namespace brayns
