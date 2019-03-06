/* Copyright (c) 2015-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/common/tasks/Task.h>

#include "ImageGenerator.h"
#include <brayns/common/utils/utils.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>

#include <brayns/parameters/ParametersManager.h>

#include "SnapshotTask.h"

namespace brayns
{
SnapshotFunctor::SnapshotFunctor(Engine &engine, SnapshotParams &&params,
                                 ImageGenerator &imageGenerator)
    : _params(std::move(params))
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

    if (_engine.supportsConcurrentSnapshots())
    {
        _camera = engine.createCamera();
        _scene =
            engine.createScene(*_params.animParams, *_params.geometryParams,
                               *_params.volumeParams);

        _renderer = engine.createRenderer(*_params.animParams,
                                          *_params.renderingParams);

        const auto &renderer = engine.getRenderer();
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
    else
    {
        _camera = engine.getCameraPtr();
        _renderer = engine.getRendererPtr();
    }
}

ImageGenerator::ImageBase64 SnapshotFunctor::_renderSnapshot(
    const size_t numAccumulations)
{
    const auto nameShort =
        _params.name.empty() ? "" : " " + shortenString(_params.name);
    const auto msg = "Render snapshot" + nameShort + "...";

    const auto isStereo =
        _camera->hasProperty("stereo") && _camera->getProperty<bool>("stereo");
    const auto names = isStereo ? strings{"0L", "0R"} : strings{"default"};

    std::vector<FrameBufferPtr> frameBuffers;
    for (const auto &name : names)
    {
        frameBuffers.push_back(
            _engine.createFrameBuffer(name, _params.size,
                                      FrameBufferFormat::rgba_i8));
    }

    for (size_t i = 0; i < numAccumulations; i++)
    {
        for (auto &frameBuffer : frameBuffers)
        {
            _camera->setBufferTarget(frameBuffer->getName());
            _camera->markModified(false);
            _camera->commit();
            _camera->resetModified();
            _renderer->render(frameBuffer);
            frameBuffer->incrementAccumFrames();
        }

        progress(msg, 1.f / numAccumulations,
                 static_cast<float>(i) / numAccumulations);
    }

    return _imageGenerator.createImage(frameBuffers, _params.format,
                                       _params.quality);
}

ImageGenerator::ImageBase64 SnapshotFunctor::_takeSnapshotConcurrent()
{
    _scene->commit();

    _camera->updateProperty("aspect", double(_params.size.x) / _params.size.y);
    _camera->commit();

    if (_params.renderingParams)
    {
        _params.renderingParams->setSamplesPerPixel(1);
        _params.renderingParams->setSubsampling(1);
    }

    _scene->commitLights();

    _renderer->setCamera(_camera);
    _renderer->setScene(_scene);
    _renderer->commit();

    return _renderSnapshot(_params.samplesPerPixel);
}

ImageGenerator::ImageBase64 SnapshotFunctor::_takeSnapshotSerial()
{
    auto lock{_engine.getRenderScopeLock()};

    auto &renderingParameters =
        _engine.getParametersManager().getRenderingParameters();
    auto &renderer = _engine.getRenderer();

    const auto aspectPrev = _camera->getProperty<double>("aspect");
    const auto sppPrev = renderingParameters.getSamplesPerPixel();

    _camera->updateProperty("aspect", double(_params.size.x) / _params.size.y);
    renderingParameters.setSamplesPerPixel(_params.samplesPerPixel);
    renderer.commit();

    // Only render one frame to avoid ghosting if camera is moved or simulation
    // frame changes between accumulation frames.
    auto image = _renderSnapshot(1);

    renderingParameters.setSamplesPerPixel(sppPrev);
    _camera->updateProperty("aspect", aspectPrev);

    return image;
}

ImageGenerator::ImageBase64 SnapshotFunctor::operator()()
{
    return _engine.supportsConcurrentSnapshots() ? _takeSnapshotConcurrent()
                                                 : _takeSnapshotSerial();
}
} // namespace brayns
