/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <brayns/common/PropertyObject.h>

#include <memory>

namespace brayns
{
/**
   Camera object

   This object in an abstract interface to a camera which is defined by a
   position and a quaternion
*/
class Camera : public PropertyObject
{
public:
    /** @name API for engine-specific code */
    //@{
    /**
       Commits the changes held by the camera object so that
       attributes become available to the underlying rendering engine
    */
    virtual void commit(){};
    //@}

    Camera() = default;

    virtual ~Camera() = default;

    Camera& operator=(const Camera& rhs);

    /**
       Sets position, and quaternion
       @param position The x, y, z coordinates of the camera position
       @param orientation The x, y, z, w values of the quaternion describing
              the camera orientation
       @param target The x, y, z coordinates of the camera target
    */
    void set(const Vector3d& position, const Quaterniond& orientation,
             const Vector3d& target = Vector3d(0.0, 0.0, 0.0));

    void setInitialState(const Vector3d& position,
                         const Quaterniond& orientation,
                         const Vector3d& target = Vector3d(0.0, 0.0, 0.0));

    /**
       Sets camera position
       @param position The x, y, z coordinates of the camera position
    */
    void setPosition(const Vector3d& position)
    {
        _updateValue(_position, position);
    }
    /**
       Sets camera target
       @param target The x, y, z coordinates of the camera target
    */
    void setTarget(const Vector3d& target) { _updateValue(_target, target); }
    /**
       Gets camera position
       @return The x, y, z coordinates of the camera position
    */
    const Vector3d& getPosition() const { return _position; }
    /**
       Gets camera target
       @return The x, y, z coordinates of the camera target
    */
    const Vector3d& getTarget() const { return _target; }
    /**
       Sets camera orientation quaternion.
       @param orientation The orientation quaternion
    */
    void setOrientation(Quaterniond orientation)
    {
        orientation = glm::normalize(orientation);
        _updateValue(_orientation, orientation);
    }
    /**
       Gets the camera orientation quaternion
       @return the orientation quaternion
    */
    const Quaterniond& getOrientation() const { return _orientation; }
    /** Resets the camera to its initial values */
    void reset();

    /** @internal Sets the name of current rendered frame buffer. */
    void setBufferTarget(const std::string& target)
    {
        _updateValue(_bufferTarget, target, false);
    }
    /** @internal @return the current rendererd frame buffer. */
    const std::string& getBufferTarget() const { return _bufferTarget; }

private:
    Vector3d _target;
    Vector3d _position;
    Quaterniond _orientation;

    Vector3d _initialTarget;
    Vector3d _initialPosition;
    Quaterniond _initialOrientation;

    std::string _bufferTarget;
};

using CameraPtr = std::shared_ptr<Camera>;
} // namespace brayns
