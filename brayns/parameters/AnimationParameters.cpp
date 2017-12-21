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

#include "AnimationParameters.h"

namespace
{
const std::string PARAM_ANIMATION_FRAME = "animation-frame";
}

namespace brayns
{
AnimationParameters::AnimationParameters()
    : AbstractParameters("Animation")
{
    _parameters.add_options()(PARAM_ANIMATION_FRAME.c_str(),
                              po::value<uint32_t>(),
                              "Scene animation frame [float]");
}

bool AnimationParameters::_parse(const po::variables_map& vm)
{
    if (vm.count(PARAM_ANIMATION_FRAME))
        _current = vm[PARAM_ANIMATION_FRAME].as<uint32_t>();
    return true;
}

void AnimationParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Animation frame          :" << _current << std::endl;
}
}
