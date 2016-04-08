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

    void translate( const Vector3f& v, bool updateTarget );
    void rotate( const Vector3f& pivot, float du, float dv, bool updateTarget );

    bool getModified() const
    {
        return _modified;
    }
    void setModified( const bool value )
    {
        _modified = value;
    }

    void setPosition( const Vector3f& value )\
    {
        _position = value;
        _modified = true;
    }
    Vector3f getPosition() const
    {
        return _position;
    }

    void setTarget( const Vector3f& value )
    {
        _target = value;
        _modified = true;
    }
    Vector3f getTarget() const
    {
        return _target;
    }

    void setUp( const Vector3f& value )
    {
        _up = value;
        _modified = true;
    }
    Vector3f getUp() const
    {
        return _up;
    }

    void setOpeningAngle( const float openingAngle )
    {
        _openingAngle=openingAngle;
        _modified=true;
    }
    float getOpeningAngle() const
    {
        return _openingAngle;
    }

    void setAspect( const float value)
    {
        _aspect = value;
        _modified = true;
    }
    float getAspect() const
    {
        return _aspect;
    }

    /*! set 'up' vector. if this vector is '0,0,0' the viewer will
     *not* apply the up-vector after camera manipulation */
    void _modify();

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

    /*! rotation matrice along x and y axis */
    Matrix4f _matrix;

};

std::ostream& operator << ( std::ostream &o, Viewport &viewport );

}

#endif // VIEWPORT_H
