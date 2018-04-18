/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "Engine.h"

#include <brayns/common/camera/Camera.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/scene/Scene.h>

#include <brayns/io/ImageManager.h>

#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
Engine::Engine(ParametersManager& parametersManager)
    : _parametersManager(parametersManager)
{
}

Engine::~Engine()
{
    if (_scene)
        _scene->reset();
}

void Engine::setActiveRenderer(const RendererType renderer)
{
    if (_activeRenderer != renderer)
        _activeRenderer = renderer;
}

void Engine::reshape(const Vector2ui& frameSize)
{
    const auto size = getSupportedFrameSize(frameSize);

    if (_frameBuffer->getSize() == size)
        return;

    _frameBuffer->resize(size);
    _camera->setAspectRatio(static_cast<float>(size.x()) /
                            static_cast<float>(size.y()));
    _camera->commit();
}

void Engine::setDefaultEpsilon()
{
    float epsilon = _parametersManager.getRenderingParameters().getEpsilon();
    if (epsilon == 0.f)
    {
        const Vector3f& boundsSize = _scene->getBounds().getSize();
        epsilon = boundsSize.length() / 1e6f;
        BRAYNS_INFO << "Default epsilon: " << epsilon << std::endl;
        _parametersManager.getRenderingParameters().setEpsilon(epsilon);
    }
}

void Engine::initializeMaterials(const MaterialsColorMap colorMap)
{
    _scene->setMaterialsColorMap(colorMap);
}

void Engine::commit()
{
    _scene->commitVolumeData();
    _scene->commitSimulationData();
    _scene->commitTransferFunctionData();
    _renderers[_activeRenderer]->commit();

    const auto& rp = _parametersManager.getRenderingParameters();
    if (rp.getStereoMode() != _camera->getStereoMode())
    {
        _camera->setStereoMode(rp.getStereoMode());
        _camera->commit();
    }
}

void Engine::render()
{
    auto fb = _snapshotFrameBuffer ? _snapshotFrameBuffer : _frameBuffer;
    _renderers[_activeRenderer]->render(fb);
}

void Engine::postRender()
{
    if (!_snapshotFrameBuffer)
    {
        _writeFrameToFile();
        return;
    }

    _processSnapshot();
}

Renderer& Engine::getRenderer()
{
    return *_renderers[_activeRenderer];
}

Vector2ui Engine::getSupportedFrameSize(const Vector2ui& size)
{
    Vector2f result = size;
    if (_parametersManager.getRenderingParameters().getStereoMode() ==
            StereoMode::side_by_side &&
        size.x() % 2 != 0)
        // In case of 3D stereo vision, make sure the width is even
        result.x() = size.x() - 1;
    return result;
}

void Engine::snapshot(const SnapshotParams& params, SnapshotReadyCallback cb)
{
    if (_snapshotFrameBuffer)
        throw std::runtime_error("Already a snapshot pending");

    _cb = cb;
    _snapshotSpp = params.samplesPerPixel;

    _snapshotFrameBuffer =
        createFrameBuffer(params.size, FrameBufferFormat::rgba_i8, true);

    _snapshotCamera = createCamera(getCamera().getType());
    *_snapshotCamera = getCamera();
    _snapshotCamera->setAspectRatio(float(params.size.x()) / params.size.y());
    _snapshotCamera->commit();
    _restoreSpp =
        _parametersManager.getRenderingParameters().getSamplesPerPixel();
    _parametersManager.getRenderingParameters().setSamplesPerPixel(1);
    _renderers[_activeRenderer]->setCamera(_snapshotCamera);

    setLastOperation("Render snapshot ...");
    setLastProgress(0.f);
}

bool Engine::continueRendering() const
{
    if (_snapshotFrameBuffer)
    {
        if (_snapshotSpp < 2)
            return false;
        return _snapshotFrameBuffer->numAccumFrames() < size_t(_snapshotSpp);
    }

    return _renderers.at(_activeRenderer)->getVariance() > 1 &&
           _frameBuffer->getAccumulation() &&
           (_frameBuffer->numAccumFrames() <
            _parametersManager.getRenderingParameters().getMaxAccumFrames());
}

void Engine::_processSnapshot()
{
    setLastProgress(float(_snapshotFrameBuffer->numAccumFrames()) /
                    _snapshotSpp);
    if (_snapshotFrameBuffer->numAccumFrames() == size_t(_snapshotSpp) ||
        _snapshotCancelled)
    {
        if (_snapshotCancelled)
            setLastProgress(1.f);
        else
            _cb(_snapshotFrameBuffer);

        _renderers[_activeRenderer]->setCamera(_camera);
        _parametersManager.getRenderingParameters().setSamplesPerPixel(
            _restoreSpp);

        _snapshotCamera.reset();
        _snapshotFrameBuffer.reset();
        _snapshotCancelled = false;
    }
}

void Engine::_writeFrameToFile()
{
    const auto& frameExportFolder =
        _parametersManager.getApplicationParameters().getFrameExportFolder();
    if (frameExportFolder.empty())
        return;
    char str[7];
    const auto frame = _parametersManager.getAnimationParameters().getFrame();
    snprintf(str, 7, "%06d", int(frame));
    const auto filename = frameExportFolder + "/" + str + ".png";
    FrameBuffer& frameBuffer = getFrameBuffer();
    ImageManager::exportFrameBufferToFile(frameBuffer, filename);
}
}
