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

#include <brayns/network/client/ClientRequest.h>
#include <brayns/network/entrypoint/EntrypointRegistry.h>

#include "CurrentEntrypoint.h"

namespace brayns
{
/**
 * @brief Helper class to dispatch a request to entrypoints.
 *
 */
class RequestDispatcher
{
public:
    /**
     * @brief Construct a dispatcher with info on current entrypoint.
     *
     * @param entrypoint Current entrypoint that can be overriden.
     */
    RequestDispatcher(CurrentEntrypoint &entrypoint);

    /**
     * @brief Dispatch client request to entrypoints.
     *
     * @param request Raw client request.
     * @param entrypoints Supported entrypoints.
     */
    void dispatch(const ClientRequest &request, const EntrypointRegistry &entrypoints);

private:
    CurrentEntrypoint &_entrypoint;
};
} // namespace brayns
