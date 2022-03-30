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
    const auto w = imageSettings.size.x;
    const auto h = imageSettings.size.y;
    const auto spp = renderer.getSamplesPerPixel();
    const auto name = renderer.getName();

    brayns::Log::info(
        "Export frame request\n"
        "- Frame size:\t{}x{}\n"
        "- Number of frames:\t{}\n"
        "- Export folder:\t{}\n"
        "- Renderer:\t{}\n"
        "- Samples per pixel:\t{}\n",
        w,
        h,
        numKeyFrames,
        path,
        name,
        spp);
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
                             const brayns::ExportFramesEntrypoint::Request &request)
    {
        brayns::ExportFramesEntrypoint::Result result = {};

        const auto &systemFrameBuffer = engine.getFrameBuffer();
        const auto &systemFrameSize = systemFrameBuffer.getFrameSize();

        // Progress
        auto progress = brayns::ProgressHandler(token, request);

        // Initialize parameters
        const auto params = request.getParams();
        const auto &path = params.path;
        const auto &imageSettings = params.image_settings;
        const auto &imageSizeSettings = imageSettings.size;
        const auto frameSize = imageSizeSettings.value_or(systemFrameSize);
        const auto &keyFrames = params.key_frames;
        const auto sequentialNaming = params.sequential_naming;

        // Renderer
        auto &renderer = engine.getRenderer();
        renderer.commit();

        // Camera (committed on the loop)
        auto &camera = engine.getCamera();

        // Save camera pos and view
        const auto currentLookAt = camera.getLookAt();

        // Update aspect ratio to match the rendered images size
        const auto aspectRatio = static_cast<float>(frameSize.x) / static_cast<float>(frameSize.y);
        camera.setAspectRatio(aspectRatio);

        // Framebuffer
        brayns::FrameBuffer frameBuffer;
        frameBuffer.setAccumulation(false);
        frameBuffer.setFormat(brayns::PixelFormat::SRGBA_I8);
        frameBuffer.setFrameSize(frameSize);
        frameBuffer.commit();

        // Scene (committed on the loop)
        auto& scene = engine.getScene();

        // Parameters manager
        auto parametersManager = paramsManager;
        auto& animParams = parametersManager.getAnimationParameters();

        // Compute for how much progress each frame accounts
        const auto progressChunk = 1.f / static_cast<float>(keyFrames.size());
        float totalProgress = 0.f;

        // Log export info
        logExportInfo(keyFrames.size(), path, imageSettings, renderer);

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
            camera.setLookAt(lookAt);
            camera.commit();

            // Render
            brayns::FrameRenderer::synchronous(camera, frameBuffer, renderer, scene);

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

        // Restore original camera position and view
        camera.setLookAt(currentLookAt);

        request.reply(result);
    }
};
} // namespace

namespace brayns
{
ExportFramesEntrypoint::ExportFramesEntrypoint(Engine &engine,
                                               ParametersManager& parmManager,
                                               CancellationToken token)
    : _engine(engine)
    , _paramsManager(parmManager)
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
    FrameExporter::exportFrames(_engine, _paramsManager, _token, request);
}

void ExportFramesEntrypoint::onCancel()
{
    _token.cancel();
}
} // namespace brayns
