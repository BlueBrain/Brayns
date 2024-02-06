/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/utils/Log.h>

#include <brayns/engine/core/FrameRenderer.h>
#include <brayns/engine/framebuffer/types/StaticFrameHandler.h>

#include <brayns/network/common/ProgressHandler.h>

#include <brayns/utils/image/ImageEncoder.h>
#include <brayns/utils/image/ImageFormat.h>
#include <brayns/utils/string/StringCase.h>

#include <chrono>
#include <thread>

namespace
{
class ParamsBuilder
{
public:
    static brayns::SnapshotParams build(const brayns::SnapshotEntrypoint::Request &request, brayns::Engine &engine)
    {
        brayns::SnapshotParams params;

        auto &camera = engine.getCamera();
        auto view = camera.getView();
        params.camera_view = view;

        auto &paramsManager = engine.getParametersManager();

        auto &appParams = paramsManager.getApplicationParameters();
        auto systemSize = appParams.getWindowSize();
        params.image_settings = brayns::ImageSettings(systemSize);

        auto &simulation = paramsManager.getSimulationParameters();
        params.simulation_frame = simulation.getFrame();

        request.getParams(params);

        return params;
    }
};

class ImageHelper
{
public:
    static void save(
        const brayns::Image &image,
        const std::string &path,
        int quality,
        const std::optional<brayns::ImageMetadata> &metadata)
    {
        try
        {
            brayns::ImageEncoder::save(image, path, quality, metadata);
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Failed to save snapshot: '{}'.", e.what());
            throw brayns::InternalErrorException(e.what());
        }
    }

    static std::string encode(
        const brayns::Image &image,
        const std::string &extension,
        int quality,
        const std::optional<brayns::ImageMetadata> &metadata)
    {
        const auto lower = brayns::StringCase::toLower(extension);
        const auto format = brayns::ImageFormat::fromExtension(lower);
        try
        {
            return brayns::ImageEncoder::encode(image, format, quality, metadata);
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Failed to encode snapshot: '{}'.", e.what());
            throw brayns::InternalErrorException(e.what());
        }
    }
};

class ReplyHandler
{
public:
    static void reply(
        const brayns::SnapshotEntrypoint::Request &request,
        const brayns::SnapshotParams &params,
        brayns::Framebuffer &framebuffer)
    {
        auto &path = params.file_path;
        auto &settings = params.image_settings;
        auto quality = settings.getQuality();
        auto &metadata = params.metadata;
        auto format = settings.getFormat();
        auto image = framebuffer.getImage();
        if (path.empty())
        {
            auto data = ImageHelper::encode(image, format, quality, metadata);
            auto result = _formatResult(data.size());
            request.reply(result, data);
            return;
        }
        ImageHelper::save(image, path, quality, metadata);
        auto result = _formatResult(0);
        request.reply(result);
    }

private:
    static brayns::SnapshotResult _formatResult(size_t colorBufferSize)
    {
        brayns::SnapshotResult result;
        result.color_buffer.offset = 0;
        result.color_buffer.size = colorBufferSize;
        return result;
    }
};

class SnapshotHandler
{
public:
    static void handle(
        const brayns::SnapshotEntrypoint::Request &request,
        const brayns::SnapshotParams &params,
        brayns::CancellationToken &token,
        brayns::Engine &engine)
    {
        // Parameters
        auto paramsManager = engine.getParametersManager();
        auto &simulation = paramsManager.getSimulationParameters();
        auto frame = params.simulation_frame;
        simulation.setFrame(frame);

        auto &factories = engine.getFactories();

        // Renderer
        auto &rendererData = params.renderer;
        auto &rendererFactory = factories.renderer;
        auto renderer = rendererFactory.createOr(rendererData, engine.getRenderer());
        renderer.commit();

        // Framebuffer
        auto &imageSettings = params.image_settings;
        auto &imageSize = imageSettings.getSize();
        auto framebuffer = brayns::Framebuffer(std::make_unique<brayns::StaticFrameHandler>());
        framebuffer.setAccumulation(false);
        framebuffer.setFormat(brayns::PixelFormat::StandardRgbaI8);
        framebuffer.setFrameSize(imageSize);
        framebuffer.commit();

        // Camera
        auto &cameraData = params.camera;
        auto &cameraFactory = factories.cameras;
        auto camera = cameraFactory.createOr(cameraData, engine.getCamera());
        camera.setAspectRatioFromFrameSize(imageSize);
        camera.setView(params.camera_view);
        camera.commit();

        // Scene
        auto &scene = engine.getScene();
        scene.update(paramsManager);
        scene.commit();

        // Render
        auto progress = brayns::ProgressHandler(token, request);
        auto future = brayns::FrameRenderer::asynchronous(camera, framebuffer, renderer, scene);
        const auto msg = "Rendering snapshot ...";
        while (!future.isReady())
        {
            const auto percentage = future.progress();
            progress.notify(msg, percentage);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        ReplyHandler::reply(request, params, framebuffer);
    }
};
} // namespace

namespace brayns
{
SnapshotEntrypoint::SnapshotEntrypoint(Engine &engine, CancellationToken token):
    _engine(engine),
    _token(token)
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
    auto params = ParamsBuilder::build(request, _engine);
    _download = params.file_path.empty();
    SnapshotHandler::handle(request, params, _token, _engine);
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
