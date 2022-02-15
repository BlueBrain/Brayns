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
class Camera : public SerializableEngineObject
{
public:
    using Ptr = std::unique_ptr<Camera>;

    Camera() = default;

    Camera(const Camera&) noexcept;
    Camera &operator=(const Camera &) noexcept;

    virtual ~Camera();

    void commit() final;

    void setPosition(const Vector3f &position) noexcept;
    void setTarget(const Vector3f &target) noexcept;
    void setUp(const Vector3f &up) noexcept;
    void setAspectRatio(const float aspectRatio) noexcept;

    const Vector3f& getPosition() const noexcept;
    const Vector3f& getTarget() const noexcept;
    const Vector3f& getUp() const noexcept;

    OSPCamera handle() const noexcept;

protected:
    virtual void commitCameraSpecificParams() = 0;

private:
    Vector3f _position {0.f};
    Vector3f _target {0.f, 0.f, 1.f};
    Vector3f _up {0.f, 1.f, 0.f};
    float _aspectRatio;

protected:
    OSPCamera _handle {nullptr};
};
} // namespace brayns
