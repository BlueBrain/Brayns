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

#include "FlyingModeManipulator.h"

#include <brayns/engine/Camera.h>

#include "../input/KeyboardHandler.h"
#include <brayns/common/Log.h>

namespace brayns
{
FlyingModeManipulator::FlyingModeManipulator(Camera& camera,
                                             KeyboardHandler& keyboardHandler)
    : AbstractManipulator(camera, keyboardHandler)
{
    _keyboardHandler.registerKeyboardShortcut(
        'a', "Strafe left",
        std::bind(&FlyingModeManipulator::_strafeLeft, this));
    _keyboardHandler.registerKeyboardShortcut(
        'd', "Strafe right",
        std::bind(&FlyingModeManipulator::_strafeRight, this));
    _keyboardHandler.registerKeyboardShortcut(
        'w', "Fly forward",
        std::bind(&FlyingModeManipulator::_flyForward, this));
    _keyboardHandler.registerKeyboardShortcut(
        's', "Fly backwards",
        std::bind(&FlyingModeManipulator::_flyBackwards, this));
}

FlyingModeManipulator::~FlyingModeManipulator()
{
    _keyboardHandler.unregisterKeyboardShortcut('a');
    _keyboardHandler.unregisterKeyboardShortcut('d');
    _keyboardHandler.unregisterKeyboardShortcut('w');
    _keyboardHandler.unregisterKeyboardShortcut('s');
}

void FlyingModeManipulator::dragLeft(const Vector2i& to, const Vector2i& from)
{
    const float du = (to.x - from.x) * getRotationSpeed();
    const float dv = (to.y - from.y) * getRotationSpeed();
    rotate(_camera.getPosition(), du, dv, AxisMode::globalY);
}

void FlyingModeManipulator::dragRight(const Vector2i& to, const Vector2i& from)
{
    const float distance = -(to.y - from.y) *
                           DEFAULT_MOUSE_MOTION_SPEED_MULTIPLIER *
                           getMotionSpeed();
    translate(Vector3f(0, 0, -1) * distance);
}

void FlyingModeManipulator::dragMiddle(const Vector2i& to, const Vector2i& from)
{
    const float x = (to.x - from.x) * DEFAULT_MOUSE_MOTION_SPEED_MULTIPLIER *
                    getMotionSpeed();
    const float y = (to.y - from.y) * DEFAULT_MOUSE_MOTION_SPEED_MULTIPLIER *
                    getMotionSpeed();
    translate({-x, y, 0.f});
}

void FlyingModeManipulator::wheel(const Vector2i& /*position*/,
                                  const float delta)
{
    translate(Vector3f(0, 0, -1) * delta * getWheelSpeed());
}

void FlyingModeManipulator::_strafeLeft()
{
    translate(Vector3f(-1, 0, 0) * getMotionSpeed());
}

void FlyingModeManipulator::_strafeRight()
{
    translate(Vector3f(1, 0, 0) * getMotionSpeed());
}

void FlyingModeManipulator::_flyForward()
{
    translate(Vector3f(0, 0, -1) * getMotionSpeed());
}

void FlyingModeManipulator::_flyBackwards()
{
    translate(Vector3f(0, 0, 1) * getMotionSpeed());
}
} // namespace brayns
