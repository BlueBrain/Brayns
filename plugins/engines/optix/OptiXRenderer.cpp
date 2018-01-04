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

#include "OptiXRenderer.h"
#include "OptiXFrameBuffer.h"
#include <chrono>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

namespace
{
const std::string CUDA_TIMESTAMP = "timestamp";
const std::string CUDA_MAX_DEPTH = "max_depth";
const std::string CUDA_RADIANCE_RAY_TYPE = "radiance_ray_type";
const std::string CUDA_SHADOW_RAY_TYPE = "shadow_ray_type";
const std::string CUDA_SCENE_EPSILON = "scene_epsilon";
const std::string CUDA_DIFFUSE_SHADING = "shading_enabled";
const std::string CUDA_ELECTRON_SHADING = "electron_shading_enabled";
const std::string CUDA_SHADOWS = "shadows";
const std::string CUDA_SOFT_SHADOWS = "soft_shadows";
const std::string CUDA_AMBIENT_OCCLUSION_STRENGTH =
    "ambient_occlusion_strength";
const std::string CUDA_AMBIENT_LIGHT_COLOR = "ambient_light_color";
const std::string CUDA_BACKGROUND_COLOR = "bg_color";
const std::string CUDA_VOLUME_SAMPLES_PER_RAY = "volumeSamplesPerRay";
const std::string CUDA_JITTER = "jitter4";
}

namespace brayns
{
OptiXRenderer::OptiXRenderer(const std::string& /*name*/,
                             ParametersManager& parametersMamager,
                             optix::Context& context)
    : Renderer(parametersMamager)
    , _context(context)
{
}

void OptiXRenderer::render(FrameBufferPtr frameBuffer)
{
    // Provide a random seed to the renderer
    optix::float4 jitter = {(float)rand() / (float)RAND_MAX,
                            (float)rand() / (float)RAND_MAX,
                            (float)rand() / (float)RAND_MAX,
                            (float)rand() / (float)RAND_MAX};
    _context[CUDA_JITTER]->setFloat(jitter);

    // Render
    const Vector2ui& size = frameBuffer->getSize();
    _context->launch(0, size.x(), size.y());

    if (!frameBuffer->getAccumulation())
        return;

    // no way to determine yet if accumulation produces new images or not
    _hasNewImage = true;
}

void OptiXRenderer::commit()
{
    const auto& sp = _parametersManager.getSceneParameters();

    _context[CUDA_TIMESTAMP]->setFloat(sp.getAnimationFrame());

    const auto& rp = _parametersManager.getRenderingParameters();

    _context[CUDA_MAX_DEPTH]->setUint(10u);
    _context[CUDA_RADIANCE_RAY_TYPE]->setUint(0u);
    _context[CUDA_SHADOW_RAY_TYPE]->setUint(1u);
    _context[CUDA_SCENE_EPSILON]->setFloat(rp.getEpsilon());

    auto mt = rp.getShading();
    _context[CUDA_DIFFUSE_SHADING]->setUint(mt == ShadingType::diffuse);
    _context[CUDA_ELECTRON_SHADING]->setUint(mt == ShadingType::electron);
    _context[CUDA_SHADOWS]->setFloat(rp.getShadows());
    _context[CUDA_SOFT_SHADOWS]->setFloat(rp.getSoftShadows());
    _context[CUDA_AMBIENT_OCCLUSION_STRENGTH]->setFloat(
        rp.getAmbientOcclusionStrength());

    auto color = rp.getBackgroundColor();
    _context[CUDA_AMBIENT_LIGHT_COLOR]->setFloat(color.x(), color.y(),
                                                 color.z());
    _context[CUDA_BACKGROUND_COLOR]->setFloat(color.x(), color.y(), color.z());

    const auto& vp = _parametersManager.getVolumeParameters();
    _context[CUDA_VOLUME_SAMPLES_PER_RAY]->setUint(vp.getSamplesPerRay());
}

void OptiXRenderer::setCamera(CameraPtr camera)
{
    OptiXCamera* optixCamera = dynamic_cast<OptiXCamera*>(camera.get());
    assert(optixCamera);
    _context->setRayGenerationProgram(0, optixCamera->impl());
}
}
