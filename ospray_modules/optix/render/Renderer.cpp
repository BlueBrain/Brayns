/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *
 * This file is part of https://github.com/BlueBrain/ospray-modules
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

#include "Renderer.h"

#include <ospray/SDK/common/Data.h>
#include <ospray/SDK/render/LoadBalancer.h>
// WIP: header is not part of the public SDK
//#include <ospray/mpiCommon/MPICommon.h>

#include "../Context.h"
#include "Material.h"

namespace bbp
{
namespace optix
{
void Renderer::commit()
{
    ospray::Renderer::commit();

    _updateVolume();

    _updateTransferFunction();

    Context::get().updateLights((ospray::Data*)getParamData("lights"));

    _context["timestamp"]->setFloat(getParam1f("timestamp", 0.f));

    _context["max_depth"]->setUint(10);
    _context["radiance_ray_type"]->setUint(0);
    _context["shadow_ray_type"]->setUint(1);
    _context["scene_epsilon"]->setFloat(epsilon);

    _context["shadows"]->setFloat(getParam1f("shadows", 0.f));
    _context["soft_shadows"]->setFloat(getParam1f("softShadows", 0.f));
    _context["ambient_occlusion_strength"]->setFloat(
        getParam1f("aoWeight", 0.f));

    _context["ambient_light_color"]->setFloat(bgColor.x, bgColor.y, bgColor.z);
    _context["bg_color"]->setFloat(bgColor.x, bgColor.y, bgColor.z);

    _context["volumeSamplesPerRay"]->setUint(
        getParam1i("volumeSamplesPerRay", 128));
}

Renderer::Renderer()
{
    _context = Context::get().getOptixContext();

    // in MPI mode we have no framebuffer, but we'll create one in renderTile().
    // to keep optix happy until then, we create a output buffer here
    _context["output_buffer"]->set(
        _context->createBuffer(RT_BUFFER_OUTPUT, RT_FORMAT_UNSIGNED_BYTE4, 0,
                               0));
    _context["accum_buffer"]->set(
        _context->createBuffer(RT_BUFFER_INPUT_OUTPUT | RT_BUFFER_GPU_LOCAL,
                               RT_FORMAT_FLOAT4, 0, 0));
}

Renderer::~Renderer()
{
    if (_frameBuffer)
        _frameBuffer->refDec();
    _frameBuffer = nullptr;

    if (_colorMapBuffer)
        _colorMapBuffer->destroy();
    _colorMapBuffer = nullptr;

    if (_emissionIntensityMapBuffer)
        _emissionIntensityMapBuffer->destroy();
    _emissionIntensityMapBuffer = nullptr;
}

void Renderer::renderTile(void* /*perFrameData*/, ospray::Tile& /*tile*/,
                          size_t /*jobID*/) const
{
    // should never come here
}

void Renderer::_updateVolume()
{
    ospray::Ref<ospray::Data> volumeData = getParamData("volumeData");
    const ospray::vec3i volumeDimensions =
        getParam3i("volumeDimensions", ospray::vec3i(0));
    const ospray::vec3f volumeElementSpacing =
        getParam3f("volumeElementSpacing", ospray::vec3f(1.f));
    const ospray::vec3f volumeOffset =
        getParam3f("volumeOffset", ospray::vec3f(0.f));
    const float volumeEpsilon = getParam1f("volumeEpsilon", 1.f);
    const auto diag = ospray::vec3f(volumeDimensions) * volumeElementSpacing;
    const float volumeDiag = ospray::reduce_max(diag);

    if (_volumeBuffer)
        _volumeBuffer->destroy();

    if (volumeData && volumeData->numBytes > 0)
    {
        _volumeBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE,
                                   volumeData->numItems);

        memcpy(_volumeBuffer->map(), volumeData->data, volumeData->numBytes);
        _volumeBuffer->unmap();
    }
    else
        _volumeBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE, 0);

    _context["volumeData"]->setBuffer(_volumeBuffer);
    _context["volumeDimensions"]->setUint(volumeDimensions.x,
                                          volumeDimensions.y,
                                          volumeDimensions.z);
    _context["volumeOffset"]->setFloat(volumeOffset.x, volumeOffset.y,
                                       volumeOffset.z);
    _context["volumeElementSpacing"]->setFloat(volumeElementSpacing.x,
                                               volumeElementSpacing.y,
                                               volumeElementSpacing.z);
    _context["volumeEpsilon"]->setFloat(volumeEpsilon);
    _context["volumeDiag"]->setFloat(volumeDiag);
}

void Renderer::_updateTransferFunction()
{
    if (_colorMapBuffer)
        _colorMapBuffer->destroy();
    _colorMapBuffer = nullptr;

    if (_emissionIntensityMapBuffer)
        _emissionIntensityMapBuffer->destroy();
    _emissionIntensityMapBuffer = nullptr;

    _transferFunctionDiffuseData = getParamData("transferFunctionDiffuseData");
    if (_transferFunctionDiffuseData)
        _colorMapBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT4,
                                   _transferFunctionDiffuseData->numItems);
    else
        _colorMapBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT4, 0);

    if (_transferFunctionDiffuseData &&
        _transferFunctionDiffuseData->numBytes != 0)
    {
        memcpy(_colorMapBuffer->map(), _transferFunctionDiffuseData->data,
               _transferFunctionDiffuseData->numBytes);
        _colorMapBuffer->unmap();
    }

    _context["colorMap"]->setBuffer(_colorMapBuffer);

    _transferFunctionEmissionData =
        getParamData("transferFunctionEmissionData");
    if (_transferFunctionEmissionData)
        _emissionIntensityMapBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3,
                                   _transferFunctionEmissionData->numItems);
    else
        _emissionIntensityMapBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, 0);

    if (_transferFunctionEmissionData &&
        _transferFunctionEmissionData->numBytes != 0)
    {
        memcpy(_emissionIntensityMapBuffer->map(),
               _transferFunctionEmissionData->data,
               _transferFunctionEmissionData->numBytes);
        _emissionIntensityMapBuffer->unmap();

        _context["colorMapSize"]->setUint(
            _transferFunctionDiffuseData->numItems);
    }

    _context["emissionIntensityMap"]->setBuffer(_emissionIntensityMapBuffer);

    _context["colorMapMinValue"]->setFloat(
        getParam1f("transferFunctionMinValue", 0.f));
    _context["colorMapRange"]->setFloat(
        getParam1f("transferFunctionRange", 0.f));
}

float Renderer::_mpiRenderFrame(ospray::FrameBuffer* fb,
                                const ospray::uint32 channelFlags)
{
    if (ospray::TiledLoadBalancer::instance->toString() ==
        "ospray::mpi::staticLoadBalancer::Master")
        return ospray::TiledLoadBalancer::instance->renderFrame(this, fb,
                                                                channelFlags);

    void* perFrameData = beginFrame(fb);
    int xSize = fb->size.x / 1; // mpicommon::numWorkers();
    xSize += xSize % TILE_SIZE;
    int xOffset = 0 /*mpicommon::workerRank()*/ * xSize;

    const ospray::region2i region{{xOffset, 0},
                                  {std::min(fb->size.x, xOffset + xSize),
                                   fb->size.y}};
    // std::cout << "#" << mpicommon::workerRank() << " " << region <<
    // std::endl;

    if (_frameBuffer && _frameBuffer->size != region.size())
    {
        _frameBuffer->refDec();
        _frameBuffer = nullptr;
    }

    if (!_frameBuffer)
    {
        _frameBuffer =
            new FrameBuffer(region.size(), OSP_FB_RGBA8, false, true, false);
        _frameBuffer->refInc();
    }
    _frameBuffer->clear(OSP_FB_COLOR | OSP_FB_ACCUM);

    // render 2D sub-region
    {
        // XXX
        _context["offset"]->setFloat(region.lower.x / 32.f,
                                     region.lower.y / 32.f);
        _context->launch(0, region.size().x, region.size().y);
    }

    // decompose 2D region into tiles for dfb
    {
        auto buffer = _frameBuffer->mapColorBuffer();
        uint8_t* colorBuffer = (uint8_t*)buffer;

        const int32_t xTiles = (region.size().x + TILE_SIZE - 1) / TILE_SIZE;
        const int32_t yTiles = (region.size().y + TILE_SIZE - 1) / TILE_SIZE;

        for (int32_t y = 0; y < yTiles; ++y)
        {
            // create tiles
            std::vector<ospray::Tile> tiles;
            tiles.reserve(xTiles);

            for (int32_t x = 0; x < xTiles; ++x)
            {
                const ospray::vec2i tileId(xOffset / TILE_SIZE + x, y);
                const ospray::int32 accumID = fb->accumID(tileId);

                tiles.emplace_back(ospray::Tile{tileId, fb->size, accumID});
            }

            // TODO tasking::parallel_for(NTASKS, [&](int taskIndex) {}
            for (int32_t idx = 0; idx < TILE_SIZE * xTiles; ++idx)
            {
                ospray::Tile& tile = tiles[idx % xTiles];

                const int32_t j = idx / xTiles;
#pragma omp simd
                for (int i = 0; i < TILE_SIZE; ++i)
                {
                    tile.r[j * TILE_SIZE + i] = colorBuffer[i * 4 + 0] / 255.f;
                    tile.g[j * TILE_SIZE + i] = colorBuffer[i * 4 + 1] / 255.f;
                    tile.b[j * TILE_SIZE + i] = colorBuffer[i * 4 + 2] / 255.f;
                    tile.a[j * TILE_SIZE + i] = colorBuffer[i * 4 + 3] / 255.f;
                }
                colorBuffer += TILE_SIZE * 4;
            }

            // send tiles
            for (auto& tile : tiles)
                fb->setTile(tile);
        }

        _frameBuffer->unmap(buffer);
    }

    // dfb->waitUntilFinished();
    endFrame(perFrameData, channelFlags);

    return fb->endFrame(ospray::inf);
}

void* Renderer::beginFrame(ospray::FrameBuffer* fb)
{
    this->currentFB = fb;
    fb->beginFrame();

    // Provide a random seed to the renderer
    ::optix::float4 jitter = {(float)rand() / (float)RAND_MAX,
                              (float)rand() / (float)RAND_MAX,
                              (float)rand() / (float)RAND_MAX,
                              (float)rand() / (float)RAND_MAX};
    _context["jitter4"]->setFloat(jitter);

    return nullptr;
}

void Renderer::endFrame(void* /*perFrameData*/,
                        const ospray::int32 /*fbChannelFlags*/)
{
}

float Renderer::renderFrame(ospray::FrameBuffer* fb,
                            const ospray::uint32 channelFlags)
{
    // device.getParamString("mpiMode", "")
    if (ospray::TiledLoadBalancer::instance)
        return _mpiRenderFrame(fb, channelFlags);

    // in the local case, we need no external loadbalancer, hence we call
    // beginFrame() and endFrame() ourselves
    auto perFrameData = beginFrame(fb);

    // Render
    auto lock = Context::get().getScopeLock();
    _context->launch(0, fb->size.x, fb->size.y);

    endFrame(perFrameData, channelFlags);
    return fb->endFrame(errorThreshold);
}
} // namespace optix
} // namespace bbp
