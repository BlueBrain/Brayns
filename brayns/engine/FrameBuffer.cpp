/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "FrameBuffer.h"

#include <brayns/common/Log.h>

#include <brayns/utils/image/ImageFlipper.h>

namespace brayns
{
FrameBuffer::FrameBuffer(const std::string& name, const Vector2ui& frameSize,
                         const PixelFormat frameBufferFormat)
    : _name(name)
    , _frameSize(frameSize)
    , _frameBufferFormat(frameBufferFormat)
{
}

size_t FrameBuffer::getColorDepth() const
{
    switch (_frameBufferFormat)
    {
    case PixelFormat::RGBA_I8:
    case PixelFormat::RGB_F32:
        return 4;
    case PixelFormat::RGB_I8:
        return 3;
    default:
        return 0;
    }
}

Image FrameBuffer::getImage()
{
    map();

    const auto colorBuffer = getColorBuffer();
    const auto& size = getSize();

    ImageInfo info;

    info.width = size.x;
    info.height = size.y;

    switch (_frameBufferFormat)
    {
    case PixelFormat::RGB_I8:
    case PixelFormat::RGB_F32:
        info.channelCount = 3;
        break;
    case PixelFormat::RGBA_I8:
        info.channelCount = 4;
        break;
    default:
        Log::warn("Invalid framebuffer format: {}.", int(_frameBufferFormat));
        return {};
    }

    switch (_frameBufferFormat)
    {
    case PixelFormat::RGB_I8:
    case PixelFormat::RGB_F32:
    case PixelFormat::RGBA_I8:
        info.channelSize = 1;
        break;
    default:
        Log::warn("Invalid framebuffer format: {}.", int(_frameBufferFormat));
        return {};
    }

    auto data = reinterpret_cast<const char*>(colorBuffer);
    auto length = info.getSize();
    Image image(info, {data, length});
    ImageFlipper::flipVertically(image);

    unmap();

    return image;
}
} // namespace brayns
