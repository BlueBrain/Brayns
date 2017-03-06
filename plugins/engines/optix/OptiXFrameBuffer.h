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

#ifndef OPTIXFRAMEBUFFER_H
#define OPTIXFRAMEBUFFER_H

#include <brayns/common/renderer/FrameBuffer.h>
#include <optixu/optixpp_namespace.h>

namespace brayns
{
/**
   OptiX specific frame buffer

   This object is the OptiX specific implementation of a frame buffer
*/
class OptiXFrameBuffer : public brayns::FrameBuffer
{
public:
    OptiXFrameBuffer(const Vector2ui& frameSize, FrameBufferFormat colorDepth,
                     bool accumulation, optix::Context& context);

    ~OptiXFrameBuffer();

    void clear() final;

    void resize(const Vector2ui& frameSize) final;

    void map() final;

    void unmap() final;

    uint8_t* getColorBuffer() final { return _colorBuffer; }
    float* getDepthBuffer() final { return 0; }
private:
    optix::Buffer _frameBuffer;
    optix::Buffer _accumBuffer;
    optix::Context& _context;
    uint8_t* _colorBuffer;
    float* _depthBuffer;
    uint16_t _accumulationFrame;
    void* _imageData;
};
}

#endif // OPTIXFRAMEBUFFER_H
