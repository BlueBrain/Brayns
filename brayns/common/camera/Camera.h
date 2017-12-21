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

#ifndef CAMERA_H
#define CAMERA_H

#include <brayns/api.h>
#include <brayns/common/BaseObject.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(Camera)

namespace brayns
{
/**
   Camera object

   This object in an abstract interface to a camera which is defined by a
   position, target and up vector
*/
class Camera : public BaseObject
{
public:
    /**
       Default constructor
       @param cameraType Type of camera (Perpective, Stereo, etc)
    */
    BRAYNS_API Camera(CameraType cameraType);

    BRAYNS_API virtual ~Camera();

    /**
       Sets position, target and up vector
       @param position The x, y, z coordinates of the camera position
       @param target The x, y, z coordinates of the camera target: the point the
              camera is "looking at" or focused on
       @param up the x, y, z coordinates of the up vector's end point
    */
    BRAYNS_API void set(const Vector3f& position, const Vector3f& target,
                        const Vector3f& up);

    BRAYNS_API void setInitialState(const Vector3f& position,
                                    const Vector3f& target, const Vector3f& up);

    /**
       Gets camera type
       @return The type of camera (Perpective, Stereo, etc)
    */
    CameraType getType() const { return _type; }
    /**
       Sets camera position
       @param position The x, y, z coordinates of the camera position
    */
    void setPosition(const Vector3f& position)
    {
        _updateValue(_position, position);
    }

    /**
       Gets camera position
       @return The x, y, z coordinates of the camera position
    */
    const Vector3f& getPosition() const { return _position; }
    /**
       Sets camera target
       @param target The x, y, z coordinates of the camera target: the point the
              camera is "looking at" or focused on
    */
    void setTarget(const Vector3f& target) { _updateValue(_target, target); }
    /**
       Gets camera target
       @return The x, y, z coordinates of the camera target: the point the
               camera is "looking at" or focused on
    */
    const Vector3f& getTarget() const { return _target; }
    /**
       Sets camera up vector
       @param up the x, y, z coordinates of the up vector's end point
    */
    void setUp(const Vector3f& up) { _updateValue(_up, up); }
    /**
       Gets camera up vector
       @return the x, y, z coordinates of the up vector's end point
    */
    const Vector3f& getUp() const { return _up; }
    /**
       Gets the camera rotation matrix
       @return the rotation matrix from the original *target* and *up* vectors
    */
    Matrix4f& getRotationMatrix() { return _matrix; }
    /**
       Commits the changes held by the camera object so that
       attributes become available to the underlying rendering engine
    */
    BRAYNS_API virtual void commit() = 0;

    /**
       The field of view is the extent of the observable world that is seen at
       any given moment.
       In case of optical instruments or sensors it is a solid angle through
       which a detector is
       sensitive to electromagnetic radiation.
    */
    /** @return the field of view of the camera */
    float getFieldOfView() const { return _fieldOfView; }
    /**
       Set the field of view of the camera
       @param fov The field of view
    */
    void setFieldOfView(const float fov) { _updateValue(_fieldOfView, fov); }
    /**
       Set the aspect ratio of the camera
       @param aspectRatio The new aspect ratio
    */
    void setAspectRatio(float aspectRatio)
    {
        _updateValue(_aspectRatio, aspectRatio);
    }

    /** @return the aspect ration of the camera */
    float getAspectRatio() const { return _aspectRatio; }
    /**
       @copydoc Camera::getAperture
       @param aperture The new aperture
    */
    void setAperture(float aperture) { _updateValue(_aperture, aperture); }
    /**
       The aperture determines how collimated the admitted rays are, which is of
       great importance for the appearance at the image plane. If an aperture is
       narrow, then highly collimated rays are admitted, resulting in a sharp
       focus at the image plane. If an aperture is wide, then uncollimated rays
       are admitted, resulting in a sharp focus only for rays with a certain
       focal length.
    */
    float getAperture() const { return _aperture; }
    /**
       @copydoc Camera::getFocalLength
       @param focalLength The new focal length
    */
    void setFocalLength(float focalLength)
    {
        _updateValue(_focalLength, focalLength);
    }

    /**
      The focal length of an optical system is a measure of how strongly the
      system converges or diverges light. For an optical system in air, it is
      the distance over which initially collimated rays are brought to a focus.
      A system with a shorter focal length has greater optical power than one
      with a long focal length; that is, it bends the rays more sharply,
      bringing them to a focus in a shorter distance.
    */
    float getFocalLength() const { return _focalLength; }
    /**
       @brief Sets the stereo mode of the camera (Left eye, Right eye or Side by
       Side)
       @param stereoMode The new stereo mode
    */
    void setStereoMode(CameraStereoMode stereoMode)
    {
        _updateValue(_stereoMode, stereoMode);
    }

    /**
      @return the stereo mode of the Camera
    */
    CameraStereoMode getStereoMode() const { return _stereoMode; }
    /**
       @brief Sets the eye separation of the stereo camera
       @param eyeSeparation The new distance bewteen eyes
    */
    void setEyeSeparation(float eyeSeparation)
    {
        _updateValue(_eyeSeparation, eyeSeparation);
    }

    /**
      @return the eye separation of the stereo Camera
    */
    float getEyeSeparation() const { return _eyeSeparation; }
    /** Resets the camera to its initial values */
    BRAYNS_API void reset();

    /** Enable/disables environment mapping */
    BRAYNS_API virtual void setEnvironmentMap(const bool environmentMap) = 0;

    /**
      Sets the camera clip planes
    */
    void setClipPlanes(const ClipPlanes clipPlanes)
    {
        _updateValue(_clipPlanes, clipPlanes);
    }

    /**
      @return the camera clip planes
    */
    const ClipPlanes& getClipPlanes() const { return _clipPlanes; }
private:
    CameraType _type{CameraType::perspective};
    Vector3f _position;
    Vector3f _target;
    Vector3f _up;

    Vector3f _initialPosition;
    Vector3f _initialTarget;
    Vector3f _initialUp;

    float _aspectRatio{1.f};
    float _aperture{0.f};
    float _focalLength{0.f};
    float _fieldOfView{45.f};

    CameraStereoMode _stereoMode{CameraStereoMode::none};
    float _eyeSeparation{0.0635f};

    ClipPlanes _clipPlanes{
        {{-1.f, 0.f, 0.f, std::numeric_limits<float>::max()},
         {1.f, 0.f, 0.f, std::numeric_limits<float>::max()},
         {0.f, -1.f, 0.f, std::numeric_limits<float>::max()},
         {0.f, 1.f, 0.f, std::numeric_limits<float>::max()},
         {0.f, 0.f, -1.f, std::numeric_limits<float>::max()},
         {0.f, 0.f, 1.f, std::numeric_limits<float>::max()}}};

    /*! rotation matrice along x and y axis */
    Matrix4f _matrix;

    SERIALIZATION_FRIEND(Camera)
};

std::ostream& operator<<(std::ostream& os, Camera& camera);
}

#endif
