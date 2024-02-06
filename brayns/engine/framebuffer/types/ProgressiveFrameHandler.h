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

#include <brayns/engine/framebuffer/IFrameHandler.h>

#include "StaticFrameHandler.h"

namespace brayns
{
class ProgressiveFrameHandler final : public IFrameHandler
{
public:
    explicit ProgressiveFrameHandler(uint32_t scale = 4);

    bool commit() override;

    void setFrameSize(const Vector2ui &frameSize) override;
    void setAccumulation(bool accumulation) noexcept override;
    void setFormat(PixelFormat frameBufferFormat) noexcept override;
    void setChannels(const std::vector<brayns::FramebufferChannel> &channels) noexcept override;
    void setToneMappingEnabled(bool enabled) noexcept override;
    void clear() noexcept override;

    void incrementAccumFrames() noexcept override;
    size_t getAccumulationFrameCount() const noexcept override;
    bool hasNewAccumulationFrame() const noexcept override;
    void resetNewAccumulationFrame() noexcept override;

    Image getImage(brayns::FramebufferChannel channel) override;

    const ospray::cpp::FrameBuffer &getHandle() const noexcept override;

private:
    bool _lowResFrame = true;
    uint32_t _scale;
    StaticFrameHandler _lowRes;
    StaticFrameHandler _highRes;
};
}
