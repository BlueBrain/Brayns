/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include "SnapshotEntrypoint.h"

#include <brayns/utils/image/ImageEncoder.h>
#include <brayns/utils/image/ImageFormat.h>

namespace brayns
{
namespace
{
void initializeParameters(Engine &engine, SnapshotParams &params)
{
    const auto &paramsManager = engine.getParametersManager();
    const auto &sysAnimParams = paramsManager.getAnimationParameters();
    const auto &sysRenderParams = paramsManager.getRenderingParameters();
    const auto &sysVolumeParams = paramsManager.getVolumeParameters();

    auto &animParams = params.animation_parameters;
    auto &renderParams = params.renderer;
    auto &volumeParams = params.volume_parameters;

    if (!animParams)
        animParams = std::make_unique<AnimationParameters>(sysAnimParams);

    if (!renderParams)
        renderParams = std::make_unique<RenderingParameters>(sysRenderParams);

    renderParams->setSamplesPerPixel(1);
    renderParams->setSubsampling(1);
    renderParams->setAccumulation(true);
    renderParams->setMaxAccumFrames(params.samples_per_pixel);

    if (!volumeParams)
        volumeParams = std::make_unique<VolumeParameters>(sysVolumeParams);
}

std::shared_ptr<Camera> initializeCamera(Engine &engine, SnapshotParams &params)
{
    auto camera = engine.createCamera();
    const auto &sysCamera = engine.getCamera();

    if (params.camera)
    {
        *camera = *params.camera;
        camera->setCurrentType(sysCamera.getCurrentType());
        camera->clonePropertiesFrom(sysCamera);
    }
    else
        *camera = sysCamera;

    return camera;
}

std::string writeSnapshotToDisk(SnapshotParams &params, FrameBuffer &fb)
{
    const auto &path = params.file_path;
    const auto &format = params.format;
    auto image = fb.getImage();
    auto filename = path + "." + format;
    auto quality = params.quality;
    try
    {
        ImageEncoder::save(image, filename, quality);
    }
    catch (const std::runtime_error &e)
    {
        Log::error("{}", e.what());
    }

    return {};
}

std::string encodeSnapshotToBase64(SnapshotParams &params, FrameBuffer &fb)
{
    try
    {
        auto image = fb.getImage();
        auto format = ImageFormat::fromExtension(params.format);
        auto quality = params.quality;
        return {ImageEncoder::encodeToBase64(image, format, quality)};
    }
    catch (const std::runtime_error &e)
    {
        Log::error("{}", e.what());
    }

    return {};
}
} // namespace

SnapshotTask::SnapshotTask(Engine &engine, SnapshotParams &&params)
    : _engine(engine)
    , _params(std::move(params))
{
}

void SnapshotTask::run()
{
    // Initialize parameters
    initializeParameters(_engine, _params);
    auto &animParams = _params.animation_parameters;
    auto &renderParams = _params.renderer;
    auto &volumeParams = _params.volume_parameters;

    // Initialize (Clone) scene
    auto scene = _engine.createScene(*animParams, *volumeParams);
    scene->copyFrom(_engine.getScene());
    scene->commit();

    // Initialize camera
    auto camera = initializeCamera(_engine, _params);
    const auto &size = _params.size;
    const auto ar = static_cast<double>(size.x) / static_cast<double>(size.y);
    camera->updateProperty("aspect", ar);
    camera->setBufferTarget("default");
    camera->commit();

    // Initialize renderer
    auto renderer = _engine.createRenderer(*animParams, *renderParams);
    const auto &sysRenderer = _engine.getRenderer();
    renderer->setCurrentType(sysRenderer.getCurrentType());
    renderer->clonePropertiesFrom(sysRenderer);
    renderer->setCamera(camera);
    renderer->setScene(scene);
    renderer->commit();

    // Initialize framebuffer
    auto frameBuffer = _engine.createFrameBuffer("default", size, PixelFormat::RGBA_I8);
    frameBuffer->setAccumulation(true);

    // Prepare notifications message
    const auto name = string_utils::shortenString(_params.name);
    const auto msg = "Render snapshot " + name + " ...";

    // Render snapshot
    // TODO WITH ENGINE REFACTORING
    // OSPRay supports a progress callback for the rendering process
    // https://github.com/ospray/ospray/tree/v1.8.5#progress-and-cancel-progress-and-cancel-unnumbered
    // Can be used to get progress information while using samples per pixel
    // instead of accumulation to speed up rendering
    size_t numAccumFrames = frameBuffer->numAccumFrames();
    const auto spp = _params.samples_per_pixel;
    while (numAccumFrames != spp)
    {
        renderer->render(frameBuffer);
        frameBuffer->incrementAccumFrames();
        numAccumFrames = frameBuffer->numAccumFrames();

        const auto totalProgress = static_cast<float>(numAccumFrames) / static_cast<float>(spp);
        progress(msg, totalProgress);
    }

    // Handle result
    if (!_params.file_path.empty())
        _image.data = writeSnapshotToDisk(_params, *frameBuffer);
    else
        _image.data = encodeSnapshotToBase64(_params, *frameBuffer);
}

void SnapshotTask::onComplete()
{
    reply(_image);
}

std::string SnapshotEntrypoint::getName() const
{
    return "snapshot";
}

std::string SnapshotEntrypoint::getDescription() const
{
    return "Take a snapshot with given parameters";
}

bool SnapshotEntrypoint::isAsync() const
{
    return true;
}

void SnapshotEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto &engine = getApi().getEngine();
    auto task = std::make_shared<SnapshotTask>(engine, std::move(params));
    launchTask(task, request);
}
} // namespace brayns
