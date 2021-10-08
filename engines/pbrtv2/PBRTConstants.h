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
    MATERIAL_GLASS = 0u,
    MATERIAL_KDSUBSURFACE = 1u,
    MATERIAL_MATTE = 2u,
    MATERIAL_MEASURED = 3u,
    MATERIAL_METAL = 4u,
    MATERIAL_MIRROR = 5u,
    MATERIAL_MIXMAT = 6u,
    MATERIAL_PLASTIC = 7u,
    MATERIAL_SHINYMETAL = 8u,
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
extern const std::string PBRT_CAMTYPE_TELECENTRIC;

// Camera properties
extern const std::string PBRT_CAMPROP_SHUTTEROPEN;
extern const std::string PBRT_CAMPROP_SHUTTERCLOSE;
extern const std::string PBRT_CAMPROP_LENSRADIUS;
extern const std::string PBRT_CAMPROP_FOCALDISTANCE;
extern const std::string PBRT_CAMPROP_FRAMEASPECTRATIO;
extern const std::string PBRT_CAMPROP_SCREENWINDOW;
extern const std::string PBRT_CAMPROP_FOV;
extern const std::string PBRT_CAMPROP_HALFFOV;

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
extern const std::string PBRT_FILMPROP_PIXELWIDTH;
extern const std::string PBRT_FILMPROP_PIXELHEIGHT;
extern const std::string PBRT_FILMPROP_CROPWINDOW;
extern const std::string PBRT_FILMPROP_FILTERSPECTRUM;
extern const std::string PBRT_FILMPROP_FILTERBANDMIN;
extern const std::string PBRT_FILMPROP_FILTERBANDMAX;

// Surface integrator names
extern const std::string PBRT_SURFACE_INTEGRATOR_AMBIENTOCCLUSION;
extern const std::string PBRT_SURFACE_INTEGRATOR_DIRECTLIGHTING;
extern const std::string PBRT_SURFACE_INTEGRATOR_PATHTRACER;
extern const std::string PBRT_SURFACE_INTEGRATOR_PHOTOMMAP;
extern const std::string PBRT_SURFACE_INTEGRATOR_IRRADIANCACHE;
extern const std::string PBRT_SURFACE_INTEGRATOR_IGI;
extern const std::string PBRT_SURFACE_INTEGRATOR_DIPOLESUBSURFACE;
extern const std::string PBRT_SURFACE_INTEGRATOR_RADIANCEPROBE;
extern const std::string PBRT_SURFACE_INTEGRATOR_DIFFUSEPRT;
extern const std::string PBRT_SURFACE_INTEGRATOR_GLOSSYPRT;
extern const std::string PBRT_SURFACE_INTEGRATOR_DEBUG;

// Volume integrator names
extern const std::string PBRT_VOLUME_INTEGRATOR_SINGLESCATTERING;
extern const std::string PBRT_VOLUME_INTEGRATOR_VPL;
extern const std::string PBRT_VOLUME_INTEGRATOR_SINGLEFLOURESCENCE;
extern const std::string PBRT_VOLUME_INTEGRATOR_SINGLEFLOURESCENCEWL;
extern const std::string PBRT_VOLUME_INTEGRATOR_SENSOR;
extern const std::string PBRT_VOLUME_INTEGRATOR_VSDFDS;
extern const std::string PBRT_VOLUME_INTEGRATOR_VSDFLS;
extern const std::string PBRT_VOLUME_INTEGRATOR_VSDFSS;
extern const std::string PBRT_VOLUME_INTEGRATOR_VSDBDG;
extern const std::string PBRT_VOLUME_INTEGRATOR_VSDBLG;
extern const std::string PBRT_VOLUME_INTEGRATOR_VSDBSG;
extern const std::string PBRT_VOLUME_INTEGRATOR_VSDSPRITELINEAR;
extern const std::string PBRT_VOLUME_INTEGRATOR_VSDSCATTERING;
extern const std::string PBRT_VOLUME_INTEGRATOR_EMISSION;
extern const std::string PBRT_VOLUME_INTEGRATOR_PATH;
extern const std::string PBRT_VOLUME_INTEGRATOR_BDPT;
extern const std::string PBRT_VOLUME_INTEGRATOR_DEBUG;

// Surface integrator properties
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_TYPE;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_POINTSFILE;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXERROR;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_GATHERSAMPLES;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXINDIRECTDEPTH;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_NUSED;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_LMAX;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_FILENAME;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_CAUSTICPHOTONS;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_NLIGHTS;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXSPECULARDEPTH;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_GLIMIT;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXPHOTONDEPTH;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_FINALGATHER;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_MINSAMPLEDISTANCE;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_NSAMPLES;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_STRATEGY;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_NSETS;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_RRTHRESHOLD;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_GATHERANGLE;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_MINWEIGHT;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_KD;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_KS;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXDEPTH;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXDIST;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_FINALGATHERSAMPLES;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_MINPIXELSPACING;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXPIXELSPACING;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_INDIRECTPHOTONS;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_ROUGHNESS;
extern const std::string PBRT_SURFACEINTEGRATOR_PROP_MAXANGLEDIFFERENCE;

// Volume integrator properties
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_TYPE;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_LIGHTDEPTH;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_MAXLAMBDA;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_MINLAMBDA;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_EYEDEPTH;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_NSETS;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_NLGIHTS;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_GLIMIT;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_PHOTONCOUNT;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_VSDDATADIRECTORY;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_PSHFILE;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_XSHIFT;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_YSHIFT;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_ZSHIFT;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_DEPTH;
extern const std::string PBRT_VOLUMEINTEGRATOR_PROP_STEPSIZE;

// Sensor shapes
extern const std::string PBRT_SENSORSHAPE_DISK;
extern const std::string PBRT_SENSORSHAPE_RECTANGLE;

// Sensor properties
extern const std::string PBRT_SENSORPROP_REFERENCE;
extern const std::string PBRT_SENSORPROP_XPIXELS;
extern const std::string PBRT_SENSORPROP_YPIXELS;
extern const std::string PBRT_SENSORPROP_FOV;
extern const std::string PBRT_SENSORPROP_RADIUS;
extern const std::string PBRT_SENSORPROP_X;
extern const std::string PBRT_SENSORPROP_Y;

// Sensor shape properties
extern const std::string PBRT_SENSORSHAPEPROP_HEIGHT;
extern const std::string PBRT_SENSORSHAPEPROP_RADIUS;
extern const std::string PBRT_SENSORSHAPEPROP_INNERRADIUS;
extern const std::string PBRT_SENSORSHAPEPROP_PHIMAX;
extern const std::string PBRT_SENSORSHAPEPROP_X;
extern const std::string PBRT_SENSORSHAPEPROP_Y;

// Sampler names
extern const std::string PBRT_SAMPLER_ADAPTIVE;
extern const std::string PBRT_SAMPLER_BESTCANDIDATE;
extern const std::string PBRT_SAMPLER_HALTON;
extern const std::string PBRT_SAMPLER_LOWDISCREPANCY;
extern const std::string PBRT_SAMPLER_RANDOM;
extern const std::string PBRT_SAMPLER_STRATIFIED;

// Sampler properties
extern const std::string PBRT_SAMPLERPROP_TYPE;
extern const std::string PBRT_SAMPLERPROP_MINSAMPLES;
extern const std::string PBRT_SAMPLERPROP_MAXSAMPLES;
extern const std::string PBRT_SAMPLERPROP_METHOD; // "contrast", "shapeid"
extern const std::string PBRT_SAMPLERPROP_PIXELSAMPLES;
extern const std::string PBRT_SAMPLERPROP_JITTER;
extern const std::string PBRT_SAMPLERPROP_XSAMPLES;
extern const std::string PBRT_SAMPLERPROP_YSAMPLES;

} // namespace brayns
