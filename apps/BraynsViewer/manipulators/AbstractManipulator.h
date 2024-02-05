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

#pragma once

#include <brayns/common/MathTypes.h>
#include <brayns/engine/Camera.h>

constexpr float DEFAULT_MOUSE_MOTION_SPEED_MULTIPLIER = 0.25f;

namespace brayns
{
class KeyboardHandler;

/**
 * Base class for camera manipulators.
 */
class AbstractManipulator
{
public:
    enum class AxisMode
    {
        globalY = 0,
        localY
    };

    AbstractManipulator(Camera& camera, KeyboardHandler& keyboardHandler);
    virtual ~AbstractManipulator() = default;

    /** Adjust manipulator behaviour to the given scene */
    virtual void adjust(const Boxd& boundingBox);
    virtual void dragLeft(const Vector2i& to, const Vector2i& from) = 0;
    virtual void dragRight(const Vector2i& to, const Vector2i& from) = 0;
    virtual void dragMiddle(const Vector2i& to, const Vector2i& from) = 0;
    virtual void wheel(const Vector2i& position, float delta) = 0;

    float getMotionSpeed() const;
    void updateMotionSpeed(float speed);

    float getRotationSpeed() const;
    float getWheelSpeed() const;
    void rotate(const Vector3d& pivot, double du, double dv, AxisMode axisMode);

protected:
    /*! target camera */
    Camera& _camera;

    /*! keyboard handler to register/deregister keyboard events */
    KeyboardHandler& _keyboardHandler;

    /*! camera speed modifier - affects how many units the camera _moves_ with
     * each unit on the screen */
    double _motionSpeed;

    /*! camera rotation speed modifier - affects how many units the camera
     * _rotates_ with each unit on the screen */
    double _rotationSpeed;

    void translate(const Vector3d& v);
};
} // namespace brayns
