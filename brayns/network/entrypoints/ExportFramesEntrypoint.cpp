/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/common/Log.h>

#include <brayns/network/common/ProgressHandler.h>

#include <brayns/utils/image/ImageEncoder.h>
#include <brayns/utils/image/ImageFormat.h>

#include <spdlog/fmt/fmt.h>

namespace brayns
{
namespace
{
void logExportInfo(const ExportFramesParams &info)
{
    const auto numKeyFrames = info.key_frames.size();
    const auto spp = info.spp;
    const auto &path = info.path;
    const auto &renderer = info.renderer_name;
    const auto w = info.image_size.x;
    const auto h = info.image_size.y;

    brayns::Log::info(
        "Export frame request\n"
        "- Frame size:\t{}x{}\n"
        "- Number of frames:\t{}\n"
        "- Samples per pixel:\t{}\n"
        "- Export folder:\t{}\n"
        "- Renderer:\t{}\n",
        w,
        h,
        numKeyFrames,
        spp,
        path,
        renderer);
}

void initializeParameters(Engine &engine, ExportFramesParams &params)
{
    const auto &paramsManager = engine.getParametersManager();
    const auto &sysVolumeParams = paramsManager.getVolumeParameters();
    const auto &sysRenderer = engine.getRenderer();

    auto &rendererName = params.renderer_name;
    auto &renderSpecificParams = params.renderer_parameters;
    auto &volumeParams = params.volume_parameters;

    if (rendererName.empty())
        rendererName = sysRenderer.getCurrentType();

    if (renderSpecificParams.empty())
        renderSpecificParams = sysRenderer.getPropertyMap(rendererName);

    if (!volumeParams)
        volumeParams = std::make_unique<VolumeParameters>(sysVolumeParams);
}

std::string writeFrameToDisk(const ExportFramesParams &params, const uint32_t frameName, FrameBuffer &fb)
{
    const auto &path = params.path;
    const auto &format = params.format;
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
    catch (const std::runtime_error &e)
    {
        Log::error("{}", e.what());
    }

    return {};
}

class FrameExporter
{
public:
    static void exportFrames(Engine &engine, CancellationToken &token, const ExportFramesEntrypoint::Request &request)
    {
        ExportFramesEntrypoint::Result result = {};

        // Progress
        auto progress = ProgressHandler(token, request);

        // Initialize parameters
        auto params = request.getParams();
        initializeParameters(engine, params);

        // Create parameter managers
        auto &paramManager = engine.getParametersManager();
        auto animParams = paramManager.getAnimationParameters();
        auto renderParams = paramManager.getRenderingParameters();
        renderParams.setSamplesPerPixel(params.spp);
        renderParams.setSubsampling(1);
        renderParams.setAccumulation(false);
        auto &volumeParams = *params.volume_parameters;

        const auto &rendererName = params.renderer_name;
        const auto &rendererSettings = params.renderer_parameters;
        const auto &imageSize = params.image_size;
        const auto nameAfterStep = params.name_after_simulation_index;
        const auto &keyFrames = params.key_frames;

        // Create the camera
        auto camera = engine.createCamera();
        const auto &systemCamera = engine.getCamera();
        camera->clonePropertiesFrom(systemCamera);
        const auto aspectRatio = static_cast<double>(imageSize.x) / static_cast<double>(imageSize.y);
        camera->setBufferTarget("default");
        camera->markModified(false);
        camera->commit();

        // Create (clone) the scene
        auto scene = engine.createScene(animParams, volumeParams);
        scene->copyFrom(engine.getScene());

        // Create the renderer
        auto renderer = engine.createRenderer(animParams, renderParams);
        renderer->setCurrentType(rendererName);
        renderer->setProperties(rendererSettings);
        renderer->setCamera(camera);
        renderer->setScene(scene);

        // Create the framebuffer
        auto frameBuffer = engine.createFrameBuffer("default", imageSize, PixelFormat::RGBA_I8);
        frameBuffer->setAccumulation(false);
        frameBuffer->markModified();

        const auto progressChunk = 1.f / static_cast<float>(keyFrames.size());
        float totalProgress = 0.f;

        logExportInfo(params);

        for (size_t i = 0; i < keyFrames.size(); ++i)
        {
            const auto &keyFrame = keyFrames[i];

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
                writeFrameToDisk(params, name, *frameBuffer);
            }
            catch (const std::exception &e)
            {
                result = {1, e.what()};
                return;
            }

            totalProgress += progressChunk;

            progress.notify(fmt::format("Frame {} out of {} done", i, keyFrames.size()), totalProgress);
        }

        request.reply(result);
    }
};
} // namespace

ExportFramesEntrypoint::ExportFramesEntrypoint(Engine &engine, CancellationToken token)
    : _engine(engine)
    , _token(token)
{
}

std::string ExportFramesEntrypoint::getMethod() const
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

void ExportFramesEntrypoint::onRequest(const Request &request)
{
    FrameExporter::exportFrames(_engine, _token, request);
}

void ExportFramesEntrypoint::onCancel()
{
    _token.cancel();
}
} // namespace brayns
