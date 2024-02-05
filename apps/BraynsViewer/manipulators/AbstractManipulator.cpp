/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "../input/KeyboardHandler.h"
#include <brayns/common/Log.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Scene.h>

namespace brayns
{
namespace
{
constexpr float DEFAULT_MOTION_SPEED = 0.03f;
constexpr float DEFAULT_ROTATION_SPEED = 0.006f;
} // namespace

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
        boundingBox.isEmpty() ? 1 : glm::compMax(boundingBox.getSize());
    auto position = boundingBox.getCenter();
    auto target = position;
    position.z += size;

    _camera.setInitialState(position, glm::identity<Quaterniond>(), target);

    _motionSpeed = DEFAULT_MOTION_SPEED * size;

    if (boundingBox.isEmpty())
        Log::info("World bounding box: empty.");
    else
        Log::info("World bounding box: {}.", boundingBox);
    Log::info("World center      : {}.", boundingBox.getCenter());
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

void AbstractManipulator::translate(const Vector3d& vector)
{
    auto orientation = _camera.getOrientation();
    const auto translation = glm::rotate(orientation, vector);

    _camera.setPosition(_camera.getPosition() + translation);
}

void AbstractManipulator::rotate(const Vector3d& pivot, const double du,
                                 const double dv, AxisMode axisMode)
{
    const Vector3d axisX =
        glm::rotate(_camera.getOrientation(), Vector3d(1.0, 0.0, 0.0));

    const Vector3d axisY =
        axisMode == AxisMode::localY
            ? glm::rotate(_camera.getOrientation(), Vector3d(0.0, 1.0, 0.0))
            : Vector3d(0.0, 1.0, 0.0);

    const Quaterniond deltaU = glm::angleAxis(-du, axisY);
    const Quaterniond deltaV = glm::angleAxis(-dv, axisX);

    const Quaterniond final = deltaU * deltaV * _camera.getOrientation();
    const Vector3d dir = glm::rotate(final, Vector3d(0.0, 0.0, -1.0));

    const double rotationRadius = glm::length(_camera.getPosition() - pivot);
    _camera.setPosition(pivot + rotationRadius * -dir);
    _camera.setOrientation(final);
    _camera.setTarget(pivot);
}
} // namespace brayns
