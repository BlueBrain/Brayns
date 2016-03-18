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

#include "Camera.h"
#include <brayns/common/log.h>
#ifdef BRAYNS_USE_ZEROBUF
#  include <zerobuf/render/camera.h>
#endif

namespace brayns
{

struct Camera::Impl
#ifdef BRAYNS_USE_ZEROBUF
    : public zerobuf::render::Camera
#endif
{
public:
    Impl( const CameraType cameraType )
        : _position( 0.f, 0.f, -1.f )
        , _target( 0.f, 0.f, 0.f )
        , _up( 0.f, 1.f, 0.f )
        , _cameraType( cameraType )
        , _aspectRatio( 1.f )
    {
    }

    void set(
        const Vector3f& position,
        const Vector3f& target,
        const Vector3f& upVector )
    {
        setPosition( position );
        setTarget( target );
        setUpVector( upVector );
    }

    const Vector3f& getPosition()
    {
    #ifdef BRAYNS_USE_ZEROBUF
        ::zerobuf::render::Vector3f& origin = getOrigin();
        _position = Vector3f( origin.getX( ), origin.getY( ), origin.getZ( ));
    #endif
        return _position;
    }

    void setPosition( const Vector3f& position )
    {
    #ifdef BRAYNS_USE_ZEROBUF
        ::zerobuf::render::Vector3f origin(
            position.x( ), position.y( ), position.z( ));
        setOrigin( origin );
    #endif
        _position = position;
    }

    const Vector3f& getTarget()
    {
    #ifdef BRAYNS_USE_ZEROBUF
        ::zerobuf::render::Vector3f& lookat = getLookAt();
        _target = Vector3f( lookat.getX( ), lookat.getY( ), lookat.getZ( ));
    #endif
        return _target;
    }

    void setTarget( const Vector3f& target )
    {
    #ifdef BRAYNS_USE_ZEROBUF
        ::zerobuf::render::Vector3f lookat(
            target.x( ), target.y( ), target.z( ));
        setLookAt( lookat );
    #endif
        _target = target;
    }

    const Vector3f& getUpVector( )
    {
    #ifdef BRAYNS_USE_ZEROBUF
        ::zerobuf::render::Vector3f& up = getUp( );
        _up = Vector3f( up.getX( ), up.getY( ), up.getZ( ));
    #endif
        return _up;
    }

    void setUpVector( const Vector3f& upVector )
    {
    #ifdef BRAYNS_USE_ZEROBUF
        ::zerobuf::render::Vector3f up(
            upVector.x( ), upVector.y( ), upVector.z( ));
        setUp( up );
    #endif
        _up = upVector;
    }

    CameraType getType( ) const
    {
        return _cameraType;
    }

private:
    Vector3f _position;
    Vector3f _target;
    Vector3f _up;
    CameraType _cameraType;
    float _aspectRatio;
};

Camera::Camera( const CameraType cameraType )
    : _impl( new Camera::Impl( cameraType ))
{
}

Camera::~Camera()
{
}

void Camera::set(
    const Vector3f& position,
    const Vector3f& target,
    const Vector3f& upVector )
{
    _impl->set( position, target, upVector );
}

const Vector3f& Camera::getPosition() const
{
    return _impl->getPosition( );
}

void Camera::setPosition( const Vector3f& position )
{
    _impl->setPosition( position );
}

const Vector3f& Camera::getTarget() const
{
    return _impl->getTarget();
}

void Camera::setTarget( const Vector3f& target )
{
    _impl->setTarget( target );
}

const Vector3f& Camera::getUpVector( ) const
{
    return _impl->getUpVector();
}

void Camera::setUpVector( const Vector3f& upVector )
{
    _impl->setUpVector( upVector );
}

CameraType Camera::getType( ) const
{
    return _impl->getType( );
}

servus::Serializable* Camera::getSerializable( )
{
#ifdef BRAYNS_USE_ZEROBUF
    return _impl.get();
#else
    return 0;
#endif
}

}
