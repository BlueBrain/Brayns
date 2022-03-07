/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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
 * @brief The Renderer class is the base class for all renderer to be available on Brayns
 */
class Renderer : public EngineObject
{
public:
    using Ptr = std::unique_ptr<Renderer>;

    Renderer() = default;

    Renderer(const Renderer&);
    Renderer &operator=(const Renderer&);

    Renderer(Renderer&&) = default;
    Renderer &operator=(Renderer&&) = default;

    virtual ~Renderer();

    /**
     * @brief Returns the number of samples per pixel that this renderer will perform to render a complete frame
     */
    int32_t getSamplesPerPixel() const noexcept;

    /**
     * @brief Returns the max ray bounces (max recursion depth) for each ray launched from the camera.
     */
    int32_t getMaxRayBounces() const noexcept;

    /**
     * @brief Returns the background color of this renderer as normalized RGB. The background color is used when a ray
     * does not intersect anything in the scene (miss)
     */
    const Vector4f &getBackgroundColor() const noexcept;

    /**
     * @brief Sets the number of samples per pixel that this renderer will perform to render a complete frame
     */
    void setSamplesPerPixel(const int32_t spp) noexcept;

    /**
     * @brief Sets the max ray bounces (max recursion depth) for each ray launched from the camera.
     */
    void setMaxRayBounces(const int32_t maxBounces) noexcept;

    /**
     * @brief Sets the background color of this renderer as normalized RGB. The background color is used when a ray
     * does not intersect anything in the scene (miss)
     */
    void setBackgroundColor(const Vector4f& background) noexcept;

    /**
     * @brief Returns the OSPRay handle of this renderer
     */
    OSPRenderer handle() const noexcept;

    /**
     * @brief commit() implementation
     */
    void commit() final;

    /**
     * @brief Subclasses must implement this to create a copy of themselves
     */
    virtual Ptr clone() const noexcept = 0;

    /**
     * @brief Returns the renderer type as a string
     */
    virtual std::string_view getName() const noexcept = 0;

protected:
    /**
     * @brief Subclasses must implement this method to return the appropiate OSPRay renderer name to be instantiated.
     */
    virtual std::string_view getOSPHandleName() const noexcept = 0;

    /**
     * @brief Subclasses must implement this method to commit their renderer-specific data to the OSPRay counterpart.
     * The base class will make sure to call ospCommit(handle) on the renderer handle, so subclasses should avoid it.
     */
    virtual void commitRendererSpecificParams() = 0;

private:
    int32_t _samplesPerPixel {1};
    int32_t _maxRayBounces {5};
    Vector4f _backgroundColor {0.004f, 0.016f, 0.102f, 0.f}; // Default background color is BBP dark blue
    OSPRenderer _handle {nullptr};
};
} // namespace brayns
