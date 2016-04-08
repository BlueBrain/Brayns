/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include <brayns/common/log.h>

#include "Viewport.h"

namespace brayns
{

const Vector3f INITIAL_UP = { 0, 1, 0 };
const Vector3f INITIAL_POSITION = { 0, 0, -1 };
const Vector3f INITIAL_DIRECTION = { 0, 0, 1 };
const Vector3f ZERO = { 0, 0, 0 };

Viewport::Viewport()
    : _modified( true )
    , _position( INITIAL_POSITION )
    , _up( INITIAL_UP )
    , _target( ZERO )
    , _openingAngle( 60.f*M_PI/360.f )
    , _aspect(1.f)
    , _matrix( Matrix4f::IDENTITY )
{
}

void Viewport::initialize(
    const Vector3f& position,
    const Vector3f& target,
    const Vector3f& up )
{
    _position = position;
    _target = target;
    _up = up;
    _modify();
}

void Viewport::_modify()
{
    _modified = true;
}

void Viewport::translate( const Vector3f& vector, bool updateTarget )
{
     _position = vector + _position;
     if( updateTarget )
        _target = vector + _target;
    _modify();
}

void Viewport::rotate(
    const Vector3f& pivot, float du, float dv, bool updateTarget)
{
    _matrix.rotate_x( dv );
    _matrix.rotate_y( -du * std::copysign( 1.f, _up.y( )));

    const Vector3f dir =  _target - _position ;
    const float l = dir.length();
    if( updateTarget )
    {
        const Vector3f target = _matrix * ( l * INITIAL_DIRECTION );
        _target = _position + target;
    }
    else
    {
        _position = _position - pivot;
        _position = _matrix * ( l * INITIAL_POSITION );
        _position = _position + pivot;
    }
    _up = _matrix * INITIAL_UP;

    _modify();
}

std::ostream &operator << ( std::ostream &os, Viewport &viewport )
{
    os << std::endl <<
        "Position:" << viewport.getPosition() << std::endl <<
        "Target  :" << viewport.getTarget() << std::endl <<
        "Up      :" << viewport.getUp() << std::endl <<
        "Aspect  :" << viewport.getAspect() << std::endl;
    return os;
}

}
