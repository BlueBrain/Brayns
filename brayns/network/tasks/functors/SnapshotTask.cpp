/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include "SnapshotTask.h"

#include <brayns/common/Log.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/utils/StringUtils.h>
#include <brayns/utils/image/Image.h>
#include <brayns/utils/image/ImageEncoder.h>
#include <brayns/utils/image/ImageFormat.h>
#include <brayns/utils/image/ImageMerger.h>

namespace brayns
{
namespace
{
void initializeParameters(Engine& engine, SnapshotParams& params)
{
    const auto& paramsManager = engine.getParametersManager();
    const auto& sysAnimParams = paramsManager.getAnimationParameters();
    const auto& sysRenderParams = paramsManager.getRenderingParameters();
    const auto& sysVolumeParams = paramsManager.getVolumeParameters();

    auto& animParams = params.animParams;
    auto& renderParams = params.renderingParams;
    auto& volumeParams = params.volumeParams;

    if (!animParams)
        animParams = std::make_unique<AnimationParameters>(sysAnimParams);

    if (!renderParams)
        renderParams = std::make_unique<RenderingParameters>(sysRenderParams);

    renderParams->setSamplesPerPixel(1);
    renderParams->setSubsampling(1);
    renderParams->setAccumulation(true);
    renderParams->setMaxAccumFrames(params.samplesPerPixel);

    if (!volumeParams)
        volumeParams = std::make_unique<VolumeParameters>(sysVolumeParams);
}

std::shared_ptr<Camera> initializeCamera(Engine& engine, SnapshotParams& params)
{
    auto camera = engine.createCamera();
    const auto& sysCamera = engine.getCamera();

    if (params.camera)
    {
        *camera = *params.camera;
        camera->setCurrentType(sysCamera.getCurrentType());
        camera->clonePropertiesFrom(sysCamera);
    }
    else
        *camera = sysCamera;

    return camera;
}

std::string writeSnapshotToDisk(SnapshotParams& params, FrameBuffer& fb)
{
    const auto& path = params.filePath;
    const auto& format = params.format;
    auto image = fb.getImage();
    auto filename = path + "." + format;
    auto quality = params.quality;
    try
    {
        ImageEncoder::save(image, filename, quality);
    }
    catch (const std::runtime_error& e)
    {
        Log::error("{}", e.what());
    }

    return {};
}

std::string encodeSnapshotToBase64(SnapshotParams& params, FrameBuffer& fb)
{
    try
    {
        auto image = fb.getImage();
        auto format = ImageFormat::fromExtension(params.format);
        auto quality = params.quality;
        return {ImageEncoder::encodeToBase64(image, format, quality)};
    }
    catch (const std::runtime_error& e)
    {
        Log::error("{}", e.what());
    }

    return {};
}
} // namespace

SnapshotFunctor::SnapshotFunctor(Engine& engine, SnapshotParams&& params)
 : _engine(engine)
 , _params(std::move(params))
{
}

std::string SnapshotFunctor::operator()()
{
    // Initialize parameters
    initializeParameters(_engine, _params);
    auto& animParams = _params.animParams;
    auto& renderParams = _params.renderingParams;
    auto& volumeParams = _params.volumeParams;

    // Initialize (Clone) scene
    auto scene = _engine.createScene(*animParams, *volumeParams);
    scene->copyFrom(_engine.getScene());
    scene->commit();

    // Initialize camera
    auto camera = initializeCamera(_engine, _params);
    const auto& size = _params.size;
    const auto ar = static_cast<double>(size.x) / static_cast<double>(size.y);
    camera->updateProperty("aspect", ar);
    camera->setBufferTarget("default");
    camera->commit();

    // Initialize renderer
    auto renderer = _engine.createRenderer(*animParams, *renderParams);
    const auto& sysRenderer = _engine.getRenderer();
    renderer->setCurrentType(sysRenderer.getCurrentType());
    renderer->clonePropertiesFrom(sysRenderer);
    renderer->setCamera(camera);
    renderer->setScene(scene);
    renderer->commit();

    // Initialize framebuffer
    auto frameBuffer =
            _engine.createFrameBuffer("default", size, PixelFormat::RGBA_I8);
    frameBuffer->setAccumulation(true);

    // Prepare notifications message
    const auto name = string_utils::shortenString(_params.name);
    const auto msg = "Render snapshot " + name + " ...";

    // Render snapshot
    // TODO WITH ENGINE REFACTORING
    // OSPRay supports a progress callback for the rendering process
    // https://github.com/ospray/ospray/tree/v1.8.5#progress-and-cancel-progress-and-cancel-unnumbered
    // Can be used to get progress information while using samples per pixel instead of accumulation
    // to speed up rendering
    size_t numAccumFrames = frameBuffer->numAccumFrames();
    const auto spp = _params.samplesPerPixel;
    while (numAccumFrames != spp)
    {
        renderer->render(frameBuffer);
        frameBuffer->incrementAccumFrames();
        numAccumFrames = frameBuffer->numAccumFrames();

        const auto numAccumFramesF = static_cast<float>(numAccumFrames);
        const auto localProgess = 1.f / numAccumFramesF;
        const auto totalProgress = numAccumFramesF / static_cast<float>(spp);
        progress(msg, localProgess, totalProgress);
    }

    // Handle result
    if (!_params.filePath.empty())
        return writeSnapshotToDisk(_params, *frameBuffer);
    else
        return encodeSnapshotToBase64(_params, *frameBuffer);
}
}
