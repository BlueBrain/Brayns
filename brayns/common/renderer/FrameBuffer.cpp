/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "FrameBuffer.h"

namespace brayns
{

FrameBuffer::FrameBuffer(const Vector2i& frameSize, const FrameBufferFormat frameBufferFormat)
    : _frameSize(frameSize)
    , _frameBufferFormat(frameBufferFormat)
{
}

size_t FrameBuffer::getColorDepth()
{
    switch(_frameBufferFormat)
    {
        case FBF_RGBA_I8:
        case FBF_RGBA_F32:
            return 4;
        case FBF_RGB_I8:
            return 3;
        default:
            return 0;
    }
}

}
