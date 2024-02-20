/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerero@epfl.ch>
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

#include "ExportGBuffersEntrypoint.h"

#include <brayns/engine/core/FrameRenderer.h>
#include <brayns/engine/framebuffer/types/StaticFrameHandler.h>

#include <brayns/network/common/ProgressHandler.h>

#include <brayns/utils/FileWriter.h>
#include <brayns/utils/image/codecs/ExrCodec.h>
#include <brayns/utils/string/StringCase.h>

#include <filesystem>
#include <thread>

namespace
{
class ParamsBuilder
{
public:
    static brayns::GBuffersParams build(const brayns::ExportGBuffersEntrypoint::Request &request, brayns::Engine &engine)
    {
        brayns::GBuffersParams params;

        auto &camera = engine.getCamera();
        auto view = camera.getView();
        params.camera_view = view;
        params.camera_near_clip = camera.getNearClippingDistance();

        auto &paramsManager = engine.getParametersManager();

        auto &appParams = paramsManager.getApplicationParameters();
        params.resolution = appParams.getWindowSize();

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
        brayns::Framebuffer &frameBuffer,
        const std::vector<brayns::FramebufferChannel> &channels,
        const std::string &filename)
    {
        auto encodedData = encode(frameBuffer, channels);
        auto savePath = _buildSavePath(filename);

        try
        {
            brayns::FileWriter::write(encodedData, savePath);
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Failed to save g-buffers: '{}'.", e.what());
            throw brayns::InternalErrorException(e.what());
        }
    }

    static std::string encode(brayns::Framebuffer &frameBuffer, const std::vector<brayns::FramebufferChannel> &channels)
    {
        try
        {
            auto codec = brayns::ExrCodec();

            if (channels.size() == 1)
            {
                return codec.encode(frameBuffer.getImage(channels.front()), 0, std::nullopt);
            }

            auto frames = _buildFrameList(frameBuffer, channels);
            return codec.encode(frames);
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Failed to encode snapshot: '{}'.", e.what());
            throw brayns::InternalErrorException(e.what());
        }
    }

private:
    static std::string _buildSavePath(const std::string &filename)
    {
        auto path = std::filesystem::path(filename);
        auto extension = path.extension().string();
        brayns::StringCase::lower(extension);

        if (extension == ".exr")
        {
            return filename;
        }

        return filename + ".exr";
    }

    static std::vector<brayns::ExrFrame> _buildFrameList(
        brayns::Framebuffer &framebuffer,
        const std::vector<brayns::FramebufferChannel> &channels)
    {
        auto frames = std::vector<brayns::ExrFrame>();
        frames.reserve(channels.size());

        for (auto channel : channels)
        {
            auto &frame = frames.emplace_back();
            frame.name = brayns::EnumInfo::getName(channel);
            frame.image = framebuffer.getImage(channel);
        }

        return frames;
    }
};

class ReplyHandler
{
public:
    static void reply(
        const brayns::ExportGBuffersEntrypoint::Request &request,
        const brayns::GBuffersParams &params,
        brayns::Framebuffer &framebuffer)
    {
        auto &path = params.file_path;

        if (path.empty())
        {
            auto data = ImageHelper::encode(framebuffer, params.channels);
            request.reply(brayns::EmptyJson(), data);
            return;
        }

        ImageHelper::save(framebuffer, params.channels, path);
        request.reply(brayns::EmptyJson());
    }
};

class ExportHandler
{
public:
    static void handle(
        const brayns::ExportGBuffersEntrypoint::Request &request,
        const brayns::GBuffersParams &params,
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
        auto &imageSize = params.resolution;
        auto framebuffer = brayns::Framebuffer(std::make_unique<brayns::StaticFrameHandler>());
        framebuffer.setAccumulation(false);
        framebuffer.setFormat(brayns::PixelFormat::RgbaF32);
        framebuffer.setFrameSize(imageSize);
        framebuffer.setChannels(params.channels);
        framebuffer.setToneMappingEnabled(false);
        framebuffer.commit();

        // Camera
        auto &cameraData = params.camera;
        auto &cameraFactory = factories.cameras;
        auto camera = cameraFactory.createOr(cameraData, engine.getCamera());
        camera.setAspectRatioFromFrameSize(imageSize);
        camera.setView(params.camera_view);
        camera.setNearClippingDistance(params.camera_near_clip);
        camera.commit();

        // Scene
        auto &scene = engine.getScene();
        scene.update(paramsManager);
        scene.commit();

        // Render
        auto progress = brayns::ProgressHandler(token, request);
        auto future = brayns::FrameRenderer::asynchronous(camera, framebuffer, renderer, scene);
        const auto msg = std::string("Exporting g-buffers ...");
        while (!future.isReady())
        {
            const auto percentage = future.progress();
            progress.notify(msg, percentage);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        ReplyHandler::reply(request, params, framebuffer);
    }
};
}

namespace brayns
{
ExportGBuffersEntrypoint::ExportGBuffersEntrypoint(Engine &engine, CancellationToken token):
    _engine(engine),
    _token(token)
{
}

std::string ExportGBuffersEntrypoint::getMethod() const
{
    return "export-gbuffers";
}

std::string ExportGBuffersEntrypoint::getDescription() const
{
    return "Renders and returns (or saves to disk) the Framebuffer G-Buffers";
}

void ExportGBuffersEntrypoint::onRequest(const Request &request)
{
    auto params = ParamsBuilder::build(request, _engine);
    _download = params.file_path.empty();
    ExportHandler::handle(request, params, _token, _engine);
}

bool ExportGBuffersEntrypoint::isAsync() const
{
    return true;
}

void ExportGBuffersEntrypoint::onCancel()
{
    _token.cancel();
}

void ExportGBuffersEntrypoint::onDisconnect()
{
    if (_download)
    {
        _token.cancel();
    }
}
} // namespace brayns
