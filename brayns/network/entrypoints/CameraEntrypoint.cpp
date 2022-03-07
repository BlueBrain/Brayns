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

SetCameraOrthographicEntrypoint::SetCameraOrthographicEntrypoint(Engine &engine)
 : SetCameraEntrypoint<OrthographicCamera>(engine)
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

GetCameraEntrypoint::GetCameraEntrypoint(Engine& engine, CameraFactory::Ptr factory)
 : _engine(engine)
 , _cameraFactory(factory)
{
}

std::string GetCameraEntrypoint::getMethod() const
{
    return "get-camera";
}

std::string GetCameraEntrypoint::getDescription() const
{
    return "Returns the current camera type and its parameters";
}

void GetCameraEntrypoint::onRequest(const Request &request)
{
    auto& camera = _engine.getCamera();

    GenericCamera result;
    result.serialize(*_cameraFactory, camera);

    request.reply(result);
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
