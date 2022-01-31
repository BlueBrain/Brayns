/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include <brayns/common/Log.h>

namespace
{
const std::string PARAM_VOLUME_DIMENSIONS = "volume-dimensions";
const std::string PARAM_VOLUME_ELEMENT_SPACING = "volume-element-spacing";
const std::string PARAM_VOLUME_OFFSET = "volume-offset";
} // namespace

namespace brayns
{
VolumeParameters::VolumeParameters()
    : AbstractParameters("Volume")
    , _dimensions(0, 0, 0)
    , _elementSpacing(1.f, 1.f, 1.f)
    , _offset(0.f, 0.f, 0.f)
{
    _parameters.add_options()(
        PARAM_VOLUME_DIMENSIONS.c_str(),
        po::fixed_tokens_value<std::vector<uint32_t>>(3, 3),
        "Volume dimensions [uint uint uint]")(
        PARAM_VOLUME_ELEMENT_SPACING.c_str(),
        po::fixed_tokens_value<std::vector<float>>(3, 3),
        "Element spacing in the volume [float float "
        "float]")(PARAM_VOLUME_OFFSET.c_str(), po::fixed_tokens_value<std::vector<float>>(3, 3), "Volume offset [float float float]");
}

void VolumeParameters::parse(const po::variables_map &vm)
{
    if (vm.count(PARAM_VOLUME_DIMENSIONS))
    {
        auto values = vm[PARAM_VOLUME_DIMENSIONS].as<std::vector<uint32_t>>();
        _dimensions = Vector3ui(values[0], values[1], values[2]);
    }
    if (vm.count(PARAM_VOLUME_ELEMENT_SPACING))
    {
        auto values = vm[PARAM_VOLUME_ELEMENT_SPACING].as<std::vector<float>>();
        _elementSpacing = Vector3f(values[0], values[1], values[2]);
    }
    if (vm.count(PARAM_VOLUME_OFFSET))
    {
        auto values = vm[PARAM_VOLUME_OFFSET].as<std::vector<float>>();
        _offset = Vector3f(values[0], values[1], values[2]);
    }
    markModified();
}

void VolumeParameters::print()
{
    AbstractParameters::print();
    Log::info("Dimensions      : {}", _dimensions);
    Log::info("Element spacing : {}", _elementSpacing);
    Log::info("Offset          : {}", _offset);
}

const Vector3ui &VolumeParameters::getDimensions() const
{
    return _dimensions;
}

void VolumeParameters::setDimensions(const Vector3ui &dim)
{
    _updateValue(_dimensions, dim);
}

const Vector3d &VolumeParameters::getElementSpacing() const
{
    return _elementSpacing;
}

void VolumeParameters::setElementSpacing(const Vector3d &spacing)
{
    _updateValue(_elementSpacing, spacing);
}

const Vector3d &VolumeParameters::getOffset() const
{
    return _offset;
}

void VolumeParameters::setOffset(const Vector3d &offset)
{
    _updateValue(_offset, offset);
}

void VolumeParameters::setGradientShading(const bool enabled)
{
    _updateValue(_gradientShading, enabled);
}

bool VolumeParameters::getGradientShading() const
{
    return _gradientShading;
}

void VolumeParameters::setSingleShade(const bool enabled)
{
    _updateValue(_singleShade, enabled);
}

bool VolumeParameters::getSingleShade() const
{
    return _singleShade;
}

void VolumeParameters::setPreIntegration(const bool enabled)
{
    _updateValue(_preIntegration, enabled);
}

bool VolumeParameters::getPreIntegration() const
{
    return _preIntegration;
}

void VolumeParameters::setAdaptiveSampling(const bool enabled)
{
    _updateValue(_adaptiveSampling, enabled);
}

bool VolumeParameters::getAdaptiveSampling() const
{
    return _adaptiveSampling;
}

void VolumeParameters::setAdaptiveMaxSamplingRate(const double value)
{
    _updateValue(_adaptiveMaxSamplingRate, value);
}

double VolumeParameters::getAdaptiveMaxSamplingRate() const
{
    return _adaptiveMaxSamplingRate;
}

void VolumeParameters::setSamplingRate(const double value)
{
    _updateValue(_samplingRate, value);
}

double VolumeParameters::getSamplingRate() const
{
    return _samplingRate;
}

void VolumeParameters::setSpecular(const Vector3d &value)
{
    _updateValue(_specular, value);
}

const Vector3d &VolumeParameters::getSpecular() const
{
    return _specular;
}

void VolumeParameters::setClipBox(const Boxd &value)
{
    _updateValue(_clipBox, value);
}

const Boxd &VolumeParameters::getClipBox() const
{
    return _clipBox;
}
} // namespace brayns
