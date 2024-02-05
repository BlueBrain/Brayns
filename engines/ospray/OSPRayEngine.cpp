/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/Log.h>
#include <brayns/parameters/ParametersManager.h>

#include "OSPRayCamera.h"
#include "OSPRayFrameBuffer.h"
#include "OSPRayMaterial.h"
#include "OSPRayRenderer.h"
#include "OSPRayScene.h"

#include <ospray/OSPConfig.h>                    // TILE_SIZE
#include <ospray/SDK/camera/PerspectiveCamera.h> // enum StereoMode
#include <ospray/version.h>

namespace brayns
{
OSPRayEngine::OSPRayEngine(ParametersManager& parametersManager)
    : Engine(parametersManager)
{
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
        Log::error("Error during ospInit(): {}.", e.what());
    }

    _createRenderers();

    _scene = std::make_shared<OSPRayScene>(
        _parametersManager.getAnimationParameters(),
        _parametersManager.getVolumeParameters());

    _createCameras();

    _renderer->setScene(_scene);
    _renderer->setCamera(_camera);
}

OSPRayEngine::~OSPRayEngine()
{
    _scene.reset();
    _frameBuffers.clear();
    _renderer.reset();
    _camera.reset();

    ospShutdown();
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

            Log::info("Using {} load balancer.",
                      useDynamicLoadBalancer ? "dynamic" : "static");
        }
    }
}

Vector2ui OSPRayEngine::getMinimumFrameSize() const
{
    return {TILE_SIZE, TILE_SIZE};
}

void OSPRayEngine::_createRenderers()
{
    _renderer = std::make_shared<OSPRayRenderer>(
        _parametersManager.getAnimationParameters(),
        _parametersManager.getRenderingParameters());

    addRendererType("raycast_Ng");
    addRendererType("raycast_Ns");

    {
        PropertyMap properties;
        properties.add({"rouletteDepth", 5, {"Roulette depth"}});
        properties.add({"maxContribution", 100000.0, {"Max contribution"}});

        addRendererType("pathtracer", properties);
    }
    {
        PropertyMap properties;
        properties.add({"aoDistance", 10000., {"Ambient occlusion distance"}});
        properties.add({"aoSamples", 1, {"Ambient occlusion samples"}});
        properties.add({"aoTransparencyEnabled",
                        true,
                        {"Ambient occlusion transparency"}});
        properties.add({"aoWeight", 0., {"Ambient occlusion weight"}});
        properties.add({"oneSidedLighting", true, {"One-sided lighting"}});
        properties.add({"shadowsEnabled", false, {"Shadows"}});

        addRendererType("scivis", properties);
    }

    addRendererType("basic");
}

FrameBufferPtr OSPRayEngine::createFrameBuffer(
    const std::string& name, const Vector2ui& frameSize,
    const PixelFormat frameBufferFormat) const
{
    return std::make_shared<OSPRayFrameBuffer>(name, frameSize,
                                               frameBufferFormat);
}

ScenePtr OSPRayEngine::createScene(AnimationParameters& animationParameters,
                                   VolumeParameters& volumeParameters) const
{
    return std::make_shared<OSPRayScene>(animationParameters, volumeParameters);
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

void OSPRayEngine::_createCameras()
{
    _camera = std::make_shared<OSPRayCamera>();

    Property fovy{"fovy", 45., {"Field of view"}};
    Property aspect{"aspect", 1., {"Aspect ratio"}};
    aspect.setReadOnly(true);
    Property eyeSeparation{"interpupillaryDistance",
                           0.0635,
                           {"Eye separation"}};
    Property enableClippingPlanes{"enableClippingPlanes", true, {"Clipping"}};

    {
        PropertyMap properties;
        properties.add(fovy);
        properties.add(aspect);
        properties.add({"apertureRadius", 0., {"Aperture radius"}});
        properties.add({"focusDistance", 1., {"Focus Distance"}});
        properties.add(enableClippingPlanes);
        addCameraType("perspective", properties);
    }
    {
        PropertyMap properties;
        properties.add({"height", 1., {"Height"}});
        properties.add(aspect);
        properties.add(enableClippingPlanes);
        addCameraType("orthographic", properties);
    }
    {
        PropertyMap properties;
        properties.add(fovy);
        properties.add(aspect);
        properties.add(enableClippingPlanes);
        addCameraType("perspectiveParallax", properties);
    }
    {
        PropertyMap properties;
        properties.add(enableClippingPlanes);
        properties.add({"half", true, {"Half sphere"}});
        addCameraType("panoramic", properties);
    }
    {
        PropertyMap properties;
        properties.add(fovy);
        properties.add(aspect);
        properties.add({"apertureRadius", 0., {"Aperture radius"}});
        properties.add({"focusDistance", 1., {"Focus Distance"}});
        properties.add(enableClippingPlanes);
        addCameraType("fisheye", properties);
    }
}
} // namespace brayns

extern "C" brayns::Engine* brayns_engine_create(
    int, const char**, brayns::ParametersManager& parametersManager)
{
    return new brayns::OSPRayEngine(parametersManager);
}
