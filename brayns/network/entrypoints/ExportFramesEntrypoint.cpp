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
struct ExportInfoLogger
{
    static void log(
        const size_t numKeyFrames,
        const std::string &path,
        const brayns::Vector2ui &frameSize,
        const brayns::Renderer &renderer)
    {
        const auto w = frameSize.x;
        const auto h = frameSize.y;
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
};

struct FrameWritter
{
    static void toDisk(const brayns::ExportFramesParams &params, const uint32_t frame, brayns::Framebuffer &fb)
    {
        const auto &path = params.path;
        const auto &imageSettings = params.image_settings;
        const auto &inputFormat = imageSettings.getFormat();
        const auto format = brayns::string_utils::toLowercase(inputFormat);
        const auto quality = imageSettings.getQuality();
        auto image = fb.getImage();

        auto frameName = fmt::format("{:05}", frame);
        auto filename = path + "/" + frameName + "." + format;
        try
        {
            brayns::ImageEncoder::save(image, filename, quality);
            brayns::Log::info("Frame saved to {}", filename);
        }
        catch (const std::runtime_error &e)
        {
            brayns::Log::error("{}", e.what());
        }
    }
};

using ExportProgressHandler =
    brayns::ProgressHandler<brayns::EntrypointRequest<brayns::ExportFramesParams, brayns::ExportFramesResult>>;

class FrameExporter
{
public:
    static brayns::ExportFramesResult exportFrames(
        brayns::Engine &engine,
        brayns::ParametersManager &paramsManager,
        brayns::ExportFramesParams &params,
        ExportProgressHandler &progress)
    {
        // Initialize parameters
        const auto &path = params.path;
        const auto &imageSettings = params.image_settings;
        const auto &frameSize = imageSettings.getSize();
        const auto &keyFrames = params.key_frames;
        const auto sequentialNaming = params.sequential_naming;

        // Renderer
        auto &rendererObject = params.renderer;
        auto renderer = rendererObject.create();
        renderer->commit();

        // Camera (committed on the loop)
        auto &cameraObject = params.camera;
        auto camera = cameraObject.create();

        auto &systemCamera = engine.getCamera();
        const auto &systemLookAt = systemCamera.getLookAt();

        // Update aspect ratio to match the rendered images size
        const auto aspectRatio = static_cast<float>(frameSize.x) / static_cast<float>(frameSize.y);
        camera->setAspectRatio(aspectRatio);

        // Framebuffer
        brayns::Framebuffer framebuffer;
        framebuffer.setAccumulation(false);
        framebuffer.setFormat(brayns::PixelFormat::StandardRgbaI8);
        framebuffer.setFrameSize(frameSize);
        framebuffer.commit();

        // Parameters manager
        auto parametersManager = paramsManager;
        auto &simulation = parametersManager.getSimulationParameters();
        // Avoid the first frame not being triggered because the copy is in "not modified" state
        simulation.markModified();

        auto &scene = engine.getScene();

        // Compute for how much progress each frame accounts
        const auto progressChunk = 1.f / static_cast<float>(keyFrames.size());
        float totalProgress = 0.f;

        // Log export info
        ExportInfoLogger::log(keyFrames.size(), path, frameSize, *renderer);

        brayns::ExportFramesEntrypoint::Result result = {};

        // Render frames
        for (size_t i = 0; i < keyFrames.size(); ++i)
        {
            const auto &keyFrame = keyFrames[i];

            // Update scene
            simulation.setFrame(keyFrame.frame_index);
            scene.preRender(parametersManager);
            scene.commit();

            // Update camera
            const auto lookAt = keyFrame.camera_view.value_or(systemLookAt);
            camera->setLookAt(lookAt);
            camera->commit();

            // Render
            brayns::FrameRenderer::synchronous(*camera, framebuffer, *renderer, scene);

            // Write frame to disk
            auto name = sequentialNaming ? i : keyFrame.frame_index;
            char frame[64];
            sprintf(frame, "%05d", static_cast<int32_t>(name));
            try
            {
                FrameWritter::toDisk(params, name, framebuffer);
            }
            catch (const std::exception &e)
            {
                result = {1, e.what()};
                return {};
            }

            totalProgress += progressChunk;

            progress.notify(fmt::format("Frame {} out of {} done", i, keyFrames.size()), totalProgress);
        }

        return result;
    }
};
} // namespace

namespace brayns
{
ExportFramesEntrypoint::ExportFramesEntrypoint(Engine &engine, ParametersManager &parmManager, CancellationToken token)
    : _engine(engine)
    , _paramsManager(parmManager)
    , _token(token)
    , _cameraFactory(EngineFactories::createCameraFactory())
    , _renderFactory(EngineFactories::createRendererFactory())
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
    // Params
    ExportFramesParams params;
    auto &systemCamera = _engine.getCamera();
    params.camera = GenericObject<Camera>(systemCamera, _cameraFactory);
    auto &systemRender = _engine.getRenderer();
    params.renderer = GenericObject<Renderer>(systemRender, _renderFactory);
    auto &systemFb = _engine.getFrameBuffer();
    params.image_settings = ImageSettings(systemFb.getFrameSize());
    request.getParams(params);

    // Progress handler
    brayns::ProgressHandler progress(_token, request);

    // Do export
    const auto result = FrameExporter::exportFrames(_engine, _paramsManager, params, progress);

    request.reply(result);
}

void ExportFramesEntrypoint::onCancel()
{
    _token.cancel();
}
} // namespace brayns
