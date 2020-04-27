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

#include "PBRTCamera.h"
#include "PBRTConstants.h"
#include "PBRTEngine.h"
#include "PBRTFrameBuffer.h"
#include "PBRTRenderer.h"
#include "PBRTScene.h"

#include <brayns/parameters/ParametersManager.h>

#include <pbrtv2/core/parallel.h>

#include <thread>

namespace brayns
{
PBRTEngine::PBRTEngine(ParametersManager& paramsManager)
    : Engine(paramsManager)
{
    // Leave 2 threads free to handle clients while rendering
    pbrt::PbrtOptions.nCores = static_cast<int>(std::thread::hardware_concurrency()) - 2;
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

    pbrt::TasksInit();
}

PBRTEngine::~PBRTEngine()
{
    pbrt::TasksCleanup();
}

void PBRTEngine::commit()
{
    Engine::commit();
}

Vector2ui PBRTEngine::getMinimumFrameSize() const
{
    return {64, 64};
}

FrameBufferPtr PBRTEngine::createFrameBuffer(const std::string &name,
                                             const Vector2ui &frameSize,
                                             FrameBufferFormat frameBufferFormat) const
{
    return std::shared_ptr<PBRTFrameBuffer>(new PBRTFrameBuffer(name,
                                                                frameSize,
                                                                frameBufferFormat));
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

RendererPtr PBRTEngine::createRenderer(const AnimationParameters &animationParameters,
                                       const RenderingParameters &renderingParameters) const
{
    return std::shared_ptr<PBRTRenderer>(new PBRTRenderer(animationParameters,
                                                          renderingParameters));
}

void PBRTEngine::_createCameras()
{
    _camera = createCamera();

    PropertyMap common;
    common.setProperty({PBRT_CAMPROP_SHUTTEROPEN, 0., {"Shutter open value"}});
    common.setProperty({PBRT_CAMPROP_SHUTTERCLOSE, 1., {"Shutter close value"}});
    common.setProperty({PBRT_CAMPROP_LENSRADIUS, 0., {"Lense radius"}});
    common.setProperty({PBRT_CAMPROP_FRAMEASPECTRATIO, 1., {"Frame aspect ratio"}});
    common.setProperty({PBRT_CAMPROP_FOCALDISTANCE, 1e6, {"Focal distance"}});
    const std::array<double, 4> cropWindow = {0.,1.,0.,1.};
    common.setProperty({PBRT_CAMPROP_SCREENWINDOW, cropWindow,{"Window crop"}});

    Property filterType{PBRT_FILTERPROP_TYPE, PBRT_FILTERTYPE_BOX, {"Image filter type"}};
    Property filterXWidth{PBRT_FILTERPROP_XWIDTH, 0.5, {"Image filter normalized width on X axis (value depends on filter type)"}};
    Property filterYWidth{PBRT_FILTERPROP_YWIDTH, 0.5, {"Image filter normalized width on Y axis (value depends on filter type)"}};
    Property filterAlpha{PBRT_FILTERPROP_ALPHA, 2., {"Image filter alpha value (only gaussian filter)"}};
    Property filterB{PBRT_FILTERPROP_B, 1. / 3., {"Image filter B parameter (only mitchell filter)"}};
    Property filterC{PBRT_FILTERPROP_C, 1. / 3., {"Image filter C parameter (only mitchell filter)"}};
    Property filterTau{PBRT_FILTERPROP_TAU, 3., {"Image filter TAU value (only sync filter)"}};

    {
        PropertyMap properties = common;
        properties.setProperty({PBRT_CAMPROP_FOV, 45., {"Field of view"}});
        properties.setProperty({PBRT_CAMPROP_HALFFOV, -1., {"Half field of view"}});
        addCameraType(PBRT_CAMTYPE_PERSPECTIVE, properties);
    }
    {
        addCameraType(PBRT_CAMTYPE_ORTOGRAPHIC, common);
    }
    {
        addCameraType(PBRT_CAMTYPE_ENVIRONMENT, common);
    }
    {
        addCameraType(PBRT_CAMTYPE_TELECENTRIC, common);
    }
}

void PBRTEngine::_createRenderers()
{
    _renderer = createRenderer(_parametersManager.getAnimationParameters(),
                               _parametersManager.getRenderingParameters());

    PropertyMap props;

    // SAMPLER SETTINGS
    props.setProperty({PBRT_SAMPLERPROP_TYPE, PBRT_SAMPLER_HALTON,
                      {"Type of numeric sampler to use"}});
    props.setProperty({PBRT_SAMPLERPROP_PIXELSAMPLES, 4,
                      {"Number of samples per pixel"}});
    props.setProperty({PBRT_SAMPLERPROP_METHOD, std::string("contrast"),
                      {"Sampling method (Adaptive)"}});
    props.setProperty({PBRT_SAMPLERPROP_MINSAMPLES, 4,
                      {"Min number of samples (Adaptive)"}});
    props.setProperty({PBRT_SAMPLERPROP_MAXSAMPLES, 32,
                      {"Max number of samples (Adaptive)"}});
    props.setProperty({PBRT_SAMPLERPROP_JITTER, true,
                      {"Wether to jitter sampling (Stratified)"}});
    props.setProperty({PBRT_SAMPLERPROP_XSAMPLES, 4,
                      {"Number of horizontal samples (Stratified)"}});
    props.setProperty({PBRT_SAMPLERPROP_YSAMPLES, 4,
                      {"Number of vertical samples (Stratified)"}});

    // SURFACE INTEGRATOR PROPERTIES
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_TYPE, PBRT_SURFACE_INTEGRATOR_DEBUG,
                       {"Surface integrator class"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_POINTSFILE, std::string(""),
                       {"Dipole subsurface integrator - path to points file"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_MAXERROR, 0.05,
                       {"Dipole subsurface integrator - max error threshold"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_GATHERSAMPLES, 16,
                       {"Instant GI integrator - num of samples gathering"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_MAXINDIRECTDEPTH, 3,
                       {"Irradiance cache integrator - max indirect path depth"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_NUSED, 50,
                       {"Photon map integrator - Number of photons for density est."}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_LMAX, 4,
                       {"Glossy PRT integrator - Max SH band number"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_FILENAME, std::string(""),
                       {"Radiance probes integrator - path to probes file"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_CAUSTICPHOTONS, 20000,
                       {"Photon map integrator - number of photons for caustics"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_NLIGHTS, 64,
                       {"Instant GI integrator - Number of light paths"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_MAXSPECULARDEPTH, 5,
                       {"Irradiance cache, Photon map - Max specular ray depth"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_GLIMIT, 10.0,
                       {"Instant GI integrator - Max value for G (geometric coupling)"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_MAXPHOTONDEPTH, 5,
                       {"Photon map integrator - Max phton path depth"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_FINALGATHER, true,
                       {"Photon map integrator - Wether to perform final gather pass"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_MINSAMPLEDISTANCE, 0.25,
                       {"Dipole subsurface integrator - minimun distance between samples"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_NSAMPLES, 2048,
                       {"Ambient occlussion, Diffuse PRT, Glossy PRT, Irradiance Cache - "
                        "Number of rays to determine radiance"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_STRATEGY, std::string("all"),
                       {"Direct lighting integrator - Light sampling strategy (all|one)"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_NSETS, 4,
                       {"Instant GI integrator - Num of independent virtual light sets"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_RRTHRESHOLD, 0.0001,
                       {"Instant GI integrator - Russian Roulette threshold"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_GATHERANGLE, 10.0,
                       {"Photon map integrator - Cone spread angle for IS photon gathering"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_MINWEIGHT, 0.5,
                       {"Irradiance cache integrator - Minimun weight to interpolate samples"}});
    const std::array<double, 3> KDparam = {0.5,0.5,0.5};
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_KD, KDparam,
                       {"Glossy prt integrator - Diffuse coefficent"}});
    const std::array<double, 3> KSparam = {0.25,0.25,0.25};
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_KS, KSparam,
                       {"Glossy prt integrator - Specular coefficent"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_MAXDEPTH, 5,
                       {"Dipole subsurface, Direct lighting, Instant GI, Path, Whitted - "
                        "Max ray depth"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_MAXDIST, 9999999.9,
                       {"Ambient occlusion (9999999.9), Photon map (0.1) - "
                        "AO - Max distance to take intersection into account. "
                        "Photon map - Max distance to photon to consider shading point"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_FINALGATHERSAMPLES, 32,
                       {"Photon map integrator - Final gathering pass sample count"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_MINPIXELSPACING, 2.5,
                       {"Irradiance cache integrator - minimum pixel spacing"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_MAXPIXELSPACING, 15.0,
                       {"Irradiance cache integrator - maximum pixel spacing"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_INDIRECTPHOTONS, 100000,
                       {"Photon map integrator - Indirect photons count"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_ROUGHNESS, 0.1,
                       {"Glossy prt integrator - Surface roughness"}});
    props.setProperty({PBRT_SURFACEINTEGRATOR_PROP_MAXANGLEDIFFERENCE, 10.0,
                       {"Irradiacne cache integrator - Max angle difference between "
                        "normal and irradiance look up point"}});

    // VOLUME INTEGRATOR PROPERTIES
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_TYPE, PBRT_VOLUME_INTEGRATOR_DEBUG,
                       {"Volume integrator class"}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_LIGHTDEPTH, 3,
                       {"Volumetric BD path integrator - Max light ray depth"}});
    //props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_MAXLAMBDA, 0, {""}});
    //props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_MINLAMBDA, 0, {""}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_EYEDEPTH, 3,
                       {"Volumetric BD path integrator - Max eye ray depth"}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_NSETS, 4,
                       {"VPL volume integrator"}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_NLGIHTS, 4,
                       {"VPL volume integrator"}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_GLIMIT, 1000.0,
                       {"VPL volume integrator"}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_PHOTONCOUNT, 1000,
                       {"Sensor volume integrator"}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_VSDDATADIRECTORY, std::string(""),
                       {"VSDFDS, VSDFLS, VSDFSS, VSD linear sprite"}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_PSHFILE, std::string(""),
                       {"VSDFDS, VSDFLS, VSDFSS, VSD linear sprite"}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_XSHIFT, 0.0,
                       {"VSDFDS, VSDFLS, VSDFSS, VSD linear sprite"}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_YSHIFT, 0.0,
                       {"VSDFDS, VSDFLS, VSDFSS, VSD linear sprite"}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_ZSHIFT, 0.0,
                       {"VSDFDS, VSDFLS, VSDFSS, VSD linear sprite"}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_DEPTH, 3,
                       {"Volumetric Path integrator - Max ray depth"}});
    props.setProperty({PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.0,
                       {"Emission, Flourescence single, Flourescence single rwl, Sensor, "
                        "Single scattering, Vol BD Path, Vol Path, VPL, VSDBDG, "
                        "VSDBLG, VSDBSG, VSD scattering - March step size"}});

    addRendererType("SamplerRenderer", props);

}
}

extern "C" brayns::Engine* brayns_engine_create(
    int, const char**, brayns::ParametersManager& parametersManager)
{
    return new brayns::PBRTEngine(parametersManager);
}
