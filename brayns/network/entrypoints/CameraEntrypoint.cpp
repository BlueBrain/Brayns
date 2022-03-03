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
GetCameraPerspectiveEntrypoint::GetCameraPerspectiveEntrypoint(Engine &engine)
 : GetCameraEntrypoint<PerspectiveCamera>(engine)
{
}

std::string GetCameraPerspectiveEntrypoint::getMethod() const
{
    return "get-camera-perspective";
}

std::string GetCameraPerspectiveEntrypoint::getDescription() const
{
    return "Return the parameters of the current perspective camera. "
           "Will result on an error if the current camera is not perspective";
}

GetCameraOrthographicEntrypoit::GetCameraOrthographicEntrypoit(Engine &engine)
 : GetCameraEntrypoint<OrthographicCamera>(engine)
{
}

std::string GetCameraOrthographicEntrypoit::getMethod() const
{
    return "get-camera-orthographic";
}

std::string GetCameraOrthographicEntrypoit::getDescription() const
{
    return "Return the parameters of the current orthographic camera. "
           "Will result on an error if the current camera is not orthographic";
}

SetCameraPerspectiveEntrypoint::SetCameraPerspectiveEntrypoint(Engine &engine)
 : SetCameraEntrypoint<PerspectiveCamera>(engine)
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

SetCameraOrthographicEntrypoit::SetCameraOrthographicEntrypoit(Engine &engine)
 : SetCameraEntrypoint<OrthographicCamera>(engine)
{
}

std::string SetCameraOrthographicEntrypoit::getMethod() const
{
    return "set-camera-orthographic";
}

std::string SetCameraOrthographicEntrypoit::getDescription() const
{
    return "Sets the current camera to an orthographic one, with the specified parameters";
}

GetCurrentCameraTypeEntrypoint::GetCurrentCameraTypeEntrypoint(Engine& engine)
 : _engine(engine)
{
}

std::string GetCurrentCameraTypeEntrypoint::getMethod() const
{
    return "get-current-camera-type";
}

std::string GetCurrentCameraTypeEntrypoint::getDescription() const
{
    return "Returns the type name of the current system camera";
}

void GetCurrentCameraTypeEntrypoint::onRequest(const Request &request)
{
    auto& camera = _engine.getCamera();
    const std::string type (camera.getName());
    request.reply(type);
}

CameraLookAtEntrypoint::CameraLookAtEntrypoint(Engine &engine)
 : _engine(engine)
{
}

std::string CameraLookAtEntrypoint::getMethod() const
{
    return "camera-look-at";
}

std::string CameraLookAtEntrypoint::getDescription() const
{
    return "Sets the camera view settings";
}

void CameraLookAtEntrypoint::onRequest(const Request &request)
{
    const auto lookAtParams = request.getParams();
    const auto& eye = lookAtParams.eye;
    const auto& target = lookAtParams.target;
    const auto& up = lookAtParams.up;

    auto& camera = _engine.getCamera();

    camera.setPosition(eye);
    camera.setTarget(target);
    camera.setUp(up);

    const auto result = Json::serialize(EmptyMessage());
    request.reply(result);
}
} // namespace brayns
