/* Copyright (c) 2017-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *
 * This file is part of https://github.com/BlueBrain/Brayns
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

#include "DeflectParameters.h"

#include <deflect/Stream.h>
#include <map>
#include <ospray/SDK/fb/PixelOp.h>

namespace brayns
{
/**
 * Implements an ospray pixel op that streams each tile to a Deflect server
 * instance. The tiles are compressed directly on the tile thread and then
 * enqueued for sending.
 *
 * The ospray module to load is called "deflect", and the pixel op name for
 * creating it is "DeflectPixelOp".
 */
class DeflectPixelOp : public ospray::PixelOp
{
public:
    struct Instance : public ospray::PixelOp::Instance
    {
        Instance(ospray::FrameBuffer* fb_, DeflectPixelOp& parent);
        ~Instance();

        void beginFrame() final;
        void endFrame() final;
        void postAccum(ospray::Tile& tile) final;
        std::string toString() const final { return "DeflectPixelOp"; }
        struct PixelsDeleter
        {
            void operator()(unsigned char* pixels) { _mm_free(pixels); }
        };
        using Pixels = std::unique_ptr<unsigned char, PixelsDeleter>;

    private:
        DeflectPixelOp& _parent;
        std::vector<Pixels> _pixels;

        unsigned char* _copyPixels(ospray::Tile& tile,
                                   const ospray::vec2i& tileSize);
    };

    /**
     * Updates the underlying deflect stream with the following parameters:
     * - "enabled" (param1i): 1 to enable streaming, 0 to disable streaming,
     *                        1 default
     * - "compression" (param1i): 1 to enable compression, 0 to send raw,
     *                            uncompressed pixels, 1 default
     * - "quality" (param1i): 1 (worst, smallest) - 100 (best, biggest) for JPEG
     *                        quality, 80 default
     */
    void commit() final;

    ospray::PixelOp::Instance* createInstance(ospray::FrameBuffer* fb,
                                              PixelOp::Instance* prev) final;

private:
    /** @internal finish pendings sends before closing the stream. */
    void _finish();

    std::unique_ptr<deflect::Stream> _deflectStream;
    std::map<pthread_t, std::shared_future<bool>> _finishFutures;
    std::mutex _mutex;
    DeflectParameters _params;
};
} // namespace brayns
