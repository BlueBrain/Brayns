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

#include "ExportFramesEntrypoint.h"

#include <brayns/utils/image/ImageEncoder.h>
#include <brayns/utils/image/ImageFormat.h>

#include <spdlog/fmt/fmt.h>

namespace brayns
{
namespace
{
void logExportInfo(const ExportFramesParams& info)
{
    const auto numKeyFrames = info.key_frames.size();
    const auto spp = info.spp;
    const auto& path = info.path;
    const auto& renderer = info.renderer_name;
    const auto w = info.image_size.x;
    const auto h = info.image_size.y;

    brayns::Log::info(
        "Export frame request\n"
        "- Frame size:\t{}x{}\n"
        "- Number of frames:\t{}\n"
        "- Samples per pixel:\t{}\n"
        "- Export folder:\t{}\n"
        "- Renderer:\t{}\n",
        w, h, numKeyFrames, spp, path, renderer);
}

void initializeParameters(Engine& engine, ExportFramesParams& params)
{
    const auto& paramsManager = engine.getParametersManager();
    const auto& sysRenderParams = paramsManager.getRenderingParameters();
    const auto& sysVolumeParams = paramsManager.getVolumeParameters();
    const auto& sysRenderer = engine.getRenderer();

    auto& rendererName = params.renderer_name;
    auto& renderSpecificParams = params.renderer_parameters;
    auto& volumeParams = params.volume_parameters;

    if (rendererName.empty())
        rendererName = sysRenderer.getCurrentType();

    if (renderSpecificParams.empty())
        renderSpecificParams = sysRenderer.getPropertyMap(rendererName);

    if (!volumeParams)
        volumeParams = std::make_unique<VolumeParameters>(sysVolumeParams);
}

std::string writeFrameToDisk(const ExportFramesParams& params,
                             const uint32_t frameName, FrameBuffer& fb)
{
    const auto& path = params.path;
    const auto& format = params.format;
    auto quality = params.quality;
    auto image = fb.getImage();

    char frame[7];
    sprintf(frame, "%05d", static_cast<int32_t>(frameName));

    auto filename = path + "/" + std::string(frame) + "." + format;
    try
    {
        ImageEncoder::save(image, filename, quality);
        Log::info("Frame saved to {}", filename);
    }
    catch (const std::runtime_error& e)
    {
        Log::error("{}", e.what());
    }

    return {};
}
} // namespace

ExportFramesTask::ExportFramesTask(Engine& engine, ExportFramesParams&& params)
    : _engine(engine)
    , _params(std::move(params))
{
}

void ExportFramesTask::run()
{
    // Initialize parameters
    initializeParameters(_engine, _params);

    // Create parameter managers
    auto& paramManager = _engine.getParametersManager();
    auto animParams = paramManager.getAnimationParameters();
    auto renderParams = paramManager.getRenderingParameters();
    renderParams.setSamplesPerPixel(_params.spp);
    renderParams.setSubsampling(1);
    renderParams.setAccumulation(false);
    auto& volumeParams = *_params.volume_parameters;

    const auto& rendererName = _params.renderer_name;
    const auto& rendererSettings = _params.renderer_parameters;
    const auto& imageSize = _params.image_size;
    const auto nameAfterStep = _params.name_after_simulation_index;
    const auto& rendererSpecificParams = _params.renderer_parameters;
    const auto& keyFrames = _params.key_frames;

    // Create the camera
    auto camera = _engine.createCamera();
    const auto& systemCamera = _engine.getCamera();
    camera->clonePropertiesFrom(systemCamera);
    const auto aspectRatio =
        static_cast<double>(imageSize.x) / static_cast<double>(imageSize.y);
    camera->setBufferTarget("default");
    camera->markModified(false);
    camera->commit();

    // Create (clone) the scene
    auto scene = _engine.createScene(animParams, volumeParams);
    scene->copyFrom(_engine.getScene());

    // Create the renderer
    auto renderer = _engine.createRenderer(animParams, renderParams);
    renderer->setCurrentType(rendererName);
    renderer->setProperties(rendererSettings);
    renderer->setCamera(camera);
    renderer->setScene(scene);

    // Create the framebuffer
    auto frameBuffer =
        _engine.createFrameBuffer("default", imageSize, PixelFormat::RGBA_I8);
    frameBuffer->setAccumulation(false);
    frameBuffer->markModified();

    const auto progressChunk = 1.f / static_cast<float>(keyFrames.size());
    float totalProgress = 0.f;

    logExportInfo(_params);

    for (size_t i = 0; i < keyFrames.size(); ++i)
    {
        const auto& keyFrame = keyFrames[i];

        animParams.setFrame(keyFrame.frame_index);
        scene->markModified(false);
        scene->commit();

        *camera = keyFrame.camera;
        camera->updateProperties(keyFrame.camera_params);
        camera->updateProperty("aspect", aspectRatio);
        camera->markModified(false);
        camera->commit();

        renderer->commit();

        frameBuffer->clear();
        renderer->render(frameBuffer);

        auto name = nameAfterStep ? keyFrame.frame_index : i;
        char frame[7];
        sprintf(frame, "%05d", static_cast<int32_t>(name));
        try
        {
            writeFrameToDisk(_params, name, *frameBuffer);
        }
        catch (const std::exception& e)
        {
            _result = {1, e.what()};
            return;
        }

        totalProgress += progressChunk;

        progress(fmt::format("Frame {} out of {} done", i, keyFrames.size()),
                 totalProgress);
    }
}

void ExportFramesTask::onComplete()
{
    reply(_result);
}

std::string ExportFramesEntrypoint::getName() const
{
    return "export-frames";
}

std::string ExportFramesEntrypoint::getDescription() const
{
    return "Export a list of keyframes as images to disk";
}

bool ExportFramesEntrypoint::isAsync() const
{
    return true;
}

void ExportFramesEntrypoint::onRequest(const Request& request)
{
    auto params = request.getParams();
    auto& engine = getApi().getEngine();
    auto task = std::make_shared<ExportFramesTask>(engine, std::move(params));
    launchTask(task, request);
}
} // namespace brayns
