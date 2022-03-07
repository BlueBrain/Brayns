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
                       brayns::CancellationToken &token,
                       brayns::CameraFactory &cameraFactory,
                       brayns::RendererFactory &rendererFactory)
    {
        auto progress = brayns::ProgressHandler(token, request);

        // Initialize parameters
        auto params = request.getParams();
        auto &cameraSettings = params.camera;
        auto &cameraView = params.camera_view;
        auto &rendererSettings = params.renderer;
        auto &imageSettings = params.image_settings;
        auto &imagePath = params.file_path;
        auto &animationSettings = params.animation;

        // Renderer
        const auto &systemRenderer = engine.getRenderer();
        auto renderer = rendererSettings? rendererSettings->deserialize(rendererFactory) : systemRenderer.clone();
        renderer->commit();

        // Camera
        const auto &systemCamera = engine.getCamera();
        auto camera = cameraSettings? cameraSettings->deserialize(cameraFactory) : systemCamera.clone();
        const auto aspectRatio = static_cast<float>(imageSettings.size.x) / static_cast<float>(imageSettings.size.y);
        camera->setAspectRatio(aspectRatio);
        if(cameraView)
        {
            camera->setPosition(cameraView->eye);
            camera->setTarget(cameraView->target);
            camera->setUp(cameraView->up);
        }
        camera->commit();

        // Framebuffer
        brayns::FrameBuffer frameBuffer;
        frameBuffer.setAccumulation(false);
        frameBuffer.setFormat(brayns::PixelFormat::SRGBA_I8);
        frameBuffer.setFrameSize(imageSettings.size);
        frameBuffer.commit();

        // Parameters
        auto parameters = engine.getParametersManager();
        if(animationSettings)
        {
            auto& animationParameters = parameters.getAnimationParameters();
            animationParameters.setDt(animationSettings->getDt());
            animationParameters.setEndFrame(animationSettings->getEndFrame());
            animationParameters.setStartFrame(animationSettings->getStartFrame());
            animationParameters.setFrame(animationSettings->getFrame());
        }

        // Scene
        auto& scene = engine.getScene();
        scene.preRender(parameters);
        scene.commit();

        // Render
        auto future = brayns::FrameRenderer::asynchronousRender(*camera, frameBuffer, *renderer, scene);

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

        ospWait(future);
        progressThread.join();

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

        request.reply(image);
    }
};
} // namespace

namespace brayns
{
SnapshotEntrypoint::SnapshotEntrypoint(Engine &engine, CancellationToken token, CameraFactory::Ptr cameraFactory, RendererFactory::Ptr rendererFactory)
    : _engine(engine)
    , _token(token)
    , _cameraFactory(cameraFactory)
    , _rendererFactory(rendererFactory)
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
    SnapshotHandler::handle(request, _engine, _token, *_cameraFactory, *_rendererFactory);
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
