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

#include <brayns/engineapi/Engine.h>

namespace brayns
{
/**
 * OptiX implementation of the ray-tracing engine.
 */
class OptiXEngine : public Engine
{
public:
    OptiXEngine(ParametersManager& parametersManager);

    ~OptiXEngine();

    /** @copydoc Engine::commit */
    void commit() final;

    /** @copydoc Engine::getMinimumFrameSize */
    Vector2ui getMinimumFrameSize() const final;

    FrameBufferPtr createFrameBuffer(
        const std::string& name, const Vector2ui& frameSize,
        FrameBufferFormat frameBufferFormat) const final;
    ScenePtr createScene(AnimationParameters& animationParameters,
                         GeometryParameters& geometryParameters,
                         VolumeParameters& volumeParameters) const final;
    CameraPtr createCamera() const final;
    RendererPtr createRenderer(
        const AnimationParameters& animationParameters,
        const RenderingParameters& renderingParameters) const final;

private:
    void _initializeContext();
    void _createRenderers();
    void _createCameras();
};
} // namespace brayns
