/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include "OSPRayEngine.h"

#include <brayns/common/input/KeyboardHandler.h>

#include <brayns/parameters/ParametersManager.h>

#include <plugins/engines/ospray/OSPRayCamera.h>
#include <plugins/engines/ospray/OSPRayFrameBuffer.h>
#include <plugins/engines/ospray/OSPRayMaterial.h>
#include <plugins/engines/ospray/OSPRayRenderer.h>
#include <plugins/engines/ospray/OSPRayScene.h>

#include <ospray/OSPConfig.h> // TILE_SIZE
#include <ospray/version.h>

namespace brayns
{
OSPRayEngine::OSPRayEngine(ParametersManager& parametersManager)
    : Engine(parametersManager)
{
    BRAYNS_INFO << "Initializing OSPRay" << std::endl;
    auto& ap = _parametersManager.getApplicationParameters();
    try
    {
        int argc = 1;
        std::vector<const char*> argv;

        // Ospray expects but ignores the application name as the first argument
        argv.push_back("Brayns");

        if (ap.getEngine() == EngineType::optix)
        {
            _type = EngineType::optix;
            argc += 2;
            argv.push_back("--osp:module:optix");
            argv.push_back("--osp:device:optix");
        }

        if (_parametersManager.getApplicationParameters()
                .getParallelRendering())
        {
            argc++;
            argv.push_back("--osp:mpi");
        }

        ospInit(&argc, argv.data());
    }
    catch (const std::exception& e)
    {
        // Note: This is necessary because OSPRay does not yet implement a
        // ospDestroy API.
        BRAYNS_ERROR << "Error during ospInit(): " << e.what() << std::endl;
    }

    for (const auto& module : ap.getOsprayModules())
    {
        try
        {
            const auto error = ospLoadModule(module.c_str());
            if (module == "deflect")
            {
                if (error != OSP_NO_ERROR)
                    BRAYNS_WARN
#if ((OSPRAY_VERSION_MAJOR == 1) && (OSPRAY_VERSION_MINOR < 3))
                        << "Could not load DeflectPixelOp module, error code "
                        << (int)error << std::endl;
#else
                        << ospDeviceGetLastErrorMsg(ospGetCurrentDevice())
                        << std::endl;
#endif
                else
                    _haveDeflectPixelOp = true;
            }
            else if (error != OSP_NO_ERROR)
                throw std::runtime_error(
                    ospDeviceGetLastErrorMsg(ospGetCurrentDevice()));
        }
        catch (const std::exception& e)
        {
            BRAYNS_ERROR << "Error while loading module " << module << ": "
                         << e.what() << std::endl;
        }
    }

    RenderingParameters& rp = _parametersManager.getRenderingParameters();
    BRAYNS_INFO << "Initializing renderers" << std::endl;

    _createRenderers();

    const auto ospFlags = _getOSPDataFlags();

    BRAYNS_INFO << "Initializing scene" << std::endl;
    _scene = std::make_shared<OSPRayScene>(_parametersManager, ospFlags);

    BRAYNS_INFO << "Initializing camera" << std::endl;
    _camera = createCamera(rp.getCameraType());
    _camera->setStereoMode(rp.getStereoMode());

    _camera->setEnvironmentMap(
        !parametersManager.getSceneParameters().getEnvironmentMap().empty());

    BRAYNS_INFO << "Initializing frame buffer" << std::endl;
    _frameSize = getSupportedFrameSize(
        _parametersManager.getApplicationParameters().getWindowSize());

    bool accumulation = rp.getAccumulation();
    if (!_parametersManager.getApplicationParameters().getFilters().empty())
        accumulation = false;

    _frameBuffer =
        createFrameBuffer(_frameSize,
                          haveDeflectPixelOp() ? FrameBufferFormat::none
                                               : FrameBufferFormat::rgba_i8,
                          accumulation);
    if (haveDeflectPixelOp())
        std::static_pointer_cast<OSPRayFrameBuffer>(_frameBuffer)
            ->enableDeflectPixelOp();

    _renderer->setScene(_scene);
    _renderer->setCamera(_camera);

    BRAYNS_INFO << "Engine initialization complete" << std::endl;
}

OSPRayEngine::~OSPRayEngine()
{
    _scene.reset();
    _frameBuffer.reset();
    _renderer.reset();
    _camera.reset();

    // HACK: need ospFinish() here; currently used by optix module to properly
    // destroy optix context
    if (name() == EngineType::optix)
        ospLoadModule("exit");
}

EngineType OSPRayEngine::name() const
{
    // can be ospray or optix
    return _type;
}

void OSPRayEngine::commit()
{
    Engine::commit();

    auto device = ospGetCurrentDevice();
    if (device && _parametersManager.getRenderingParameters().isModified())
    {
        const auto useDynamicLoadBalancer =
            _parametersManager.getApplicationParameters()
                .getDynamicLoadBalancer();
        if (_useDynamicLoadBalancer != useDynamicLoadBalancer)
        {
            ospDeviceSet1i(device, "dynamicLoadBalancer",
                           useDynamicLoadBalancer);
            ospDeviceCommit(device);
            _useDynamicLoadBalancer = useDynamicLoadBalancer;

            BRAYNS_INFO << "Using "
                        << (useDynamicLoadBalancer ? "dynamic" : "static")
                        << " load balancer" << std::endl;
        }
    }

    auto osprayFrameBuffer =
        std::static_pointer_cast<OSPRayFrameBuffer>(_frameBuffer);
    const auto& streamParams = _parametersManager.getStreamParameters();
    if (streamParams.isModified() || _camera->isModified())
    {
        const auto isStereo =
            _camera->getStereoMode() == StereoMode::side_by_side;
        osprayFrameBuffer->setStreamingParams(streamParams, isStereo);
    }
}

void OSPRayEngine::preRender()
{
    const auto& renderParams = _parametersManager.getRenderingParameters();
    if (renderParams.getAccumulation() != _frameBuffer->getAccumulation())
    {
        _frameBuffer->setAccumulation(renderParams.getAccumulation());
        _frameBuffer->resize(_frameBuffer->getSize());
    }
}

Vector2ui OSPRayEngine::getSupportedFrameSize(const Vector2ui& size)
{
    if (!haveDeflectPixelOp())
        return Engine::getSupportedFrameSize(size);

    Vector2f result = size;
    const auto isStereo = _camera->getStereoMode() == StereoMode::side_by_side;
    if (isStereo)
    {
        if (size.x() % (TILE_SIZE * 2) != 0)
            result.x() = size.x() - size.x() % (TILE_SIZE * 2);
    }
    else
    {
        if (size.x() % TILE_SIZE != 0)
            result.x() = size.x() - size.x() % TILE_SIZE;
    }

    if (size.y() % TILE_SIZE != 0)
        result.y() = size.y() - size.y() % TILE_SIZE;

    return result;
}

Vector2ui OSPRayEngine::getMinimumFrameSize() const
{
    const auto isStereo = _camera->getStereoMode() == StereoMode::side_by_side;
    if (isStereo)
        return {TILE_SIZE * 2, TILE_SIZE};
    return {TILE_SIZE, TILE_SIZE};
}

void OSPRayEngine::_createRenderers()
{
    auto& rp = _parametersManager.getRenderingParameters();
    auto ospRenderer = std::make_shared<OSPRayRenderer>(
        _parametersManager.getAnimationParameters(), rp);

    for (const auto& renderer : rp.getRenderers())
    {
        PropertyMap properties;
        if (renderer == "pathtracingrenderer")
        {
            properties.setProperty({"aoDistance", "Ambient occlusion distance",
                                    10000.f, 1e-20f, 1e20f});
            properties.setProperty(
                {"aoWeight", "Ambient occlusion weight", 0.f, 0.f, 1.f});
            properties.setProperty(
                {"shadows", "Shadow intensity", 0.f, 0.f, 1.f});
            properties.setProperty(
                {"softShadows", "Shadow softness", 0.f, 0.f, 1.f});
        }
        if (renderer == "proximityrenderer")
        {
            properties.setProperty(
                {"detectionDistance", "Detection distance", 1.f});
            properties.setProperty({"detectionFarColor", "Detection far color",
                                    std::array<float, 3>{{1.f, 0.f, 0.f}}});
            properties.setProperty({"detectionNearColor",
                                    "Detection near color",
                                    std::array<float, 3>{{0.f, 1.f, 0.f}}});
            properties.setProperty({"detectionOnDifferentMaterial",
                                    "Detection on different material", false});
            properties.setProperty(
                {"electronShading", "Electron shading", false});
        }
        if (renderer == "simulationrenderer")
        {
            properties.setProperty({"aoDistance", "Ambient occlusion distance",
                                    10000.f, 1e-20f, 1e20f});
            properties.setProperty(
                {"aoWeight", "Ambient occlusion weight", 0.f, 0.f, 1.f});
            properties.setProperty({"detectionDistance", "Detection distance",
                                    15.f, 0.f, 10000.f});
            properties.setProperty(
                {"electronShading", "Electron shading", false});
            properties.setProperty(
                {"shadingEnabled", "Diffuse shading", false});
            properties.setProperty(
                {"shadows", "Shadow intensity", 0.f, 0.f, 1.f});
            properties.setProperty(
                {"softShadows", "Shadow softness", 0.f, 0.f, 1.f});
        }
        if (renderer == "scivis")
        {
            properties.setProperty({"aoDistance", "Ambient occlusion distance",
                                    10000.f, 1e-20f, 1e20f});
            properties.setProperty(
                {"aoSamples", "Ambient occlusion samples", 1, 0, 128});
            properties.setProperty({"aoTransparencyEnabled",
                                    "Ambient occlusion transpareny", true});
            properties.setProperty(
                {"aoWeight", "Ambient occlusion weight", 0.f, 0.f, 1.f});
            properties.setProperty(
                {"oneSidedLighting", "One-sided lighting", true});
            properties.setProperty({"shadowsEnabled", "Shadows", false});
        }
        ospRenderer->setProperties(renderer, properties);
    }
    ospRenderer->setCurrentType(rp.getCurrentRenderer());
    ospRenderer->createOSPRenderer();
    _renderer = ospRenderer;
}

FrameBufferPtr OSPRayEngine::createFrameBuffer(
    const Vector2ui& frameSize, const FrameBufferFormat frameBufferFormat,
    const bool accumulation) const
{
    return std::make_shared<OSPRayFrameBuffer>(frameSize, frameBufferFormat,
                                               accumulation);
}

ScenePtr OSPRayEngine::createScene(ParametersManager& parametersManager) const
{
    return std::make_shared<OSPRayScene>(parametersManager, _getOSPDataFlags());
}

CameraPtr OSPRayEngine::createCamera(const CameraType type) const
{
    auto& rp = _parametersManager.getRenderingParameters();
    auto name = rp.getCameraTypeAsString(type);
    try
    {
        return std::make_shared<OSPRayCamera>(type, name);
    }
    catch (const std::runtime_error& e)
    {
        BRAYNS_WARN << e.what() << ". Using default camera instead"
                    << std::endl;
        rp.initializeDefaultCameras();
        name = rp.getCameraTypeAsString(CameraType::default_);
        return std::make_shared<OSPRayCamera>(type, name);
    }
}

RendererPtr OSPRayEngine::createRenderer(
    const AnimationParameters& animationParameters,
    const RenderingParameters& renderingParameters) const
{
    return std::make_shared<OSPRayRenderer>(animationParameters,
                                            renderingParameters);
}

uint32_t OSPRayEngine::_getOSPDataFlags() const
{
    return _parametersManager.getGeometryParameters().getMemoryMode() ==
                   MemoryMode::shared
               ? OSP_DATA_SHARED_BUFFER
               : 0;
}
}
