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
#include <brayns/engine/FrameRenderer.h>
#include <brayns/network/common/ProgressHandler.h>
#include <brayns/utils/StringUtils.h>
#include <brayns/utils/image/ImageEncoder.h>
#include <brayns/utils/image/ImageFormat.h>

#include <spdlog/fmt/fmt.h>

namespace
{


void logExportInfo(const size_t numKeyFrames,
                   const std::string& path,
                   const brayns::GenericImageSettings& imageSettings,
                   const brayns::Renderer& renderer)
{
    const auto spp = renderer.getSamplesPerPixel();
    const auto rendererName = renderer.getName();
    const auto w = imageSettings.size.x;
    const auto h = imageSettings.size.y;

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

std::string writeFrameToDisk(const brayns::ExportFramesParams &params, const uint32_t frame, brayns::FrameBuffer &fb)
{
    const auto &path = params.path;
    const auto &imageSettings = params.image_settings;
    const auto format = brayns::string_utils::toLowercase(imageSettings.format);
    auto quality = imageSettings.quality;
    auto image = fb.getImage();

    char frameName[64];
    sprintf(frameName, "%05d", static_cast<int32_t>(frame));

    auto filename = path + "/" + std::string(frameName) + "." + format;
    try
    {
        brayns::ImageEncoder::save(image, filename, quality);
        brayns::Log::info("Frame saved to {}", filename);
    }
    catch (const std::runtime_error &e)
    {
        brayns::Log::error("{}", e.what());
    }

    return {};
}

class FrameExporter
{
public:
    static void exportFrames(brayns::Engine &engine,
                             brayns::ParametersManager& paramsManager,
                             brayns::CancellationToken &token,
                             brayns::CameraFactory& cameraFactory,
                             brayns::RendererFactory& rendererFactory,
                             const brayns::ExportFramesEntrypoint::Request &request)
    {
        brayns::ExportFramesEntrypoint::Result result = {};

        // Progress
        auto progress = brayns::ProgressHandler(token, request);

        // Initialize parameters
        const auto params = request.getParams();
        const auto &path = params.path;
        const auto &imageSettings = params.image_settings;
        const auto &rendererSettings = params.renderer;
        const auto &cameraSettings = params.camera;
        const auto &keyFrames = params.key_frames;
        const auto sequentialNaming = params.sequential_naming;

        // Renderer
        const auto &systemRenderer = engine.getRenderer();
        auto renderer = rendererSettings? rendererSettings->deserialize(rendererFactory) : systemRenderer.clone();
        renderer->commit();

        // Camera
        const auto &systemCamera = engine.getCamera();
        auto camera = cameraSettings? cameraSettings->deserialize(cameraFactory) : systemCamera.clone();
        const auto aspectRatio = static_cast<float>(imageSettings.size.x) / static_cast<float>(imageSettings.size.y);
        camera->setAspectRatio(aspectRatio);

        // Framebuffer
        brayns::FrameBuffer frameBuffer;
        frameBuffer.setAccumulation(false);
        frameBuffer.setFormat(brayns::PixelFormat::SRGBA_I8);
        frameBuffer.setFrameSize(imageSettings.size);
        frameBuffer.commit();

        // Scene
        auto& scene = engine.getScene();

        // Parameters manager
        auto parametersManager = paramsManager;
        auto& animParams = parametersManager.getAnimationParameters();

        // Compute for how much progress each frame accounts
        const auto progressChunk = 1.f / static_cast<float>(keyFrames.size());
        float totalProgress = 0.f;

        // Log export info
        logExportInfo(keyFrames.size(), path, imageSettings, *renderer);

        // Render frames
        for (size_t i = 0; i < keyFrames.size(); ++i)
        {
            const auto &keyFrame = keyFrames[i];

            // Update scene
            animParams.setFrame(keyFrame.frame_index);
            scene.preRender(parametersManager);
            scene.commit();

            // Update camera
            const auto &lookAt = keyFrame.camera_view;
            camera->setPosition(lookAt.eye);
            camera->setTarget(lookAt.target);
            camera->setUp(lookAt.up);
            camera->commit();

            // Render
            brayns::FrameRenderer::synchronousRender(*camera, frameBuffer, *renderer, scene);

            // Write frame to disk
            auto name = sequentialNaming ? i : keyFrame.frame_index;
            char frame[64];
            sprintf(frame, "%05d", static_cast<int32_t>(name));
            try
            {
                writeFrameToDisk(params, name, frameBuffer);
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

namespace brayns
{
ExportFramesEntrypoint::ExportFramesEntrypoint(Engine &engine,
                                               ParametersManager& parmManager,
                                               CancellationToken token,
                                               CameraFactory::Ptr cameraFactory,
                                               RendererFactory::Ptr rendererFactory)
    : _engine(engine)
    , _paramsManager(parmManager)
    , _token(token)
    , _cameraFactory(cameraFactory)
    , _rendererFactory(rendererFactory)
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
    FrameExporter::exportFrames(_engine, _paramsManager, _token, *_cameraFactory, *_rendererFactory, request);
}

void ExportFramesEntrypoint::onCancel()
{
    _token.cancel();
}
} // namespace brayns
