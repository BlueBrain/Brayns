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

#include "SnapshotEntrypoint.h"

#include <brayns/common/Log.h>

#include <brayns/engine/FrameRenderer.h>

#include <brayns/network/common/ProgressHandler.h>

#include <brayns/utils/StringUtils.h>
#include <brayns/utils/image/ImageEncoder.h>
#include <brayns/utils/image/ImageFormat.h>

#include <chrono>
#include <thread>

namespace
{
struct SnapshotResultHandler
{
    static std::string
        writeToDisk(const std::string &path, const brayns::ImageSettings &imageSettings, brayns::Framebuffer &fb)
    {
        const auto &formatName = imageSettings.getFormat();
        const auto format = brayns::string_utils::toLowercase(formatName);
        const auto quality = imageSettings.getQuality();
        auto image = fb.getImage();
        try
        {
            brayns::ImageEncoder::save(image, path, quality);
        }
        catch (const std::runtime_error &e)
        {
            brayns::Log::error("{}", e.what());
        }

        return {};
    }

    static std::string encodeToBase64(const brayns::ImageSettings &imageSettings, brayns::Framebuffer &fb)
    {
        const auto &formatName = imageSettings.getFormat();
        const auto fixedFormat = brayns::string_utils::toLowercase(formatName);
        const auto format = brayns::ImageFormat::fromExtension(fixedFormat);
        const auto quality = imageSettings.getQuality();
        auto image = fb.getImage();
        try
        {
            return {brayns::ImageEncoder::encodeToBase64(image, format, quality)};
        }
        catch (const std::runtime_error &e)
        {
            brayns::Log::error("{}", e.what());
        }

        return {};
    }
};

class SnapshotHandler
{
public:
    using SnapshotProgress =
        brayns::ProgressHandler<brayns::EntrypointRequest<brayns::SnapshotParams, brayns::ImageBase64Message>>;

    static brayns::ImageBase64Message
        handle(brayns::SnapshotParams &params, SnapshotProgress &progress, brayns::Engine &engine)
    {
        // Initialize parameters

        auto &imageSettings = params.image_settings;
        const auto &imageSize = imageSettings.getSize();
        auto &imagePath = params.file_path;

        // Parameters
        auto paramsManager = engine.getParametersManager();
        auto &simulation = paramsManager.getSimulationParameters();
        auto frame = params.simulation_frame;
        simulation.setFrame(frame);

        // Renderer
        auto &rendererObject = params.renderer;
        auto renderer = rendererObject.create();
        renderer->commit();

        // Camera
        auto &cameraObject = params.camera;
        auto camera = cameraObject.create();
        const auto aspectRatio = static_cast<float>(imageSize.x) / static_cast<float>(imageSize.y);
        camera->setAspectRatio(aspectRatio);
        const auto &cameraLookAt = params.camera_view;
        camera->setLookAt(cameraLookAt);
        camera->commit();

        // Framebuffer
        brayns::Framebuffer framebuffer;
        framebuffer.setAccumulation(false);
        framebuffer.setFormat(brayns::PixelFormat::StandardRgbaI8);
        framebuffer.setFrameSize(imageSize);
        framebuffer.commit();

        // Scene
        auto &scene = engine.getScene();
        scene.preRender(paramsManager);
        scene.commit();

        // Render
        auto future = brayns::FrameRenderer::asynchronous(*camera, framebuffer, *renderer, scene);
        const auto msg = "Rendering snapshot ...";
        while (!future.isReady())
        {
            const auto percentage = future.progress();
            progress.notify(msg, percentage);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        // Handle result
        brayns::ImageBase64Message image;

        if (!params.file_path.empty())
        {
            image.data = SnapshotResultHandler::writeToDisk(imagePath, imageSettings, framebuffer);
        }
        else
        {
            image.data = SnapshotResultHandler::encodeToBase64(imageSettings, framebuffer);
        }

        return image;
    }
};
} // namespace

namespace brayns
{
SnapshotEntrypoint::SnapshotEntrypoint(Engine &engine, CancellationToken token)
    : _engine(engine)
    , _token(token)
    , _cameraFactory(EngineFactories::createCameraFactory())
    , _renderFactory(EngineFactories::createRendererFactory())
{
}

std::string SnapshotEntrypoint::getMethod() const
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
    SnapshotParams params;

    auto &systemCamera = _engine.getCamera();
    params.camera = GenericObject<Camera>(systemCamera, _cameraFactory);

    auto systemLookAt = systemCamera.getLookAt();
    params.camera_view = systemLookAt;

    auto &systemRenderer = _engine.getRenderer();
    params.renderer = GenericObject<Renderer>(systemRenderer, _renderFactory);

    auto &systemFramebuffer = _engine.getFramebuffer();
    auto systemSize = systemFramebuffer.getFrameSize();
    params.image_settings = ImageSettings(systemSize);

    auto &paramsManager = _engine.getParametersManager();
    auto &simulation = paramsManager.getSimulationParameters();
    params.simulation_frame = simulation.getFrame();

    request.getParams(params);

    _download = params.file_path.empty();

    brayns::ProgressHandler progress(_token, request);

    auto result = SnapshotHandler::handle(params, progress, _engine);

    request.reply(result);
}

void SnapshotEntrypoint::onCancel()
{
    _token.cancel();
}

void SnapshotEntrypoint::onDisconnect()
{
    if (_download)
    {
        _token.cancel();
    }
}
} // namespace brayns
