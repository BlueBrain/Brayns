/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AbstractManipulator.h"

#include "Camera.h"
#include <brayns/common/input/KeyboardHandler.h>
#include <brayns/common/log.h>
#include <brayns/common/scene/Scene.h>

namespace brayns
{
namespace
{
constexpr float DEFAULT_MOTION_SPEED = 0.03f;
constexpr float DEFAULT_ROTATION_SPEED = 0.006f;
}

AbstractManipulator::AbstractManipulator(Camera& camera,
                                         KeyboardHandler& keyboardHandler)
    : _camera(camera)
    , _keyboardHandler(keyboardHandler)
    , _motionSpeed{DEFAULT_ROTATION_SPEED}
    , _rotationSpeed{DEFAULT_ROTATION_SPEED}
{
}

void AbstractManipulator::adjust(const Boxd& boundingBox)
{
    const auto size =
        boundingBox.isEmpty() ? 1 : boundingBox.getSize().find_max();
    auto position = boundingBox.getCenter();
    auto target = position;
    position.z() += size;

    const Quaterniond identity;
    _camera.setInitialState(position, identity, target);

    _motionSpeed = DEFAULT_MOTION_SPEED * size;

    if (boundingBox.isEmpty())
        BRAYNS_INFO << "World bounding box: empty" << std::endl;
    else
        BRAYNS_INFO << "World bounding box: " << boundingBox << std::endl;
    BRAYNS_INFO << "World center      : " << boundingBox.getCenter()
                << std::endl;
}

float AbstractManipulator::getRotationSpeed() const
{
    return _rotationSpeed;
}

float AbstractManipulator::getWheelSpeed() const
{
    return getMotionSpeed() * 5.f;
}

float AbstractManipulator::getMotionSpeed() const
{
    return _motionSpeed;
}

void AbstractManipulator::updateMotionSpeed(const float speed)
{
    _motionSpeed *= speed;
}

void AbstractManipulator::translate(const Vector3f& vector)
{
    auto orientation = _camera.getOrientation();
    const auto translation = orientation.rotate(vector);

    _camera.setPosition(_camera.getPosition() + translation);
}

void AbstractManipulator::rotate(const Vector3f& pivot, const float du,
                                 const float dv, AxisMode axisMode)
{
    const Vector3d axisX =
        _camera.getOrientation().rotate(Vector3d(1.0, 0.0, 0.0));

    const Vector3d axisY =
        axisMode == AxisMode::localY
            ? _camera.getOrientation().rotate(Vector3d(0.0, 1.0, 0.0))
            : Vector3d(0.0, 1.0, 0.0);

    const Quaterniond deltaU(-du, axisY);
    const Quaterniond deltaV(-dv, axisX);

    const Quaterniond final = deltaU * deltaV * _camera.getOrientation();
    const Vector3d dir = final.rotate(Vector3d(0.0, 0.0, -1.0));

    const double rotationRadius =
        Vector3d(_camera.getPosition() - pivot).length();
    _camera.setPosition(pivot + rotationRadius * -dir);
    _camera.setOrientation(final);
}
}
