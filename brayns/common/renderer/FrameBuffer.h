/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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
#include <brayns/common/BaseObject.h>
#include <brayns/common/types.h>

namespace brayns
{
class FrameBuffer : public BaseObject
{
public:
    BRAYNS_API FrameBuffer(const std::string& name, const Vector2ui& frameSize,
                           FrameBufferFormat frameBufferFormat);
    virtual ~FrameBuffer() {}
    virtual void clear() { _accumFrames = 0; }
    virtual void map() = 0;
    virtual void unmap() = 0;

    virtual const uint8_t* getColorBuffer() const = 0;
    virtual size_t getColorDepth() const;
    virtual const float* getDepthBuffer() const = 0;

    virtual void resize(const Vector2ui& frameSize) = 0;

    virtual Vector2ui getSize() const { return _frameSize; }
    const Vector2ui& getFrameSize() const { return _frameSize; }
    virtual void setAccumulation(const bool accumulation)
    {
        _accumulation = accumulation;
    }
    bool getAccumulation() const { return _accumulation; }
    FrameBufferFormat getFrameBufferFormat() const
    {
        return _frameBufferFormat;
    }
    const std::string& getName() const { return _name; }
    void incrementAccumFrames() { ++_accumFrames; }
    size_t numAccumFrames() const { return _accumFrames; }
    virtual void setSubsampling(const size_t) {}
protected:
    const std::string _name;
    Vector2ui _frameSize;
    const FrameBufferFormat _frameBufferFormat;
    bool _accumulation{true};
    std::atomic_size_t _accumFrames{0};
};
}
#endif // FRAMEBUFFER_H
