/* Copyright (c) 2020, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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
#ifndef PBRTRENDERER_H
#define PBRTRENDERER_H

#include "PBRTCamera.h"

#include <brayns/engine/Renderer.h>

#include <pbrt/core/integrator.h>

#include <unordered_map>

namespace brayns
{
class PBRTRenderer;

using PBRTRenderFactory = std::function<std::shared_ptr<pbrt::Integrator>(PBRTRenderer&)>;
using PBRTSamplerFactory = std::function<std::shared_ptr<pbrt::Sampler>(PBRTRenderer&)>;

class PBRTRenderer : public Renderer
{
public:
    PBRTRenderer(const AnimationParameters&,
                 const RenderingParameters&);
    ~PBRTRenderer();

    void render(FrameBufferPtr frameBuffer) final;
    void commit() final;
    void setCamera(CameraPtr camera) final;

    PBRTCamera* getPBRTCamera() { return _camera; }

    std::shared_ptr<pbrt::Sampler> getSampler() { return _pbrtSampler; }

private:
    std::unordered_map<std::string, PBRTRenderFactory> _renderFactory;
    std::unordered_map<std::string, PBRTSamplerFactory> _samplerFactory;

    PBRTCamera* _camera;

    std::string _currentRenderer;
    std::string _currentSampler;

    mutable std::shared_ptr<pbrt::Integrator> _pbrtRenderer {nullptr};
    mutable std::shared_ptr<pbrt::Sampler> _pbrtSampler {nullptr};
};
}

#endif
