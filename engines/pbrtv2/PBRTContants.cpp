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

#include <string>

namespace brayns
{
std::string materialClassToString(const PBRTMaterialClass matClass)
{
    switch (matClass)
    {
    case PBRTMaterialClass::MATERIAL_UBER:
        return "uber";
    case PBRTMaterialClass::MATERIAL_GLASS:
        return "glass";
    case PBRTMaterialClass::MATERIAL_MATTE:
        return "matte";
    case PBRTMaterialClass::MATERIAL_MIRROR:
        return "mirror";
    case PBRTMaterialClass::MATERIAL_MIXMAT:
        return "mix";
    case PBRTMaterialClass::MATERIAL_PLASTIC:
        return "plastic";
    case PBRTMaterialClass::MATERIAL_SUBSTRATE:
        return "substrate";
    case PBRTMaterialClass::MATERIAL_SUBSURFACE:
        return "subsurface";
    case PBRTMaterialClass::MATERIAL_TRANSLUCENT:
        return "translucent";
    case PBRTMaterialClass::MATERIAL_KDSUBSURFACE:
        return "kdsubsurface";
    case PBRTMaterialClass::MATERIAL_METAL:
        return "metal";
    case PBRTMaterialClass::MATERIAL_SHINYMETAL:
        return "shinymetal";
    case PBRTMaterialClass::MATERIAL_MEASURED:
        return "measured";
    }

    return "";
}

const std::string PBRT_CAMTYPE_PERSPECTIVE = "perspective";
const std::string PBRT_CAMTYPE_ORTOGRAPHIC = "orthographic";
const std::string PBRT_CAMTYPE_ENVIRONMENT = "environment";
const std::string PBRT_CAMTYPE_TELECENTRIC = "telecentric";

const std::string PBRT_CAMPROP_SHUTTEROPEN = "shutteropen";
const std::string PBRT_CAMPROP_SHUTTERCLOSE = "shutterclose";
const std::string PBRT_CAMPROP_LENSRADIUS = "lensradius";
const std::string PBRT_CAMPROP_FOCALDISTANCE = "focaldistance";
const std::string PBRT_CAMPROP_FRAMEASPECTRATIO = "frameaspectratio";
const std::string PBRT_CAMPROP_SCREENWINDOW = "screenwindow";
const std::string PBRT_CAMPROP_FOV = "fov";
const std::string PBRT_CAMPROP_HALFFOV = "halffov";

const std::string PBRT_FILTERTYPE_BOX = "box";
const std::string PBRT_FILTERTYPE_GAUSSIAN = "gaussian";
const std::string PBRT_FILTERTYPE_MITCHELL = "mitchell";
const std::string PBRT_FILTERTYPE_SINC = "sinc";
const std::string PBRT_FILTERTYPE_TRIANGLE = "triangle";

const std::string PBRT_FILTERPROP_TYPE = "filter_type";
const std::string PBRT_FILTERPROP_XWIDTH = "xwidth";
const std::string PBRT_FILTERPROP_YWIDTH = "ywidth";
const std::string PBRT_FILTERPROP_ALPHA = "alpha";
const std::string PBRT_FILTERPROP_B = "B";
const std::string PBRT_FILTERPROP_C = "C";
const std::string PBRT_FILTERPROP_TAU = "tau";

const std::string PBRT_FILMPROP_XRESOLUTION = "xresolution";
const std::string PBRT_FILMPROP_YRESOLUTION = "yresolution";
const std::string PBRT_FILMPROP_PIXELWIDTH = "pixelwidth";
const std::string PBRT_FILMPROP_PIXELHEIGHT = "pixelheight";
const std::string PBRT_FILMPROP_CROPWINDOW = "cropwindow";
const std::string PBRT_FILMPROP_FILTERSPECTRUM = "spectralfilter";
const std::string PBRT_FILMPROP_FILTERBANDMIN = "filter1bandmin";
const std::string PBRT_FILMPROP_FILTERBANDMAX = "filter1bandmax";

const std::string PBRT_SURFACE_INTEGRATOR_AMBIENTOCCLUSION = "ambientocclusion";
const std::string PBRT_SURFACE_INTEGRATOR_DIRECTLIGHTING = "directlighting";
const std::string PBRT_SURFACE_INTEGRATOR_PATHTRACER = "path";
const std::string PBRT_SURFACE_INTEGRATOR_PHOTOMMAP = "photonmap";
const std::string PBRT_SURFACE_INTEGRATOR_IRRADIANCACHE = "irradiancecache";
const std::string PBRT_SURFACE_INTEGRATOR_IGI = "igi";
const std::string PBRT_SURFACE_INTEGRATOR_DIPOLESUBSURFACE = "dipolesubsurface";
const std::string PBRT_SURFACE_INTEGRATOR_RADIANCEPROBE = "useprobes";
const std::string PBRT_SURFACE_INTEGRATOR_DIFFUSEPRT = "diffuseprt";
const std::string PBRT_SURFACE_INTEGRATOR_GLOSSYPRT = "diffuseprt";
const std::string PBRT_SURFACE_INTEGRATOR_DEBUG = "debug";

const std::string PBRT_VOLUME_INTEGRATOR_SINGLESCATTERING = "single";
const std::string PBRT_VOLUME_INTEGRATOR_VPL = "vpl";
const std::string PBRT_VOLUME_INTEGRATOR_SINGLEFLOURESCENCE =
    "singlefluorescence";
const std::string PBRT_VOLUME_INTEGRATOR_SINGLEFLOURESCENCEWL =
    "singlefluorescencerwl";
const std::string PBRT_VOLUME_INTEGRATOR_SENSOR = "sensor";
const std::string PBRT_VOLUME_INTEGRATOR_VSDFDS = "vsdfds";
const std::string PBRT_VOLUME_INTEGRATOR_VSDFLS = "vsdfls";
const std::string PBRT_VOLUME_INTEGRATOR_VSDFSS = "vsdfss";
const std::string PBRT_VOLUME_INTEGRATOR_VSDBDG = "vsdbdg";
const std::string PBRT_VOLUME_INTEGRATOR_VSDBLG = "vsdblg";
const std::string PBRT_VOLUME_INTEGRATOR_VSDBSG = "vsdbsg";
const std::string PBRT_VOLUME_INTEGRATOR_VSDSPRITELINEAR = "vsdspritelinear";
const std::string PBRT_VOLUME_INTEGRATOR_VSDSCATTERING = "vsdscattering";
const std::string PBRT_VOLUME_INTEGRATOR_EMISSION = "emission";
const std::string PBRT_VOLUME_INTEGRATOR_PATH = "path";
const std::string PBRT_VOLUME_INTEGRATOR_BDPT = "bdpt";
const std::string PBRT_VOLUME_INTEGRATOR_DEBUG = "debug";

const std::string PBRT_SURFACEINTEGRATOR_PROP_TYPE = "surfaceIntegratorClass";
const std::string PBRT_SURFACEINTEGRATOR_PROP_POINTSFILE = "surfPointsfile";
const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXERROR = "surfMaxerror";
const std::string PBRT_SURFACEINTEGRATOR_PROP_GATHERSAMPLES =
    "surfGathersamples";
const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXINDIRECTDEPTH =
    "surfMaxindirectdepth";
const std::string PBRT_SURFACEINTEGRATOR_PROP_NUSED = "surfNused";
const std::string PBRT_SURFACEINTEGRATOR_PROP_LMAX = "surfLmax";
const std::string PBRT_SURFACEINTEGRATOR_PROP_FILENAME = "surfFilename";
const std::string PBRT_SURFACEINTEGRATOR_PROP_CAUSTICPHOTONS =
    "surfCausticphotons";
const std::string PBRT_SURFACEINTEGRATOR_PROP_NLIGHTS = "surfNlights";
const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXSPECULARDEPTH =
    "surfMaxspeculardepth";
const std::string PBRT_SURFACEINTEGRATOR_PROP_GLIMIT = "surfGlimit";
const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXPHOTONDEPTH =
    "surfMaxphotondepth";
const std::string PBRT_SURFACEINTEGRATOR_PROP_FINALGATHER = "surfFinalgather";
const std::string PBRT_SURFACEINTEGRATOR_PROP_MINSAMPLEDISTANCE =
    "surfMinsampledistance";
const std::string PBRT_SURFACEINTEGRATOR_PROP_NSAMPLES = "surfNsamples";
const std::string PBRT_SURFACEINTEGRATOR_PROP_STRATEGY = "surfStrategy";
const std::string PBRT_SURFACEINTEGRATOR_PROP_NSETS = "surfNsets";
const std::string PBRT_SURFACEINTEGRATOR_PROP_RRTHRESHOLD = "surfRrthreshold";
const std::string PBRT_SURFACEINTEGRATOR_PROP_GATHERANGLE = "surfGatherangle";
const std::string PBRT_SURFACEINTEGRATOR_PROP_MINWEIGHT = "surfMinweight";
const std::string PBRT_SURFACEINTEGRATOR_PROP_KD = "surfKd";
const std::string PBRT_SURFACEINTEGRATOR_PROP_KS = "surfKs";
const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXDEPTH = "surfMaxdepth";
const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXDIST = "surfMaxdist";
const std::string PBRT_SURFACEINTEGRATOR_PROP_FINALGATHERSAMPLES =
    "surfFinalgathersamples";
const std::string PBRT_SURFACEINTEGRATOR_PROP_MINPIXELSPACING =
    "surfMinpixelspacing";
const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXPIXELSPACING =
    "surfMaxpixelspacing";
const std::string PBRT_SURFACEINTEGRATOR_PROP_INDIRECTPHOTONS =
    "surfIndirectphotons";
const std::string PBRT_SURFACEINTEGRATOR_PROP_ROUGHNESS = "surfRoughness";
const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXANGLEDIFFERENCE =
    "surfMaxangledifference";

const std::string PBRT_VOLUMEINTEGRATOR_PROP_TYPE = "volumeIntegratorClass";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_LIGHTDEPTH = "volLightdepth";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_MAXLAMBDA = "volMaxLambda";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_MINLAMBDA = "volMinLambda";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_EYEDEPTH = "volEyedepth";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_NSETS = "volNsets";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_NLGIHTS = "volNlights";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_GLIMIT = "volGlimit";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_PHOTONCOUNT = "volPhotoncount";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_VSDDATADIRECTORY =
    "volVsddatadirectory";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_PSHFILE = "volPshfile";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_XSHIFT = "volXshfit";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_YSHIFT = "volYshift";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_ZSHIFT = "volZshift";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_DEPTH = "volDepth";
const std::string PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE = "volStepsize";

const std::string PBRT_SENSORSHAPE_DISK = "disk";
const std::string PBRT_SENSORSHAPE_RECTANGLE = "rectangle";

const std::string PBRT_SENSORPROP_REFERENCE = "reference";
const std::string PBRT_SENSORPROP_XPIXELS = "xpixels";
const std::string PBRT_SENSORPROP_YPIXELS = "ypixels";
const std::string PBRT_SENSORPROP_FOV = "fov";
const std::string PBRT_SENSORPROP_RADIUS = "radius";
const std::string PBRT_SENSORPROP_X = "x";
const std::string PBRT_SENSORPROP_Y = "y";

const std::string PBRT_SENSORSHAPEPROP_HEIGHT = "height";
const std::string PBRT_SENSORSHAPEPROP_RADIUS = "radius";
const std::string PBRT_SENSORSHAPEPROP_INNERRADIUS = "innerradius";
const std::string PBRT_SENSORSHAPEPROP_PHIMAX = "phimax";
const std::string PBRT_SENSORSHAPEPROP_X = "x";
const std::string PBRT_SENSORSHAPEPROP_Y = "y";

const std::string PBRT_SAMPLER_ADAPTIVE = "adaptive";
const std::string PBRT_SAMPLER_BESTCANDIDATE = "bestcandidate";
const std::string PBRT_SAMPLER_HALTON = "halton";
const std::string PBRT_SAMPLER_LOWDISCREPANCY = "lowdiscrepancy";
const std::string PBRT_SAMPLER_RANDOM = "random";
const std::string PBRT_SAMPLER_STRATIFIED = "stratified";

const std::string PBRT_SAMPLERPROP_TYPE = "samplerClass";
const std::string PBRT_SAMPLERPROP_MINSAMPLES = "minsamples";
const std::string PBRT_SAMPLERPROP_MAXSAMPLES = "maxsamples";
const std::string PBRT_SAMPLERPROP_METHOD = "method";
const std::string PBRT_SAMPLERPROP_PIXELSAMPLES = "pixelsamples";
const std::string PBRT_SAMPLERPROP_JITTER = "jitter";
const std::string PBRT_SAMPLERPROP_XSAMPLES = "xsamples";
const std::string PBRT_SAMPLERPROP_YSAMPLES = "ysamples";

} // namespace brayns
