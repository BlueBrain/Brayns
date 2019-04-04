/* Copyright (c) 2015-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "VolumeParameters.h"

namespace
{
const std::string PARAM_VOLUME_DIMENSIONS = "volume-dimensions";
const std::string PARAM_VOLUME_ELEMENT_SPACING = "volume-element-spacing";
const std::string PARAM_VOLUME_OFFSET = "volume-offset";
}

namespace brayns
{
VolumeParameters::VolumeParameters()
    : AbstractParameters("Volume")
    , _dimensions(0, 0, 0)
    , _elementSpacing(1.f, 1.f, 1.f)
    , _offset(0.f, 0.f, 0.f)
{
    _parameters.add_options()(PARAM_VOLUME_DIMENSIONS.c_str(),
                              po::fixed_tokens_value<uints>(3, 3),
                              "Volume dimensions [uint uint uint]")(
        PARAM_VOLUME_ELEMENT_SPACING.c_str(),
        po::fixed_tokens_value<floats>(3, 3),
        "Element spacing in the volume [float float float]")(
        PARAM_VOLUME_OFFSET.c_str(), po::fixed_tokens_value<floats>(3, 3),
        "Volume offset [float float float]");
}

void VolumeParameters::parse(const po::variables_map& vm)
{
    if (vm.count(PARAM_VOLUME_DIMENSIONS))
    {
        auto values = vm[PARAM_VOLUME_DIMENSIONS].as<uints>();
        _dimensions = Vector3ui(values[0], values[1], values[2]);
    }
    if (vm.count(PARAM_VOLUME_ELEMENT_SPACING))
    {
        auto values = vm[PARAM_VOLUME_ELEMENT_SPACING].as<floats>();
        _elementSpacing = Vector3f(values[0], values[1], values[2]);
    }
    if (vm.count(PARAM_VOLUME_OFFSET))
    {
        auto values = vm[PARAM_VOLUME_OFFSET].as<floats>();
        _offset = Vector3f(values[0], values[1], values[2]);
    }
    markModified();
}

void VolumeParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Dimensions      : " << _dimensions << std::endl;
    BRAYNS_INFO << "Element spacing : " << _elementSpacing << std::endl;
    BRAYNS_INFO << "Offset          : " << _offset << std::endl;
}
}
