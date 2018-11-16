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

#include "DeflectPixelOp.h"

#include <ospray/SDK/fb/FrameBuffer.h>

namespace
{
template <typename T>
std::future<T> make_ready_future(const T value)
{
    std::promise<T> promise;
    promise.set_value(value);
    return promise.get_future();
}

#pragma omp declare simd
inline unsigned char clampCvt(float f)
{
    if (f < 0.f)
        f = 0.f;
    if (f > 1.f)
        f = 1.f;
    return f * 255.f;
}

const size_t ALIGNMENT = 64;
}

namespace bbp
{
DeflectPixelOp::Instance::Instance(ospray::FrameBuffer* fb_,
                                   DeflectPixelOp& parent)
    : _parent(parent)
{
    fb = fb_;
}

DeflectPixelOp::Instance::~Instance() = default;

void DeflectPixelOp::Instance::beginFrame()
{
    if (!_parent._deflectStream)
        return;
    const size_t numTiles = fb->getTotalTiles();
    if (_pixels.size() < numTiles)
    {
        _pixels.resize(numTiles);

        for (auto& i : _pixels)
        {
            if (i)
                continue;

            i.reset((unsigned char*)_mm_malloc(TILE_SIZE * TILE_SIZE * 4,
                                               ALIGNMENT));
            memset(i.get(), 255, TILE_SIZE * TILE_SIZE * 4);
        }
    }
}

void DeflectPixelOp::Instance::endFrame()
{
    if (!_parent._deflectStream)
        return;

    auto sharedFuture = _parent._deflectStream->finishFrame().share();
    for (auto& i : _parent._finishFutures)
        i.second = sharedFuture;
}

void DeflectPixelOp::Instance::postAccum(ospray::Tile& tile)
{
    if (!_parent._deflectStream)
        return;

    unsigned int x = tile.region.lower.x;
    bool isRightEye = false;
    if (_parent._settings.stereo)
    {
        unsigned int widthHalf = fb->getNumPixels().x / 2;
        isRightEye = x >= widthHalf;
        if (isRightEye)
            x -= widthHalf;
    }

    const auto& fbSize = fb->getNumPixels();
    ospray::vec2i tileSize{TILE_SIZE, TILE_SIZE};

    if (tile.region.lower.x + TILE_SIZE >= fbSize.x)
        tileSize.x = fbSize.x % TILE_SIZE;
    if (tile.region.lower.y + TILE_SIZE >= fbSize.y)
        tileSize.y = fbSize.y % TILE_SIZE;

    deflect::ImageWrapper image(_copyPixels(tile, tileSize), tileSize.x,
                                tileSize.y, deflect::RGBA, x,
                                tile.region.lower.y);
    image.compressionPolicy = _parent._settings.compression
                                  ? deflect::COMPRESSION_ON
                                  : deflect::COMPRESSION_OFF;
    image.compressionQuality = _parent._settings.quality;
    image.subsampling = deflect::ChromaSubsampling::YUV420;
    image.rowOrder = deflect::RowOrder::bottom_up;
    if (_parent._settings.stereo)
        image.view =
            isRightEye ? deflect::View::right_eye : deflect::View::left_eye;

    try
    {
        auto i = _parent._finishFutures.find(pthread_self());
        if (i == _parent._finishFutures.end())
        {
            // only for the first frame
            std::lock_guard<std::mutex> _lock(_parent._mutex);
            _parent._finishFutures.insert(
                {pthread_self(), make_ready_future(true)});
        }
        else
            i->second.wait(); // complete previous frame

        _parent._deflectStream->send(image).get();
    }
    catch (const std::exception& exc)
    {
        std::cerr << "Encountered error during sendImage: " << exc.what()
                  << std::endl;
    }
}

unsigned char* DeflectPixelOp::Instance::_copyPixels(
    ospray::Tile& tile, const ospray::vec2i& tileSize)
{
    const size_t tileID =
        tile.region.lower.y / TILE_SIZE * fb->getNumTiles().x +
        tile.region.lower.x / TILE_SIZE;

#ifdef __GNUC__
    unsigned char* __restrict__ pixels =
        (unsigned char*)__builtin_assume_aligned(_pixels[tileID].get(),
                                                 ALIGNMENT);
#else
    unsigned char* __restrict__ pixels = _pixels[tileID].get();
#endif
    float* __restrict__ red = tile.r;
    float* __restrict__ green = tile.g;
    float* __restrict__ blue = tile.b;

    if (tileSize.x < TILE_SIZE)
    {
        int index = 0;
        for (int i = 0; i < tileSize.y; ++i)
        {
            for (int j = 0; j < tileSize.x; ++j)
            {
                pixels[index + 0] = clampCvt(red[i * TILE_SIZE + j]);
                pixels[index + 1] = clampCvt(green[i * TILE_SIZE + j]);
                pixels[index + 2] = clampCvt(blue[i * TILE_SIZE + j]);
                index += 4;
            }
        }
        return pixels;
    }

// clang-format off
    // AVX vectorization with icc 17 reports (create with -qopt-report=5)
    //LOOP BEGIN at ../plugins/engines/ospray/modules/deflect/DeflectPixelOp.cpp(130,16)
    //   remark #15388: vectorization support: reference red[i] has aligned access
    //   remark #15388: vectorization support: reference green[i] has aligned access
    //   remark #15388: vectorization support: reference blue[i] has aligned access
    //   remark #15329: vectorization support: non-unit strided store was emulated for the variable <U137_V[i*4]>, stride is 4
    //   remark #15329: vectorization support: non-unit strided store was emulated for the variable <U137_V[i*4+1]>, stride is 4
    //   remark #15329: vectorization support: non-unit strided store was emulated for the variable <U137_V[i*4+2]>, stride is 4
    //   remark #15305: vectorization support: vector length 32
    //   remark #15301: OpenMP SIMD LOOP WAS VECTORIZED
    //   remark #15448: unmasked aligned unit stride loads: 3
    //   remark #15453: unmasked strided stores: 3
    //   remark #15475: --- begin vector cost summary ---
    //   remark #15476: scalar cost: 56
    //   remark #15477: vector cost: 9.460
    //   remark #15478: estimated potential speedup: 5.910
    //   remark #15487: type converts: 3
    //   remark #15488: --- end vector cost summary ---
    //   remark #25015: Estimate of max trip count of loop=128
    //LOOP END
// clang-format on
#ifdef __INTEL_COMPILER
#pragma vector aligned
#endif
#pragma omp simd
    for (int i = 0; i < TILE_SIZE * TILE_SIZE; ++i)
    {
        pixels[i * 4 + 0] = clampCvt(red[i]);
        pixels[i * 4 + 1] = clampCvt(green[i]);
        pixels[i * 4 + 2] = clampCvt(blue[i]);
    }
    return pixels;
}

void DeflectPixelOp::commit()
{
    const std::string hostname = getParamString("hostname", "");
    const std::string id = getParamString("id", "");
    const bool changed = _settings.id != id || _settings.hostname != hostname;

    _settings.id = id;
    _settings.hostname = hostname;
    _settings.port = getParam1i("port", deflect::Stream::defaultPortNumber);
    _settings.compression = getParam1i("compression", 1);
    _settings.stereo = getParam1i("stereo", 0);
    _settings.quality = getParam1i("quality", 80);
    _settings.streamEnabled = getParam1i("enabled", 1);

    if (changed || (_deflectStream && _deflectStream->isConnected() &&
                    !_settings.streamEnabled))
    {
        finish();
        _deflectStream.reset();
    }

    if (!_deflectStream && _settings.streamEnabled)
    {
        try
        {
            _deflectStream.reset(new deflect::Stream(_settings.id,
                                                     _settings.hostname,
                                                     _settings.port));
        }
        catch (const std::runtime_error& ex)
        {
            std::cerr << "Deflect failed to initialize. " << ex.what()
                      << std::endl;
            _settings.streamEnabled = false;
        }
    }
}

ospray::PixelOp::Instance* DeflectPixelOp::createInstance(
    ospray::FrameBuffer* fb, PixelOp::Instance* /*prev*/)
{
    return new Instance(fb, *this);
}

void DeflectPixelOp::finish()
{
    for (auto& i : _finishFutures)
        i.second.wait();
    _finishFutures.clear();
}

} // namespace bbp

namespace ospray
{
extern "C" void ospray_init_module_deflect()
{
    std::cout << "#deflect: initializing ospray deflect plugin" << std::endl;
}
OSP_REGISTER_PIXEL_OP(bbp::DeflectPixelOp, DeflectPixelOp);
}
