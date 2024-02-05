/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <brayns/common/BaseObject.h>
#include <brayns/common/propertymap/PropertyMap.h>

namespace brayns
{
constexpr auto PARAM_RESOLUTION_SCALING = "resolutionScaling";
constexpr auto PARAM_CAMERA_SCALING = "cameraScaling";

class OpenDeckParameters : public BaseObject
{
public:
    OpenDeckParameters();

    bool parse(int argc, const char** argv);

    double getResolutionScaling() const
    {
        return _props[PARAM_RESOLUTION_SCALING].as<double>();
    }
    void setResolutionScaling(const double resScaling)
    {
        _updateProperty(PARAM_RESOLUTION_SCALING, resScaling);
    }

    double getCameraScaling() const
    {
        return _props[PARAM_CAMERA_SCALING].as<double>();
    }
    void setCameraScaling(const double cameraScaling)
    {
        _updateProperty(PARAM_CAMERA_SCALING, cameraScaling);
    }

    const PropertyMap& getPropertyMap() const { return _props; }
    PropertyMap& getPropertyMap() { return _props; }

private:
    PropertyMap _props;

    template <typename T>
    void _updateProperty(const char* property, const T& newValue)
    {
        if (!_isEqual(_props[property].as<T>(), newValue))
        {
            _props.update(property, newValue);
            markModified();
        }
    }
};
} // namespace brayns
