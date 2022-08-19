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

#include <brayns/common/MathTypes.h>
#include <brayns/common/ModifiedFlag.h>
#include <brayns/common/PixelFormat.h>
#include <brayns/utils/image/Image.h>

#include <ospray/ospray_cpp/FrameBuffer.h>

namespace brayns
{
/**
 * @brief The Framebuffer class is the object that handles the frames rendered by a given renderer
 */
class Framebuffer
{
public:
    Framebuffer() = default;

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(const Framebuffer &) = delete;

    Framebuffer(Framebuffer &&) = delete;
    Framebuffer &operator=(Framebuffer &&) = delete;

    /**
     * @brief Maps Ospray backend framebuffer to an accessible system buffer
     */
    void map();

    /**
     * @brief Removes Ospray backend framebuffer mapping to system buffer
     */
    void unmap();

    /**
     * @brief Returns the Ospray mapped framebuffer as a system buffer. To be valid, a call to map() must be
     * made before calling this method. After calling unmap() the pointer returned by this method is invalidated
     */
    const uint8_t *getColorBuffer() const;

    /**
     * @brief Syncs this object data to the Ospray backend framebuffer
     * @returns true if there was anything to commit
     */
    bool commit();

    /**
     * @brief Sets the frame dimensions (width x height)
     */
    void setFrameSize(const Vector2ui &frameSize);

    /**
     * @brief Returns the current frame dimensions (width x height)
     */
    const Vector2ui &getFrameSize() const noexcept;

    /**
     * @brief Enables or disables accumulation. Accumulation is a proccess in which a frame is integrated
     * over multiple calls render calls, rendering 1 sample per pixel on each call (It allows for more
     * interactivity as it does not block the main thread).
     */
    void setAccumulation(const bool accumulation) noexcept;

    /**
     * @brief Returns wether this framebuffer is integrating on accumulation mode or not
     */
    bool isAccumulating() const noexcept;

    /**
     * @brief Sets the framebuffer pixel format
     */
    void setFormat(PixelFormat frameBufferFormat) noexcept;

    /**
     * @brief Returns the framebuffer current pixel format
     */
    PixelFormat getFrameBufferFormat() const noexcept;

    /**
     * @brief If on accumulation mode, it resets the accumulation to 0
     */
    void clear() noexcept;

    /**
     * @brief Increments the number of accumulation frames currently integrated into this framebuffer.
     * If not in accumulation mode, the action has no effect.
     */
    void incrementAccumFrames() noexcept;

    /**
     * @brief Returns the number of accumulation frames currently integrated into this framebuffer. If
     * not in accumulation mode, the result has not value.
     */
    int32_t numAccumFrames() const noexcept;

    /**
     * @brief Return true if a new frame was rendered since the last reset
     *
     * @return true A new frame was rendred into the framebuffer since the last reset
     * @return false No new frames was rendered
     */
    bool hasNewAccumulationFrame() const noexcept;

    /**
     * @brief Sets the new accumulation frame flag to false
     */
    void resetNewAccumulationFrame() noexcept;

    /**
     * @brief Returns an Image object with the current contents of the framebuffer
     */
    Image getImage();

    /**
     * @brief Returns the Ospray backend framebuffer handle object
     */
    const ospray::cpp::FrameBuffer &getHandle() const noexcept;

private:
    Vector2ui _frameSize{800u, 600u};
    PixelFormat _frameBufferFormat{PixelFormat::StandardRgbaI8};
    int32_t _accumFrames{0};
    bool _accumulation = true;
    bool _newAccumulationFrame = false;
    ospray::cpp::FrameBuffer _handle;
    uint8_t *_colorBuffer = nullptr;
    ModifiedFlag _flag;
};
} // namespace brayns
