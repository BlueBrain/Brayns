/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "CameraLookAtEntrypoint.h"

namespace brayns
{
SetCameraLookAtEntrypoint::SetCameraLookAtEntrypoint(Engine &engine)
    : _engine(engine)
{
}

std::string SetCameraLookAtEntrypoint::getMethod() const
{
    return "set-camera-look-at";
}

std::string SetCameraLookAtEntrypoint::getDescription() const
{
    return "Sets the camera view settings";
}

void SetCameraLookAtEntrypoint::onRequest(const Request &request)
{
    auto &camera = _engine.getCamera();
    auto view = camera.getView();
    request.getParams(view);
    camera.setView(view);
    request.reply(EmptyMessage());
}

GetCameraLookAtEntrypoint::GetCameraLookAtEntrypoint(Engine &engine)
    : _engine(engine)
{
}

std::string GetCameraLookAtEntrypoint::getMethod() const
{
    return "get-camera-look-at";
}

std::string GetCameraLookAtEntrypoint::getDescription() const
{
    return "Returns the camera view settings";
}

void GetCameraLookAtEntrypoint::onRequest(const Request &request)
{
    auto &camera = _engine.getCamera();
    auto &view = camera.getView();
    request.reply(view);
}
}
