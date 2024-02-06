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
#include "IFrameHandler.h"
#include "PixelFormat.h"

#include <brayns/utils/MathTypes.h>

#include <memory>

namespace brayns
{
class Framebuffer
{
public:
    /**
     * @brief Construct a new Framebuffer object with the given frame handler. Passing a null frame handler will lead
     * to undefined behaviour.
     * @param frame The frame handler to use.
     */
    explicit Framebuffer(std::unique_ptr<IFrameHandler> frame);

    /**
     * @brief Sets the frame handler the framebuffer will use. Passing a null frame handler will lead to undefined
     * behaviour.
     * @param frame The frame handler to user.
     */
    void setFrameHandler(std::unique_ptr<IFrameHandler> frame);

    /**
     * @brief Syncs this object data to the Ospray backend framebuffer.
     * @returns true if there was anything to commit.
     */
    bool commit();

    /**
     * @brief Sets the frame dimensions (width x height).
     */
    void setFrameSize(const Vector2ui &frameSize) noexcept;

    /**
     * @brief Enables or disables rendering accumulation.
     * @param accumulation True to enable it, false to disable it.
     */
    void setAccumulation(bool accumulation) noexcept;

    /**
     * @brief Sets the framebuffer pixel format.
     * @param frameBufferFormat Format to set.
     */
    void setFormat(PixelFormat frameBufferFormat) noexcept;

    /**
     * @brief Set the channels this framebuffer will store.
     * @param channels List of channels to store.
     */
    void setChannels(const std::vector<FramebufferChannel> &channels) noexcept;

    /**
     * @brief Enables or disables the usage of tone mapping when rendering on this framebuffer.
     *
     * @param enabled
     */
    void setToneMappingEnabled(bool enabled) noexcept;

    /**
     * @brief Resets the accumulation frames to 0.
     */
    void clear() noexcept;

    /**
     * @brief Increments the number of accumulation frames currently integrated into this framebuffer.
     * If not in accumulation mode, the action has no effect.
     */
    void incrementAccumFrames() noexcept;

    /**
     * @brief Returns the number of accumulation frames integrated into this framebuffer. If
     * not in accumulation mode, the result has no meaning.
     */
    size_t getAccumulationFrameCount() const noexcept;

    /**
     * @brief Return true if a new frame was rendered since the last reset.
     *
     * @return true A new frame was rendred into the framebuffer since the last reset
     * @return false No new frames were rendered
     */
    bool hasNewAccumulationFrame() const noexcept;

    /**
     * @brief Sets the new accumulation frame flag to false.
     */
    void resetNewAccumulationFrame() noexcept;

    /**
     * @brief Returns an Image object with the current contents of the specified channel.
     * @brief channel The channel to retrieve as an Image.
     * @return Image The image object.
     */
    Image getImage(FramebufferChannel channel = FramebufferChannel::Color);

    /**
     * @brief Returns the Ospray backend framebuffer handle object.
     * @returns const ospray::cpp::FrameBuffer
     */
    const ospray::cpp::FrameBuffer &getHandle() const noexcept;

private:
    std::unique_ptr<IFrameHandler> _frame;
};
}
