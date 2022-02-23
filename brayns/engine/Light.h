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

    Light() = default;

    Light(const Light&);
    Light &operator=(const Light&);

    Light(Light&&) = default;
    Light &operator=(Light&&) = default;

    virtual ~Light();

    /**
     * @brief Sets the light color as normalized RGB
     */
    void setColor(const Vector3f &color) noexcept;

    /**
     * @brief Sets the light intensity. A value of 0.0 will disable the light. Must be >= 0.0
     */
    void setIntensity(const float intensity) noexcept;

    /**
     * @brief Sets wether the light is visible on the scene rendering.
     */
    void setVisible(const bool visible) noexcept;

    /**
     * @brief Return the lights current color as normalized RGB
     */
    const Vector3f &getColor() const noexcept;

    /**
     * @brief Returns the light current intensity
     */
    float getIntensity() const noexcept;

    /**
     * @brief Returns wether the light is visible on the scene or not
     */
    bool isVisible() const noexcept;

protected:
    friend class Scene;

    /**
     * @brief Returns the size in bytes of the light
     */
    virtual uint64_t getSizeInBytes() const noexcept = 0;

    /**
     * @brief commit() implementation
     */
    void commit() final;

    /**
     * @brief Returns this light OSPRay handle
     */
    OSPLight handle() const noexcept;

    /**
     * @brief Subclasses must implement this method to return the OSPRay handle ID so that it can be instantiated.
     */
    virtual std::string_view getOSPHandleName() const noexcept = 0;

    /**
     * @brief Subclasses must implement this method to set their light-speicfic parameters onto the OSPRay handle.
     * The Light base class will make sure to call ospCommit(handle) on the light handle.
     */
    virtual void commitLightSpecificParams() = 0;

private:
    Vector3f _color {1.f};
    float _intensity {1.};
    bool _visible {true};
    OSPLight _handle {nullptr};
};
} // namespace brayns
