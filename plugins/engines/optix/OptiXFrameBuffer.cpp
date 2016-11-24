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

#include "OptiXFrameBuffer.h"

#include <brayns/common/log.h>

#include <optixu/optixu_math_namespace.h>

namespace brayns
{

OptiXFrameBuffer::OptiXFrameBuffer(
    const Vector2ui& frameSize,
    FrameBufferFormat colorDepth,
    bool accumulation,
    optix::Context& context )
    : FrameBuffer( frameSize, colorDepth, accumulation )
    , _frameBuffer( 0 )
    , _accumBuffer( 0 )
    , _context( context )
    , _colorBuffer( 0 )
    , _depthBuffer( 0 )
    , _accumulationFrame( 0 )
    , _imageData( 0 )
{
    resize(frameSize);
}

OptiXFrameBuffer::~OptiXFrameBuffer()
{
    unmap();
    if( _frameBuffer ) _frameBuffer->destroy();
    if( _accumBuffer ) _accumBuffer->destroy();
}

void OptiXFrameBuffer::resize(
    const Vector2ui& frameSize )
{
    _frameSize = frameSize;

    unmap();

    RTformat format;
    switch( _frameBufferFormat )
    {
        case FBF_RGBA_I8:
            format = RT_FORMAT_UNSIGNED_BYTE4;
            break;
        case FBF_RGBA_F32:
            format = RT_FORMAT_FLOAT4;
            break;
        default:
            format = RT_FORMAT_UNKNOWN;
    }

    _frameBuffer = _context->createBuffer(
        RT_BUFFER_OUTPUT,
        format,
        _frameSize.x(), _frameSize.y( ));
    _context["output_buffer"]->set( _frameBuffer );

    _accumBuffer = _context->createBuffer(
        RT_BUFFER_INPUT_OUTPUT | RT_BUFFER_GPU_LOCAL,
        RT_FORMAT_FLOAT4,
        _frameSize.x(), _frameSize.y() );
    _context["accum_buffer"]->set( _accumBuffer );

    clear();
}

void OptiXFrameBuffer::clear()
{
    _accumulationFrame = 0;
}

void OptiXFrameBuffer::map()
{
    // Now unmap the buffer
    if( !_frameBuffer )
        return;

    rtBufferMap( _frameBuffer->get(), &_imageData );

    _context["frame"]->setUint( _accumulationFrame++ );
    switch( _frameBufferFormat )
    {
        case FBF_RGBA_I8:
            _colorBuffer = (uint8_t*)(_imageData);
            break;
        case FBF_RGBA_F32:
            _depthBuffer = (float*)_imageData;
            break;
        default:
            BRAYNS_ERROR << "Unsupported format" << std::endl;
    }
}

void OptiXFrameBuffer::unmap()
{
    // Now unmap the buffer
    if( !_frameBuffer )
        return;

    rtBufferUnmap( _frameBuffer->get( ) );
    _colorBuffer = nullptr;
    _depthBuffer = nullptr;
}

}
