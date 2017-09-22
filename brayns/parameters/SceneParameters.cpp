/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "SceneParameters.h"

namespace
{
const std::string PARAM_ANIMATION_FRAME = "animation-frame";
const std::string PARAM_COLOR_MAP_FILE = "color-map-file";
const std::string PARAM_ENVIRONMENT_MAP = "environment-map";
}

namespace brayns
{
SceneParameters::SceneParameters()
    : AbstractParameters("Scene")
{
    _parameters.add_options()(PARAM_ANIMATION_FRAME.c_str(),
                              po::value<uint32_t>(),
                              "Scene animation frame [float]")(
        PARAM_COLOR_MAP_FILE.c_str(), po::value<std::string>(),
        "Color map filename [string]")(PARAM_ENVIRONMENT_MAP.c_str(),
                                       po::value<std::string>(),
                                       "Environment map filename [string]");
}

bool SceneParameters::_parse(const po::variables_map& vm)
{
    if (vm.count(PARAM_ANIMATION_FRAME))
        _animationFrame = vm[PARAM_ANIMATION_FRAME].as<uint32_t>();
    if (vm.count(PARAM_COLOR_MAP_FILE))
        _colorMapFilename = vm[PARAM_COLOR_MAP_FILE].as<std::string>();
    if (vm.count(PARAM_ENVIRONMENT_MAP))
        _environmentMap = vm[PARAM_ENVIRONMENT_MAP].as<std::string>();
    return true;
}

void SceneParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Animation frame          :" << _animationFrame << std::endl;
    BRAYNS_INFO << "Color Map filename       :" << _colorMapFilename
                << std::endl;
    BRAYNS_INFO << "Environment map filename : " << _environmentMap
                << std::endl;
}
}
