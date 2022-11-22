/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/utils/image/Image.h>

#include <ospray/ospray_cpp/FrameBuffer.h>

namespace brayns
{
/**
 * @brief Pixel storage format in framebuffer.
 *
 */
enum class PixelFormat
{
    /**
     * @brief 4 channels of 8 bits each with lineal color curve.
     *
     */
    RgbaI8,

    /**
     * @brief 4 channels of 8 bits each with non-lineal color curve.
     *
     */
    StandardRgbaI8,

    /**
     * @brief 4 channels of 32 bits each.
     *
     */
    RgbaF32
};

class IFramebufferType
{
public:
    IFramebufferType() = default;

    IFramebufferType(const IFramebufferType &) = delete;
    IFramebufferType &operator=(const IFramebufferType &) = delete;

    IFramebufferType(IFramebufferType &&) = delete;
    IFramebufferType &operator=(IFramebufferType &&) = delete;

    virtual ~IFramebufferType() = default;

    /**
     * @brief Maps Ospray backend framebuffer to an accessible system buffer
     */
    virtual void map() = 0;

    /**
     * @brief Removes Ospray backend framebuffer mapping to system buffer
     */
    virtual void unmap() = 0;

    /**
     * @brief Returns the Ospray mapped framebuffer as a system buffer. To be valid, a call to map() must be
     * made before calling this method. After calling unmap() the pointer returned by this method is invalidated
     */
    virtual const uint8_t *getColorBuffer() const = 0;

    /**
     * @brief Syncs this object data to the Ospray backend framebuffer
     * @returns true if there was anything to commit
     */
    virtual bool commit() = 0;

    /**
     * @brief Sets the frame dimensions (width x height)
     */
    virtual void setFrameSize(const Vector2ui &frameSize) = 0;

    /**
     * @brief Returns the current frame dimensions (width x height)
     */
    virtual const Vector2ui &getFrameSize() const noexcept = 0;

    /**
     * @brief Returns the frame aspect ratio (width / height)
     * @return float
     */
    virtual float getAspectRatio() const noexcept = 0;

    /**
     * @brief Enables or disables accumulation. Accumulation is a proccess in which a frame is integrated
     * over multiple calls render calls, rendering 1 sample per pixel on each call (It allows for more
     * interactivity as it does not block the main thread).
     */
    virtual void setAccumulation(const bool accumulation) noexcept = 0;

    /**
     * @brief Returns wether this framebuffer is integrating on accumulation mode or not
     */
    virtual bool isAccumulating() const noexcept = 0;

    /**
     * @brief Sets the framebuffer pixel format
     */
    virtual void setFormat(PixelFormat frameBufferFormat) noexcept = 0;

    /**
     * @brief Returns the framebuffer current pixel format
     */
    virtual PixelFormat getFrameBufferFormat() const noexcept = 0;

    /**
     * @brief If on accumulation mode, it resets the accumulation to 0
     */
    virtual void clear() noexcept = 0;

    /**
     * @brief Increments the number of accumulation frames currently integrated into this framebuffer.
     * If not in accumulation mode, the action has no effect.
     */
    virtual void incrementAccumFrames() noexcept = 0;

    /**
     * @brief Returns the number of accumulation frames currently integrated into this framebuffer. If
     * not in accumulation mode, the result has not value.
     */
    virtual int32_t numAccumFrames() const noexcept = 0;

    /**
     * @brief Return true if a new frame was rendered since the last reset
     *
     * @return true A new frame was rendred into the framebuffer since the last reset
     * @return false No new frames was rendered
     */
    virtual bool hasNewAccumulationFrame() const noexcept = 0;

    /**
     * @brief Sets the new accumulation frame flag to false
     */
    virtual void resetNewAccumulationFrame() noexcept = 0;

    /**
     * @brief Returns an Image object with the current contents of the framebuffer
     */
    virtual Image getImage() = 0;

    /**
     * @brief Returns the Ospray backend framebuffer handle object
     */
    virtual const ospray::cpp::FrameBuffer &getHandle() const noexcept = 0;
};
} // namespace brayns
