/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/engine/Engine.h>

namespace brayns
{
/**
 * OSPRay implementation of the ray-tracing engine.
 */
class OSPRayEngine : public Engine
{
public:
    OSPRayEngine(ParametersManager& parametersManager);

    ~OSPRayEngine();

    /** @copydoc Engine::commit */
    void commit() final;

    /** @copydoc Engine::getMinimumFrameSize */
    Vector2ui getMinimumFrameSize() const final;

    FrameBufferPtr createFrameBuffer(const std::string& name,
                                     const Vector2ui& frameSize,
                                     PixelFormat frameBufferFormat) const final;

    ScenePtr createScene(AnimationParameters& animationParameters,
                         VolumeParameters& volumeParameters) const final;
    CameraPtr createCamera() const final;
    RendererPtr createRenderer(
        const AnimationParameters& animationParameters,
        const RenderingParameters& renderingParameters) const final;

private:
    void _createCameras();
    void _createRenderers();

    bool _useDynamicLoadBalancer{false};
};
} // namespace brayns
