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

#include "CameraEntrypoint.h"

namespace brayns
{
GetCameraEntrypoint::GetCameraEntrypoint(const Camera &camera, INetworkInterface &interface)
    : GetEntrypoint(camera, interface)
{
}

std::string GetCameraEntrypoint::getName() const
{
    return "get-camera";
}

std::string GetCameraEntrypoint::getDescription() const
{
    return "Get the current state of the camera";
}

SetCameraEntrypoint::SetCameraEntrypoint(Camera &camera)
    : SetEntrypoint(camera)
{
}

std::string SetCameraEntrypoint::getName() const
{
    return "set-camera";
}

std::string SetCameraEntrypoint::getDescription() const
{
    return "Set the current state of the camera";
}
} // namespace brayns
