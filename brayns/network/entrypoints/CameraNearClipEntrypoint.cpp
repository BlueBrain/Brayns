/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "CameraNearClipEntrypoint.h"

namespace brayns
{
SetCameraNearClipEntrypoint::SetCameraNearClipEntrypoint(Engine &engine):
    _engine(engine)
{
}

std::string SetCameraNearClipEntrypoint::getMethod() const
{
    return "set-camera-near-clip";
}

std::string SetCameraNearClipEntrypoint::getDescription() const
{
    return "Update the camera near clipping distance";
}

void SetCameraNearClipEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto &camera = _engine.getCamera();
    camera.setNearClippingDistance(params.distance);
    request.reply(EmptyJson());
}

GetCameraNearClipEntrypoint::GetCameraNearClipEntrypoint(Engine &engine):
    _engine(engine)
{
}

std::string GetCameraNearClipEntrypoint::getMethod() const
{
    return "get-camera-near-clip";
}

std::string GetCameraNearClipEntrypoint::getDescription() const
{
    return "Retreive the current camera clipping distance";
}

void GetCameraNearClipEntrypoint::onRequest(const Request &request)
{
    auto &camera = _engine.getCamera();
    auto distance = camera.getNearClippingDistance();
    request.reply({distance});
}
}
