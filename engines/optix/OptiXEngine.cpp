/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include <engines/optix/braynsOptixEngine_generated_AdvancedSimulation.cu.ptx.h>
#include <engines/optix/braynsOptixEngine_generated_BBP.cu.ptx.h>
#include <engines/optix/braynsOptixEngine_generated_BasicSimulation.cu.ptx.h>
#include <engines/optix/braynsOptixEngine_generated_PBR.cu.ptx.h>

#include <brayns/common/input/KeyboardHandler.h>
#include <brayns/parameters/ParametersManager.h>

#include "OptiXCamera.h"
#include "OptiXEngine.h"
#include "OptiXFrameBuffer.h"
#include "OptiXOpenDeckCamera.h"
#include "OptiXPerspectiveCamera.h"
#include "OptiXRenderer.h"
#include "OptiXScene.h"

namespace brayns
{
OptiXEngine::OptiXEngine(ParametersManager& parametersManager)
    : Engine(parametersManager)
{
    BRAYNS_INFO << "Initializing OptiX" << std::endl;
    _initializeContext();

    BRAYNS_INFO << "Initializing scene" << std::endl;
    _scene = std::make_shared<OptiXScene>(
        _parametersManager.getAnimationParameters(),
        _parametersManager.getGeometryParameters(),
        _parametersManager.getVolumeParameters());

    BRAYNS_INFO << "Initializing renderers" << std::endl;
    _createRenderers();

    BRAYNS_INFO << "Initializing cameras" << std::endl;
    _createCameras();

    BRAYNS_INFO << "Engine initialization complete" << std::endl;
}

OptiXEngine::~OptiXEngine()
{
    _scene.reset();
    for (auto& fb : _frameBuffers)
        fb.reset();
    _renderer.reset();
    _camera.reset();

    _frameBuffers.clear();
}

void OptiXEngine::_initializeContext()
{
    // Set up context
    auto context = OptiXContext::get().getOptixContext();
    if (!context)
        BRAYNS_THROW(std::runtime_error("Failed to initialize OptiX"));
}

void OptiXEngine::_createCameras()
{
    _camera = createCamera();

    const bool isStereo =
        _parametersManager.getApplicationParameters().isStereo();
    Property stereoProperty{"stereo", isStereo, {"Stereo"}};
    stereoProperty.markReadOnly();
    Property fovy{"fovy", 45., .1, 360., {"Field of view"}};
    Property aspect{"aspect", 1., {"Aspect ratio"}};
    aspect.markReadOnly();
    Property eyeSeparation{"interpupillaryDistance",
                           0.0635,
                           {"Eye separation"}};

    OptiXContext& context = OptiXContext::get();

    auto camera = std::make_shared<OptiXPerspectiveCamera>();

    PropertyMap properties;
    properties.setProperty(fovy);
    properties.setProperty(aspect);
    properties.setProperty({"apertureRadius", 0., {"Aperture radius"}});
    properties.setProperty({"focusDistance", 1., {"Focus Distance"}});
    properties.setProperty({"height", 1., {"Height"}});
    if (isStereo)
    {
        properties.setProperty(stereoProperty);
        properties.setProperty(eyeSeparation);
        properties.setProperty(
            {"zeroParallaxPlane", 1., {"Zero parallax plane"}});
    }

    context.addCamera("perspective", camera);
    addCameraType("perspective", properties);

    {
        PropertyMap properties;
        properties.setProperty({"segmentId", 7});
        properties.setProperty(
            {"interpupillaryDistance", 0.065, {"Eye separation"}});
        properties.setProperty(
            {"headPosition", std::array<double, 3>{{0.0, 2.0, 0.0}}});
        properties.setProperty(
            {"headRotation", std::array<double, 4>{{0.0, 0.0, 0.0, 1.0}}});
        context.addCamera("opendeck", std::make_shared<OptiXOpenDeckCamera>());
        addCameraType("opendeck", properties);
    }
}

void OptiXEngine::_createRenderers()
{
    _renderer = std::make_shared<OptiXRenderer>(
        _parametersManager.getAnimationParameters(),
        _parametersManager.getRenderingParameters());
    _renderer->setScene(_scene);

    { // Advanced renderer
        const std::string CUDA_ADVANCED_SIMULATION =
            braynsOptixEngine_generated_AdvancedSimulation_cu_ptx;

        OptiXContext& context = OptiXContext::get();

        auto osp = std::make_shared<OptixShaderProgram>();
        osp->closest_hit =
            context.getOptixContext()->createProgramFromPTXString(
                CUDA_ADVANCED_SIMULATION, "closest_hit_radiance");
        osp->closest_hit_textured =
            context.getOptixContext()->createProgramFromPTXString(
                CUDA_ADVANCED_SIMULATION, "closest_hit_radiance_textured");
        osp->any_hit = context.getOptixContext()->createProgramFromPTXString(
            CUDA_ADVANCED_SIMULATION, "any_hit_shadow");
        // Exception program
        osp->exception_program =
            context.getOptixContext()->createProgramFromPTXString(
                CUDA_ADVANCED_SIMULATION, "exception");
        context.getOptixContext()->setExceptionProgram(0,
                                                       osp->exception_program);
        context.getOptixContext()["bad_color"]->setFloat(1.0f, 0.0f, 0.0f);

        context.addRenderer("advanced_simulation", osp);

        PropertyMap properties;
        properties.setProperty({"shadingEnabled", true, {"Shading enabled"}});
        properties.setProperty(
            {"electronShadingEnabled", true, {"Electron shading enabled"}});
        properties.setProperty({"shadows", 0., 0., 1., {"Shadow strength"}});
        properties.setProperty(
            {"softShadows", 0., 0., 1., {"Soft shadow strength"}});
        properties.setProperty({"ambientOcclusionStrength",
                                0.,
                                0.,
                                1.,
                                {"Ambient occlusion strength"}});
        properties.setProperty(
            {"maxDepth", 3, 1, 20, {"Max ray recursion depth"}});

        addRendererType("advanced_simulation", properties);
    }

    { // Basic simulation / Basic renderer
        const std::string CUDA_BASIC_SIMULATION_RENDERER =
            braynsOptixEngine_generated_BasicSimulation_cu_ptx;
        OptiXContext& context = OptiXContext::get();

        auto osp = std::make_shared<OptixShaderProgram>();
        osp->closest_hit =
            context.getOptixContext()->createProgramFromPTXString(
                CUDA_BASIC_SIMULATION_RENDERER, "closest_hit_radiance");
        osp->closest_hit_textured =
            context.getOptixContext()->createProgramFromPTXString(
                CUDA_BASIC_SIMULATION_RENDERER,
                "closest_hit_radiance_textured");
        osp->any_hit = context.getOptixContext()->createProgramFromPTXString(
            CUDA_BASIC_SIMULATION_RENDERER, "any_hit_shadow");

        context.addRenderer("basic_simulation", osp);
        addRendererType("basic_simulation");

        context.addRenderer("basic", osp);
        addRendererType("basic");
    }

    { // PBR renderer
        const std::string CUDA_PBR = braynsOptixEngine_generated_PBR_cu_ptx;

        OptiXContext& context = OptiXContext::get();

        auto osp = std::make_shared<OptixShaderProgram>();
        osp->closest_hit =
            context.getOptixContext()->createProgramFromPTXString(
                CUDA_PBR, "closest_hit_radiance");
        osp->closest_hit_textured =
            context.getOptixContext()->createProgramFromPTXString(
                CUDA_PBR, "closest_hit_radiance");

        osp->any_hit = context.getOptixContext()->createProgramFromPTXString(
            CUDA_PBR, "any_hit_shadow");

        context.addRenderer("pbr", osp);

        addRendererType("pbr");
    }

    { // BBP renderer
        const std::string CUDA_BBP = braynsOptixEngine_generated_BBP_cu_ptx;

        OptiXContext& context = OptiXContext::get();

        auto osp = std::make_shared<OptixShaderProgram>();
        osp->closest_hit =
            context.getOptixContext()->createProgramFromPTXString(
                CUDA_BBP, "closest_hit_radiance");
        osp->closest_hit_textured =
            context.getOptixContext()->createProgramFromPTXString(
                CUDA_BBP, "closest_hit_radiance");
        osp->any_hit = context.getOptixContext()->createProgramFromPTXString(
            CUDA_BBP, "any_hit_shadow");

        context.addRenderer("bbp", osp);

        addRendererType("bbp");
    }
}

ScenePtr OptiXEngine::createScene(AnimationParameters& animationParameters,
                                  GeometryParameters& geometryParameters,
                                  VolumeParameters& volumeParameters) const
{
    return std::make_shared<OptiXScene>(animationParameters, geometryParameters,
                                        volumeParameters);
}

FrameBufferPtr OptiXEngine::createFrameBuffer(
    const std::string& name, const Vector2ui& frameSize,
    FrameBufferFormat frameBufferFormat) const
{
    return std::make_shared<OptiXFrameBuffer>(name, frameSize,
                                              frameBufferFormat);
}

RendererPtr OptiXEngine::createRenderer(
    const AnimationParameters& animationParameters,
    const RenderingParameters& renderingParameters) const
{
    return std::make_shared<OptiXRenderer>(animationParameters,
                                           renderingParameters);
}

CameraPtr OptiXEngine::createCamera() const
{
    return std::make_shared<OptiXCamera>();
}

void OptiXEngine::commit()
{
    Engine::commit();
}

Vector2ui OptiXEngine::getMinimumFrameSize() const
{
    return {1, 1};
}
} // namespace brayns

extern "C" brayns::Engine* brayns_engine_create(
    int, const char**, brayns::ParametersManager& parametersManager)
{
    return new brayns::OptiXEngine(parametersManager);
}
