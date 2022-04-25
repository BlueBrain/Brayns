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

#include <brayns/common/BaseObject.h>
#include <brayns/common/MathTypes.h>

#include <ospray/ospray.h>

#include <memory>

namespace brayns
{
struct LookAt
{
    Vector3f position{0.f};
    Vector3f target{0.f, 0.f, 1.f};
    Vector3f up{0.f, 1.f, 0.f};
};

bool operator==(const LookAt &a, const LookAt &b) noexcept;

class Camera : public BaseObject
{
public:
    Camera() = default;

    Camera(const Camera &);
    Camera &operator=(const Camera &);

    Camera(Camera &&) noexcept;
    Camera &operator=(Camera &&) noexcept;

    virtual ~Camera();

    /**
     * @brief Returns the camera type as a string
     */
    virtual std::string getName() const noexcept = 0;

    /**
     * @brief Creates a copy of the current camera
     *
     * @return std::unique_ptr<Camera>
     */
    virtual std::unique_ptr<Camera> clone() const noexcept = 0;

    /**
     * @brief Commit implementation. Derived camera types must override commitCameraSpecificParams(),
     * which will be called during commit() to perform camera-specific synchronization with OSPRay
     * @returns true if there was anything to commit
     */
    bool commit();

    /**
     * @brief Sets the look-at parameters of the camera
     * @param params LookAt
     */
    void setLookAt(const LookAt &params) noexcept;

    /**
     * @brief Returns the look-at parameters of the camera
     * @return const LookAt &
     */
    const LookAt &getLookAt() const noexcept;

    /**
     * @brief Sets the resolution aspect ratio on to which this camera will be generating rays
     */
    void setAspectRatio(const float aspectRatio) noexcept;

    /**
     * @brief Returns the OSPRay handle of this camera
     */
    OSPCamera handle() const noexcept;

protected:
    /**
     * @brief Subclasses must implement this method so that the appropiate OSPRay camera object maight be
     * instantiated
     */
    virtual std::string getOSPHandleName() const noexcept = 0;

    /**
     * @brief Subclasses of the Camera must implement this method to commit to OSPRay camera type specific
     * parameters. The camera class will call ospCommit(_handle), thus sublcass may avoid calling it.
     */
    virtual void commitCameraSpecificParams() = 0;

private:
    LookAt _lookAtParams;
    float _aspectRatio{1.f};
    OSPCamera _handle{nullptr};
};
} // namespace brayns
