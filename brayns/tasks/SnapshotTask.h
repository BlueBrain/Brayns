/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/Log.h>
#include <brayns/common/tasks/Task.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/utils/StringUtils.h>
#include <brayns/utils/image/Image.h>
#include <brayns/utils/image/ImageEncoder.h>
#include <brayns/utils/image/ImageFormat.h>
#include <brayns/utils/image/ImageMerger.h>

#include <fstream>

namespace brayns
{
struct SnapshotParams
{
    std::unique_ptr<AnimationParameters> animParams;
    std::unique_ptr<VolumeParameters> volumeParams;
    std::unique_ptr<RenderingParameters> renderingParams;
    std::unique_ptr<Camera> camera;
    int samplesPerPixel{1};
    Vector2ui size;
    size_t quality{100};
    std::string format;
    std::string name;
    std::string filePath;
};

/**
 * A functor for snapshot rendering and conversion to a base64-encoded image for
 * the web client.
 */
class SnapshotFunctor : public TaskFunctor
{
public:
    SnapshotFunctor(Engine& engine, SnapshotParams params)
        : _params(std::move(params))
        , _camera(engine.createCamera())
        , _engine(engine)
    {
        if (_params.animParams == nullptr)
        {
            _params.animParams = std::make_unique<AnimationParameters>(
                engine.getParametersManager().getAnimationParameters());
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

        _scene = engine.createScene(*_params.animParams, *_params.volumeParams);

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

    std::string operator()()
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
        const auto names = isStereo ? std::vector<std::string>{"0L", "0R"}
                                    : std::vector<std::string>{"default"};
        std::vector<FrameBufferPtr> frameBuffers;
        for (const auto& name : names)
            frameBuffers.push_back(
                _engine.createFrameBuffer(name, _params.size,
                                          PixelFormat::RGBA_I8));

        while (frameBuffers[0]->numAccumFrames() !=
               size_t(_params.samplesPerPixel))
        {
            for (const auto& frameBuffer : frameBuffers)
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

        if (!_params.filePath.empty() && frameBuffers.size() == 1)
        {
            auto& fb = *frameBuffers[0];
            _writeToDisk(fb);
            return {};
        }
        else
        {
            std::vector<Image> images;
            images.reserve(frameBuffers.size());
            for (const auto& frameBuffer : frameBuffers)
            {
                images.push_back(frameBuffer->getImage());
            }
            try
            {
                auto image = ImageMerger::merge(images);
                auto format = ImageFormat::fromExtension(_params.format);
                auto quality = _params.quality;
                return {ImageEncoder::encodeToBase64(image, format, quality)};
            }
            catch (const std::runtime_error& e)
            {
                Log::error("{}", e.what());
            }
        }
    }

private:
    void _writeToDisk(FrameBuffer& fb)
    {
        auto image = fb.getImage();
        auto filename = _params.filePath + "." + _params.format;
        auto quality = _params.quality;
        try
        {
            ImageEncoder::save(image, filename, quality);
        }
        catch (const std::runtime_error& e)
        {
            Log::error("{}", e.what());
        }
    }

    SnapshotParams _params;
    FrameBufferPtr _frameBuffer;
    CameraPtr _camera;
    RendererPtr _renderer;
    ScenePtr _scene;
    Engine& _engine;
};
} // namespace brayns
