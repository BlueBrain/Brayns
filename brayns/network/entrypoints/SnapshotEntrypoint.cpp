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
std::string writeSnapshotToDisk(const std::string& path, const
                                brayns::GenericImageSettings &imageSettings,
                                brayns::FrameBuffer &fb)
{
    const auto &format = imageSettings.format;
    auto image = fb.getImage();
    auto filename = path + "." + format;
    auto quality = imageSettings.quality;
    try
    {
        brayns::ImageEncoder::save(image, filename, quality);
    }
    catch (const std::runtime_error &e)
    {
        brayns::Log::error("{}", e.what());
    }

    return {};
}

std::string encodeSnapshotToBase64(const brayns::GenericImageSettings &imageSettings, brayns::FrameBuffer &fb)
{
    try
    {
        auto image = fb.getImage();
        auto format = brayns::ImageFormat::fromExtension(imageSettings.format);
        auto quality = imageSettings.quality;
        return {brayns::ImageEncoder::encodeToBase64(image, format, quality)};
    }
    catch (const std::runtime_error &e)
    {
        brayns::Log::error("{}", e.what());
    }

    return {};
}

class SnapshotHandler
{
public:
    using Request = brayns::SnapshotEntrypoint::Request;

    static void handle(const Request &request,
                       brayns::Engine &engine,
                       brayns::CancellationToken &token)
    {
        auto progress = brayns::ProgressHandler(token, request);

        const auto &systemFrameBuffer = engine.getFrameBuffer();
        const auto &systemFrameSize = systemFrameBuffer.getFrameSize();
        const auto &systemCamera = engine.getCamera();
        const auto &systemLookAt = systemCamera.getLookAt();
        const auto &systemParameters = engine.getParametersManager();
        const auto &systemAnimationParams = systemParameters.getAnimationParameters();
        const auto systemAnimationFrame = systemAnimationParams.getFrame();

        // Initialize parameters
        auto params = request.getParams();
        auto &cameraView = params.camera_view;
        auto cameraLookAt = cameraView.value_or(systemLookAt);
        auto &imageSettings = params.image_settings;
        auto &imageSettingsSize = imageSettings.size;
        auto imageSize = imageSettingsSize.value_or(systemFrameSize);
        auto &imagePath = params.file_path;
        auto &animationFrame = params.animation_frame;
        auto animationFrameIndex = animationFrame.value_or(systemAnimationFrame);

        // Renderer
        auto &renderer = engine.getRenderer();
        renderer.commit();

        // Camera
        auto &camera = engine.getCamera();
        const auto aspectRatio = static_cast<float>(imageSize.x) / static_cast<float>(imageSize.y);
        camera.setAspectRatio(aspectRatio);
        camera.setLookAt(cameraLookAt);
        camera.commit();

        // Framebuffer
        brayns::FrameBuffer frameBuffer;
        frameBuffer.setAccumulation(false);
        frameBuffer.setFormat(brayns::PixelFormat::SRGBA_I8);
        frameBuffer.setFrameSize(imageSize);
        frameBuffer.commit();

        // Parameters
        auto parameters = systemParameters;
        auto &animation = parameters.getAnimationParameters();
        animation.setFrame(animationFrameIndex);

        // Scene
        auto& scene = engine.getScene();
        scene.preRender(parameters);
        scene.commit();

        // Render
        auto future = brayns::FrameRenderer::asynchronous(camera, frameBuffer, renderer, scene);
/*
        auto progressThread = std::thread([&]()
        {
            const auto msg = "Rendering snapshot ...";
            while(!ospIsReady(future))
            {
                const auto percentage = ospGetProgress(future);
                progress.notify(msg, percentage);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
*/

        //progressThread.join();
        const auto msg = "Rendering snapshot ...";
        while(!ospIsReady(future))
        {
            const auto percentage = ospGetProgress(future);
            progress.notify(msg, percentage);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        ospWait(future);

        // Handle result
        brayns::ImageBase64Message image;

        if (!params.file_path.empty())
        {
            image.data = writeSnapshotToDisk(imagePath, imageSettings, frameBuffer);
        }
        else
        {
            image.data = encodeSnapshotToBase64(imageSettings, frameBuffer);
        }

        // Restore camera view
        camera.setLookAt(systemLookAt);

        request.reply(image);
    }
};
} // namespace

namespace brayns
{
SnapshotEntrypoint::SnapshotEntrypoint(Engine &engine, CancellationToken token)
    : _engine(engine)
    , _token(token)
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
    SnapshotHandler::handle(request, _engine, _token);
}

void SnapshotEntrypoint::onCancel()
{
    _token.cancel();
}

void SnapshotEntrypoint::onDisconnect(const ClientRef &client)
{
    if (_client && client == *_client)
    {
        _token.cancel();
    }
}
} // namespace brayns
