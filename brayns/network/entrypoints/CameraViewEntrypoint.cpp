/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "CameraViewEntrypoint.h"

namespace brayns
{
SetCameraViewEntrypoint::SetCameraViewEntrypoint(Engine &engine):
    _engine(engine)
{
}

std::string SetCameraViewEntrypoint::getMethod() const
{
    return "set-camera-view";
}

std::string SetCameraViewEntrypoint::getDescription() const
{
    return "Sets the camera view settings";
}

void SetCameraViewEntrypoint::onRequest(const Request &request)
{
    auto &camera = _engine.getCamera();
    auto view = camera.getView();
    request.getParams(view);
    camera.setView(view);
    request.reply(EmptyJson());
}

GetCameraViewEntrypoint::GetCameraViewEntrypoint(Engine &engine):
    _engine(engine)
{
}

std::string GetCameraViewEntrypoint::getMethod() const
{
    return "get-camera-view";
}

std::string GetCameraViewEntrypoint::getDescription() const
{
    return "Returns the camera view settings";
}

void GetCameraViewEntrypoint::onRequest(const Request &request)
{
    auto &camera = _engine.getCamera();
    auto &view = camera.getView();
    request.reply(view);
}
}
