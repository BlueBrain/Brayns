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

#include "InspectCenterManipulator.h"

#include "../input/KeyboardHandler.h"
#include <brayns/engine/Camera.h>

namespace brayns
{
InspectCenterManipulator::InspectCenterManipulator(Camera& camera,
                                                   KeyboardHandler& handler)
    : AbstractManipulator{camera, handler}
{
    _keyboardHandler.registerKeyboardShortcut(
        'a', "Rotate left",
        std::bind(&InspectCenterManipulator::_rotateLeft, this));
    _keyboardHandler.registerKeyboardShortcut(
        'd', "Rotate right",
        std::bind(&InspectCenterManipulator::_rotateRight, this));
    _keyboardHandler.registerKeyboardShortcut(
        'w', "Rotate up",
        std::bind(&InspectCenterManipulator::_rotateUp, this));
    _keyboardHandler.registerKeyboardShortcut(
        's', "Rotate down",
        std::bind(&InspectCenterManipulator::_rotateDown, this));

    _keyboardHandler.registerSpecialKey(
        SpecialKey::LEFT, "Turn left",
        std::bind(&InspectCenterManipulator::_turnLeft, this));
    _keyboardHandler.registerSpecialKey(
        SpecialKey::RIGHT, "Turn right",
        std::bind(&InspectCenterManipulator::_turnRight, this));
    _keyboardHandler.registerSpecialKey(
        SpecialKey::UP, "Turn up",
        std::bind(&InspectCenterManipulator::_turnUp, this));
    _keyboardHandler.registerSpecialKey(
        SpecialKey::DOWN, "Turn down",
        std::bind(&InspectCenterManipulator::_turnDown, this));
}

InspectCenterManipulator::~InspectCenterManipulator()
{
    _keyboardHandler.unregisterKeyboardShortcut('a');
    _keyboardHandler.unregisterKeyboardShortcut('d');
    _keyboardHandler.unregisterKeyboardShortcut('w');
    _keyboardHandler.unregisterKeyboardShortcut('s');

    _keyboardHandler.unregisterSpecialKey(SpecialKey::LEFT);
    _keyboardHandler.unregisterSpecialKey(SpecialKey::RIGHT);
    _keyboardHandler.unregisterSpecialKey(SpecialKey::UP);
    _keyboardHandler.unregisterSpecialKey(SpecialKey::DOWN);
}

void InspectCenterManipulator::dragLeft(const Vector2i& to,
                                        const Vector2i& from)
{
    const float du = (to.x - from.x) * DEFAULT_MOUSE_MOTION_SPEED_MULTIPLIER *
                     getRotationSpeed();
    const float dv = (to.y - from.y) * DEFAULT_MOUSE_MOTION_SPEED_MULTIPLIER *
                     getRotationSpeed();
    rotate(_camera.getTarget(), du, dv, AxisMode::localY);
}

void InspectCenterManipulator::dragRight(const Vector2i& to,
                                         const Vector2i& from)
{
    const float distance = -(to.y - from.y) *
                           DEFAULT_MOUSE_MOTION_SPEED_MULTIPLIER *
                           getMotionSpeed();
    if (distance < glm::length(_camera.getTarget() - _camera.getPosition()))
        translate(Vector3f(0, 0, -1) * distance);
}

void InspectCenterManipulator::dragMiddle(const Vector2i& to,
                                          const Vector2i& from)
{
    const float x = (to.x - from.x) * DEFAULT_MOUSE_MOTION_SPEED_MULTIPLIER *
                    getMotionSpeed();
    const float y = (to.y - from.y) * DEFAULT_MOUSE_MOTION_SPEED_MULTIPLIER *
                    getMotionSpeed();
    const Vector3d translation(-x, y, 0);
    translate(translation);
    _camera.setTarget(_camera.getTarget() +
                      glm::rotate(_camera.getOrientation(), translation));
}

void InspectCenterManipulator::wheel(const Vector2i& /*position*/, float delta)
{
    delta *= getWheelSpeed();
    if (delta < glm::length(_camera.getTarget() - _camera.getPosition()))
        translate(Vector3f(0, 0, -1) * delta);
}

void InspectCenterManipulator::_rotateLeft()
{
    rotate(_camera.getTarget(), -getRotationSpeed(), 0, AxisMode::localY);
}

void InspectCenterManipulator::_rotateRight()
{
    rotate(_camera.getTarget(), getRotationSpeed(), 0, AxisMode::localY);
}

void InspectCenterManipulator::_rotateUp()
{
    rotate(_camera.getTarget(), 0, -getRotationSpeed(), AxisMode::localY);
}

void InspectCenterManipulator::_rotateDown()
{
    rotate(_camera.getTarget(), 0, getRotationSpeed(), AxisMode::localY);
}

void InspectCenterManipulator::_turnLeft()
{
    rotate(_camera.getTarget(), getRotationSpeed(), 0, AxisMode::localY);
}

void InspectCenterManipulator::_turnRight()
{
    rotate(_camera.getTarget(), -getRotationSpeed(), 0, AxisMode::localY);
}

void InspectCenterManipulator::_turnUp()
{
    rotate(_camera.getTarget(), 0, getRotationSpeed(), AxisMode::localY);
}

void InspectCenterManipulator::_turnDown()
{
    rotate(_camera.getTarget(), 0, -getRotationSpeed(), AxisMode::localY);
}
} // namespace brayns
