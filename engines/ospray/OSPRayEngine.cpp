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

#include "OSPRayCamera.h"
#include "OSPRayFrameBuffer.h"
#include "OSPRayMaterial.h"
#include "OSPRayRenderer.h"
#include "OSPRayScene.h"

#include "ispc/render/AdvancedSimulationRenderer.h" // enum Shading

#include <ospray/OSPConfig.h>                    // TILE_SIZE
#include <ospray/SDK/camera/PerspectiveCamera.h> // enum StereoMode
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
        std::vector<const char*> argv;

        // Ospray expects but ignores the application name as the first argument
        argv.push_back("Brayns");

        // Setup log and error output
        argv.push_back("--osp:logoutput");
        argv.push_back("cout");
        argv.push_back("--osp:erroroutput");
        argv.push_back("cerr");

        if (ap.getEngine() == EngineType::optix)
        {
            _type = EngineType::optix;
            argv.push_back("--osp:module:optix");
            argv.push_back("--osp:device:optix");
        }

        if (_parametersManager.getApplicationParameters()
                .getParallelRendering())
        {
            argv.push_back("--osp:mpi");
        }

        int argc = argv.size();
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
                        << ospDeviceGetLastErrorMsg(ospGetCurrentDevice())
                        << std::endl;
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

    BRAYNS_INFO << "Initializing renderers" << std::endl;

    _createRenderers();

    const auto ospFlags = _getOSPDataFlags();

    BRAYNS_INFO << "Initializing scene" << std::endl;
    _scene = std::make_shared<OSPRayScene>(_parametersManager, ospFlags);

    BRAYNS_INFO << "Initializing camera" << std::endl;
    _createCameras();

    _renderer->setScene(_scene);
    _renderer->setCamera(_camera);

    BRAYNS_INFO << "Engine initialization complete" << std::endl;
}

OSPRayEngine::~OSPRayEngine()
{
    _scene.reset();
    _frameBuffers.clear();
    _renderer.reset();
    _camera.reset();

    ospShutdown();
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

    for (auto frameBuffer : _frameBuffers)
    {
        auto osprayFrameBuffer =
            std::static_pointer_cast<OSPRayFrameBuffer>(frameBuffer);
        const auto& streamParams = _parametersManager.getStreamParameters();

        if (haveDeflectPixelOp())
            osprayFrameBuffer->enableDeflectPixelOp();

        if (streamParams.isModified())
        {
            osprayFrameBuffer->setStreamingParams(streamParams);
        }
    }
}

Vector2ui OSPRayEngine::getMinimumFrameSize() const
{
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
        if (renderer == "pathtracing")
        {
            properties.setProperty(
                {"shadows", "Shadow intensity", 0., {0., 1.}});
            properties.setProperty(
                {"softShadows", "Shadow softness", 0., {0., 1.}});
        }
        if (renderer == "proximity")
        {
            properties.setProperty(
                {"alphaCorrection", "Alpha correction", 0.5, {0.001, 1.}});
            properties.setProperty(
                {"detectionDistance", "Detection distance", 1.});
            properties.setProperty({"detectionFarColor", "Detection far color",
                                    std::array<double, 3>{{1., 0., 0.}}});
            properties.setProperty({"detectionNearColor",
                                    "Detection near color",
                                    std::array<double, 3>{{0., 1., 0.}}});
            properties.setProperty({"detectionOnDifferentMaterial",
                                    "Detection on different material", false});
            properties.setProperty(
                {"electronShadingEnabled", "Electron shading", false});
            properties.setProperty(
                {"surfaceShadingEnabled", "Surface shading", true});
        }
        if (renderer == "basic_simulation")
        {
            properties.setProperty(
                {"alphaCorrection", "Alpha correction", 0.5, {0.001, 1.}});
        }
        if (renderer == "advanced_simulation")
        {
            properties.setProperty(
                {"aoDistance", "Ambient occlusion distance", 10000.});
            properties.setProperty(
                {"aoWeight", "Ambient occlusion weight", 0., {0., 1.}});
            properties.setProperty(
                {"detectionDistance", "Detection distance", 15.});
            properties.setProperty(
                {"shading",
                 "Shading",
                 (int)AdvancedSimulationRenderer::Shading::none,
                 {"None", "Diffuse", "Electron"}});
            properties.setProperty(
                {"shadows", "Shadow intensity", 0., {0., 1.}});
            properties.setProperty(
                {"softShadows", "Shadow softness", 0., {0., 1.}});
            properties.setProperty({"samplingThreshold",
                                    "Threshold under which sampling is ignored",
                                    0.001,
                                    {0.001, 1.}});
            properties.setProperty({"volumeSpecularExponent",
                                    "Volume specular exponent",
                                    20.,
                                    {1., 100.}});
            properties.setProperty({"volumeAlphaCorrection",
                                    "Volume alpha correction",
                                    0.5,
                                    {0.001, 1.}});
        }
        if (renderer == "scivis")
        {
            properties.setProperty(
                {"aoDistance", "Ambient occlusion distance", 10000.});
            properties.setProperty(
                {"aoSamples", "Ambient occlusion samples", 1, {0, 128}});
            properties.setProperty({"aoTransparencyEnabled",
                                    "Ambient occlusion transparency", true});
            properties.setProperty(
                {"aoWeight", "Ambient occlusion weight", 0., {0., 1.}});
            properties.setProperty(
                {"oneSidedLighting", "One-sided lighting", true});
            properties.setProperty({"shadowsEnabled", "Shadows", false});
        }
        ospRenderer->setProperties(renderer, properties);
    }
    ospRenderer->setCurrentType(rp.getCurrentRenderer());
    _renderer = ospRenderer;
}

FrameBufferPtr OSPRayEngine::createFrameBuffer(
    const std::string& name, const Vector2ui& frameSize,
    const FrameBufferFormat frameBufferFormat) const
{
    // Use format 'none' for the per-tile streaming to avoid tile readback
    // to the MPI master
    return std::make_shared<OSPRayFrameBuffer>(name, frameSize,
                                               haveDeflectPixelOp()
                                                   ? FrameBufferFormat::none
                                                   : frameBufferFormat);
}

ScenePtr OSPRayEngine::createScene(ParametersManager& parametersManager) const
{
    return std::make_shared<OSPRayScene>(parametersManager, _getOSPDataFlags());
}

CameraPtr OSPRayEngine::createCamera() const
{
    return std::make_shared<OSPRayCamera>();
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
               ? uint32_t(OSP_DATA_SHARED_BUFFER)
               : 0;
}

void OSPRayEngine::_createCameras()
{
    auto ospCamera = std::make_shared<OSPRayCamera>();

    const bool isStereo =
        _parametersManager.getApplicationParameters().isStereo();
    PropertyMap::Property stereoProperty{"stereo", "Stereo", isStereo};
    stereoProperty.markReadOnly();
    PropertyMap::Property fovy{"fovy", "Field of view", 45., {.1, 360.}};
    PropertyMap::Property aspect{"aspect", "Aspect ratio", 1.};
    aspect.markReadOnly();
    PropertyMap::Property eyeSeparation{"interpupillaryDistance",
                                        "Eye separation", 0.0635};

    RenderingParameters& rp = _parametersManager.getRenderingParameters();
    for (const auto& camera : rp.getCameras())
    {
        PropertyMap properties;
        if (camera == "perspective")
        {
            properties.setProperty(fovy);
            properties.setProperty(aspect);
            properties.setProperty({"apertureRadius", "Aperture radius", 0.});
            properties.setProperty({"focusDistance", "Focus Distance", 1.});
            if (isStereo)
            {
                properties.setProperty(stereoProperty);
                properties.setProperty(eyeSeparation);
            }
        }
        if (camera == "orthographic")
        {
            properties.setProperty({"height", "Height", 1.});
            properties.setProperty(aspect);
        }
        if (camera == "stereoFull")
        {
            properties.setProperty(fovy);
            properties.setProperty(aspect);
            if (isStereo)
            {
                properties.setProperty(stereoProperty);
                properties.setProperty(eyeSeparation);
                properties.setProperty(
                    {"zeroParallaxPlane", "Zero parallax plane", 1.});
            }
        }
        ospCamera->setProperties(camera, properties);
    }
    ospCamera->setCurrentType(rp.getCameraType());
    ospCamera->createOSPCamera();
    _camera = ospCamera;

    _camera->setEnvironmentMap(
        !_parametersManager.getSceneParameters().getEnvironmentMap().empty());
}
}
