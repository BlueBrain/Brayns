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

#include "InspectCenterManipulator.h"

#include "Camera.h"
#include <brayns/common/input/KeyboardHandler.h>

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
    const float du = (to.x() - from.x()) * getRotationSpeed();
    const float dv = (to.y() - from.y()) * getRotationSpeed();
    rotate(_camera.getTarget(), du, dv, false);
}

void InspectCenterManipulator::dragRight(const Vector2i& to,
                                         const Vector2i& from)
{
    const float distance = -(to.y() - from.y()) * getMotionSpeed();
    if (distance < (_camera.getTarget() - _camera.getPosition()).length())
        translate(Vector3f::forward() * distance, false);
}

void InspectCenterManipulator::dragMiddle(const Vector2i& to,
                                          const Vector2i& from)
{
    const float x = (to.x() - from.x()) * getMotionSpeed();
    const float y = (to.y() - from.y()) * getMotionSpeed();
    translate({-x, y, 0.f}, true);
}

void InspectCenterManipulator::wheel(const Vector2i& /*position*/, float delta)
{
    delta *= getWheelSpeed();
    if (delta < (_camera.getTarget() - _camera.getPosition()).length())
        translate(Vector3f::forward() * delta, false);
}

void InspectCenterManipulator::_rotateLeft()
{
    rotate(_camera.getTarget(), -getRotationSpeed(), 0, false);
}

void InspectCenterManipulator::_rotateRight()
{
    rotate(_camera.getTarget(), getRotationSpeed(), 0, false);
}

void InspectCenterManipulator::_rotateUp()
{
    rotate(_camera.getTarget(), 0, -getRotationSpeed(), false);
}

void InspectCenterManipulator::_rotateDown()
{
    rotate(_camera.getTarget(), 0, getRotationSpeed(), false);
}

void InspectCenterManipulator::_turnLeft()
{
    rotate(_camera.getPosition(), getRotationSpeed(), 0, true);
}

void InspectCenterManipulator::_turnRight()
{
    rotate(_camera.getPosition(), -getRotationSpeed(), 0, true);
}

void InspectCenterManipulator::_turnUp()
{
    rotate(_camera.getPosition(), 0, getRotationSpeed(), true);
}

void InspectCenterManipulator::_turnDown()
{
    rotate(_camera.getPosition(), 0, -getRotationSpeed(), true);
}
}
