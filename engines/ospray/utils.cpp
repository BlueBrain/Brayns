/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include "utils.h"

#include <brayns/common/PropertyObject.h>
#include <brayns/common/Transformation.h>
#include <brayns/common/log.h>

namespace brayns
{
#define SET_SCALAR(OSP, TYPE)                                       \
    ospSet1##OSP(ospObject, prop->name.c_str(), prop->get<TYPE>()); \
    break;
#define SET_ARRAY(OSP, TYPE, NUM)                           \
    ospSet##OSP(ospObject, prop->name.c_str(),              \
                prop->get<std::array<TYPE, NUM>>().data()); \
    break;
#define SET_ARRAY_FLOAT(OSP, NUM)                                \
    {                                                            \
        std::array<float, NUM> data;                             \
        const auto input = prop->get<std::array<double, NUM>>(); \
        for (size_t i = 0; i < NUM; ++i)                         \
            data[i] = input[i];                                  \
        ospSet##OSP(ospObject, prop->name.c_str(), data.data()); \
        break;                                                   \
    }

void setOSPRayProperties(const PropertyObject& object, OSPObject ospObject)
{
    if (!object.hasProperties())
        return;
    try
    {
        for (const auto& prop : object.getPropertyMap().getProperties())
        {
            switch (prop->type)
            {
            case PropertyMap::Property::Type::Double:
                SET_SCALAR(f, double);
            case PropertyMap::Property::Type::Int:
                SET_SCALAR(i, int32_t);
            case PropertyMap::Property::Type::Bool:
                SET_SCALAR(i, bool);
            case PropertyMap::Property::Type::String:
                ospSetString(ospObject, prop->name.c_str(),
                             prop->get<std::string>().c_str());
                break;
            case PropertyMap::Property::Type::Vec2d:
                SET_ARRAY_FLOAT(2fv, 2);
            case PropertyMap::Property::Type::Vec2i:
                SET_ARRAY(2iv, int32_t, 2);
            case PropertyMap::Property::Type::Vec3d:
                SET_ARRAY_FLOAT(3fv, 3);
            case PropertyMap::Property::Type::Vec3i:
                SET_ARRAY(3iv, int32_t, 3);
            case PropertyMap::Property::Type::Vec4d:
                SET_ARRAY_FLOAT(4fv, 4);
            }
        }
    }
    catch (const std::exception& e)
    {
        BRAYNS_ERROR << "Failed to apply properties for ospObject "
                     << object.getCurrentType() << std::endl;
    }
}

ospcommon::affine3f transformationToAffine3f(
    const Transformation& transformation)
{
    // https://stackoverflow.com/a/18436193
    const auto& quat = transformation.getRotation();
    const float x = atan2(2 * (quat.w() * quat.x() + quat.y() * quat.z()),
                          1 - 2 * (quat.x() * quat.x() + quat.y() * quat.y()));
    const float y = asin(2 * (quat.w() * quat.y() - quat.z() * quat.x()));
    const float z = atan2(2 * (quat.w() * quat.z() + quat.x() * quat.y()),
                          1 - 2 * (quat.y() * quat.y() + quat.z() * quat.z()));

    ospcommon::affine3f rot{ospcommon::one};
    rot = ospcommon::affine3f::rotate({1, 0, 0}, x) * rot;
    rot = ospcommon::affine3f::rotate({0, 1, 0}, y) * rot;
    rot = ospcommon::affine3f::rotate({0, 0, 1}, z) * rot;

    const auto& center = transformation.getRotationCenter();
    const auto& translation = transformation.getTranslation();
    const auto& scale = transformation.getScale();

    return ospcommon::affine3f::translate(
               {float(center.x() / (1. / scale.x())),
                float(center.y() / (1. / scale.y())),
                float(center.z() / (1. / scale.z()))}) *
           rot * ospcommon::affine3f::scale(
                     {float(scale.x()), float(scale.y()), float(scale.z())}) *
           ospcommon::affine3f::translate(
               {float(translation.x() - center.x()),
                float(translation.y() - center.y()),
                float(translation.z() - center.z())});
}

void addInstance(OSPModel rootModel, OSPModel modelToAdd,
                 const Transformation& transform)
{
    auto affine = transformationToAffine3f(transform);
    OSPGeometry instance = ospNewInstance(modelToAdd, (osp::affine3f&)affine);
    ospCommit(instance);
    ospAddGeometry(rootModel, instance);
    ospRelease(instance);
}

void addInstance(OSPModel rootModel, OSPModel modelToAdd,
                 const ospcommon::affine3f& affine)
{
    OSPGeometry instance = ospNewInstance(modelToAdd, (osp::affine3f&)affine);
    ospCommit(instance);
    ospAddGeometry(rootModel, instance);
    ospRelease(instance);
}
}
