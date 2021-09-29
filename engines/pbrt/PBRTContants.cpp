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
    case PBRTMaterialClass::MATERIAL_DISNEY:
        return "disney";
    case PBRTMaterialClass::MATERIAL_MIRROR:
        return "mirror";
    case PBRTMaterialClass::MATERIAL_MIXMAT:
        return "mix";
    case PBRTMaterialClass::MATERIAL_FOURIER:
        return "fourier";
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
    }

    return "";
}

const std::string PBRT_CAMTYPE_PERSPECTIVE = "perspective";
const std::string PBRT_CAMTYPE_ORTOGRAPHIC = "orthographic";
const std::string PBRT_CAMTYPE_ENVIRONMENT = "environment";
const std::string PBRT_CAMTYPE_REALISTIC = "realistic";

const std::string PBRT_CAMPROP_SHUTTEROPEN = "shutteropen";
const std::string PBRT_CAMPROP_SHUTTERCLOSE = "shutterclose";
const std::string PBRT_CAMPROP_LENSRADIUS = "lensradius";
const std::string PBRT_CAMPROP_FOCALDISTANCE = "focaldistance";
const std::string PBRT_CAMPROP_FRAMEASPECTRATIO = "frameaspectratio";
const std::string PBRT_CAMPROP_SCREENWINDOW = "screenwindow";
const std::string PBRT_CAMPROP_FOV = "fov";
const std::string PBRT_CAMPROP_HALFFOV = "halffov";
const std::string PBRT_CAMPROP_LENSFILE = "lensfile";
const std::string PBRT_CAMPROP_APERTUREDIAMETER = "aperturediameter";
const std::string PBRT_CAMPROP_FOCUSDISTANCE = "focusdistance";
const std::string PBRT_CAMPROP_SIMPLEWEIGHTING = "simpleweighting";

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
const std::string PBRT_FILMPROP_SCALE = "scale";
const std::string PBRT_FILMPROP_DIAGONAL = "diagonal";
const std::string PBRT_FILMPROP_MAXSAMPLELUMINANCE = "maxsampleluminance";

const std::string PBRT_INTEGRATOR_AMBIENTOCCLUSION = "ambientocclusion";
const std::string PBRT_INTEGRATOR_BIDIRPATHTRACER = "bdpt";
const std::string PBRT_INTEGRATOR_DIRECTLIGHTING = "directlighting";
const std::string PBRT_INTEGRATOR_METROPOLIS = "mlt";
const std::string PBRT_INTEGRATOR_PATHTRACER = "path";
const std::string PBRT_INTEGRATOR_SPROGRESSIVEPHOTOMMAP = "sppm";
const std::string PBRT_INTEGRATOR_VOLUMETRICPATHTRACER = "volpath";
const std::string PBRT_INTEGRATOR_WHITTED = "whitted";
const std::string PBRT_INTEGRATOR_DEBUG = "debug";

const std::string PBRT_INTEGRATORPROP_PIXELBOUNDS = "pixelbounds";
const std::string PBRT_INTEGRATORPROP_COSSAMPLE = "cossample";
const std::string PBRT_INTEGRATORPROP_NSAMPLES = "nsamples";
const std::string PBRT_INTEGRATORPROP_MAXDEPTH = "maxdepth";
const std::string PBRT_INTEGRATORPROP_VISUALIZESTRATEGIES =
    "visualizestrategies";
const std::string PBRT_INTEGRATORPROP_VISUALIZEWEIGHTS = "visualizeweights";
const std::string PBRT_INTEGRATORPROP_LIGHTSAMPLINGSTRATEGY =
    "lightsamplestrategy";
const std::string PBRT_INTEGRATORPROP_LIGHTSTRATEGY = "strategy";
const std::string PBRT_INTEGRATORPROP_BOOTSTRAPSAMPLES = "bootstrapsamples";
const std::string PBRT_INTEGRATORPROP_CHAINS = "chains";
const std::string PBRT_INTEGRATORPROP_MUTATIONSPERPIXEL = "mutationsperpixel";
const std::string PBRT_INTEGRATORPROP_LARGESTEPPROBABILITY =
    "largestepprobability";
const std::string PBRT_INTEGRATORPROP_SIGMA = "sigma";
const std::string PBRT_INTEGRATORPROP_RRTHRESHOLD = "rrthreshold";
const std::string PBRT_INTEGRATORPROP_NUMITERATIONS = "numiterations";
const std::string PBRT_INTEGRATORPROP_PHOTONSPERITERATION =
    "photonsperiteration";
const std::string PBRT_INTEGRATORPROP_IMAGEWRITEFREQUENCY =
    "imagewritefrequency";
const std::string PBRT_INTEGRATORPROP_RADIUS = "radius";

const std::string PBRT_SAMPLER_HALTON = "halton";
const std::string PBRT_SAMPLER_MAXMIN = "maxmindist";
const std::string PBRT_SAMPLER_RANDOM = "random";
const std::string PBRT_SAMPLER_SOBOL = "sobol";
const std::string PBRT_SAMPLER_STRATIFIED = "stratified";
const std::string PBRT_SAMPLER_ZEROTWOSEQUENCE = "02sequence";

const std::string PBRT_SAMPLERPROP_TYPE = "integrator_sampler";
const std::string PBRT_SAMPLERPROP_PIXELSAMPLES = "pixelsamples";
const std::string PBRT_SAMPLERPROP_DIMENSIONS = "dimensions";
const std::string PBRT_SAMPLERPROP_SAMPLEPIXELCENTER = "samplepixelcenter";
const std::string PBRT_SAMPLERPROP_JITTER = "jitter";
const std::string PBRT_SAMPLERPROP_XSAMPLES = "xsamples";
const std::string PBRT_SAMPLERPROP_YSAMPLES = "ysamples";

} // namespace brayns
