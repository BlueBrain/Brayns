/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "AbstractParameters.h"

#include <brayns/common/Log.h>

namespace brayns
{
AbstractParameters::AbstractParameters(const std::string &name)
    : _name(name)
    , _parameters(name)
{
}

void AbstractParameters::print()
{
    Log::info("-= {} parameters =-", _name);
}

po::options_description &AbstractParameters::parameters()
{
    return _parameters;
}

std::string AbstractParameters::asString(const bool flag)
{
    return flag ? "on" : "off";
}
} // namespace brayns