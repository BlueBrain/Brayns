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

#include "PBRTRenderer.h"
#include "PBRTConstants.h"
#include "PBRTFrameBuffer.h"
#include "PBRTModel.h"
#include "PBRTScene.h"
#include "util/PBRTDebugIntegrator.h"

#include <brayns/common/log.h>

#include <brayns/engine/FrameBuffer.h>

#include <pbrt/core/paramset.h>

#include <pbrt/integrators/ao.h>
#include <pbrt/integrators/bdpt.h>
#include <pbrt/integrators/directlighting.h>
#include <pbrt/integrators/mlt.h>
#include <pbrt/integrators/path.h>
#include <pbrt/integrators/sppm.h>
#include <pbrt/integrators/volpath.h>
#include <pbrt/integrators/whitted.h>

#include <pbrt/samplers/halton.h>
#include <pbrt/samplers/maxmin.h>
#include <pbrt/samplers/random.h>
#include <pbrt/samplers/sobol.h>
#include <pbrt/samplers/stratified.h>
#include <pbrt/samplers/zerotwosequence.h>

namespace brayns
{
std::shared_ptr<pbrt::Integrator> AOIntegratorFactory(PBRTRenderer& renderer)
{
    std::shared_ptr<const pbrt::Camera> pbrtCamera = renderer.getPBRTCamera()->impl();
    pbrt::Bounds2i pixelBounds = pbrtCamera->film->GetSampleBounds();
    if(renderer.hasProperty(PBRT_INTEGRATORPROP_PIXELBOUNDS))
    {
        auto pbounds = renderer.getProperty<std::array<double, 4>>(PBRT_INTEGRATORPROP_PIXELBOUNDS);
        pixelBounds = pbrt::Bounds2i(pbrt::Point2i(static_cast<int>(pbounds[0]),
                                                   static_cast<int>(pbounds[1])),
                                     pbrt::Point2i(static_cast<int>(pbounds[2]),
                                                   static_cast<int>(pbounds[3])));
    }

    auto cossample = renderer.getPropertyOrValue<bool>(PBRT_INTEGRATORPROP_COSSAMPLE, true);
    auto nsamples = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_NSAMPLES, 64);

    return std::shared_ptr<pbrt::Integrator>(new pbrt::AOIntegrator(cossample,
                                                                    nsamples,
                                                                    pbrtCamera,
                                                                    renderer.getSampler(),
                                                                    pixelBounds));
}

/*
Possible light sampling strategies:
    - "power"
    - "uniform"
    - "spatial"
*/

std::shared_ptr<pbrt::Integrator> BDPTIntegratorFactory(PBRTRenderer& renderer)
{
    std::shared_ptr<const pbrt::Camera> pbrtCamera = renderer.getPBRTCamera()->impl();
    pbrt::Bounds2i pixelBounds = pbrtCamera->film->GetSampleBounds();
    if(renderer.hasProperty(PBRT_INTEGRATORPROP_PIXELBOUNDS))
    {
        auto pbounds = renderer.getProperty<std::array<double, 4>>(PBRT_INTEGRATORPROP_PIXELBOUNDS);
        pixelBounds = pbrt::Bounds2i(pbrt::Point2i(static_cast<int>(pbounds[0]),
                                                   static_cast<int>(pbounds[1])),
                                     pbrt::Point2i(static_cast<int>(pbounds[2]),
                                                   static_cast<int>(pbounds[3])));
    }

    auto maxdepth = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_MAXDEPTH, 5);
    auto visualizestrategies = renderer.getPropertyOrValue<bool>(PBRT_INTEGRATORPROP_VISUALIZESTRATEGIES,
                                                                 false);
    auto visualizeweights = renderer.getPropertyOrValue<bool>(PBRT_INTEGRATORPROP_VISUALIZEWEIGHTS, false);

    if(visualizeweights || visualizestrategies) maxdepth = 5;

    auto lightstrategy = renderer.getPropertyOrValue<std::string>(PBRT_INTEGRATORPROP_LIGHTSAMPLINGSTRATEGY, "power");

    return std::shared_ptr<pbrt::Integrator>(new pbrt::BDPTIntegrator(renderer.getSampler(),
                                                                      pbrtCamera,
                                                                      maxdepth,
                                                                      visualizestrategies,
                                                                      visualizeweights,
                                                                      pixelBounds,
                                                                      lightstrategy));
}

std::shared_ptr<pbrt::Integrator> DirectLightingIntegratorFactory(PBRTRenderer& renderer)
{
    std::shared_ptr<const pbrt::Camera> pbrtCamera = renderer.getPBRTCamera()->impl();
    pbrt::Bounds2i pixelBounds = pbrtCamera->film->GetSampleBounds();
    if(renderer.hasProperty(PBRT_INTEGRATORPROP_PIXELBOUNDS))
    {
        auto pbounds = renderer.getProperty<std::array<double, 4>>(PBRT_INTEGRATORPROP_PIXELBOUNDS);
        pixelBounds = pbrt::Bounds2i(pbrt::Point2i(static_cast<int>(pbounds[0]),
                                                   static_cast<int>(pbounds[1])),
                                     pbrt::Point2i(static_cast<int>(pbounds[2]),
                                                   static_cast<int>(pbounds[3])));
    }

    auto maxdepth = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_MAXDEPTH, 5);

    auto lightStrategyStr = renderer.getPropertyOrValue<std::string>(PBRT_INTEGRATORPROP_LIGHTSTRATEGY, "all");
    pbrt::LightStrategy strategy = pbrt::LightStrategy::UniformSampleAll;
    if(lightStrategyStr == "one")
        strategy = pbrt::LightStrategy::UniformSampleOne;

    return std::shared_ptr<pbrt::Integrator>(new pbrt::DirectLightingIntegrator(strategy,
                                                                                maxdepth,
                                                                                pbrtCamera,
                                                                                renderer.getSampler(),
                                                                                pixelBounds));
}

std::shared_ptr<pbrt::Integrator> MetropolisLTIntegratorFactory(PBRTRenderer& renderer)
{
    std::shared_ptr<const pbrt::Camera> pbrtCamera = renderer.getPBRTCamera()->impl();
    auto maxdepth = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_MAXDEPTH, 5);
    auto nbootstrap = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_BOOTSTRAPSAMPLES, 100000);
    auto nchains = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_CHAINS, 1000);
    auto mutationsperpixel = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_MUTATIONSPERPIXEL, 100);
    auto largestepprobability = static_cast<pbrt::Float>(
                                    renderer.getPropertyOrValue<double>(PBRT_INTEGRATORPROP_LARGESTEPPROBABILITY,
                                                                        0.3));
    auto sigma = static_cast<pbrt::Float>(renderer.getPropertyOrValue<double>(PBRT_INTEGRATORPROP_SIGMA, 0.01));

    return std::shared_ptr<pbrt::Integrator>(new pbrt::MLTIntegrator(pbrtCamera,
                                                                     maxdepth,
                                                                     nbootstrap,
                                                                     nchains,
                                                                     mutationsperpixel,
                                                                     sigma,
                                                                     largestepprobability));
}

std::shared_ptr<pbrt::Integrator> PathTracerIntegratorFactory(PBRTRenderer& renderer)
{
    std::shared_ptr<const pbrt::Camera> pbrtCamera = renderer.getPBRTCamera()->impl();
    pbrt::Bounds2i pixelBounds = pbrtCamera->film->GetSampleBounds();
    if(renderer.hasProperty(PBRT_INTEGRATORPROP_PIXELBOUNDS))
    {
        auto pbounds = renderer.getProperty<std::array<double, 4>>(PBRT_INTEGRATORPROP_PIXELBOUNDS);
        pixelBounds = pbrt::Bounds2i(pbrt::Point2i(static_cast<int>(pbounds[0]),
                                                   static_cast<int>(pbounds[1])),
                                     pbrt::Point2i(static_cast<int>(pbounds[2]),
                                                   static_cast<int>(pbounds[3])));
    }

    auto maxdepth = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_MAXDEPTH, 5);
    auto rrthreshold = static_cast<pbrt::Float>(
                renderer.getPropertyOrValue<double>(PBRT_INTEGRATORPROP_RRTHRESHOLD,1.));
    auto lightstrategy = renderer.getPropertyOrValue<std::string>(PBRT_INTEGRATORPROP_LIGHTSAMPLINGSTRATEGY,
                                                                  "spatial");


    return std::shared_ptr<pbrt::Integrator>(new pbrt::PathIntegrator(maxdepth,
                                                                      pbrtCamera,
                                                                      renderer.getSampler(),
                                                                      pixelBounds,
                                                                      rrthreshold,
                                                                      lightstrategy));
}

std::shared_ptr<pbrt::Integrator> SPPMIntegratorFactory(PBRTRenderer& renderer)
{
    std::shared_ptr<const pbrt::Camera> pbrtCamera = renderer.getPBRTCamera()->impl();
    auto camCopy = pbrtCamera;
    auto maxdepth = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_MAXDEPTH, 5);
    auto niterations = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_NUMITERATIONS, 64);
    auto photonsperiter = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_PHOTONSPERITERATION,
                                                           -1);
    auto writefreq = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_IMAGEWRITEFREQUENCY, 1 << 31);
    auto radius = static_cast<pbrt::Float>(renderer.getPropertyOrValue<double>(PBRT_INTEGRATORPROP_RADIUS,
                                                                               1.));

    return std::shared_ptr<pbrt::Integrator>(new pbrt::SPPMIntegrator(camCopy,
                                                                      niterations,
                                                                      photonsperiter,
                                                                      maxdepth,
                                                                      radius,
                                                                      writefreq));
}

std::shared_ptr<pbrt::Integrator> VolPathTracerIntegratorFactory(PBRTRenderer& renderer)
{
    std::shared_ptr<const pbrt::Camera> pbrtCamera = renderer.getPBRTCamera()->impl();
    auto maxdepth = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_MAXDEPTH, 5);
    pbrt::Bounds2i pixelBounds = pbrtCamera->film->GetSampleBounds();
    if(renderer.hasProperty(PBRT_INTEGRATORPROP_PIXELBOUNDS))
    {
        auto pbounds = renderer.getProperty<std::array<double, 4>>(PBRT_INTEGRATORPROP_PIXELBOUNDS);
        pixelBounds = pbrt::Bounds2i(pbrt::Point2i(static_cast<int>(pbounds[0]),
                                                   static_cast<int>(pbounds[1])),
                                     pbrt::Point2i(static_cast<int>(pbounds[2]),
                                                   static_cast<int>(pbounds[3])));
    }

    auto rrthreshold = static_cast<pbrt::Float>(
                renderer.getPropertyOrValue<double>(PBRT_INTEGRATORPROP_RRTHRESHOLD,1.));
    auto lightstrategy = renderer.getPropertyOrValue<std::string>(PBRT_INTEGRATORPROP_LIGHTSAMPLINGSTRATEGY,
                                                                  "spatial");

    return std::shared_ptr<pbrt::Integrator>(new pbrt::VolPathIntegrator(maxdepth,
                                                                         pbrtCamera,
                                                                         renderer.getSampler(),
                                                                         pixelBounds,
                                                                         rrthreshold,
                                                                         lightstrategy));
}

std::shared_ptr<pbrt::Integrator> WhittedIntegratorFactory(PBRTRenderer& renderer)
{
    std::shared_ptr<const pbrt::Camera> pbrtCamera = renderer.getPBRTCamera()->impl();
    auto maxdepth = renderer.getPropertyOrValue<int>(PBRT_INTEGRATORPROP_MAXDEPTH, 5);
    pbrt::Bounds2i pixelBounds = pbrtCamera->film->GetSampleBounds();
    if(renderer.hasProperty(PBRT_INTEGRATORPROP_PIXELBOUNDS))
    {
        auto pbounds = renderer.getProperty<std::array<double, 4>>(PBRT_INTEGRATORPROP_PIXELBOUNDS);
        pixelBounds = pbrt::Bounds2i(pbrt::Point2i(static_cast<int>(pbounds[0]),
                                                   static_cast<int>(pbounds[1])),
                                     pbrt::Point2i(static_cast<int>(pbounds[2]),
                                                   static_cast<int>(pbounds[3])));
    }

    return std::shared_ptr<pbrt::Integrator>(new pbrt::WhittedIntegrator(maxdepth,
                                                                         pbrtCamera,
                                                                         renderer.getSampler(),
                                                                         pixelBounds));
}

std::shared_ptr<pbrt::Integrator> DebugIntegratorFactory(PBRTRenderer& renderer)
{
    std::shared_ptr<const pbrt::Camera> pbrtCamera = renderer.getPBRTCamera()->impl();
    pbrt::Bounds2i pixelBounds = pbrtCamera->film->GetSampleBounds();
    return std::make_shared<PBRTDebugIntegrator>(pbrtCamera, renderer.getSampler(), pixelBounds);
}

// -----------------------------------------------------------------------------------------------

std::shared_ptr<pbrt::Sampler> ZeroTwoSequenceSamplerFactory(PBRTRenderer& renderer)
{
    auto pixelsamples = renderer.getPropertyOrValue<int>(PBRT_SAMPLERPROP_PIXELSAMPLES, 16);
    auto dimensions = renderer.getPropertyOrValue<int>(PBRT_SAMPLERPROP_DIMENSIONS, 4);
    return std::shared_ptr<pbrt::Sampler>(
                new pbrt::ZeroTwoSequenceSampler(pixelsamples, dimensions));
}

std::shared_ptr<pbrt::Sampler> MaxMinSamplerFactory(PBRTRenderer& renderer)
{
    auto pixelsamples = renderer.getPropertyOrValue<int>(PBRT_SAMPLERPROP_PIXELSAMPLES, 16);
    auto dimensions = renderer.getPropertyOrValue<int>(PBRT_SAMPLERPROP_DIMENSIONS, 4);
    return std::shared_ptr<pbrt::Sampler>(
                new pbrt::MaxMinDistSampler(pixelsamples, dimensions));
}

std::shared_ptr<pbrt::Sampler> HaltonSamplerFactory(PBRTRenderer& renderer)
{
    const PBRTCamera& pbrtCam = static_cast<const PBRTCamera&>(*renderer.getPBRTCamera());
    auto sbounds = pbrtCam.getFilm()->GetSampleBounds();

    auto pixelsamples = renderer.getPropertyOrValue<int>(PBRT_SAMPLERPROP_PIXELSAMPLES, 16);
    auto samplepixelcenter = renderer.getPropertyOrValue<bool>(PBRT_SAMPLERPROP_SAMPLEPIXELCENTER, false);
    return std::shared_ptr<pbrt::Sampler>(
                new pbrt::HaltonSampler(pixelsamples, sbounds, samplepixelcenter));
}

std::shared_ptr<pbrt::Sampler> SobolSamplerFactory(PBRTRenderer& renderer)
{
    const PBRTCamera& pbrtCam = static_cast<const PBRTCamera&>(*renderer.getPBRTCamera());
    auto sbounds = pbrtCam.getFilm()->GetSampleBounds();

    auto pixelsamples = renderer.getPropertyOrValue<int>(PBRT_SAMPLERPROP_PIXELSAMPLES, 16);
    return std::shared_ptr<pbrt::Sampler>(
                new pbrt::SobolSampler(pixelsamples, sbounds));
}

std::shared_ptr<pbrt::Sampler> RandomSamplerFactory(PBRTRenderer& renderer)
{
    auto pixelsamples = renderer.getPropertyOrValue<int>(PBRT_SAMPLERPROP_PIXELSAMPLES, 4);
    return std::shared_ptr<pbrt::Sampler>(
                new pbrt::RandomSampler(pixelsamples));
}

std::shared_ptr<pbrt::Sampler> StratifiedSamplerFactory(PBRTRenderer& renderer)
{

    auto dimensions = renderer.getPropertyOrValue<int>(PBRT_SAMPLERPROP_DIMENSIONS, 4);
    auto jitter = renderer.getPropertyOrValue<bool>(PBRT_SAMPLERPROP_JITTER, true);
    auto xsamp = renderer.getPropertyOrValue<int>(PBRT_SAMPLERPROP_XSAMPLES, 4);
    auto ysamp = renderer.getPropertyOrValue<int>(PBRT_SAMPLERPROP_YSAMPLES, 4);
    return std::shared_ptr<pbrt::Sampler>(
                new pbrt::StratifiedSampler(xsamp, ysamp, jitter, dimensions));
}

// -----------------------------------------------------------------------------------------------

PBRTRenderer::PBRTRenderer(const AnimationParameters& animP,
                           const RenderingParameters& rendP)
    : Renderer(animP, rendP)
    , _camera(nullptr)
    , _currentRenderer(PBRT_INTEGRATOR_DEBUG)
    , _currentSampler(PBRT_SAMPLER_HALTON)
{
    _renderFactory[PBRT_INTEGRATOR_WHITTED] = WhittedIntegratorFactory;
    _renderFactory[PBRT_INTEGRATOR_METROPOLIS] = MetropolisLTIntegratorFactory;
    _renderFactory[PBRT_INTEGRATOR_PATHTRACER] = PathTracerIntegratorFactory;
    _renderFactory[PBRT_INTEGRATOR_DIRECTLIGHTING] = DirectLightingIntegratorFactory;
    _renderFactory[PBRT_INTEGRATOR_BIDIRPATHTRACER] = BDPTIntegratorFactory;
    _renderFactory[PBRT_INTEGRATOR_AMBIENTOCCLUSION] = AOIntegratorFactory;
    _renderFactory[PBRT_INTEGRATOR_VOLUMETRICPATHTRACER] = VolPathTracerIntegratorFactory;
    _renderFactory[PBRT_INTEGRATOR_SPROGRESSIVEPHOTOMMAP] = SPPMIntegratorFactory;
    _renderFactory[PBRT_INTEGRATOR_DEBUG] = DebugIntegratorFactory;

    _samplerFactory[PBRT_SAMPLER_SOBOL] = SobolSamplerFactory;
    _samplerFactory[PBRT_SAMPLER_HALTON] = HaltonSamplerFactory;
    _samplerFactory[PBRT_SAMPLER_MAXMIN] = MaxMinSamplerFactory;
    _samplerFactory[PBRT_SAMPLER_RANDOM] = RandomSamplerFactory;
    _samplerFactory[PBRT_SAMPLER_STRATIFIED] = StratifiedSamplerFactory;
    _samplerFactory[PBRT_SAMPLER_ZEROTWOSEQUENCE] = ZeroTwoSequenceSamplerFactory;
}

void PBRTRenderer::render(FrameBufferPtr frameBuffer)
{
    if(!_pbrtRenderer || !_camera)
        return;

    PBRTCamera& pbrtCam = *_camera;

    PBRTScene& pbrtScene = static_cast<PBRTScene&>(*_scene.get());

    auto pbrtFrameBuffer =
        std::static_pointer_cast<PBRTFrameBuffer>(frameBuffer);
    auto lock = pbrtFrameBuffer->getScopeLock();

    // Adjust the camera if the framebuffer size has changed
    // (in pbrt, the camera holds the film object)
    pbrtCam.manualCommit(frameBuffer->getSize());
    // Recreate the inegrator if the camera changed
    commit();

    const auto pbrtSceneImpl = pbrtScene.getPBRTScene();
    if(pbrtSceneImpl)
    {
        this->_pbrtRenderer->Render(*pbrtSceneImpl);
        PBRTFrameBuffer* pbrtFB = static_cast<PBRTFrameBuffer*>(frameBuffer.get());
        pbrtFB->setBackgroundColor(_renderingParameters.getBackgroundColor());
        pbrtFB->fillColorBuffer(pbrtCam.getFilm()->_rgb);
        frameBuffer->markModified();
        pbrtCam.getFilm()->Clear();
    }
    else
        BRAYNS_WARN << "PBRTRenderer: PBRT Scene is null, skipping rendernig!" << std::endl;
}

void PBRTRenderer::commit()
{
    if(!_camera)
        return;

    const RenderingParameters& rp = _renderingParameters;
    const PBRTCamera& cam = *_camera;
    auto propSamplerName = getPropertyOrValue(PBRT_SAMPLERPROP_TYPE, _currentSampler);
    auto curSamplerChanged = _currentSampler != propSamplerName;

    auto rendererChanged = _currentRenderer != getCurrentType();

    // Nothing changed, no need to recreate anything
    if(!rp.isModified() && !isModified() && !cam.cameraChanged() &&
       !rendererChanged && !curSamplerChanged)
        return;


    // Recreate sampler if changed
    if(curSamplerChanged || !_pbrtSampler || isModified())
    {
        _pbrtSampler.reset();
        _currentSampler = propSamplerName;
        auto it = _samplerFactory.find(_currentSampler);
        if(it == _samplerFactory.end())
        {
            BRAYNS_WARN << "PBRT Renderer: Unknown sampler type " << _currentSampler
                        << ", Falling back to Halton" << std::endl;
            _currentSampler = PBRT_SAMPLER_HALTON;
            it = _samplerFactory.find(_currentSampler);
        }
        _pbrtSampler = it->second(*this);
    }

    if(rendererChanged || cam.cameraChanged() || isModified())
    {
        _currentRenderer = getCurrentType();
        auto renderFactoryIt = _renderFactory.find(_currentRenderer);

        // Recreate the renderer if is not initialized
        if(!_pbrtRenderer || cam.cameraChanged())
        {
            if(renderFactoryIt == _renderFactory.end())
            {
                BRAYNS_WARN << "PBRT Renderer: Unknown renderer type " << _currentRenderer
                            << ", falling back to Debug" << std::endl;
                _currentRenderer = PBRT_INTEGRATOR_DEBUG;
                renderFactoryIt = _renderFactory.find(_currentRenderer);
            }
            _pbrtRenderer = renderFactoryIt->second(*this);
        }
        // Or if the new requested is a known one
        else if(renderFactoryIt != _renderFactory.end())
        {
            _pbrtRenderer = renderFactoryIt->second(*this);
        }

        _scene->visitModels([](Model& m)
        {
            m.markInstancesDirty();
        });
        auto pbrtScene = static_cast<PBRTScene*>(_scene.get());
        pbrtScene->setCurrentRenderer(_currentRenderer);

        cam.setCameraChanged(false);
    }
}

void PBRTRenderer::setCamera(CameraPtr camera)
{
    _camera = static_cast<PBRTCamera*>(camera.get());
    assert(_camera);
    markModified();
}
}
