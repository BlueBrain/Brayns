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

#include <pbrtv2/core/integrator.h>
#include <pbrtv2/core/volume.h>
#include <pbrtv2/renderers/samplerrenderer.h>

#include <unordered_map>

namespace brayns
{
class PBRTRenderer;

using PBRTSurfaceIntegratorFactory =
    std::function<pbrt::SurfaceIntegrator*(PBRTRenderer&)>;
using PBRTVolumeIntegratorFactory =
    std::function<pbrt::VolumeIntegrator*(PBRTRenderer&)>;
using PBRTSamplerFactory = std::function<pbrt::Sampler*(PBRTRenderer&)>;

class PBRTRenderer : public Renderer
{
public:
    PBRTRenderer(const AnimationParameters&, const RenderingParameters&);
    ~PBRTRenderer();

    void render(FrameBufferPtr frameBuffer) final;

    void manualCommit(const Vector2i& res);
    void commit() final;

    void setCamera(CameraPtr camera) final;

    PBRTCamera* getPBRTCamera() noexcept { return _camera; }

private:
    std::unordered_map<std::string, PBRTSurfaceIntegratorFactory>
        _surfaceIntegratorFactory;
    std::unordered_map<std::string, PBRTVolumeIntegratorFactory>
        _volumeIntegratorFactory;
    std::unordered_map<std::string, PBRTSamplerFactory> _samplerFactory;

    PBRTCamera* _camera;
    Vector2i _currentRes{16, 16};

    std::string _currentSurfIntegrator;
    std::string _currentVolIntegrator;
    std::string _currentSampler;

    mutable std::shared_ptr<pbrt::SamplerRenderer> _renderer{nullptr};
};
} // namespace brayns

#endif
