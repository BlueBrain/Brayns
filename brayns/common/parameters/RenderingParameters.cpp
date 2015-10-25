/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#include "RenderingParameters.h"
#include <brayns/common/log.h>
#include <brayns/common/exceptions.h>

#include <boost/lexical_cast.hpp>

const std::string DEFAULT_RENDERER = "raycast_eyelight";

const std::string PARAM_MODULE = "--module";
const std::string PARAM_RENDERER = "--renderer";
const std::string PARAM_SPP = "--spp";
const std::string PARAM_DOF = "--dof";
const std::string PARAM_AMBIENT_OCCLUSION = "--ambient-occlusion";
const std::string PARAM_NO_LIGHT_SHADING = "--no-light-shading";
const std::string PARAM_SHADOWS = "--shadows";
const std::string PARAM_SOFT_SHADOWS = "--soft-shadows";
const std::string PARAM_ELECTRON_SHADING = "--electron-shading";
const std::string PARAM_GRADIENT_BACKGROUND = "--gradient-background";
const std::string PARAM_RADIANCE = "--radiance";
const std::string PARAM_BACKGROUND_COLOR = "--background-color";
const std::string PARAM_FRAME_NUMBER = "--frame-number";

namespace brayns
{

RenderingParameters::RenderingParameters()
    : _renderer(DEFAULT_RENDERER),
      _ambientOcclusionStrength(0.f), _dof(false), _dofStrength(0.f),
      _electronShading(false), _gradientBackground(false),
      _lightShading(true), _lightEmittingMaterials(false),
      _spp(1), _shadows(false), _softShadows(false), _backgroundColor(Vector3f(0.8,0.8,0.8)),
      _frameNumber(std::numeric_limits<uint16_t>::max())
{
    _parameters[PARAM_MODULE] =
        {PMT_STRING, "Name of the OSPRay module"};
    _parameters[PARAM_RENDERER] =
        {PMT_STRING, "Name of the renderer"};
    _parameters[PARAM_SPP] =
        {PMT_INTEGER, "Number of samples per pixel"};
    _parameters[PARAM_DOF] =
        {PMT_FLOAT, "Depth of field strength"};
    _parameters[PARAM_AMBIENT_OCCLUSION] =
        {PMT_FLOAT, "Ambient occlusion strength"};
    _parameters[PARAM_NO_LIGHT_SHADING] =
        {PMT_BOOLEAN, "Disables light shading"};
    _parameters[PARAM_SHADOWS] =
        {PMT_BOOLEAN, "Enables shadows"};
    _parameters[PARAM_SOFT_SHADOWS] =
        {PMT_BOOLEAN, "Enables soft shadows"};
    _parameters[PARAM_ELECTRON_SHADING] =
        {PMT_BOOLEAN, "Enables electron shading"};
    _parameters[PARAM_GRADIENT_BACKGROUND] =
        {PMT_BOOLEAN, "Enables gradient beckground"};
    _parameters[PARAM_RADIANCE] =
        {PMT_BOOLEAN, "Enables radiance"};
    _parameters[PARAM_BACKGROUND_COLOR] =
        {PMT_FLOAT3, "Background color"};
    _parameters[PARAM_FRAME_NUMBER] =
        {PMT_INTEGER, "Frame number"};
}

void RenderingParameters::parse(int argc, const char **argv)
{
    for (int i=1;i<argc;i++)
    {
        std::string arg = argv[i];
        if(arg == PARAM_RENDERER)
            _renderer = argv[++i];
        else if (arg == PARAM_MODULE)
            _module = argv[++i];
        else if (arg == PARAM_SPP)
            _spp = atoi(argv[++i]);
        else if (arg==PARAM_AMBIENT_OCCLUSION)
            _ambientOcclusionStrength = boost::lexical_cast<float>(argv[++i]);
        else if (arg==PARAM_SHADOWS)
            _shadows = true;
        else if (arg == PARAM_SOFT_SHADOWS)
            _softShadows = true;
        else if (arg==PARAM_NO_LIGHT_SHADING)
            _lightShading = false;
        else if (arg == PARAM_ELECTRON_SHADING)
            _electronShading = true;
        else if (arg==PARAM_DOF)
        {
            _dof = true;
            _dofStrength = boost::lexical_cast<float>(argv[++i]);
        }
        else if (arg == PARAM_GRADIENT_BACKGROUND)
            _gradientBackground = true;
        else if (arg == PARAM_BACKGROUND_COLOR)
        {
            float r = boost::lexical_cast<float>(argv[++i]);
            float g = boost::lexical_cast<float>(argv[++i]);
            float b = boost::lexical_cast<float>(argv[++i]);
            _backgroundColor = Vector3f(r, g, b);
        }
        else if (arg == PARAM_FRAME_NUMBER)
            _frameNumber = boost::lexical_cast<size_t>(argv[++i]);
    }
}

void RenderingParameters::display() const
{
    BRAYNS_INFO << "Rendering options: " << std::endl;
    BRAYNS_INFO << "- OSPRay Module : " << _module << std::endl;
    BRAYNS_INFO << "- Renderer      : " << _renderer << std::endl;
    BRAYNS_INFO << "- Samples per pixel       : " <<
                   _spp << std::endl;
    BRAYNS_INFO << "- Ambient occlusion       : " <<
                   _ambientOcclusionStrength << std::endl;
    BRAYNS_INFO << "- Shadows                 : " <<
                   (_shadows ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Soft shadows            : " <<
                   (_softShadows ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Light shading           : " <<
                   (_lightShading ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Electron shading        : " <<
                   (_electronShading ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Depth of field          : " <<
                   (_dof ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Depth of field strength : " <<
                   _dofStrength << std::endl;
    BRAYNS_INFO << "- Gradient background     : " <<
                   (_gradientBackground ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Frame number            : " <<
                   _frameNumber << std::endl;
}

}
