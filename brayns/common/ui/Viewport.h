/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <brayns/common/mathTypes.h>

namespace brayns
{

class Viewport
{
public:
    Viewport();

    void initialize(
        const Vector3f& position,
        const Vector3f& target,
        const Vector3f& up );

    void translate(const Vector3f& v, bool updateTarget = false);
    void rotate(const Vector3f& pivot, float du, float dv);

    bool getModified() const { return _modified; }
    void setModified(bool value) { _modified = value; }

    void setPosition(const Vector3f& value) { _position = value; _modified=true; }
    Vector3f getPosition() const { return _position; }

    void setTarget(const Vector3f& value) { _target = value; _modified=true; }
    Vector3f getTarget() const { return _target; }

    void setUp(const Vector3f& value) { _up = value; _modified=true; }
    Vector3f getUp() const { return _up; }

    void setOpeningAngle(float openingAngle) { _openingAngle=openingAngle; _modified=true; }
    float getOpeningAngle() const { return _openingAngle; }

    void setAspect(float value) { _aspect = value; _modified=true; }
    float getAspect() const { return _aspect; }

    void setViewMatrix(const Matrix4f& matrix) { _viewMatrix=matrix; _modified=true; }
    Matrix4f getViewMatrix() const { return _viewMatrix; }

    /*! set 'up' vector. if this vector is '0,0,0' the viewer will
     *not* apply the up-vector after camera manipulation */
    void _snapUp();

private:
    bool _modified; /* the viewPort will set this flag any time any of
                      its values get changed. */

    Vector3f _position;
    Vector3f _up;
    Vector3f _target;

    /*! opening angle, in radians, along Y direction */
    float _openingAngle;

    /*! aspect ration i Y:X */
    float _aspect;

    /*! camera viw matrix in which the Z axis is the depth axis, and X
      and Y axes are parallel to the screen X and Y axis. The frame
      itself remains normalized. */
    Matrix4f _viewMatrix;

};

std::ostream &operator<<(std::ostream &o, Viewport &viewport);

}

#endif // VIEWPORT_H
