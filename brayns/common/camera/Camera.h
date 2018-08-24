/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
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

#ifndef CAMERA_H
#define CAMERA_H

#include <brayns/api.h>
#include <brayns/common/PropertyObject.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(Camera)

namespace brayns
{
/**
   Camera object

   This object in an abstract interface to a camera which is defined by a
   position, target and up vector
*/
class Camera : public PropertyObject
{
public:
    BRAYNS_API Camera() = default;

    BRAYNS_API virtual ~Camera() = default;

    BRAYNS_API Camera& operator=(const Camera& rhs);

    /**
       Sets position, target and up vector
       @param position The x, y, z coordinates of the camera position
       @param target The x, y, z coordinates of the camera target: the point the
              camera is "looking at" or focused on
       @param up the x, y, z coordinates of the up vector's end point
    */
    BRAYNS_API void set(const Vector3d& position, const Vector3d& target,
                        const Vector3d& up);

    BRAYNS_API void setInitialState(const Vector3d& position,
                                    const Vector3d& target, const Vector3d& up);

    BRAYNS_API void setInitialState(const Boxd& boundingBox);

    /**
       Sets camera position
       @param position The x, y, z coordinates of the camera position
    */
    void setPosition(const Vector3d& position)
    {
        _updateValue(_position, position);
    }

    /**
       Gets camera position
       @return The x, y, z coordinates of the camera position
    */
    const Vector3d& getPosition() const { return _position; }
    /**
       Sets camera target
       @param target The x, y, z coordinates of the camera target: the point the
              camera is "looking at" or focused on
    */
    void setTarget(const Vector3d& target) { _updateValue(_target, target); }
    /**
       Gets camera target
       @return The x, y, z coordinates of the camera target: the point the
               camera is "looking at" or focused on
    */
    const Vector3d& getTarget() const { return _target; }
    /**
       Sets camera up vector
       @param up the x, y, z coordinates of the up vector's end point
    */
    void setUp(const Vector3d& up) { _updateValue(_up, up); }
    /**
       Gets camera up vector
       @return the x, y, z coordinates of the up vector's end point
    */
    const Vector3d& getUp() const { return _up; }
    /**
       Gets the camera rotation matrix
       @return the rotation matrix from the original *target* and *up* vectors
    */
    Matrix4f& getRotationMatrix() { return _matrix; }
    /**
       Commits the changes held by the camera object so that
       attributes become available to the underlying rendering engine
    */
    BRAYNS_API virtual void commit(){};

    /** Resets the camera to its initial values */
    BRAYNS_API void reset();

    /** Enable/disables environment mapping */
    BRAYNS_API virtual void setEnvironmentMap(
        const bool environmentMap BRAYNS_UNUSED){};

    virtual bool isSideBySideStereo() const { return false; }
private:
    Vector3d _position;
    Vector3d _target;
    Vector3d _up;

    Vector3d _initialPosition;
    Vector3d _initialTarget;
    Vector3d _initialUp;

    /*! rotation matrice along x and y axis */
    Matrix4f _matrix;

    SERIALIZATION_FRIEND(Camera)
};

std::ostream& operator<<(std::ostream& os, Camera& camera);
}

#endif
