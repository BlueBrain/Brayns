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

#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
Engine::Engine(ParametersManager& parametersManager)
    : _parametersManager(parametersManager)
    , _lastProgress(0)
{
    resetFrameNumber();
}

Engine::~Engine()
{
    _scene->reset();
}

void Engine::setActiveRenderer(const RendererType renderer)
{
    if (_activeRenderer != renderer)
        _activeRenderer = renderer;
}

void Engine::reshape(const Vector2ui& frameSize)
{
    if (_frameBuffer->getSize() == frameSize)
        return;

    const auto size = getSupportedFrameSize(frameSize);
    _frameBuffer->resize(size);
    _camera->setAspectRatio(static_cast<float>(size.x()) /
                            static_cast<float>(size.y()));
}

void Engine::commit()
{
    auto& sceneParams = _parametersManager.getSceneParameters();
    sceneParams.setTimestamp(sceneParams.getTimestamp() +
                             sceneParams.getAnimationDelta());

    _frameBuffer->clear();
}

void Engine::setDefaultCamera()
{
    const Vector2i& frameSize = _frameBuffer->getSize();

    const Boxf& worldBounds = _scene->getWorldBounds();
    const Vector3f& target = worldBounds.getCenter();
    const Vector3f& diag = worldBounds.getSize();
    Vector3f position = target;
    position.z() += diag.find_max();

    const Vector3f up = Vector3f(0.f, 1.f, 0.f);
    _camera->setInitialState(position, target, up);
    _camera->setAspectRatio(static_cast<float>(frameSize.x()) /
                            static_cast<float>(frameSize.y()));

    BRAYNS_INFO << "World bounding box: " << worldBounds << std::endl;
    BRAYNS_INFO << "World center      : " << worldBounds.getCenter()
                << std::endl;
}

void Engine::setDefaultEpsilon()
{
    float epsilon = _parametersManager.getRenderingParameters().getEpsilon();
    if (epsilon == 0.f)
    {
        const Vector3f& worldBoundsSize = _scene->getWorldBounds().getSize();
        epsilon = worldBoundsSize.length() / 1e6f;
        BRAYNS_INFO << "Default epsilon: " << epsilon << std::endl;
        _parametersManager.getRenderingParameters().setEpsilon(epsilon);
    }
}

void Engine::initializeMaterials(const MaterialType materialType)
{
    _scene->setMaterials(materialType);
    _scene->commit();
}

void Engine::render()
{
    ++_frameNumber;
}

Renderer& Engine::getRenderer()
{
    return *_renderers[_activeRenderer];
}

void Engine::resetFrameNumber()
{
    _frameNumber = -1;
}

Vector2ui Engine::getSupportedFrameSize(const Vector2ui& size)
{
    Vector2f result = size;
    if (getCamera().getType() == CameraType::stereo && size.x() % 2 != 0)
        // In case of 3D stereo vision, make sure the width is even
        result.x() = size.x() - 1;
    return result;
}
}
