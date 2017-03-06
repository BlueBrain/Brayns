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

#include "OptiXRenderer.h"
#include "OptiXFrameBuffer.h"
#include <chrono>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

namespace brayns
{
OptiXRenderer::OptiXRenderer(const std::string& /*name*/,
                             ParametersManager& parametersMamager,
                             optix::Context& context)
    : Renderer(parametersMamager)
    , _context(context)
    , _frame(0)
{
}

void OptiXRenderer::render(FrameBufferPtr frameBuffer)
{
    // Provide a random seed to the renderer
    optix::float4 jitter = {(float)rand() / (float)RAND_MAX,
                            (float)rand() / (float)RAND_MAX,
                            (float)rand() / (float)RAND_MAX,
                            (float)rand() / (float)RAND_MAX};
    _context["jitter4"]->setFloat(jitter);

    // Render
    const Vector2ui& size = frameBuffer->getSize();
    _context->launch(0, size.x(), size.y());

    ++_frame;
}

void OptiXRenderer::commit()
{
    SceneParameters& sp = _parametersManager.getSceneParameters();

    _context["timestamp"]->setFloat(sp.getTimestamp());

    RenderingParameters& rp = _parametersManager.getRenderingParameters();

    _context["max_depth"]->setUint(10);
    _context["radiance_ray_type"]->setUint(0);
    _context["shadow_ray_type"]->setUint(1);
    _context["scene_epsilon"]->setFloat(rp.getEpsilon());

    ShadingType mt = rp.getShading();
    _context["shading_enabled"]->setUint(mt == ShadingType::diffuse);
    _context["electron_shading_enabled"]->setUint(mt == ShadingType::electron);
    _context["shadows_enabled"]->setUint(rp.getShadows());
    _context["soft_shadows_enabled"]->setUint(rp.getSoftShadows());
    _context["ambient_occlusion_strength"]->setFloat(
        rp.getAmbientOcclusionStrength());

    Vector3f color = rp.getBackgroundColor();
    _context["ambient_light_color"]->setFloat(color.x(), color.y(), color.z());
    _context["bg_color"]->setFloat(color.x(), color.y(), color.z());
    _frame = 0;
}

void OptiXRenderer::setCamera(CameraPtr camera)
{
    OptiXCamera* optixCamera = dynamic_cast<OptiXCamera*>(camera.get());
    assert(optixCamera);
    _context->setRayGenerationProgram(0, optixCamera->impl());
}
}
