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

#include "CameraEntrypoint.h"

namespace brayns
{
GetCameraTypeEntrypoint::GetCameraTypeEntrypoint(Engine &engine):
    _engine(engine)
{
}

std::string GetCameraTypeEntrypoint::getMethod() const
{
    return "get-camera-type";
}

std::string GetCameraTypeEntrypoint::getDescription() const
{
    return "Returns the type of the current camera";
}

void GetCameraTypeEntrypoint::onRequest(const Request &request)
{
    auto &camera = _engine.getCamera();
    auto name = camera.getName();
    request.reply(name);
}

SetCameraPerspectiveEntrypoint::SetCameraPerspectiveEntrypoint(Engine &engine):
    SetCameraEntrypoint<Perspective>(engine)
{
}

std::string SetCameraPerspectiveEntrypoint::getMethod() const
{
    return "set-camera-perspective";
}

std::string SetCameraPerspectiveEntrypoint::getDescription() const
{
    return "Sets the current camera to a perspective one, with the specified parameters";
}

SetCameraOrthographicEntrypoint::SetCameraOrthographicEntrypoint(Engine &engine):
    SetCameraEntrypoint<Orthographic>(engine)
{
}

std::string SetCameraOrthographicEntrypoint::getMethod() const
{
    return "set-camera-orthographic";
}

std::string SetCameraOrthographicEntrypoint::getDescription() const
{
    return "Sets the current camera to an orthographic one, with the specified parameters";
}

GetCameraPerspectiveEntrypoint::GetCameraPerspectiveEntrypoint(Engine &engine):
    GetCameraEntrypoint<Perspective>(engine)
{
}

std::string GetCameraPerspectiveEntrypoint::getMethod() const
{
    return "get-camera-perspective";
}

std::string GetCameraPerspectiveEntrypoint::getDescription() const
{
    return "Returns the current camera as perspective";
}

GetCameraOrthographicEntrypoint::GetCameraOrthographicEntrypoint(Engine &engine):
    GetCameraEntrypoint<Orthographic>(engine)
{
}

std::string GetCameraOrthographicEntrypoint::getMethod() const
{
    return "get-camera-orthographic";
}

std::string GetCameraOrthographicEntrypoint::getDescription() const
{
    return "Returns the current camera as orthographic";
}
} // namespace brayns
