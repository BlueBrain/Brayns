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

    _context["shading_enabled"]->setUint(getParam1i("shadingEnabled", 1));
    _context["electron_shading_enabled"]->setUint(
        getParam1i("electronShading", 0));
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
    // in the local case, we need no external loadbalancer, hence we call
    // beginFrame() and endFrame() ourselves
    auto perFrameData = beginFrame(fb);

    // Render
    auto lock = Context::get().getScopeLock();
    _context->launch(0, fb->size.x, fb->size.y);

    endFrame(perFrameData, channelFlags);
    return fb->endFrame(errorThreshold);
}
}
}
