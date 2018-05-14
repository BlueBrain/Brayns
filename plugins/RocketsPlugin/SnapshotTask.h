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
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>

#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
struct SnapshotParams
{
    std::unique_ptr<AnimationParameters> animParams;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<RenderingParameters> renderingParams;
    int samplesPerPixel{1};
    Vector2ui size;
    size_t quality{100};
    std::string format; // ImageMagick formats apply
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
                    ImageGenerator& imageGenerator)
        : _params(std::move(params))
        , _frameBuffer(engine.createFrameBuffer(_params.size,
                                                FrameBufferFormat::rgba_i8,
                                                true))
        , _camera(engine.createCamera(_params.camera
                                          ? _params.camera->getType()
                                          : engine.getCamera().getType()))
        , _renderer(engine.createRenderer(
              engine.getActiveRenderer(),
              _params.animParams
                  ? *_params.animParams
                  : engine.getParametersManager().getAnimationParameters(),
              _params.renderingParams
                  ? *_params.renderingParams
                  : engine.getParametersManager().getRenderingParameters()))
        , _scene(engine.createScene({_renderer}, engine.getParametersManager()))
        , _imageGenerator(imageGenerator)
    {
        if (_params.camera)
            *_camera = *_params.camera;
        else
            *_camera = engine.getCamera();

        *_scene = engine.getScene();
    }

    ImageGenerator::ImageBase64 operator()()
    {
        _camera->setAspectRatio(float(_params.size.x()) / _params.size.y());
        _camera->commit();

        _scene->commitLights();
        _scene->commit();

        if (_params.renderingParams)
            _params.renderingParams->setSamplesPerPixel(1);

        _renderer->setCamera(_camera);
        _renderer->setScene(_scene);
        _renderer->commit();

        std::stringstream msg;
        msg << "Render snapshot";
        if (!_params.name.empty())
            msg << " " << shortenString(_params.name);
        msg << " ...";

        while (_frameBuffer->numAccumFrames() !=
               size_t(_params.samplesPerPixel))
        {
            _renderer->render(_frameBuffer);
            progress(msg.str(), 1.f / _frameBuffer->numAccumFrames(),
                     float(_frameBuffer->numAccumFrames()) /
                         _params.samplesPerPixel);
        }

        _scene->reset();

        return _imageGenerator.createImage(*_frameBuffer, _params.format,
                                           _params.quality);
    }

private:
    SnapshotParams _params;
    FrameBufferPtr _frameBuffer;
    CameraPtr _camera;
    RendererPtr _renderer;
    ScenePtr _scene;
    ImageGenerator& _imageGenerator;
};
}
