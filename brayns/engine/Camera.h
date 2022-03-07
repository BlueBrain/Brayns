/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#pragma once

#include <brayns/common/MathTypes.h>
#include <brayns/engine/EngineObject.h>

#include <ospray/ospray.h>

#include <memory>

namespace brayns
{
class Camera : public EngineObject
{
public:
    using Ptr = std::unique_ptr<Camera>;

    Camera() = default;

    Camera(const Camera&);
    Camera &operator=(const Camera &);

    Camera(Camera&&) = default;
    Camera &operator=(Camera&&) = default;

    virtual ~Camera();

    /**
     * @brief Returns the camera type as a string
     */
    virtual std::string_view getName() const noexcept = 0;

    /**
     * @brief Creates a copy of this camera
     */
    virtual Ptr clone() const noexcept = 0;

    /**
     * @brief Commit implementation. Derived camera types must override commitCameraSpecificParams(),
     * which will be called during commit() to perform camera-specific synchronization with OSPRay
     */
    void commit() final;

    /**
     * @brief Sets the camera position on world coordinates
     */
    void setPosition(const Vector3f &position) noexcept;

    /**
     * @brief Sets the point in world space coordinates at which the camera is looking at
     */
    void setTarget(const Vector3f &target) noexcept;

    /**
     * @brief Sets the Up vector of this camera. Will be used to compute the real Up vector,
     * thus it doesnt have to be the exact up vector of the camera, but to point at the
     * hemisphere in which the UP vector must be contained.
     * The computation is as follows:
     *  - forward = normalize(target - position)
     *  - strafe = cross(forward, up)
     *  - real UP = cross(strafe, forward) (the one committed to OSPRay)
     */
    void setUp(const Vector3f &up) noexcept;

    /**
     * @brief Sets the resolution aspect ratio on to which this camera will be generating rays
     */
    void setAspectRatio(const float aspectRatio) noexcept;

    /**
     * @brief Returns the camera current position in space
     */
    const Vector3f& getPosition() const noexcept;

    /**
     * @brief Returns the camera current view target in space
     */
    const Vector3f& getTarget() const noexcept;

    /**
     * @brief Returns the user-specified camera up vector (It might be not the same that is being
     * commited to OSPRay, see setUp())
     */
    const Vector3f& getUp() const noexcept;

    /**
     * @brief Returns the OSPRay handle of this camera
     */
    OSPCamera handle() const noexcept;

protected:
    /**
     * @brief Subclasses must implement this method so that the appropiate OSPRay camera object maight be
     * instantiated
     */
    virtual std::string_view getOSPHandleName() const noexcept = 0;

    /**
     * @brief Subclasses of the Camera must implement this method to commit to OSPRay camera type specific
     * parameters. The camera class will call ospCommit(_handle), thus sublcass may avoid calling it.
     */
    virtual void commitCameraSpecificParams() = 0;

private:
    Vector3f _position {0.f};
    Vector3f _target {0.f, 0.f, 1.f};
    Vector3f _up {0.f, 1.f, 0.f};
    float _aspectRatio {1.f};
    OSPCamera _handle {nullptr};
};
} // namespace brayns
