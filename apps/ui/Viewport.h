/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
