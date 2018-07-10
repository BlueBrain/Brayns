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

#include "OSPRayCamera.h"

#include <brayns/common/log.h>
#include <brayns/parameters/ParametersManager.h>

#include <ospray/SDK/common/OSPCommon.h>

namespace brayns
{
OSPRayCamera::OSPRayCamera(const CameraType type)
    : Camera(type)
    , _currentType(type)
    , _camera{ospNewCamera(
        RenderingParameters::getCameraTypeAsString(type).c_str())}
{
    if (!_camera)
        throw std::runtime_error(
            RenderingParameters::getCameraTypeAsString(type) +
            " is not a registered camera");
}

OSPRayCamera::~OSPRayCamera()
{
    ospRelease(_camera);
}

#define SET_SCALAR(OSP, TYPE) \
    ospSet1##OSP(_camera, prop->name.c_str(), prop->get<TYPE>());
#define SET_STRING() \
    ospSetString(_camera, prop->name.c_str(), prop->get<std::string>().c_str());
#define SET_ARRAY(OSP, TYPE, NUM)            \
    ospSet##OSP(_camera, prop->name.c_str(), \
                prop->get<std::array<TYPE, NUM>>().data());

void OSPRayCamera::commit()
{
    if (!isModified())
        return;

    const auto type = getType();
    if (_currentType != type)
    {
        auto name = RenderingParameters::getCameraTypeAsString(type);
        auto camera = ospNewCamera(name.c_str());
        if (type == CameraType::default_)
        {
            BRAYNS_WARN << name << "is not a registered camera. Using default"
                " camera instead" << std::endl;
            RenderingParameters::resetDefaultCamera();
            name = RenderingParameters::getCameraTypeAsString(type);
            camera = ospNewCamera(name.c_str());
        }
        if (!camera)
            throw std::runtime_error(name + " is not a registered camera");

        ospRelease(_camera);
        _camera = camera;
        _currentType = type;
    }

    const auto& position = getPosition();
    const auto& target = getTarget();
    const auto dir = normalize(target - position);
    const auto& up = getUp();

    ospSet3f(_camera, "pos", position.x(), position.y(), position.z());
    ospSet3f(_camera, "dir", dir.x(), dir.y(), dir.z());
    ospSet3f(_camera, "up", up.x(), up.y(), up.z());
    ospSet1f(_camera, "aspect", getAspectRatio());
    ospSet1f(_camera, "apertureRadius", getAperture());
    ospSet1f(_camera, "focusDistance", getFocalLength());
    ospSet1i(_camera, "stereoMode", static_cast<uint>(getStereoMode()));
    ospSet1f(_camera, "interpupillaryDistance", getEyeSeparation());
    ospSet1f(_camera, "fovy", getFieldOfView());
    ospSet1f(_camera, "architectural", true);
    ospSet1f(_camera, "zeroParallaxPlane", getZeroParallaxPlane());

    for (const auto& prop : _properties.getProperties())
    {
        switch (prop->type)
        {
        case PropertyMap::Property::Type::Float:
            SET_SCALAR(f, float);
            break;
        case PropertyMap::Property::Type::Int:
            SET_SCALAR(i, int32_t);
            break;
        case PropertyMap::Property::Type::String:
            SET_STRING();
            break;
        case PropertyMap::Property::Type::Vec2f:
            SET_ARRAY(2fv, float, 2);
            break;
        case PropertyMap::Property::Type::Vec2i:
            SET_ARRAY(2iv, int32_t, 2);
            break;
        case PropertyMap::Property::Type::Vec3f:
            SET_ARRAY(3fv, float, 3);
            break;
        case PropertyMap::Property::Type::Vec3i:
            SET_ARRAY(3iv, int32_t, 3);
            break;
        case PropertyMap::Property::Type::Vec4f:
            SET_ARRAY(4fv, float, 4);
            break;
        }
    }

    // Clip planes
    const auto& clipPlanes = getClipPlanes();
    if (!clipPlanes.empty())
    {
        auto clipPlaneData =
            ospNewData(clipPlanes.size(), OSP_FLOAT4, clipPlanes.data());
        ospSetData(_camera, "clipPlanes", clipPlaneData);
        ospRelease(clipPlaneData);
    }
    else
        ospRemoveParam(_camera, "clipPlanes");

    ospCommit(_camera);
}

void OSPRayCamera::setEnvironmentMap(const bool environmentMap)
{
    ospSet1i(_camera, "environmentMap", environmentMap);
    ospCommit(_camera);
}
}
