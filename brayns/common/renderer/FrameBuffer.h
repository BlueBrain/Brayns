/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <brayns/api.h>
#include <brayns/common/types.h>

namespace brayns
{

enum FrameBufferFormat
{
  FBF_RGBA_NONE,
  FBF_RGBA_I8,
  FBF_RGB_I8,
  FBF_RGBA_F32,
};

class FrameBuffer
{
public:
    BRAYNS_API FrameBuffer(const Vector2i& frameSize, FrameBufferFormat frameBufferFormat);
    BRAYNS_API virtual ~FrameBuffer() {}

    BRAYNS_API virtual void clear() = 0;
    BRAYNS_API virtual void map() = 0;
    BRAYNS_API virtual void unmap() = 0;

    BRAYNS_API virtual uint8_t* getColorBuffer() = 0;
    BRAYNS_API virtual size_t getColorDepth();
    BRAYNS_API virtual float* getDepthBuffer() = 0;

    BRAYNS_API virtual void resize(const Vector2i& frameSize) = 0;

    BRAYNS_API Vector2i getSize() const { return _frameSize; }

protected:
    Vector2i _frameSize;
    FrameBufferFormat _frameBufferFormat;
};

}
#endif // FRAMEBUFFER_H
