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

const std::string PARAM_SPP                 = "spp";
const std::string PARAM_DOF                 = "dof";
const std::string PARAM_AMBIENT_OCCLUSION   = "ambient-occlusion";
const std::string PARAM_NO_LIGHT_SHADING    = "no-light-shading";
const std::string PARAM_SHADOWS             = "shadows";
const std::string PARAM_SOFT_SHADOWS        = "soft-shadows";
const std::string PARAM_ELECTRON_SHADING    = "electron-shading";
const std::string PARAM_GRADIENT_BACKGROUND = "gradient-background";
const std::string PARAM_RADIANCE            = "radiance";

namespace brayns
{

RenderingParameters::RenderingParameters()
    : ambientOcclusionStrength_(0.f), dof_(false), dofStrength_(0.f),
      electronShading_(false), gradientBackground_(false),
      lightShading_(true), lightEmittingMaterials_(false),
      spp_(1), shadows_(false), softShadows_(false)
{
    parameters_[PARAM_SPP] =
        {ptInteger, "Number of samples per pixel"};
    parameters_[PARAM_DOF] =
        {ptFloat, "Depth of field strength"};
    parameters_[PARAM_AMBIENT_OCCLUSION] =
        {ptFloat, "Ambient occlusion strength"};
    parameters_[PARAM_NO_LIGHT_SHADING] =
        {ptBoolean, "Disables light shading"};
    parameters_[PARAM_SHADOWS] =
        {ptBoolean, "Enables shadows"};
    parameters_[PARAM_SOFT_SHADOWS] =
        {ptBoolean, "Enables soft shadows"};
    parameters_[PARAM_ELECTRON_SHADING] =
        {ptBoolean, "Enables electron shading"};
    parameters_[PARAM_GRADIENT_BACKGROUND] =
        {ptBoolean, "Enables gradient beckground"};
    parameters_[PARAM_RADIANCE] =
        {ptBoolean, "Enables radiance"};
}

void RenderingParameters::parse(int argc, const char **argv)
{
    for (int i=1;i<argc;i++)
    {
        std::string arg = argv[i];
        arg.erase(0,2);
        if (arg == PARAM_SPP)
            spp_ = atoi(argv[++i]);
        else if (arg==PARAM_AMBIENT_OCCLUSION)
            ambientOcclusionStrength_ = atof(argv[++i]);
        else if (arg==PARAM_SHADOWS)
            shadows_ = true;
        else if (arg == PARAM_SOFT_SHADOWS)
            softShadows_ = true;
        else if (arg==PARAM_NO_LIGHT_SHADING)
            lightShading_ = false;
        else if (arg == PARAM_ELECTRON_SHADING)
            electronShading_ = true;
        else if (arg==PARAM_DOF)
        {
            dof_ = true;
            dofStrength_ = atof(argv[++i]);
        }
        else if (arg == PARAM_GRADIENT_BACKGROUND)
            gradientBackground_ = true;
    }
}

void RenderingParameters::display() const
{
    BRAYNS_INFO << "Rendering options: " << std::endl;
    BRAYNS_INFO << "- Samples per pixel       : " <<
                   spp_ << std::endl;
    BRAYNS_INFO << "- Ambient occlusion       : " <<
                   ambientOcclusionStrength_ << std::endl;
    BRAYNS_INFO << "- Shadows                 : " <<
                   (shadows_ ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Soft shadows            : " <<
                   (softShadows_ ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Light shading           : " <<
                   (lightShading_ ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Electron shading        : " <<
                   (electronShading_ ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Depth of field          : " <<
                   (dof_ ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Depth of field strength : " <<
                   dofStrength_ << std::endl;
    BRAYNS_INFO << "- Gradient background     : " <<
                   (gradientBackground_ ? "on": "off") << std::endl;
}

}
