/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef OSPRAYFRAMEBUFFER_H
#define OSPRAYFRAMEBUFFER_H

#include <brayns/common/renderer/FrameBuffer.h>
#include <ospray.h>

namespace brayns
{

class OSPRayFrameBuffer : public brayns::FrameBuffer
{
public:
    OSPRayFrameBuffer(const Vector2i& frameSize, FrameBufferFormat colorDepth);
    ~OSPRayFrameBuffer();

    void clear() final;
    void resize(const Vector2i& frameSize) final;
    void map() final;
    void unmap() final;

    uint8_t* getColorBuffer() final { return _colorBuffer; }
    float* getDepthBuffer() final { return _depthBuffer; }

    OSPFrameBuffer impl() { return _frameBuffer; }

private:
    OSPFrameBuffer _frameBuffer;
    uint8_t* _colorBuffer;
    float* _depthBuffer;
};

}
#endif // OSPRAYFRAMEBUFFER_H
