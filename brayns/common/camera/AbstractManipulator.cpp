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

namespace brayns
{
namespace
{
const float DEFAULT_MOTION_SPEED = 0.001f;
const float DEFAULT_ROTATION_SPEED = 0.005f;
}

AbstractManipulator::AbstractManipulator(Camera& camera,
                                         KeyboardHandler& keyboardHandler)
    : _camera(camera)
    , _keyboardHandler(keyboardHandler)
    , _motionSpeed{DEFAULT_MOTION_SPEED}
    , _rotationSpeed{DEFAULT_ROTATION_SPEED}
{
}

AbstractManipulator::~AbstractManipulator()
{
}

float AbstractManipulator::getRotationSpeed() const
{
    return _rotationSpeed;
}

float AbstractManipulator::getWheelSpeed() const
{
    return getMotionSpeed() * 100.f;
}

float AbstractManipulator::getMotionSpeed() const
{
    const auto& position = _camera.getPosition();
    const auto& target = _camera.getTarget();
    return Vector3f{target - position}.length() * _motionSpeed;
}

void AbstractManipulator::updateMotionSpeed(const float speed)
{
    _motionSpeed *= speed;
}

void AbstractManipulator::translate(const Vector3f& vector,
                                    const bool updateTarget)
{
    auto& matrix = _camera.getRotationMatrix();
    const auto translation = Vector3f{matrix * vector};

    _camera.setPosition(_camera.getPosition() + translation);
    if (updateTarget)
        _camera.setTarget(_camera.getTarget() + translation);
}

void AbstractManipulator::rotate(const Vector3f& pivot, const float du,
                                 const float dv, const bool updateTarget)
{
    auto& matrix = _camera.getRotationMatrix();
    matrix.rotate_x(-dv);
    matrix.rotate_y(-du);

    const auto dir = _camera.getTarget() - _camera.getPosition();
    const auto newPivotToCam =
        Vector3f{matrix * Vector3f::unitZ()} * dir.length();

    if (updateTarget)
        _camera.setTarget(_camera.getPosition() - newPivotToCam);
    else
        _camera.setPosition(pivot + newPivotToCam);

    _camera.setUp(matrix * Vector3f::unitY());
}
}
