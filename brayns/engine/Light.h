/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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
/**
 * @brief The Light class defines the common base class for all lights
 */
class Light : public SerializableEngineObject
{
public:
    using Ptr = std::unique_ptr<Light>;

    virtual ~Light();

    void setColor(const Vector3f &color) noexcept;
    void setIntensity(const float intensity) noexcept;
    void setVisible(const bool visible) noexcept;

    const Vector3f &getColor() const noexcept;
    float getIntensity() const noexcept;
    bool isVisible() const noexcept;

    void commit() final;

    OSPLight handle() const noexcept;

protected:
    virtual void commitLightSpecificParams() = 0;

    OSPLight _handle {nullptr};

private:
    Vector3f _color {1.f};
    float _intensity {1.};
    bool _visible {true};


};
} // namespace brayns
