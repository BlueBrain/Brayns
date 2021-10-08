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

#pragma once

#include <cstdint>
#include <string>

namespace brayns
{
// Supported materials
enum class PBRTMaterialClass : uint8_t
{
    MATERIAL_DISNEY = 0u,
    MATERIAL_FOURIER = 1u,
    MATERIAL_GLASS = 2u,
    MATERIAL_KDSUBSURFACE = 3u,
    MATERIAL_MATTE = 4u,
    MATERIAL_METAL = 5u,
    MATERIAL_MIRROR = 6u,
    MATERIAL_MIXMAT = 7u,
    MATERIAL_PLASTIC = 8u,
    MATERIAL_SUBSTRATE = 9u,
    MATERIAL_SUBSURFACE = 10u,
    MATERIAL_TRANSLUCENT = 11u,
    MATERIAL_UBER = 12u
};
std::string materialClassToString(const PBRTMaterialClass matClass);

// Camera type names
extern const std::string PBRT_CAMTYPE_PERSPECTIVE;
extern const std::string PBRT_CAMTYPE_ORTOGRAPHIC;
extern const std::string PBRT_CAMTYPE_ENVIRONMENT;
extern const std::string PBRT_CAMTYPE_REALISTIC;

// Camera properties
extern const std::string PBRT_CAMPROP_SHUTTEROPEN;
extern const std::string PBRT_CAMPROP_SHUTTERCLOSE;
extern const std::string PBRT_CAMPROP_LENSRADIUS;
extern const std::string PBRT_CAMPROP_FOCALDISTANCE;
extern const std::string PBRT_CAMPROP_FRAMEASPECTRATIO;
extern const std::string PBRT_CAMPROP_SCREENWINDOW;
extern const std::string PBRT_CAMPROP_FOV;
extern const std::string PBRT_CAMPROP_HALFFOV;
extern const std::string PBRT_CAMPROP_LENSFILE;
extern const std::string PBRT_CAMPROP_APERTUREDIAMETER;
extern const std::string PBRT_CAMPROP_FOCUSDISTANCE;
extern const std::string PBRT_CAMPROP_SIMPLEWEIGHTING;

// Image filter method names
extern const std::string PBRT_FILTERTYPE_BOX;
extern const std::string PBRT_FILTERTYPE_GAUSSIAN;
extern const std::string PBRT_FILTERTYPE_MITCHELL;
extern const std::string PBRT_FILTERTYPE_SINC;
extern const std::string PBRT_FILTERTYPE_TRIANGLE;

// Image filter properties
extern const std::string PBRT_FILTERPROP_TYPE;
extern const std::string PBRT_FILTERPROP_XWIDTH;
extern const std::string PBRT_FILTERPROP_YWIDTH;
extern const std::string PBRT_FILTERPROP_ALPHA;
extern const std::string PBRT_FILTERPROP_B;
extern const std::string PBRT_FILTERPROP_C;
extern const std::string PBRT_FILTERPROP_TAU;

// Film properties
extern const std::string PBRT_FILMPROP_XRESOLUTION;
extern const std::string PBRT_FILMPROP_YRESOLUTION;
extern const std::string PBRT_FILMPROP_SCALE;
extern const std::string PBRT_FILMPROP_DIAGONAL;
extern const std::string PBRT_FILMPROP_MAXSAMPLELUMINANCE;

// Integrator names
extern const std::string PBRT_INTEGRATOR_AMBIENTOCCLUSION;
extern const std::string PBRT_INTEGRATOR_BIDIRPATHTRACER;
extern const std::string PBRT_INTEGRATOR_DIRECTLIGHTING;
extern const std::string PBRT_INTEGRATOR_METROPOLIS;
extern const std::string PBRT_INTEGRATOR_PATHTRACER;
extern const std::string PBRT_INTEGRATOR_SPROGRESSIVEPHOTOMMAP;
extern const std::string PBRT_INTEGRATOR_VOLUMETRICPATHTRACER;
extern const std::string PBRT_INTEGRATOR_WHITTED;
extern const std::string PBRT_INTEGRATOR_DEBUG;

// Integrator properties
extern const std::string PBRT_INTEGRATORPROP_PIXELBOUNDS;
extern const std::string PBRT_INTEGRATORPROP_COSSAMPLE;
extern const std::string PBRT_INTEGRATORPROP_NSAMPLES;
extern const std::string PBRT_INTEGRATORPROP_MAXDEPTH;
extern const std::string PBRT_INTEGRATORPROP_VISUALIZESTRATEGIES;
extern const std::string PBRT_INTEGRATORPROP_VISUALIZEWEIGHTS;
extern const std::string PBRT_INTEGRATORPROP_LIGHTSAMPLINGSTRATEGY;
extern const std::string PBRT_INTEGRATORPROP_LIGHTSTRATEGY;
extern const std::string PBRT_INTEGRATORPROP_BOOTSTRAPSAMPLES;
extern const std::string PBRT_INTEGRATORPROP_CHAINS;
extern const std::string PBRT_INTEGRATORPROP_MUTATIONSPERPIXEL;
extern const std::string PBRT_INTEGRATORPROP_LARGESTEPPROBABILITY;
extern const std::string PBRT_INTEGRATORPROP_SIGMA;
extern const std::string PBRT_INTEGRATORPROP_RRTHRESHOLD;
extern const std::string PBRT_INTEGRATORPROP_NUMITERATIONS;
extern const std::string PBRT_INTEGRATORPROP_PHOTONSPERITERATION;
extern const std::string PBRT_INTEGRATORPROP_IMAGEWRITEFREQUENCY;
extern const std::string PBRT_INTEGRATORPROP_RADIUS;

// Sampler names
extern const std::string PBRT_SAMPLER_HALTON;
extern const std::string PBRT_SAMPLER_MAXMIN;
extern const std::string PBRT_SAMPLER_RANDOM;
extern const std::string PBRT_SAMPLER_SOBOL;
extern const std::string PBRT_SAMPLER_STRATIFIED;
extern const std::string PBRT_SAMPLER_ZEROTWOSEQUENCE;

// Sampler properties
extern const std::string PBRT_SAMPLERPROP_TYPE;
extern const std::string PBRT_SAMPLERPROP_PIXELSAMPLES;
extern const std::string PBRT_SAMPLERPROP_DIMENSIONS;
extern const std::string PBRT_SAMPLERPROP_SAMPLEPIXELCENTER;
extern const std::string PBRT_SAMPLERPROP_JITTER;
extern const std::string PBRT_SAMPLERPROP_XSAMPLES;
extern const std::string PBRT_SAMPLERPROP_YSAMPLES;

} // namespace brayns
