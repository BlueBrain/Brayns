/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "Camera.h"
#include <brayns/common/log.h>
#ifdef BRAYNS_USE_ZEROBUF
#  include <zerobuf/render/fovCamera.h>
#endif

namespace brayns
{

struct Camera::Impl
#ifdef BRAYNS_USE_ZEROBUF
    : public zerobuf::render::FovCamera
#endif
{
public:
    Impl( const CameraType cameraType )
        : _cameraType( cameraType )
        , _position( 0.f, 0.f, -1.f )
        , _target( 0.f, 0.f, 0.f )
        , _up( 0.f, 1.f, 0.f )
        , _aspectRatio( 1.f )
        , _aperture( 0.f )
        , _focalLength( 0.f )
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

    const Vector3f& getPosition() const
    {
    #ifdef BRAYNS_USE_ZEROBUF
        const ::zerobuf::render::Vector3f& origin = getOrigin();
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

    const Vector3f& getTarget() const
    {
    #ifdef BRAYNS_USE_ZEROBUF
        const ::zerobuf::render::Vector3f& lookat = getLookAt();
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

    const Vector3f& getUpVector( ) const
    {
    #ifdef BRAYNS_USE_ZEROBUF
        const ::zerobuf::render::Vector3f& up = getUp( );
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

    void setAspectRatio( const float aspectRatio )
    {
        _aspectRatio = aspectRatio;
    }

    float getAspectRatio( ) const
    {
        return _aspectRatio;
    }

    void setAperture( const float aperture )
    {
    #ifdef BRAYNS_USE_ZEROBUF
        setFovAperture( aperture );
    #endif
        _aperture = aperture;
    }

    float getAperture( ) const
    {
    #ifdef BRAYNS_USE_ZEROBUF
        _aperture = getFovAperture( );
    #endif
        return _aperture;
    }

    void setFocalLength( const float focalLength )
    {
    #ifdef BRAYNS_USE_ZEROBUF
        setFovFocalLength( focalLength );
    #endif
        _focalLength = focalLength;
    }

    float getFocalLength( ) const
    {
    #ifdef BRAYNS_USE_ZEROBUF
        _focalLength = getFovFocalLength();
    #endif
        return _focalLength;
    }

private:
    CameraType _cameraType;
    mutable Vector3f _position;
    mutable Vector3f _target;
    mutable Vector3f _up;
    mutable float _aspectRatio;
    mutable float _aperture;
    mutable float _focalLength;
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

}
