/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
