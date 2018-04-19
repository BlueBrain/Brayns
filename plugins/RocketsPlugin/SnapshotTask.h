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
#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/utils/Utils.h>

namespace brayns
{
struct SnapshotParams
{
    int samplesPerPixel{1};
    Vector2ui size;
    std::string format; // ImageMagick formats apply
    size_t quality{100};
    std::string name;
};

/**
 * A functor for snapshot rendering and conversion to a base64-encoded image for
 * the web client.
 */
class SnapshotFunctor : public TaskFunctor
{
public:
    SnapshotFunctor(Engine& engine, const SnapshotParams& params,
                    ImageGenerator& imageGenerator)
        : _frameBuffer(engine.createFrameBuffer(params.size,
                                                FrameBufferFormat::rgba_i8,
                                                true))
        , _camera(engine.createCamera(engine.getCamera().getType()))
        , _scene(engine.getScenePtr())
        , _renderer(engine.createRenderer(engine.getActiveRenderer()))
        , _params(params)
        , _imageGenerator(imageGenerator)
        , _dataLock(engine.dataMutex(), std::defer_lock)
    {
        *_camera = engine.getCamera();
        _camera->setAspectRatio(float(params.size.x()) / params.size.y());
        _camera->commit();

        _renderer->setCamera(_camera);
    }

    ImageGenerator::ImageBase64 operator()()
    {
        while (!_dataLock.try_lock_for(std::chrono::seconds(1)))
            progress("Waiting for scene access ...", 0.f, 0.f);

        _renderer->setScene(_scene);
        _renderer->commit();

        std::stringstream msg;
        msg << "Render snapshot ";
        if (_params.name.empty())
            msg << "...";
        else
            msg << shortenString(_params.name) << " ...";

        while (_frameBuffer->numAccumFrames() !=
               size_t(_params.samplesPerPixel))
        {
            _renderer->render(_frameBuffer);
            progress(msg.str(), 1.f / _frameBuffer->numAccumFrames(),
                     float(_frameBuffer->numAccumFrames()) /
                         _params.samplesPerPixel);
        }

        _dataLock.unlock();
        return _imageGenerator.createImage(*_frameBuffer, _params.format,
                                           _params.quality);
    }

private:
    FrameBufferPtr _frameBuffer;
    CameraPtr _camera;
    ScenePtr _scene;
    RendererPtr _renderer;
    SnapshotParams _params;
    ImageGenerator& _imageGenerator;
    std::shared_lock<std::shared_timed_mutex> _dataLock;
};

auto createSnapshotTask(const SnapshotParams& params, const uintptr_t,
                        Engine& engine, ImageGenerator& imageGenerator)
{
    using SnapshotTask = DeferredTask<ImageGenerator::ImageBase64>;
    return std::make_shared<SnapshotTask>(
        SnapshotFunctor{engine, params, imageGenerator});
}
}
