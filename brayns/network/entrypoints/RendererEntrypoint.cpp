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

#include "RendererEntrypoint.h"

namespace brayns
{
GetRendererEntrypoint::GetRendererEntrypoint(const RenderingParameters &parameters, INetworkInterface &interface)
    : GetEntrypoint(parameters, interface)
{
}

std::string GetRendererEntrypoint::getName() const
{
    return "get-renderer";
}

std::string GetRendererEntrypoint::getDescription() const
{
    return "Get the current state of the renderer";
}

SetRendererEntrypoint::SetRendererEntrypoint(RenderingParameters &parameters, Engine &engine)
    : SetEntrypoint(parameters, engine)
{
}

std::string SetRendererEntrypoint::getName() const
{
    return "set-renderer";
}

std::string SetRendererEntrypoint::getDescription() const
{
    return "Set the current state of the renderer";
}
} // namespace brayns
