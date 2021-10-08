/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

namespace brayns
{
FrameBuffer::FrameBuffer(const std::string& name, const Vector2ui& frameSize,
                         const FrameBufferFormat frameBufferFormat)
    : _name(name)
    , _frameSize(frameSize)
    , _frameBufferFormat(frameBufferFormat)
{
}

size_t FrameBuffer::getColorDepth() const
{
    switch (_frameBufferFormat)
    {
    case FrameBufferFormat::rgba_i8:
    case FrameBufferFormat::bgra_i8:
    case FrameBufferFormat::rgb_f32:
        return 4;
    case FrameBufferFormat::rgb_i8:
        return 3;
    default:
        return 0;
    }
}

freeimage::ImagePtr FrameBuffer::getImage()
{
#ifdef BRAYNS_USE_FREEIMAGE
    map();
    const auto colorBuffer = getColorBuffer();
    const auto& size = getSize();

    freeimage::ImagePtr image(
        FreeImage_ConvertFromRawBits(const_cast<uint8_t*>(colorBuffer), size.x,
                                     size.y, getColorDepth() * size.x,
                                     8 * getColorDepth(), 0xFF0000, 0x00FF00,
                                     0x0000FF, false));

    unmap();

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
    freeimage::SwapRedBlue32(image.get());
#endif
    return image;
#else
    return nullptr;
#endif
}
} // namespace brayns
