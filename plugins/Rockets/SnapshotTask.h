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
#include <brayns/common/utils/stringUtils.h>
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
                    ImageGenerator& imageGenerator)
        : _params(std::move(params))
        , _camera(engine.createCamera())
        , _imageGenerator(imageGenerator)
        , _engine(engine)
    {
        if (_params.animParams == nullptr)
        {
            _params.animParams = std::make_unique<AnimationParameters>(
                engine.getParametersManager().getAnimationParameters());
        }

        if (_params.geometryParams == nullptr)
        {
            _params.geometryParams = std::make_unique<GeometryParameters>(
                engine.getParametersManager().getGeometryParameters());
        }

        if (_params.renderingParams == nullptr)
        {
            _params.renderingParams = std::make_unique<RenderingParameters>(
                engine.getParametersManager().getRenderingParameters());
        }

        if (_params.volumeParams == nullptr)
        {
            _params.volumeParams = std::make_unique<VolumeParameters>(
                engine.getParametersManager().getVolumeParameters());
        }

        _scene =
            engine.createScene(*_params.animParams, *_params.geometryParams,
                               *_params.volumeParams);

        _renderer = engine.createRenderer(*_params.animParams,
                                          *_params.renderingParams);

        const auto& renderer = engine.getRenderer();
        _renderer->setCurrentType(renderer.getCurrentType());
        _renderer->clonePropertiesFrom(renderer);
        if (_params.camera)
        {
            *_camera = *_params.camera;
            _camera->setCurrentType(engine.getCamera().getCurrentType());
            _camera->clonePropertiesFrom(engine.getCamera());
        }
        else
            *_camera = engine.getCamera();

        _scene->copyFrom(engine.getScene());
    }

    ImageGenerator::ImageBase64 operator()()
    {
        _scene->commit();

        _camera->updateProperty("aspect",
                                double(_params.size.x) / _params.size.y);
        _camera->commit();

        if (_params.renderingParams)
        {
            _params.renderingParams->setSamplesPerPixel(1);
            _params.renderingParams->setSubsampling(1);
        }

        _renderer->setCamera(_camera);
        _renderer->setScene(_scene);
        _renderer->commit();

        std::stringstream msg;
        msg << "Render snapshot";
        if (!_params.name.empty())
            msg << " " << string_utils::shortenString(_params.name);
        msg << " ...";

        const auto isStereo = _camera->hasProperty("stereo") &&
                              _camera->getProperty<bool>("stereo");
        const auto names = isStereo ? strings{"0L", "0R"} : strings{"default"};
        std::vector<FrameBufferPtr> frameBuffers;
        for (const auto& name : names)
            frameBuffers.push_back(
                _engine.createFrameBuffer(name, _params.size,
                                          FrameBufferFormat::rgba_i8));

        while (frameBuffers[0]->numAccumFrames() !=
               size_t(_params.samplesPerPixel))
        {
            for (auto frameBuffer : frameBuffers)
            {
                _camera->setBufferTarget(frameBuffer->getName());
                _camera->markModified(false);
                _camera->commit();
                _camera->resetModified();
                _renderer->render(frameBuffer);
                frameBuffer->incrementAccumFrames();
            }

            progress(msg.str(), 1.f / frameBuffers[0]->numAccumFrames(),
                     float(frameBuffers[0]->numAccumFrames()) /
                         _params.samplesPerPixel);
        }

        return _imageGenerator.createImage(frameBuffers, _params.format,
                                           _params.quality);
    }

private:
    SnapshotParams _params;
    FrameBufferPtr _frameBuffer;
    CameraPtr _camera;
    RendererPtr _renderer;
    ScenePtr _scene;
    ImageGenerator& _imageGenerator;
    Engine& _engine;
};
} // namespace brayns
