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
#include <brayns/common/utils/utils.h>

namespace brayns
{
void toOSPRayProperties(const PropertyMap& object, OSPObject ospObject)
{
    try
    {
        for (const auto& prop : object.getProperties())
        {
            switch (prop->type)
            {
            case Property::Type::Double:
                osphelper::set(ospObject, prop->name.c_str(),
                               static_cast<float>(prop->get<double>()));
                break;
            case Property::Type::Int:
                osphelper::set(ospObject, prop->name.c_str(),
                               prop->get<int32_t>());
                break;
            case Property::Type::Bool:
                osphelper::set(ospObject, prop->name.c_str(),
                               prop->get<bool>());
                break;
            case Property::Type::String:
                osphelper::set(ospObject, prop->name.c_str(),
                               prop->get<std::string>());
                break;
            case Property::Type::Vec2d:
                osphelper::set(ospObject, prop->name.c_str(),
                               Vector2f(toGlmVec(
                                   prop->get<std::array<double, 2>>())));
                break;
            case Property::Type::Vec2i:
                osphelper::set(ospObject, prop->name.c_str(),
                               toGlmVec(prop->get<std::array<int32_t, 2>>()));
                break;
            case Property::Type::Vec3d:
                osphelper::set(ospObject, prop->name.c_str(),
                               Vector3f(toGlmVec(
                                   prop->get<std::array<double, 3>>())));
                break;
            case Property::Type::Vec3i:
                osphelper::set(ospObject, prop->name.c_str(),
                               toGlmVec(prop->get<std::array<int32_t, 3>>()));
                break;
            case Property::Type::Vec4d:
                osphelper::set(ospObject, prop->name.c_str(),
                               Vector4f(toGlmVec(
                                   prop->get<std::array<double, 4>>())));
                break;
            }
        }
    }
    catch (const std::exception& e)
    {
        BRAYNS_ERROR << "Failed to apply properties for ospObject" << std::endl;
    }
}

void toOSPRayProperties(const PropertyObject& object, OSPObject ospObject)
{
    toOSPRayProperties(object.getPropertyMap(), ospObject);
}

template <typename T, std::size_t N>
auto _toStdArray(const ospcommon::vec_t<T, N>& input)
{
    std::array<T, N> array;
    array.fill(0);
    std::copy(&input[0], &input[N - 1], array.begin());
    return array;
}

void fromOSPRayProperties(PropertyMap& object, ospray::ManagedObject& ospObject)
{
    for (const auto& prop : object.getProperties())
    {
        switch (prop->type)
        {
        case Property::Type::Double:
            prop->set(double(
                ospObject.getParam1f(prop->name.c_str(), prop->get<double>())));
            break;
        case Property::Type::Int:
            prop->set(
                ospObject.getParam1i(prop->name.c_str(), prop->get<int32_t>()));
            break;
        case Property::Type::Bool:
            // FIXME(jonask): When supported by OSPRay use bool
            // bool's are stored as int within ospray
            prop->set(static_cast<bool>(
                ospObject.getParam(prop->name.c_str(),
                                   static_cast<int>(prop->get<bool>()))));
            break;
        case Property::Type::String:
            prop->set(ospObject.getParamString(prop->name.c_str(),
                                               prop->get<std::string>()));
            break;
        case Property::Type::Vec2d:
            prop->set(_toStdArray<double, 2>(
                ospObject.getParam<ospcommon::vec2f>(prop->name.c_str(),
                                                     ospcommon::vec2f())));
            break;
        case Property::Type::Vec2i:
            prop->set(_toStdArray<int32_t, 2>(
                ospObject.getParam<ospcommon::vec2i>(prop->name.c_str(),
                                                     ospcommon::vec2i())));
            break;
        case Property::Type::Vec3d:
            prop->set(_toStdArray<double, 3>(
                ospObject.getParam<ospcommon::vec3f>(prop->name.c_str(),
                                                     ospcommon::vec3f())));
            break;
        case Property::Type::Vec3i:
            prop->set(_toStdArray<int32_t, 3>(
                ospObject.getParam<ospcommon::vec3i>(prop->name.c_str(),
                                                     ospcommon::vec3i())));
            break;
        case Property::Type::Vec4d:
            prop->set(_toStdArray<double, 4>(
                ospObject.getParam<ospcommon::vec4f>(prop->name.c_str(),
                                                     ospcommon::vec4f())));
            break;
        }
    }
}

ospcommon::affine3f transformationToAffine3f(
    const Transformation& transformation)
{
    // https://stackoverflow.com/a/18436193
    const auto& quat = transformation.getRotation();
    const float x = atan2(2 * (quat.w * quat.x + quat.y * quat.z),
                          1 - 2 * (quat.x * quat.x + quat.y * quat.y));
    const float y = asin(2 * (quat.w * quat.y - quat.z * quat.x));
    const float z = atan2(2 * (quat.w * quat.z + quat.x * quat.y),
                          1 - 2 * (quat.y * quat.y + quat.z * quat.z));

    ospcommon::affine3f rot{ospcommon::one};
    rot = ospcommon::affine3f::rotate({1, 0, 0}, x) * rot;
    rot = ospcommon::affine3f::rotate({0, 1, 0}, y) * rot;
    rot = ospcommon::affine3f::rotate({0, 0, 1}, z) * rot;

    const auto& rotationCenter = transformation.getRotationCenter();
    const auto& translation = transformation.getTranslation();
    const auto& scale = transformation.getScale();

    return ospcommon::affine3f::scale(
               {float(scale.x), float(scale.y), float(scale.z)}) *
           ospcommon::affine3f::translate({float(translation.x),
                                           float(translation.y),
                                           float(translation.z)}) *
           ospcommon::affine3f::translate({float(rotationCenter.x),
                                           float(rotationCenter.y),
                                           float(rotationCenter.z)}) *
           rot *
           ospcommon::affine3f::translate({float(-rotationCenter.x),
                                           float(-rotationCenter.y),
                                           float(-rotationCenter.z)});
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

namespace osphelper
{
void set(OSPObject obj, const char* id, const char* s)
{
    ospSetString(obj, id, s);
}
void set(OSPObject obj, const char* id, const std::string& s)
{
    ospSetString(obj, id, s.c_str());
}
void set(OSPObject obj, const char* id, float v)
{
    ospSet1f(obj, id, v);
}
void set(OSPObject obj, const char* id, bool v)
{
    ospSet1b(obj, id, v);
}
void set(OSPObject obj, const char* id, int32_t v)
{
    ospSet1i(obj, id, v);
}
void set(OSPObject obj, const char* id, const Vector2f& v)
{
    ospSet2fv(obj, id, glm::value_ptr(v));
}
void set(OSPObject obj, const char* id, const Vector2i& v)
{
    ospSet2iv(obj, id, glm::value_ptr(v));
}
void set(OSPObject obj, const char* id, const Vector3f& v)
{
    ospSet3fv(obj, id, glm::value_ptr(v));
}
void set(OSPObject obj, const char* id, const Vector3i& v)
{
    ospSet3iv(obj, id, glm::value_ptr(v));
}
void set(OSPObject obj, const char* id, const Vector4f& v)
{
    ospSet4fv(obj, id, glm::value_ptr(v));
}
} // namespace osphelper
} // namespace brayns
