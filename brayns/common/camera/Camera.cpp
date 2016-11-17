/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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
        : _cameraType( cameraType )
        , _position( 0.f, 0.f, 0.f )
        , _target( 0.f, 0.f, 0.f )
        , _up( 0.f, 0.f, 0.f )
        , _aspectRatio( 1.f )
        , _aperture( 0.f )
        , _focalLength( 0.f )
        , _fieldOfView( 60.f )
    {
    }

    void set(
        const Vector3f& position,
        const Vector3f& target,
        const Vector3f& up )
    {
        setPosition( position );
        setTarget( target );
        setUp( up );
    }

    void setInitialState(
        const Vector3f& position,
        const Vector3f& target,
        const Vector3f& up )
    {
        _initialPosition = position;
        _initialTarget = target;
        _initialUp = up;
        set( position, target, up );
    }

    void reset()
    {
        set( _initialPosition, _initialTarget, _initialUp );
        _matrix = Matrix4f();
    }

    const Vector3f& getPosition() const
    {
    #ifdef BRAYNS_USE_ZEROBUF
        const floats& origin = getOriginVector();
        _position = Vector3f( origin[0], origin[1], origin[2] );
    #endif
        return _position;
    }

    void setPosition( const Vector3f& position )
    {
        if( _position.equals( position ))
            return;
    #ifdef BRAYNS_USE_ZEROBUF
        const floats& origin = { position.x(), position.y(), position.z() };
        zerobuf::render::Camera::setOrigin( origin );
    #endif
        _position = position;
        modified = true;
    }

    const Vector3f& getTarget() const
    {
    #ifdef BRAYNS_USE_ZEROBUF
        const floats& lookat = getLook_atVector();
        _target = Vector3f( lookat[0], lookat[1], lookat[2] );
    #endif
        return _target;
    }

    void setTarget( const Vector3f& target )
    {
        if( _target.equals( target ))
            return;
    #ifdef BRAYNS_USE_ZEROBUF
        const floats& lookat = { target.x(), target.y(), target.z() };
        zerobuf::render::Camera::setLook_at( lookat );
    #endif
        _target = target;
        modified = true;
    }

    const Vector3f& getUp( ) const
    {
    #ifdef BRAYNS_USE_ZEROBUF
        const floats& up = getUpVector();
        _up = Vector3f( up[0], up[1], up[2] );
    #endif
        return _up;
    }

    void setUp( const Vector3f& upVector )
    {
        if( _up.equals( upVector ))
            return;
    #ifdef BRAYNS_USE_ZEROBUF
        const floats& up = { upVector.x(), upVector.y(), upVector.z() };
        zerobuf::render::Camera::setUp( up );
    #endif
        _up = upVector;
        modified = true;
    }

    CameraType getType( ) const
    {
        return _cameraType;
    }

    void setFieldOfView( const float fieldOfView )
    {
        if( _fieldOfView == fieldOfView )
            return;
        _fieldOfView = fieldOfView;
        modified = true;
    }

    float getFieldOfView( ) const
    {
        return _fieldOfView;
    }

    void setAspectRatio( const float aspectRatio )
    {
        if( _aspectRatio == aspectRatio )
            return;
        _aspectRatio = aspectRatio;
        modified = true;
    }

    float getAspectRatio( ) const
    {
        return _aspectRatio;
    }

    void setAperture( const float aperture )
    {
        if( _aperture == aperture )
            return;
    #ifdef BRAYNS_USE_ZEROBUF
        zerobuf::render::Camera::setAperture( aperture );
    #endif
        _aperture = aperture;
        modified = true;
    }

    float getAperture( ) const
    {
    #ifdef BRAYNS_USE_ZEROBUF
        _aperture = zerobuf::render::Camera::getAperture( );
    #endif
        return _aperture;
    }

    void setFocalLength( const float focalLength )
    {
        if( _focalLength == focalLength )
            return;
    #ifdef BRAYNS_USE_ZEROBUF
        zerobuf::render::Camera::setFocal_length( focalLength );
    #endif
        _focalLength = focalLength;
        modified = true;
    }

    float getFocalLength( ) const
    {
    #ifdef BRAYNS_USE_ZEROBUF
        _focalLength = zerobuf::render::Camera::getFocal_length();
    #endif
        return _focalLength;
    }

    bool modified = false;

    /*! rotation matrice along x and y axis */
    Matrix4f _matrix;

private:
    CameraType _cameraType;
    mutable Vector3f _position;
    mutable Vector3f _target;
    mutable Vector3f _up;

    Vector3f _initialPosition;
    Vector3f _initialTarget;
    Vector3f _initialUp;

    mutable float _aspectRatio;
    mutable float _aperture;
    mutable float _focalLength;
    mutable float _fieldOfView;
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

void Camera::setInitialState(
    const Vector3f& position,
    const Vector3f& target,
    const Vector3f& upVector )
{
    _impl->setInitialState( position, target, upVector );
}

void Camera::reset( )
{
    return _impl->reset( );
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

const Vector3f& Camera::getUp( ) const
{
    return _impl->getUp();
}

vmml::Matrix4f& Camera::getRotationMatrix()
{
    return _impl->_matrix;
}

void Camera::setUp( const Vector3f& upVector )
{
    _impl->setUp( upVector );
}

CameraType Camera::getType( ) const
{
    return _impl->getType( );
}

void Camera::setFieldOfView( const float fov )
{
    _impl->setFieldOfView( fov );
}

float Camera::getFieldOfView( ) const
{
    return _impl->getFieldOfView();
}

void Camera::setAspectRatio( const float aspectRatio )
{
    _impl->setAspectRatio( aspectRatio );
}

float Camera::getAspectRatio( ) const
{
    return _impl->getAspectRatio( );
}

void Camera::setAperture( const float aperture )
{
    _impl->setAperture( aperture );
}

float Camera::getAperture( ) const
{
    return _impl->getAperture( );
}

void Camera::setFocalLength( const float focalLength )
{
    _impl->setFocalLength( focalLength );
}

float Camera::getFocalLength( ) const
{
    return _impl->getFocalLength( );
}

servus::Serializable* Camera::getSerializable( )
{
#ifdef BRAYNS_USE_ZEROBUF
    return _impl.get();
#else
    return 0;
#endif
}

bool Camera::getModified() const
{
    return _impl->modified;
}

void Camera::resetModified()
{
    _impl->modified = false;
}

std::ostream& operator << ( std::ostream& os, Camera& camera )
{
    const auto& position = camera.getPosition();
    const auto& target = camera.getTarget();
    const auto& up = camera.getUp();
    os <<
        position.x() << "," <<
        position.y() << "," <<
        position.z() << "," <<
        target.x() << "," <<
        target.y() << "," <<
        target.z() << "," <<
        up.x() << "," <<
        up.y() << "," <<
        up.z();
    return os;
}

}
