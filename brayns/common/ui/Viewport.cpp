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

#include "Viewport.h"

namespace brayns
{

Viewport::Viewport()
    : _modified(true)
    , _position(0,0,-1)
    , _up(0,1,0)
    , _target(0,0,0)
    , _openingAngle(60.f*M_PI/360.f)
    , _aspect(1.f)
{
    _viewMatrix = Matrix4f::IDENTITY;
}

void Viewport::initialize(
    const Vector3f& position,
    const Vector3f& target,
    const Vector3f& up )
{
    _viewMatrix = Matrix4f::IDENTITY;
    _position = position;
    _target = target;
    _up = up;

    const Vector3f dir = normalize(_target - _position);

    const Vector3f x = normalize( vmml::cross( dir, _up ));
    const Vector3f y = normalize( vmml::cross( dir, x ));
    const Vector3f z = normalize( dir );

    _viewMatrix.set_column(0, x);
    _viewMatrix.set_column(1, y);
    _viewMatrix.set_column(2, z);
    _viewMatrix.set_column(3, _position);

    _viewMatrix.set_row(3, Vector4f(0,0,0,1));

    _snapUp();
}

void Viewport::_snapUp()
{
    Vector4f _vx, _vy, _vz;
    _viewMatrix.get_column(0, _vx);
    _viewMatrix.get_column(1, _vy);
    _viewMatrix.get_column(2, _vz);

    const Vector3f vx( _vx[0], _vx[1], _vx[2] );
    const Vector3f vy( _vy[0], _vy[1], _vy[2] );
    const Vector3f vz( _vz[0], _vz[1], _vz[2] );

    if (fabsf(dot(_up,vz)) < 1e-3f)
        return;
    _viewMatrix.set_column(0, normalize( vmml::cross( vy, _up )));
    _viewMatrix.set_column(1, normalize( vmml::cross( vx, vy )));
    _viewMatrix.set_column(2, normalize( vmml::cross( vz, vx )));

    _modified = true;
}

void Viewport::translate(const Vector3f& vector, bool updateTarget)
{
    Matrix4f xfm( Matrix4f::IDENTITY );
    xfm.set_translation(vector);

    _viewMatrix = xfm * _viewMatrix;
    const Vector4f position( _position[0], _position[1], _position[2], 1.0f );
    _position = xfm * position;
    if( updateTarget )
    {
        const Vector4f target( _target[0], _target[1], _target[2], 1.0f );
        _target = xfm * target;
    }

    _snapUp();
}

void Viewport::rotate(const Vector3f& pivot, float du, float dv)
{
    Matrix4f xfm1( Matrix4f::IDENTITY );
    Matrix4f xfm2( Matrix4f::IDENTITY );
    Matrix4f xfm3( Matrix4f::IDENTITY );
    Matrix4f xfm4( Matrix4f::IDENTITY );

    xfm1.set_translation(pivot);
    xfm2.rotate_x(dv);
    xfm3.rotate_y(-du);
    xfm4.set_translation(-pivot);

    const Matrix4f xfm = xfm1 * xfm2 * xfm3 * xfm4;

    _viewMatrix = xfm * _viewMatrix;
    Vector4f position( _position[0], _position[1], _position[2], 1.0f );
    Vector4f target( _target[0], _target[1], _target[2], 1.0f );
    _position = xfm * position;
    _target = xfm * target;

    _snapUp();
}

std::ostream &operator<<(std::ostream &os, Viewport &viewport)
{
    os << std::endl <<
        "Position:" << viewport.getPosition() << std::endl <<
        "Target  :" << viewport.getTarget() << std::endl <<
        "Up      :" << viewport.getUp() << std::endl <<
        "Aspect  :" << viewport.getAspect() << std::endl <<
        "Matrix  :" << viewport.getViewMatrix().get_row(0) << std::endl <<
        "         " << viewport.getViewMatrix().get_row(1) << std::endl <<
        "         " << viewport.getViewMatrix().get_row(2) << std::endl <<
        "         " << viewport.getViewMatrix().get_row(3) << std::endl;
    return os;
}

}
