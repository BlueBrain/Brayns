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

#include "Framebuffer.h"

namespace brayns
{
Framebuffer::Framebuffer(std::unique_ptr<IFrameHandler> frame)
{
    setFrameHandler(std::move(frame));
}

void Framebuffer::setFrameHandler(std::unique_ptr<IFrameHandler> frame)
{
    assert(frame);
    _frame = std::move(frame);
}

bool Framebuffer::commit()
{
    return _frame->commit();
}

void Framebuffer::setFrameSize(const Vector2ui &frameSize) noexcept
{
    _frame->setFrameSize(frameSize);
}

void Framebuffer::setAccumulation(bool accumulation) noexcept
{
    _frame->setAccumulation(accumulation);
}

void Framebuffer::setFormat(PixelFormat format) noexcept
{
    _frame->setFormat(format);
}

void Framebuffer::setChannels(const std::vector<FramebufferChannel> &channels) noexcept
{
    _frame->setChannels(channels);
}

void Framebuffer::setToneMappingEnabled(bool enabled) noexcept
{
    _frame->setToneMappingEnabled(enabled);
}

void Framebuffer::clear() noexcept
{
    _frame->clear();
}

void Framebuffer::incrementAccumFrames() noexcept
{
    _frame->incrementAccumFrames();
}

size_t Framebuffer::getAccumulationFrameCount() const noexcept
{
    return _frame->getAccumulationFrameCount();
}

bool Framebuffer::hasNewAccumulationFrame() const noexcept
{
    return _frame->hasNewAccumulationFrame();
}

void Framebuffer::resetNewAccumulationFrame() noexcept
{
    _frame->resetNewAccumulationFrame();
}

Image Framebuffer::getImage(FramebufferChannel channel)
{
    return _frame->getImage(channel);
}

const ospray::cpp::FrameBuffer &Framebuffer::getHandle() const noexcept
{
    return _frame->getHandle();
}
}
