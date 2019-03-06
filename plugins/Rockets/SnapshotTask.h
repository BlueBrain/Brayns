/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include <brayns/common/tasks/Task.h>

#include "ImageGenerator.h"
#include <brayns/common/utils/utils.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>

#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
struct SnapshotParams
{
    std::unique_ptr<AnimationParameters> animParams;
    std::unique_ptr<GeometryParameters> geometryParams;
    std::unique_ptr<VolumeParameters> volumeParams;
    std::unique_ptr<RenderingParameters> renderingParams;
    std::unique_ptr<Camera> camera;
    int samplesPerPixel{1};
    Vector2ui size;
    size_t quality{100};
    std::string format; // FreeImage formats apply
    std::string name;
};

/**
 * A functor for snapshot rendering and conversion to a base64-encoded image for
 * the web client.
 */
class SnapshotFunctor : public TaskFunctor
{
public:
    SnapshotFunctor(Engine& engine, SnapshotParams&& params,
                    ImageGenerator& imageGenerator);

    ImageGenerator::ImageBase64 operator()();

private:
    ImageGenerator::ImageBase64 _takeSnapshotConcurrent();
    ImageGenerator::ImageBase64 _takeSnapshotSerial();
    ImageGenerator::ImageBase64 _renderSnapshot(const size_t numAccumulations);

    SnapshotParams _params;
    FrameBufferPtr _frameBuffer;
    CameraPtr _camera;
    RendererPtr _renderer;
    ScenePtr _scene;
    ImageGenerator& _imageGenerator;
    Engine& _engine;
};
} // namespace brayns
