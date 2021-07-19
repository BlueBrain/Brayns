/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/network/message/Message.h>

#include <brayns/engine/Camera.h>

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(CameraMessage)
BRAYNS_MESSAGE_ENTRY(Quaterniond, orientation, "Camera orientation XYZW")
BRAYNS_MESSAGE_ENTRY(Vector3d, position, "Camera position XYZ")
BRAYNS_MESSAGE_ENTRY(Vector3d, target, "Camera target XYZ")
BRAYNS_MESSAGE_ENTRY(std::string, current, "Camera current type")
BRAYNS_MESSAGE_ENTRY(std::vector<std::string>, types, "Available camera types")

static Camera& extract(PluginAPI& api)
{
    auto& engine = api.getEngine();
    return engine.getCamera();
}

void dump(Camera& camera) const
{
    camera.setOrientation(orientation);
    camera.setPosition(position);
    camera.setTarget(target);
    camera.setCurrentType(current);
}

void load(const Camera& camera)
{
    orientation = camera.getOrientation();
    position = camera.getPosition();
    target = camera.getTarget();
    current = camera.getCurrentType();
    types = camera.getTypes();
}

BRAYNS_MESSAGE_END()
} // namespace brayns