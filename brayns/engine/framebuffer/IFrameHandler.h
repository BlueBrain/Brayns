/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "FramebufferChannel.h"
#include "PixelFormat.h"

#include <brayns/utils/MathTypes.h>
#include <brayns/utils/image/Image.h>

#include <ospray/ospray_cpp/FrameBuffer.h>

namespace brayns
{
class IFrameHandler
{
public:
    IFrameHandler() = default;

    IFrameHandler(const IFrameHandler &) = delete;
    IFrameHandler &operator=(const IFrameHandler &) = delete;

    IFrameHandler(IFrameHandler &&) = delete;
    IFrameHandler &operator=(IFrameHandler &&) = delete;

    virtual ~IFrameHandler() = default;

    /**
     * @copydoc Framebuffer::commit()
     */
    virtual bool commit() = 0;

    /**
     * @copydoc Framebuffer::setFrameSize(const Vector2ui &)
     */
    virtual void setFrameSize(const Vector2ui &frameSize) = 0;

    /**
     * @copydoc Framebuffer::setAccumulation(bool)
     */
    virtual void setAccumulation(bool accumulation) noexcept = 0;

    /**
     * @copydoc Framebuffer::setFormat(PixelFormat)
     */
    virtual void setFormat(PixelFormat frameBufferFormat) noexcept = 0;

    /**
     * @copydoc Framebuffer::setChannels(const std::vector<FramebufferChannel> &)
     */
    virtual void setChannels(const std::vector<FramebufferChannel> &channels) noexcept = 0;

    /**
     * @copydoc Framebuffer::setToneMappingEnabled(bool)
     */
    virtual void setToneMappingEnabled(bool enabled) noexcept = 0;

    /**
     * @copydoc Framebuffer::clear()
     */
    virtual void clear() noexcept = 0;

    /**
     * @copydoc Framebuffer::incrementAccumFrames()
     */
    virtual void incrementAccumFrames() noexcept = 0;

    /**
     * @copydoc Framebuffer::getAccumulationFrameCount()
     */
    virtual size_t getAccumulationFrameCount() const noexcept = 0;

    /**
     * @copydoc Framebuffer::hasNewAccumulationFrame()
     */
    virtual bool hasNewAccumulationFrame() const noexcept = 0;

    /**
     * @copydoc Framebuffer::resetNewAccumulationFrame()
     */
    virtual void resetNewAccumulationFrame() noexcept = 0;

    /**
     * @copydoc Framebuffer::getImage()
     */
    virtual Image getImage(FramebufferChannel channel) = 0;

    /**
     * @copydoc Framebuffer::getHandle()
     */
    virtual const ospray::cpp::FrameBuffer &getHandle() const noexcept = 0;
};
} // namespace brayns
