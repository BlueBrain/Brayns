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

#pragma once

#include <brayns/api.h>
#include <brayns/common/BaseObject.h>
#include <brayns/common/types.h>
#include <brayns/common/utils/imageUtils.h>

namespace brayns
{
class FrameBuffer : public BaseObject
{
public:
    /** @name API for engine-specific code */
    //@{
    /** Map the buffer for reading with get*Buffer(). */
    virtual void map() = 0;
    /** Unmap the buffer for reading with get*Buffer(). */
    virtual void unmap() = 0;
    /** @return the color buffer, valid only after map(). */
    virtual const uint8_t* getColorBuffer() const = 0;
    /** @return the depth buffer, valid only after map(). */
    virtual const float* getDepthBuffer() const = 0;
    /** Resize the framebuffer to the new size. */
    virtual void resize(const Vector2ui& frameSize) = 0;
    /** Clear the framebuffer. */
    virtual void clear() { _accumFrames = 0; }
    /** @return the current framebuffer size. */
    virtual Vector2ui getSize() const { return _frameSize; }
    /** Enable/disable accumulation state on the framebuffer. */
    virtual void setAccumulation(const bool accumulation)
    {
        _accumulation = accumulation;
    }
    /** Set a new framebuffer format. */
    virtual void setFormat(FrameBufferFormat frameBufferFormat)
    {
        _frameBufferFormat = frameBufferFormat;
    }

    /** Set a new subsampling with a factor from 1 to x of the current size. */
    virtual void setSubsampling(const size_t) {}
    /** Create and set a pixelop (pre/post filter) on the framebuffer. */
    virtual void createPixelOp(const std::string& /*name*/){};
    /** Update the current pixelop with the given properties. */
    virtual void updatePixelOp(const PropertyMap& /*properties*/){};
    //@}

    BRAYNS_API FrameBuffer(const std::string& name, const Vector2ui& frameSize,
                           FrameBufferFormat frameBufferFormat);

    size_t getColorDepth() const;
    const Vector2ui& getFrameSize() const { return _frameSize; }
    bool getAccumulation() const { return _accumulation; }
    FrameBufferFormat getFrameBufferFormat() const
    {
        return _frameBufferFormat;
    }
    const std::string& getName() const { return _name; }
    void incrementAccumFrames() { ++_accumFrames; }
    size_t numAccumFrames() const { return _accumFrames; }
    freeimage::ImagePtr getImage();

protected:
    const std::string _name;
    Vector2ui _frameSize;
    FrameBufferFormat _frameBufferFormat;
    bool _accumulation{true};
    std::atomic_size_t _accumFrames{0};
};
} // namespace brayns
