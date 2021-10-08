/* Copyright (c) 2020, EPFL/Blue Brain Project
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

#include "PBRTEngine.h"
#include "PBRTCamera.h"
#include "PBRTConstants.h"
#include "PBRTFrameBuffer.h"
#include "PBRTRenderer.h"
#include "PBRTScene.h"

#include <brayns/parameters/ParametersManager.h>

#include <pbrt/core/parallel.h>

#include <thread>

namespace brayns
{
PBRTEngine::PBRTEngine(ParametersManager &paramsManager)
    : Engine(paramsManager)
{
    google::InitGoogleLogging("BraynsPBRTEngine");
    FLAGS_stderrthreshold = 1;

    // Leave 2 threads free to handle clients while rendering
    pbrt::PbrtOptions.nThreads =
        static_cast<int>(std::thread::hardware_concurrency()) - 2;
    pbrt::SampledSpectrum::Init();

    // Create the camera (and the film, a.k.a. framebuffer)
    _createCameras();

    // Create the scene
    _scene = createScene(paramsManager.getAnimationParameters(),
                         paramsManager.getGeometryParameters(),
                         paramsManager.getVolumeParameters());

    // Make the integrators available
    _createRenderers();
    _renderer->setScene(_scene);
    _renderer->setCamera(_camera);

    pbrt::ParallelInit();
}

PBRTEngine::~PBRTEngine()
{
    pbrt::ParallelCleanup();
}

void PBRTEngine::commit()
{
    Engine::commit();
}

Vector2ui PBRTEngine::getMinimumFrameSize() const
{
    return {64, 64};
}

FrameBufferPtr PBRTEngine::createFrameBuffer(
    const std::string &name, const Vector2ui &frameSize,
    FrameBufferFormat frameBufferFormat) const
{
    return std::shared_ptr<PBRTFrameBuffer>(
        new PBRTFrameBuffer(name, frameSize, frameBufferFormat));
}

ScenePtr PBRTEngine::createScene(AnimationParameters &animationParameters,
                                 GeometryParameters &geometryParameters,
                                 VolumeParameters &volumeParameters) const
{
    return std::shared_ptr<PBRTScene>(new PBRTScene(animationParameters,
                                                    geometryParameters,
                                                    volumeParameters));
}

CameraPtr PBRTEngine::createCamera() const
{
    return std::make_shared<PBRTCamera>();
}

RendererPtr PBRTEngine::createRenderer(
    const AnimationParameters &animationParameters,
    const RenderingParameters &renderingParameters) const
{
    return std::shared_ptr<PBRTRenderer>(
        new PBRTRenderer(animationParameters, renderingParameters));
}

void PBRTEngine::_createCameras()
{
    _camera = createCamera();

    Property shutteropen{PBRT_CAMPROP_SHUTTEROPEN, 0., {"Shutter open value"}};
    Property shutterclose{PBRT_CAMPROP_SHUTTERCLOSE,
                          1.,
                          {"Shutter close value"}};

    Property filterType{PBRT_FILTERPROP_TYPE,
                        PBRT_FILTERTYPE_BOX,
                        {"Image filter type"}};
    Property filterXWidth{PBRT_FILTERPROP_XWIDTH,
                          0.5,
                          {"Image filter normalized width on X axis (value "
                           "depends on filter type)"}};
    Property filterYWidth{PBRT_FILTERPROP_YWIDTH,
                          0.5,
                          {"Image filter normalized width on Y axis (value "
                           "depends on filter type)"}};
    Property filterAlpha{PBRT_FILTERPROP_ALPHA,
                         2.,
                         {"Image filter alpha value (only gaussian filter)"}};
    Property filterB{PBRT_FILTERPROP_B,
                     1. / 3.,
                     {"Image filter B parameter (only mitchell filter)"}};
    Property filterC{PBRT_FILTERPROP_C,
                     1. / 3.,
                     {"Image filter C parameter (only mitchell filter)"}};
    Property filterTau{PBRT_FILTERPROP_TAU,
                       3.,
                       {"Image filter TAU value (only sync filter)"}};

    // Property filmXRes{PBRT_FILMPROP_XRESOLUTION, 1920, {"PBRT Framebuffer
    // horizontal resolution"}}; Property filmYRes{PBRT_FILMPROP_YRESOLUTION,
    // 1080, {"PBRT Framebuffer vertical resolution"}};
    Property filmScale{PBRT_FILMPROP_SCALE, 1., {"Framebuffer scale"}};
    Property filmDiagonal{PBRT_FILMPROP_DIAGONAL,
                          35.,
                          {"Framebuffer diagonal"}};
    Property filmMaxSampling{PBRT_FILMPROP_MAXSAMPLELUMINANCE,
                             999999.9,
                             {"Framebuffer luminance value limit"}};

    {
        PropertyMap properties;
        properties.setProperty(shutteropen);
        properties.setProperty(shutterclose);
        properties.setProperty(filterType);
        properties.setProperty(filterXWidth);
        properties.setProperty(filterYWidth);
        properties.setProperty(filterAlpha);
        properties.setProperty(filterB);
        properties.setProperty(filterC);
        properties.setProperty(filterTau);
        properties.setProperty(filmScale);
        properties.setProperty(filmDiagonal);
        properties.setProperty(filmMaxSampling);
        properties.setProperty({PBRT_CAMPROP_LENSRADIUS, 0., {"Lense radius"}});
        properties.setProperty(
            {PBRT_CAMPROP_FOCUSDISTANCE, 1e6, {"Focus Distance"}});
        properties.setProperty(
            {PBRT_CAMPROP_FRAMEASPECTRATIO, 1., {"Frame aspect ratio"}});
        properties.setProperty({PBRT_CAMPROP_FOV, 45., {"Field of view"}});
        properties.setProperty(
            {PBRT_CAMPROP_HALFFOV, -1., {"Half field of view"}});
        addCameraType(PBRT_CAMTYPE_PERSPECTIVE, properties);
    }
    {
        PropertyMap properties;
        properties.setProperty(shutteropen);
        properties.setProperty(shutterclose);
        properties.setProperty(filterType);
        properties.setProperty(filterXWidth);
        properties.setProperty(filterYWidth);
        properties.setProperty(filterAlpha);
        properties.setProperty(filterB);
        properties.setProperty(filterC);
        properties.setProperty(filterTau);
        properties.setProperty(filmScale);
        properties.setProperty(filmDiagonal);
        properties.setProperty(filmMaxSampling);
        properties.setProperty({PBRT_CAMPROP_LENSRADIUS, 0., {"Lense radius"}});
        properties.setProperty(
            {PBRT_CAMPROP_FOCUSDISTANCE, 1e6, {"Focus Distance"}});
        properties.setProperty(
            {PBRT_CAMPROP_FRAMEASPECTRATIO, 1., {"Frame aspect ratio"}});
        addCameraType(PBRT_CAMTYPE_ORTOGRAPHIC, properties);
    }
    {
        PropertyMap properties;
        properties.setProperty(shutteropen);
        properties.setProperty(shutterclose);
        properties.setProperty(filterType);
        properties.setProperty(filterXWidth);
        properties.setProperty(filterYWidth);
        properties.setProperty(filterAlpha);
        properties.setProperty(filterB);
        properties.setProperty(filterC);
        properties.setProperty(filterTau);
        properties.setProperty(filmScale);
        properties.setProperty(filmDiagonal);
        properties.setProperty(filmMaxSampling);
        properties.setProperty({PBRT_CAMPROP_LENSRADIUS, 0., {"Lense radius"}});
        properties.setProperty(
            {PBRT_CAMPROP_FOCUSDISTANCE, 1e6, {"Focus Distance"}});
        properties.setProperty(
            {PBRT_CAMPROP_FRAMEASPECTRATIO, 1., {"Frame aspect ratio"}});
        addCameraType(PBRT_CAMTYPE_ENVIRONMENT, properties);
    }
    {
        PropertyMap properties;
        properties.setProperty(shutteropen);
        properties.setProperty(shutterclose);
        properties.setProperty(filterType);
        properties.setProperty(filterXWidth);
        properties.setProperty(filterYWidth);
        properties.setProperty(filterAlpha);
        properties.setProperty(filterB);
        properties.setProperty(filterC);
        properties.setProperty(filterTau);
        properties.setProperty(filmScale);
        properties.setProperty(filmDiagonal);
        properties.setProperty(filmMaxSampling);
        properties.setProperty(
            {PBRT_CAMPROP_LENSFILE, std::string(""), {"Lense map file path"}});
        properties.setProperty(
            {PBRT_CAMPROP_FOCALDISTANCE, 10., {"Focal distance"}});
        properties.setProperty(
            {PBRT_CAMPROP_APERTUREDIAMETER, 1., {"Apperture diameter"}});
        properties.setProperty(
            {PBRT_CAMPROP_SIMPLEWEIGHTING, true, {"Enable simple weightings"}});
        addCameraType(PBRT_CAMTYPE_REALISTIC, properties);
    }
}

void PBRTEngine::_createRenderers()
{
    _renderer = createRenderer(_parametersManager.getAnimationParameters(),
                               _parametersManager.getRenderingParameters());

    Property samplerType{PBRT_SAMPLERPROP_TYPE,
                         PBRT_SAMPLER_HALTON,
                         {"Type of numeric sampler to use"}};
    Property samplerPixelSamples{PBRT_SAMPLERPROP_PIXELSAMPLES,
                                 16,
                                 {"Number of samples per pixel"}};
    Property samplerDimensions{PBRT_SAMPLERPROP_DIMENSIONS,
                               4,
                               {"Sampler dimension"}};
    Property samplerSampleCenter{
        PBRT_SAMPLERPROP_SAMPLEPIXELCENTER,
        false,
        {"Whether to sample center of pixel (only halton sampler)"}};
    Property samplerJitter{
        PBRT_SAMPLERPROP_JITTER,
        true,
        {"Wether to jitter sampling (only stratified sampler)"}};
    Property samplerXSamples{
        PBRT_SAMPLERPROP_XSAMPLES,
        4,
        {"Number of horizontal samples (only stratified sampler)"}};
    Property samplerYSamples{
        PBRT_SAMPLERPROP_YSAMPLES,
        4,
        {"Number of vertical samples (only stratified sampler)"}};

    {
        // AMBIENT OCCLUSION INTEGRATOR
        PropertyMap properties;
        properties.setProperty(samplerType);
        properties.setProperty(samplerPixelSamples);
        properties.setProperty(samplerDimensions);
        properties.setProperty(samplerSampleCenter);
        properties.setProperty(samplerJitter);
        properties.setProperty(samplerXSamples);
        properties.setProperty(samplerYSamples);
        properties.setProperty(
            {PBRT_INTEGRATORPROP_COSSAMPLE, true, {"Use cosine sampling"}});
        properties.setProperty(
            {PBRT_INTEGRATORPROP_NSAMPLES, 64, {"Number of samples"}});
        addRendererType(PBRT_INTEGRATOR_AMBIENTOCCLUSION, properties);
    }

    {
        // BIDIRECTIONAL PATH TRACER INTEGRATOR
        PropertyMap properties;
        properties.setProperty(samplerType);
        properties.setProperty(samplerPixelSamples);
        properties.setProperty(samplerDimensions);
        properties.setProperty(samplerSampleCenter);
        properties.setProperty(samplerJitter);
        properties.setProperty(samplerXSamples);
        properties.setProperty(samplerYSamples);
        properties.setProperty(
            {PBRT_INTEGRATORPROP_MAXDEPTH, 5, {"Max ray depth"}});
        properties.setProperty({PBRT_INTEGRATORPROP_VISUALIZESTRATEGIES,
                                false,
                                {"Render sampling strategies"}});
        properties.setProperty({PBRT_INTEGRATORPROP_VISUALIZEWEIGHTS,
                                false,
                                {"Render weight distribution"}});
        properties.setProperty({PBRT_INTEGRATORPROP_LIGHTSAMPLINGSTRATEGY,
                                std::string("power"),
                                {"Light sampling strategy"}});
        addRendererType(PBRT_INTEGRATOR_BIDIRPATHTRACER, properties);
    }

    {
        // DIRECT LIGHTING INTEGRATOR
        PropertyMap properties;
        properties.setProperty(samplerType);
        properties.setProperty(samplerPixelSamples);
        properties.setProperty(samplerDimensions);
        properties.setProperty(samplerSampleCenter);
        properties.setProperty(samplerJitter);
        properties.setProperty(samplerXSamples);
        properties.setProperty(samplerYSamples);
        properties.setProperty(
            {PBRT_INTEGRATORPROP_MAXDEPTH, 5, {"Max ray depth"}});
        properties.setProperty({PBRT_INTEGRATORPROP_LIGHTSTRATEGY,
                                std::string("all"),
                                {"Light strategy"}});
        addRendererType(PBRT_INTEGRATOR_DIRECTLIGHTING, properties);
    }

    {
        // METROPOLIS LIGHT TRANSPORT INTEGRATOR
        PropertyMap properties;
        properties.setProperty(samplerType);
        properties.setProperty(samplerPixelSamples);
        properties.setProperty(samplerDimensions);
        properties.setProperty(samplerSampleCenter);
        properties.setProperty(samplerJitter);
        properties.setProperty(samplerXSamples);
        properties.setProperty(samplerYSamples);
        properties.setProperty(
            {PBRT_INTEGRATORPROP_MAXDEPTH, 5, {"Max ray depth"}});
        properties.setProperty({PBRT_INTEGRATORPROP_BOOTSTRAPSAMPLES,
                                100000,
                                {"Number of bootstrap samples"}});
        properties.setProperty(
            {PBRT_INTEGRATORPROP_CHAINS,
             1000,
             {"Max number of unique Markov chains to follow"}});
        properties.setProperty({PBRT_INTEGRATORPROP_MUTATIONSPERPIXEL,
                                100,
                                {"Number of mutations per pixel"}});
        properties.setProperty(
            {PBRT_INTEGRATORPROP_LARGESTEPPROBABILITY,
             0.3,
             {"Normalized large step probability discard chance"}});
        properties.setProperty({PBRT_INTEGRATORPROP_SIGMA,
                                0.01,
                                {"Standard deviation of the perturbation "
                                 "applied to random samplers"}});
        addRendererType(PBRT_INTEGRATOR_METROPOLIS, properties);
    }

    {
        // PATH TRACER INTEGRATOR
        PropertyMap properties;
        properties.setProperty(samplerType);
        properties.setProperty(samplerPixelSamples);
        properties.setProperty(samplerDimensions);
        properties.setProperty(samplerSampleCenter);
        properties.setProperty(samplerJitter);
        properties.setProperty(samplerXSamples);
        properties.setProperty(samplerYSamples);
        properties.setProperty(
            {PBRT_INTEGRATORPROP_MAXDEPTH, 5, {"Max ray depth"}});
        properties.setProperty({PBRT_INTEGRATORPROP_RRTHRESHOLD,
                                1.,
                                {"Russian roulette threshold"}});
        properties.setProperty({PBRT_INTEGRATORPROP_LIGHTSAMPLINGSTRATEGY,
                                std::string("spatial"),
                                {"Light sampling strategy"}});
        addRendererType(PBRT_INTEGRATOR_PATHTRACER, properties);
    }

    {
        // STOCHASTIC PROGRESSIVE PHOTON MAPPING INTEGRATOR
        PropertyMap properties;
        properties.setProperty(samplerType);
        properties.setProperty(samplerPixelSamples);
        properties.setProperty(samplerDimensions);
        properties.setProperty(samplerSampleCenter);
        properties.setProperty(samplerJitter);
        properties.setProperty(samplerXSamples);
        properties.setProperty(samplerYSamples);
        properties.setProperty(
            {PBRT_INTEGRATORPROP_MAXDEPTH, 5, {"Max ray depth"}});
        properties.setProperty({PBRT_INTEGRATORPROP_NUMITERATIONS,
                                64,
                                {"Number of interations to perform"}});
        properties.setProperty(
            {PBRT_INTEGRATORPROP_PHOTONSPERITERATION,
             -1,
             {"Number of photons to emit per iteration (-1 is default"}});
        properties.setProperty({PBRT_INTEGRATORPROP_IMAGEWRITEFREQUENCY,
                                1 << 31,
                                {"Write to frame buffer frequency (after X "
                                 "amount of emitted photons"}});
        properties.setProperty(
            {PBRT_INTEGRATORPROP_RADIUS, 1., {"Initial photon search radius"}});
        addRendererType(PBRT_INTEGRATOR_SPROGRESSIVEPHOTOMMAP, properties);
    }

    {
        // VOLUMETRIC PATH TRACER INTEGRATOR
        PropertyMap properties;
        properties.setProperty(samplerType);
        properties.setProperty(samplerPixelSamples);
        properties.setProperty(samplerDimensions);
        properties.setProperty(samplerSampleCenter);
        properties.setProperty(samplerJitter);
        properties.setProperty(samplerXSamples);
        properties.setProperty(samplerYSamples);
        properties.setProperty(
            {PBRT_INTEGRATORPROP_MAXDEPTH, 5, {"Max ray depth"}});
        properties.setProperty({PBRT_INTEGRATORPROP_RRTHRESHOLD,
                                1.,
                                {"Russian roulette threshold"}});
        properties.setProperty({PBRT_INTEGRATORPROP_LIGHTSAMPLINGSTRATEGY,
                                std::string("spatial"),
                                {"Light sampling strategy"}});
        addRendererType(PBRT_INTEGRATOR_VOLUMETRICPATHTRACER, properties);
    }

    {
        // WHITTED INTEGRATOR
        PropertyMap properties;
        properties.setProperty(samplerType);
        properties.setProperty(samplerPixelSamples);
        properties.setProperty(samplerDimensions);
        properties.setProperty(samplerSampleCenter);
        properties.setProperty(samplerJitter);
        properties.setProperty(samplerXSamples);
        properties.setProperty(samplerYSamples);
        properties.setProperty(
            {PBRT_INTEGRATORPROP_MAXDEPTH, 5, {"Max ray depth"}});
        addRendererType(PBRT_INTEGRATOR_WHITTED, properties);
    }

    {
        // DEBUG INTEGRATOR
        PropertyMap properties;
        properties.setProperty(samplerType);
        properties.setProperty(samplerPixelSamples);
        properties.setProperty(samplerDimensions);
        properties.setProperty(samplerSampleCenter);
        properties.setProperty(samplerJitter);
        properties.setProperty(samplerXSamples);
        properties.setProperty(samplerYSamples);
        addRendererType(PBRT_INTEGRATOR_DEBUG, properties);
    }
}
} // namespace brayns

extern "C" brayns::Engine *brayns_engine_create(
    int, const char **, brayns::ParametersManager &parametersManager)
{
    return new brayns::PBRTEngine(parametersManager);
}
