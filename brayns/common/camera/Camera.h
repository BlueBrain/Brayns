/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <brayns/api.h>
#include <brayns/common/types.h>

#include <servus/serializable.h>

namespace brayns
{

enum CameraType
{
    CT_UNDEFINED = 0,
    CT_PERSPECTIVE,
    CT_STEREO,
    CT_ORTHOGRAPHIC
};

/**
   Camera object

   This object in an abstract interface to a camera which is defined by a
   position, target and up vector
*/
class Camera
{
public:

    /**
       Default constructor
       @param cameraType Type of camera (Perpective, Stereo, etc)
    */
    BRAYNS_API Camera( CameraType cameraType );

    BRAYNS_API virtual ~Camera();

    /**
       Sets position, target and up vector
       @param position The x, y, z coordinates of the camera position
       @param target The x, y, z coordinates of the camera target: the point the
              camera is "looking at" or focused on
       @param up the x, y, z coordinates of the up vector's end point
    */
    BRAYNS_API virtual void set(
        const Vector3f& position,
        const Vector3f& target,
        const Vector3f& upVector );

    /**
       Gets camera type
       @return The type of camera (Perpective, Stereo, etc)
    */
    BRAYNS_API CameraType getType( ) const;

    /**
       Sets camera position
       @param position The x, y, z coordinates of the camera position
    */
    BRAYNS_API virtual void setPosition( const Vector3f& );

    /**
       Gets camera position
       @return The x, y, z coordinates of the camera position
    */
    BRAYNS_API virtual const Vector3f& getPosition( ) const;

    /**
       Sets camera target
       @param target The x, y, z coordinates of the camera target: the point the
              camera is "looking at" or focused on
    */
    BRAYNS_API virtual void setTarget( const Vector3f& );

    /**
       Gets camera target
       @return The x, y, z coordinates of the camera target: the point the
               camera is "looking at" or focused on
    */
    BRAYNS_API virtual const Vector3f& getTarget( ) const;

    /**
       Sets camera up vector
       @param up the x, y, z coordinates of the up vector's end point
    */
    BRAYNS_API virtual void setUpVector( const Vector3f& );

    /**
       Gets camera up vector
       @return the x, y, z coordinates of the up vector's end point
    */
    BRAYNS_API virtual const Vector3f& getUpVector( ) const;

    /**
       Commits the changes held by the camera object so that
       attributes become available to the underlying rendering engine
    */
    BRAYNS_API virtual void commit( ) =  0;

    /**
       Set the aspec ratio of the camera
       @param aspectRatio The new aspect ratio
    */
    BRAYNS_API virtual void setAspectRatio( float aspectRatio ) = 0;

    /**
       Returns the serializable of the Camera
       @param A serializable of the Camera
    */
    BRAYNS_API servus::Serializable* getSerializable( );

private:
    struct Impl;

    std::unique_ptr< Impl > _impl;
};

}

#endif // CAMERA_H
