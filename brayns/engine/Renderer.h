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

#include <brayns/common/BaseObject.h>
#include <brayns/common/MathTypes.h>

#include <ospray/ospray_cpp/Renderer.h>

#include <memory>
#include <string_view>

namespace brayns
{
/**
 * @brief The Renderer class is the base class for all renderer to be available on Brayns
 */
class Renderer : public BaseObject
{
public:
    Renderer(const std::string &handleID);

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    Renderer(Renderer &&) = delete;
    Renderer &operator=(Renderer &&) = delete;

    virtual ~Renderer() = default;

    /**
     * @brief Returns the renderer type as a string
     */
    virtual std::string getName() const noexcept = 0;

    /**
     * @brief Creates a copy of the renderer object
     *
     * @return std::unique_ptr<Renderer>
     */
    virtual std::unique_ptr<Renderer> clone() const noexcept = 0;

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
    void setBackgroundColor(const Vector4f &background) noexcept;

    /**
     * @brief Returns the OSPRay handle of this renderer
     */
    const ospray::cpp::Renderer &getOsprayRenderer() const noexcept;

    /**
     * @brief commit() implementation
     * @returns true if there was anything to commit
     */
    bool commit();

protected:
    /**
     * @brief Subclasses must implement this method to commit their renderer-specific data to the OSPRay counterpart.
     * The base class will make sure to call ospCommit(handle) on the renderer handle, so subclasses should avoid it.
     */
    virtual void commitRendererSpecificParams() = 0;

private:
    int32_t _samplesPerPixel{1};
    int32_t _maxRayBounces{3};
    Vector4f _backgroundColor{0.004f, 0.016f, 0.102f, 0.f}; // Default background color is BBP dark blue
    ospray::cpp::Renderer _osprayRenderer;
};
} // namespace brayns
