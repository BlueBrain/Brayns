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

#include "PBRTConstants.h"
#include "PBRTFrameBuffer.h"
#include "PBRTModel.h"
#include "PBRTRenderer.h"
#include "PBRTScene.h"
#include "util/PBRTDebugIntegrator.h"

#include <brayns/common/log.h>

#include <brayns/engine/FrameBuffer.h>

#include <pbrtv2/core/paramset.h>

#include <pbrtv2/film/image.h>

#include <pbrtv2/integrators/ambientocclusion.h>        // surface
#include <pbrtv2/integrators/diffuseprt.h>              // surface
#include <pbrtv2/integrators/dipolesubsurface.h>        // surface
#include <pbrtv2/integrators/directlighting.h>          // surface
#include <pbrtv2/integrators/emission.h>                // volume
#include <pbrtv2/integrators/fluorescencesingle.h>      // volume
#include <pbrtv2/integrators/fluorescencesinglerwl.h>   // volume
#include <pbrtv2/integrators/glossyprt.h>               // surface
#include <pbrtv2/integrators/igi.h>                     // surface
#include <pbrtv2/integrators/irradiancecache.h>         // surface
#include <pbrtv2/integrators/path.h>                    // surface
#include <pbrtv2/integrators/photonmap.h>               // surface
#include <pbrtv2/integrators/sensor.h>                  // volume
#include <pbrtv2/integrators/single.h>                  // volume
#include <pbrtv2/integrators/useprobes.h>               // surface
#include <pbrtv2/integrators/volbdpt.h>                 // volume
#include <pbrtv2/integrators/volpath.h>                 // volume
#include <pbrtv2/integrators/vpl.h>                     // volume
#include <pbrtv2/integrators/vsd.h>
#include <pbrtv2/integrators/vsdbdg.h>                  // volume
#include <pbrtv2/integrators/vsdblg.h>                  // volume
#include <pbrtv2/integrators/vsdbsg.h>                  // volume
#include <pbrtv2/integrators/vsdfds.h>                  // volume
#include <pbrtv2/integrators/vsdfls.h>                  // volume
#include <pbrtv2/integrators/vsdfss.h>                  // volume
#include <pbrtv2/integrators/vsdlinearsprite.h>         // volume
#include <pbrtv2/integrators/vsdscattering.h>           // volume
#include <pbrtv2/integrators/whitted.h>                 // surface

#include <pbrtv2/samplers/adaptive.h>
#include <pbrtv2/samplers/bestcandidate.h>
#include <pbrtv2/samplers/halton.h>
#include <pbrtv2/samplers/lowdiscrepancy.h>
#include <pbrtv2/samplers/random.h>
#include <pbrtv2/samplers/stratified.h>

namespace brayns
{

inline float getFloat(PBRTRenderer& r, const std::string& prop, float defValue)
{
    return static_cast<float>(
                r.getPropertyOrValue<double>(prop, static_cast<double>(defValue)));
}

inline int getInt(PBRTRenderer& r, const std::string& prop, int defValue)
{
    return r.getPropertyOrValue<int>(prop, defValue);
}

inline pbrt::Spectrum getSpectrum(PBRTRenderer& r, const std::string& prop, 
                                  const std::array<float, 3>& defValue)
{
    if(r.hasProperty(prop))
    {
        const auto p = r.getProperty<std::array<double, 3>>(prop);
        float fltP[3] = {
            static_cast<float>(p[0]),
            static_cast<float>(p[1]),
            static_cast<float>(p[2])
        };
        return pbrt::Spectrum::FromRGB(fltP);
    }

    return pbrt::Spectrum::FromRGB(&defValue[0]);
}

// SURFACE INTEGRATORS
// -----------------------------------------------------------------------------------------------

pbrt::SurfaceIntegrator* AOIntegratorFactory(PBRTRenderer& renderer)
{
    const auto maxDist = getFloat(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXDIST, 99999999.9f);
    const auto nsamples = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_NSAMPLES, 2048);
    return new pbrt::AmbientOcclusionIntegrator(nsamples, maxDist);
}

pbrt::SurfaceIntegrator* DiffusePRTIntegratorFactory(PBRTRenderer& renderer)
{
    const auto lmax = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_LMAX, 4);
    const auto nsamples = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_NSAMPLES, 4096);
    return new pbrt::DiffusePRTIntegrator(lmax, nsamples);
}

pbrt::SurfaceIntegrator* DipoleSubsurfaceIntegratorFactory(PBRTRenderer& renderer)
{
    const auto maxDepth = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXDEPTH, 5);
    const auto maxError = getFloat(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXERROR, .05f);
    const auto minDist = getFloat(renderer, PBRT_SURFACEINTEGRATOR_PROP_MINSAMPLEDISTANCE, .25f);
    const auto pointsfile = renderer.getPropertyOrValue<std::string>(PBRT_SURFACEINTEGRATOR_PROP_POINTSFILE, 
                                                                     "");
    return new pbrt::DipoleSubsurfaceIntegrator(maxDepth, maxError, minDist, pointsfile);
}

pbrt::SurfaceIntegrator* DirectlightingIntegratorFactory(PBRTRenderer& renderer)
{
    const auto maxDepth = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXDEPTH, 5);
    const auto strategy = renderer.getPropertyOrValue<std::string>(PBRT_SURFACEINTEGRATOR_PROP_STRATEGY, 
                                                                   "all");
    pbrt::LightStrategy strategyImpl;
    if(strategy == std::string("all"))
        strategyImpl = pbrt::LightStrategy::SAMPLE_ALL_UNIFORM;
    else if(strategy == std::string("one"))
        strategyImpl = pbrt::LightStrategy::SAMPLE_ONE_UNIFORM;
    else
        throw std::runtime_error("PBRTEngine -> DirectLighting surface integrator: "
                                 "surfStrategy must be either \"all\" or \"one\", "
                                 "but got \"" + strategy + "\"");

    return new pbrt::DirectLightingIntegrator(strategyImpl, maxDepth);
}

pbrt::SurfaceIntegrator* GlossyPRTIntegratorFactory(PBRTRenderer& renderer)
{
    const auto lmax = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_LMAX, 4);
    const auto nsamples = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_NSAMPLES, 4096);
    const auto Kd = getSpectrum(renderer, PBRT_SURFACEINTEGRATOR_PROP_KD, {.5f, .5f, .5f});
    const auto Ks = getSpectrum(renderer, PBRT_SURFACEINTEGRATOR_PROP_KS, {.25f, .25f, .25f});
    const auto roughness = getFloat(renderer, PBRT_SURFACEINTEGRATOR_PROP_ROUGHNESS, 0.1f);
    return new pbrt::GlossyPRTIntegrator(Kd, Ks, roughness, lmax, nsamples);
}

pbrt::SurfaceIntegrator* IGIIntegratorFactory(PBRTRenderer& renderer)
{
    const auto nLightPaths = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_NLIGHTS, 64);
    const auto nLightSets = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_NSETS, 4);
    const auto rrThresh = getFloat(renderer, PBRT_SURFACEINTEGRATOR_PROP_RRTHRESHOLD, .0001f);
    const auto maxDepth = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXDEPTH, 5);
    const auto glimit = getFloat(renderer, PBRT_SURFACEINTEGRATOR_PROP_GLIMIT, 10.f);
    const auto gatherSamples = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_GATHERSAMPLES, 16);
    return new pbrt::IGIIntegrator(nLightPaths, nLightSets, rrThresh, maxDepth, glimit,
                                   gatherSamples);
}

pbrt::SurfaceIntegrator* IrradianceCacheIntegratorFactory(PBRTRenderer& renderer)
{
    const auto minWeight = getFloat(renderer, PBRT_SURFACEINTEGRATOR_PROP_MINWEIGHT, 0.5f);
    const auto minSpacing = getFloat(renderer, PBRT_SURFACEINTEGRATOR_PROP_MINPIXELSPACING, 2.5f);
    const auto maxSpacing = getFloat(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXPIXELSPACING, 15.f);
    const auto maxAngle = getFloat(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXANGLEDIFFERENCE, 10.f);
    const auto maxSpecularDepth = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXSPECULARDEPTH, 5);
    const auto maxIndirectDepth = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXINDIRECTDEPTH, 3);
    const auto nsamples = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_NSAMPLES, 4096);
    return new pbrt::IrradianceCacheIntegrator(minWeight, minSpacing, maxSpacing, maxAngle,
                                               maxSpecularDepth, maxIndirectDepth, nsamples);
}

pbrt::SurfaceIntegrator* PathIntegratorFactory(PBRTRenderer& renderer)
{
    const auto maxDepth = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXDEPTH, 1);
    return new pbrt::PathIntegrator(maxDepth);
}

pbrt::SurfaceIntegrator* PhotonMapIntegratorFactory(PBRTRenderer& renderer)
{
    const auto nCaustic = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_CAUSTICPHOTONS, 20000);
    const auto nIndirect = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_INDIRECTPHOTONS, 100000);
    const auto nUsed = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_NUSED, 50);
    const auto maxSpecularDepth = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXSPECULARDEPTH, 5);
    const auto maxPhotonDepth = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXPHOTONDEPTH, 5);
    const auto finalGather = renderer.getPropertyOrValue<bool>(PBRT_SURFACEINTEGRATOR_PROP_FINALGATHER, true);
    const auto gatherSamples = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_FINALGATHERSAMPLES, 32);
    const auto maxDist = getFloat(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXDIST, .1f);
    const auto gatherAngle = getFloat(renderer, PBRT_SURFACEINTEGRATOR_PROP_GATHERANGLE, 10.f);
    return new pbrt::PhotonIntegrator(nCaustic, nIndirect, nUsed, maxSpecularDepth, maxPhotonDepth,
                                      maxDist, finalGather, gatherSamples, gatherAngle);
}

pbrt::SurfaceIntegrator* RadianceProbesIntegratorFactory(PBRTRenderer& renderer)
{
    const auto filename = renderer.getPropertyOrValue<std::string>(PBRT_SURFACEINTEGRATOR_PROP_FILENAME, 
                                                                   "probes.out");
    return new pbrt::UseRadianceProbes(filename);
}

pbrt::SurfaceIntegrator* WhittedIntegratorFactory(PBRTRenderer& renderer)
{
    const auto maxDepth = getInt(renderer, PBRT_SURFACEINTEGRATOR_PROP_MAXDEPTH, 5);
    return new pbrt::WhittedIntegrator(maxDepth);
}

pbrt::SurfaceIntegrator* DebugSurfaceIntegratorFactory(PBRTRenderer&)
{
    return new PBRTDebugSurfaceIntegrator();
}

// VOLUME INTEGRATORS
// -----------------------------------------------------------------------------------------------

pbrt::VolumeIntegrator* EmissionIntegratorFactory(PBRTRenderer& renderer)
{
    const auto stepSize  = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.f);
    return new pbrt::EmissionIntegrator(stepSize);
}

pbrt::VolumeIntegrator* FlourescenceSingleIntegratorFactory(PBRTRenderer& renderer)
{
    const auto stepSize  = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.f);
    return new pbrt::SingleScatteringFluorescenceIntegrator(stepSize);
}

pbrt::VolumeIntegrator* FlourescenceSingleRWLIntegratorFactory(PBRTRenderer& renderer)
{
    const auto stepSize  = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.f);
    return new pbrt::SingleScatteringFluorescenceRWLIntegrator(stepSize);
}

pbrt::VolumeIntegrator* SensorIntegratorFactory(PBRTRenderer& renderer)
{
    const auto stepSize  = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.f);
    const auto photonCount = getInt(renderer, PBRT_VOLUMEINTEGRATOR_PROP_PHOTONCOUNT, 1000);
    return new pbrt::SensorIntegrator(stepSize, static_cast<uint64_t>(photonCount));
}

pbrt::VolumeIntegrator* SingleScatteringIntegratorFactory(PBRTRenderer& renderer)
{
    const auto stepSize  = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.f);
    return new pbrt::SingleScatteringIntegrator(stepSize);
}

pbrt::VolumeIntegrator* VolBDPathIntegratorFactory(PBRTRenderer& renderer)
{
    const auto stepSize  = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.f);
    const auto maxEyeDepth = getInt(renderer, PBRT_VOLUMEINTEGRATOR_PROP_EYEDEPTH, 3);
    const auto maxlightDepth = getInt(renderer, PBRT_VOLUMEINTEGRATOR_PROP_LIGHTDEPTH, 3);
    return new pbrt::VolumeBDPTIntegrator(stepSize, static_cast<uint64_t>(maxEyeDepth), 
                                          static_cast<uint64_t>(maxlightDepth));
}

pbrt::VolumeIntegrator* VolPathIntegratorFactory(PBRTRenderer& renderer)
{
    const auto stepSize  = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.f);
    const auto depth = getInt(renderer, PBRT_VOLUMEINTEGRATOR_PROP_DEPTH, 3);
    return new pbrt::VolumePatIntegrator(stepSize, depth);
}

pbrt::VolumeIntegrator* VPLIntegratorFactory(PBRTRenderer& renderer)
{
    const auto stepSize  = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.f);
    const auto nLightPaths = getInt(renderer, PBRT_VOLUMEINTEGRATOR_PROP_NLGIHTS, 4);
    const auto nLightSets = getInt(renderer, PBRT_VOLUMEINTEGRATOR_PROP_NSETS, 4);
    const auto glimit = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_GLIMIT, 1000.f);
    return new pbrt::VPLIntegrator(stepSize, nLightPaths, nLightSets, glimit);
}

pbrt::VolumeIntegrator* VSDBDGIntegratorFactory(PBRTRenderer& renderer)
{
    const auto stepSize  = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.f);
    return new pbrt::VSDBackwardDirectIntegrator(stepSize);
}

pbrt::VolumeIntegrator* VSDBLGIntegratorFactory(PBRTRenderer& renderer)
{
    const auto stepSize  = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.f);
    return new pbrt::VSDBackwardLinearIntegrator(stepSize);
}

pbrt::VolumeIntegrator* VSDBSGIntegratorFactory(PBRTRenderer& renderer)
{
    const auto stepSize  = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.f);
    return new pbrt::VSDBackwardScatteringIntegrator(stepSize);
}

pbrt::VolumeIntegrator* VSDFDSIntegratorFactory(PBRTRenderer& renderer)
{
    const auto vsdDataDirectory = renderer.getPropertyOrValue<std::string>(
                                            PBRT_VOLUMEINTEGRATOR_PROP_VSDDATADIRECTORY, "");
    const auto pshFileName = renderer.getPropertyOrValue<std::string>(
                                            PBRT_VOLUMEINTEGRATOR_PROP_PSHFILE, "");
    pbrt::Vector shift;
    shift.x = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_XSHIFT, 0.f);
    shift.y = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_YSHIFT, 0.f);
    shift.z = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_ZSHIFT, 0.f);
    return new pbrt::VSDForwardDirectIntegrator(vsdDataDirectory, pshFileName, shift);
}

pbrt::VolumeIntegrator* VSDFLSIntegratorFactory(PBRTRenderer& renderer)
{
    const auto vsdDataDirectory = renderer.getPropertyOrValue<std::string>(
                                            PBRT_VOLUMEINTEGRATOR_PROP_VSDDATADIRECTORY, "");
    const auto pshFileName = renderer.getPropertyOrValue<std::string>(
                                            PBRT_VOLUMEINTEGRATOR_PROP_PSHFILE, "");
    pbrt::Vector shift;
    shift.x = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_XSHIFT, 0.f);
    shift.y = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_YSHIFT, 0.f);
    shift.z = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_ZSHIFT, 0.f);
    return new pbrt::VSDForwardLinearIntegrator(vsdDataDirectory, pshFileName, shift);
}

pbrt::VolumeIntegrator* VSDFSSIntegratorFactory(PBRTRenderer& renderer)
{
    const auto vsdDataDirectory = renderer.getPropertyOrValue<std::string>(
                                            PBRT_VOLUMEINTEGRATOR_PROP_VSDDATADIRECTORY, "");
    const auto pshFileName = renderer.getPropertyOrValue<std::string>(
                                            PBRT_VOLUMEINTEGRATOR_PROP_PSHFILE, "");
    pbrt::Vector shift;
    shift.x = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_XSHIFT, 0.f);
    shift.y = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_YSHIFT, 0.f);
    shift.z = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_ZSHIFT, 0.f);
    return new pbrt::VSDForwardScatteringIntegrator(vsdDataDirectory, pshFileName, shift);
}

pbrt::VolumeIntegrator* VSDLinearSpriteIntegratorFactory(PBRTRenderer& renderer)
{
    const auto vsdDataDirectory = renderer.getPropertyOrValue<std::string>(
                                            PBRT_VOLUMEINTEGRATOR_PROP_VSDDATADIRECTORY, "");
    const auto pshFileName = renderer.getPropertyOrValue<std::string>(
                                            PBRT_VOLUMEINTEGRATOR_PROP_PSHFILE, "");
    pbrt::Vector shift;
    shift.x = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_XSHIFT, 0.f);
    shift.y = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_YSHIFT, 0.f);
    shift.z = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_ZSHIFT, 0.f);
    return new pbrt::VSDLinearSpriteIntegrator(vsdDataDirectory, pshFileName, shift);
}

pbrt::VolumeIntegrator* VSDScatteringIntegratorFactory(PBRTRenderer& renderer)
{
    const auto stepSize  = getFloat(renderer, PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE, 1.f);
    return new pbrt::VSDScatteringIntegrator(stepSize);
}

pbrt::VolumeIntegrator* DebugVolumeIntegratorFactory(PBRTRenderer&)
{
    return new PBRTDebugVolumeIntegrator();
}

// SAMPLERS
// -----------------------------------------------------------------------------------------------

pbrt::Sampler* AdaptiveSamplerFactory(PBRTRenderer& renderer)
{
    const PBRTCamera& pbrtCam = static_cast<const PBRTCamera&>(*renderer.getPBRTCamera());

    int xstart, xend, ystart, yend;
    pbrtCam.impl()->film->GetSampleExtent(&xstart, &xend, &ystart, &yend);
    const auto minsamp = getInt(renderer, PBRT_SAMPLERPROP_MINSAMPLES, 4);
    const auto maxsamp = getInt(renderer, PBRT_SAMPLERPROP_MAXSAMPLES, 32);
    const auto method = renderer.getPropertyOrValue<std::string>(PBRT_SAMPLERPROP_METHOD, "contrast");
    
    if(method != "contrast" && method != "shapeid")
        throw std::runtime_error("PBRTRenderer -> AdaptiveSamplerFactory: Available method property "
                                 "values are \"contrast\" and \"shapeid\", but it was given "
                                 "\""+ method + "\"");

    return new pbrt::AdaptiveSampler(xstart, xend, ystart, yend, minsamp, maxsamp, method,
                                     pbrtCam.impl()->shutterOpen, pbrtCam.impl()->shutterClose);
}

pbrt::Sampler* BestCandidateSamplerFactory(PBRTRenderer& renderer)
{
    const PBRTCamera& pbrtCam = static_cast<const PBRTCamera&>(*renderer.getPBRTCamera());

    int xstart, xend, ystart, yend;
    pbrtCam.impl()->film->GetSampleExtent(&xstart, &xend, &ystart, &yend);
    const auto nsamp = getInt(renderer, PBRT_SAMPLERPROP_PIXELSAMPLES, 4);
    
    return new pbrt::BestCandidateSampler(xstart, xend, ystart, yend, nsamp,
                                          pbrtCam.impl()->shutterOpen, pbrtCam.impl()->shutterClose);
}

pbrt::Sampler* HaltonSamplerFactory(PBRTRenderer& renderer)
{
    const PBRTCamera& pbrtCam = static_cast<const PBRTCamera&>(*renderer.getPBRTCamera());

    int xstart, xend, ystart, yend;
    pbrtCam.impl()->film->GetSampleExtent(&xstart, &xend, &ystart, &yend);
    const auto nsamp = getInt(renderer, PBRT_SAMPLERPROP_PIXELSAMPLES, 4);
    
    return new pbrt::HaltonSampler(xstart, xend, ystart, yend, nsamp,
                                   pbrtCam.impl()->shutterOpen, pbrtCam.impl()->shutterClose);
}

pbrt::Sampler* LowDiscrepancySamplerFactory(PBRTRenderer& renderer)
{
    const PBRTCamera& pbrtCam = static_cast<const PBRTCamera&>(*renderer.getPBRTCamera());

    int xstart, xend, ystart, yend;
    pbrtCam.impl()->film->GetSampleExtent(&xstart, &xend, &ystart, &yend);
    const auto nsamp = getInt(renderer, PBRT_SAMPLERPROP_PIXELSAMPLES, 4);
    
    return new pbrt::LDSampler(xstart, xend, ystart, yend, nsamp,
                               pbrtCam.impl()->shutterOpen, pbrtCam.impl()->shutterClose);
}

pbrt::Sampler* RandomSamplerFactory(PBRTRenderer& renderer)
{
    const PBRTCamera& pbrtCam = static_cast<const PBRTCamera&>(*renderer.getPBRTCamera());

    int xstart, xend, ystart, yend;
    pbrtCam.impl()->film->GetSampleExtent(&xstart, &xend, &ystart, &yend);
    const auto nsamp = getInt(renderer, PBRT_SAMPLERPROP_PIXELSAMPLES, 4);
    
    return new pbrt::RandomSampler(xstart, xend, ystart, yend, nsamp,
                                   pbrtCam.impl()->shutterOpen, pbrtCam.impl()->shutterClose);
}

pbrt::Sampler* StratifiedSamplerFactory(PBRTRenderer& renderer)
{
    const PBRTCamera& pbrtCam = static_cast<const PBRTCamera&>(*renderer.getPBRTCamera());
    
    const auto jitter = renderer.getPropertyOrValue<bool>(PBRT_SAMPLERPROP_JITTER, true);
    int xstart, xend, ystart, yend;
    pbrtCam.impl()->film->GetSampleExtent(&xstart, &xend, &ystart, &yend);
    const auto xsamp = getInt(renderer, PBRT_SAMPLERPROP_XSAMPLES, 2);
    const auto ysamp = getInt(renderer, PBRT_SAMPLERPROP_YSAMPLES, 2);
    return new pbrt::StratifiedSampler(xstart, xend, ystart, yend, xsamp, ysamp, jitter, 
                                       pbrtCam.impl()->shutterOpen, pbrtCam.impl()->shutterClose);
}

// -----------------------------------------------------------------------------------------------

PBRTRenderer::PBRTRenderer(const AnimationParameters& animP,
                           const RenderingParameters& rendP)
    : Renderer(animP, rendP)
    , _camera(nullptr)
    , _currentSurfIntegrator(PBRT_SURFACE_INTEGRATOR_DEBUG)
    , _currentVolIntegrator(PBRT_VOLUME_INTEGRATOR_DEBUG)
    , _currentSampler(PBRT_SAMPLER_HALTON)
{
    _surfaceIntegratorFactory[PBRT_SURFACE_INTEGRATOR_AMBIENTOCCLUSION] = AOIntegratorFactory;
    _surfaceIntegratorFactory[PBRT_SURFACE_INTEGRATOR_DIRECTLIGHTING] = DirectlightingIntegratorFactory;
    _surfaceIntegratorFactory[PBRT_SURFACE_INTEGRATOR_PATHTRACER] = PathIntegratorFactory;
    _surfaceIntegratorFactory[PBRT_SURFACE_INTEGRATOR_PHOTOMMAP] = PhotonMapIntegratorFactory;
    _surfaceIntegratorFactory[PBRT_SURFACE_INTEGRATOR_IRRADIANCACHE] = IrradianceCacheIntegratorFactory;
    _surfaceIntegratorFactory[PBRT_SURFACE_INTEGRATOR_IGI] = IGIIntegratorFactory;
    _surfaceIntegratorFactory[PBRT_SURFACE_INTEGRATOR_DIPOLESUBSURFACE] = DipoleSubsurfaceIntegratorFactory;
    _surfaceIntegratorFactory[PBRT_SURFACE_INTEGRATOR_RADIANCEPROBE] = RadianceProbesIntegratorFactory;
    _surfaceIntegratorFactory[PBRT_SURFACE_INTEGRATOR_DIFFUSEPRT] = DiffusePRTIntegratorFactory;
    _surfaceIntegratorFactory[PBRT_SURFACE_INTEGRATOR_GLOSSYPRT] = GlossyPRTIntegratorFactory;
    _surfaceIntegratorFactory[PBRT_SURFACE_INTEGRATOR_DEBUG] = DebugSurfaceIntegratorFactory;

    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_SINGLESCATTERING] = SingleScatteringIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_VPL] = VPLIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_SINGLEFLOURESCENCE] = FlourescenceSingleIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_SINGLEFLOURESCENCEWL] =  FlourescenceSingleRWLIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_SENSOR] = SensorIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_VSDFDS] = VSDFDSIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_VSDFLS] = VSDFLSIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_VSDFSS] = VSDFSSIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_VSDBDG] = VSDBDGIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_VSDBLG] = VSDBLGIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_VSDBSG] = VSDBSGIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_VSDSPRITELINEAR] = VSDLinearSpriteIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_VSDSCATTERING] = VSDScatteringIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_EMISSION] = EmissionIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_PATH] = VolPathIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_BDPT] = VolBDPathIntegratorFactory;
    _volumeIntegratorFactory[PBRT_VOLUME_INTEGRATOR_DEBUG] = DebugVolumeIntegratorFactory;

    _samplerFactory[PBRT_SAMPLER_ADAPTIVE] = AdaptiveSamplerFactory;
    _samplerFactory[PBRT_SAMPLER_BESTCANDIDATE] = BestCandidateSamplerFactory;
    _samplerFactory[PBRT_SAMPLER_HALTON] = HaltonSamplerFactory;
    _samplerFactory[PBRT_SAMPLER_LOWDISCREPANCY] = LowDiscrepancySamplerFactory;
    _samplerFactory[PBRT_SAMPLER_RANDOM] = RandomSamplerFactory;
    _samplerFactory[PBRT_SAMPLER_STRATIFIED] = StratifiedSamplerFactory;
}

PBRTRenderer::~PBRTRenderer()
{
}

void PBRTRenderer::render(FrameBufferPtr frameBuffer)
{
    if(!_renderer || !_camera)
        return;

    PBRTCamera& pbrtCam = *_camera;

    PBRTScene& pbrtScene = static_cast<PBRTScene&>(*_scene.get());
    if(!pbrtScene.needsToRender() && !pbrtCam.needsToRender())
    {
        frameBuffer->markModified();
        return;
    }

    auto pbrtFrameBuffer =
        std::static_pointer_cast<PBRTFrameBuffer>(frameBuffer);
    auto lock = pbrtFrameBuffer->getScopeLock();

    // Adjust the camera if the framebuffer size has changed
    // (in pbrt, the camera holds the film object)
    // The renderer holds the camera, se we must recommit
    // everything
    manualCommit(frameBuffer->getSize());

    const auto pbrtSceneImpl = pbrtScene.getPBRTScene();
    if(pbrtSceneImpl)
    {
        _renderer->Render(pbrtSceneImpl);
        PBRTFrameBuffer* pbrtFB = static_cast<PBRTFrameBuffer*>(frameBuffer.get());
        pbrtFB->fillColorBuffer(pbrtCam.getFilm()->_rgb);
        frameBuffer->markModified();
        pbrtScene.setNeedsToRender(false);
        pbrtCam.setNeedsToRender(false);
        pbrtCam.getFilm()->_rgb.clear();
    }
    else
        BRAYNS_WARN << "PBRTRenderer: PBRT Scene is null, skipping rendernig!" << std::endl;
}

void PBRTRenderer::manualCommit(const Vector2i& res)
{
    if(!_camera)
        return;

    const RenderingParameters& rp = _renderingParameters;
    PBRTCamera& cam = *_camera;
    const auto propSamplerName = getPropertyOrValue(PBRT_SAMPLERPROP_TYPE, _currentSampler);
    const auto propSurfIntegrator = getPropertyOrValue<std::string>
                                    (PBRT_SURFACEINTEGRATOR_PROP_TYPE, _currentSurfIntegrator);
    const auto propVolIntegrator = getPropertyOrValue<std::string>
                                   (PBRT_VOLUMEINTEGRATOR_PROP_TYPE, _currentVolIntegrator);

    const auto curSamplerChanged = _currentSampler != propSamplerName;
    const auto curSurfIntChanged = _currentSurfIntegrator != propSurfIntegrator;
    const auto curVolIntChanged = _currentVolIntegrator != propVolIntegrator;

    // Nothing changed, no need to recreate anything
    if(!rp.isModified() && !isModified() && !cam.cameraChanged() &&
       !curSurfIntChanged && !curVolIntChanged && !curSamplerChanged &&
       _currentRes == res)
        return;

    _currentRes = res;

    _renderer.reset();

    _camera->markModified();
    _camera->manualCommit(res);

    // Recreate sampler
    _currentSampler = propSamplerName;
    auto it = _samplerFactory.find(_currentSampler);
    if(it == _samplerFactory.end())
    {
        BRAYNS_WARN << "PBRT Renderer: Unknown sampler type " << _currentSampler
                    << ", Falling back to Halton" << std::endl;
        _currentSampler = PBRT_SAMPLER_HALTON;
        it = _samplerFactory.find(_currentSampler);
    }
    pbrt::Sampler* sampler = it->second(*this);

    // Recreate surface integrator
    _currentSurfIntegrator = propSurfIntegrator;
    auto surfaceFactoryIt = _surfaceIntegratorFactory.find(_currentSurfIntegrator);
    if(surfaceFactoryIt == _surfaceIntegratorFactory.end())
    {
        BRAYNS_WARN << "PBRT Renderer: Unknown surface integrator " << _currentSurfIntegrator
                    << ", falling back to Debug" << std::endl;
        _currentSurfIntegrator = PBRT_SURFACE_INTEGRATOR_DEBUG;
        surfaceFactoryIt = _surfaceIntegratorFactory.find(_currentSurfIntegrator);
    }
    pbrt::SurfaceIntegrator* surfaceIntegrator = surfaceFactoryIt->second(*this);

    // Recreate volume integrator
    _currentVolIntegrator = propVolIntegrator;
    auto volumeFactoryIt = _volumeIntegratorFactory.find(_currentVolIntegrator);
    if(volumeFactoryIt == _volumeIntegratorFactory.end())
    {
        BRAYNS_WARN << "PBRT Renderer: Unknown volume integrator " << _currentVolIntegrator
                    << ", falling back to Debug" << std::endl;
        _currentVolIntegrator = PBRT_VOLUME_INTEGRATOR_DEBUG;
        volumeFactoryIt = _volumeIntegratorFactory.find(_currentVolIntegrator);
    }
    pbrt::VolumeIntegrator* volumeIntegrator = volumeFactoryIt->second(*this);

    _renderer = std::make_shared<pbrt::SamplerRenderer>(sampler, cam.impl(),
                                                        surfaceIntegrator,
                                                        volumeIntegrator, false);


    _scene->visitModels([](Model& m)
    {
        m.markInstancesDirty();
    });
    _scene->markModified();
    _scene->commit();

    cam.setCameraChanged(false);
}

void PBRTRenderer::commit()
{
    manualCommit(_currentRes);
}

void PBRTRenderer::setCamera(CameraPtr camera)
{
    _camera = static_cast<PBRTCamera*>(camera.get());
    assert(_camera);
    markModified();
}
}
