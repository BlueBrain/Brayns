/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#ifndef ABSTRACTMANIPULATOR_H
#define ABSTRACTMANIPULATOR_H

#include <brayns/common/types.h>

namespace brayns
{

/**
 * The different modes for moving the camera.
 */
enum class CameraMode
{
    FLYING,
    INSPECT_CENTER
};

/**
 * Base class for camera manipulators.
 */
class AbstractManipulator
{
public:
    AbstractManipulator( Camera& camera, KeyboardHandler& keyboardHandler );
    virtual ~AbstractManipulator();

    virtual void dragLeft( const Vector2i& to, const Vector2i& from ) = 0;
    virtual void dragRight( const Vector2i& to, const Vector2i& from ) = 0;
    virtual void dragMiddle( const Vector2i& to, const Vector2i& from ) = 0;
    virtual void wheel( const Vector2i& position, float delta ) = 0;

    float getMotionSpeed() const;
    void setMotionSpeed( float speed );

    float getRotationSpeed() const;

protected:
    /*! target camera */
    Camera& _camera;

    /*! keyboard handler to register/deregister keyboard events */
    KeyboardHandler& _keyboardHandler;

    /*! camera speed modifier - affects how many units the camera _moves_ with
     * each unit on the screen */
    float _motionSpeed;

    /*! camera rotation speed modifier - affects how many units the camera
     * _rotates_ with each unit on the screen */
    float _rotationSpeed;

    void translate( const Vector3f& v, bool updateTarget );
    void rotate( const Vector3f& pivot, float du, float dv, bool updateTarget );
};

}

#endif
