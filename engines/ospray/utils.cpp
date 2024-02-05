/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/Log.h>
#include <brayns/common/PropertyObject.h>
#include <brayns/common/Transformation.h>

namespace brayns
{
void toOSPRayProperties(const PropertyMap& object, OSPObject ospObject)
{
    try
    {
        for (const auto& prop : object)
        {
            auto setProperty = [&](const auto& value) {
                osphelper::set(ospObject, prop.getName().c_str(), value);
            };

            prop.visit<double>(setProperty);
            prop.visit<int>(setProperty);
            prop.visit<bool>(setProperty);
            prop.visit<std::string>(setProperty);
            prop.visit<Vector2d>(setProperty);
            prop.visit<Vector2i>(setProperty);
            prop.visit<Vector3d>(setProperty);
            prop.visit<Vector3i>(setProperty);
            prop.visit<Vector4d>(setProperty);
        }
    }
    catch (const std::exception& e)
    {
        Log::error("Failed to apply properties for ospObject.");
    }
}

void toOSPRayProperties(const PropertyObject& object, OSPObject ospObject)
{
    toOSPRayProperties(object.getPropertyMap(), ospObject);
}

template <typename T, typename U, int S>
auto _toGlm(const ospcommon::vec_t<U, S>& input)
{
    glm::vec<S, T> glm;
    for (int i = 0; i < S; ++i)
    {
        glm[i] = T(input[i]);
    }
    return glm;
}

void fromOSPRayProperties(PropertyMap& object, ospray::ManagedObject& ospObject)
{
    for (auto& prop : object)
    {
        auto name = prop.getName().c_str();
        prop.visit<double>([&](const auto& value) {
            double newValue = ospObject.getParam1f(name, float(value));
            prop.setValue(newValue);
        });
        prop.visit<int>([&](const auto& value) {
            int newValue = ospObject.getParam1i(name, value);
            prop.setValue(newValue);
        });
        prop.visit<bool>([&](const auto& value) {
            bool newValue = ospObject.getParam(name, int(value));
            prop.setValue(newValue);
        });
        prop.visit<std::string>([&](const auto& value) {
            auto newValue = ospObject.getParam(name, value);
            prop.setValue(newValue);
        });
        prop.visit<Vector2d>([&](const auto&) {
            auto newValue = ospObject.getParam(name, ospcommon::vec2f());
            prop.setValue(_toGlm<double>(newValue));
        });
        prop.visit<Vector2i>([&](const auto&) {
            auto newValue = ospObject.getParam(name, ospcommon::vec2i());
            prop.setValue(_toGlm<int>(newValue));
        });
        prop.visit<Vector3d>([&](const auto&) {
            auto newValue = ospObject.getParam(name, ospcommon::vec3f());
            prop.setValue(_toGlm<double>(newValue));
        });
        prop.visit<Vector3i>([&](const auto&) {
            auto newValue = ospObject.getParam(name, ospcommon::vec3i());
            prop.setValue(_toGlm<int>(newValue));
        });
        prop.visit<Vector4d>([&](const auto&) {
            auto newValue = ospObject.getParam(name, ospcommon::vec4f());
            prop.setValue(_toGlm<double>(newValue));
        });
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

    // We start scaling the object,
    // then we move it at `translation` location,
    // and we rotate it around `rotationCenter`.
    //
    // To make the object rotate around its own center,
    // juste set `rotationCenter` to be equal to `translation`.
    return ospcommon::affine3f::translate({float(rotationCenter.x),
                                           float(rotationCenter.y),
                                           float(rotationCenter.z)}) *
           rot *
           ospcommon::affine3f::translate(
               {float(translation.x - rotationCenter.x),
                float(translation.y - rotationCenter.y),
                float(translation.z - rotationCenter.z)}) *
           ospcommon::affine3f::scale(
               {float(scale.x), float(scale.y), float(scale.z)});
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
void set(OSPObject obj, const char* id, double v)
{
    ospSet1f(obj, id, float(v));
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
void set(OSPObject obj, const char* id, const Vector2d& v)
{
    Vector2f tmp = v;
    ospSet2fv(obj, id, glm::value_ptr(tmp));
}
void set(OSPObject obj, const char* id, const Vector2i& v)
{
    ospSet2iv(obj, id, glm::value_ptr(v));
}
void set(OSPObject obj, const char* id, const Vector3f& v)
{
    ospSet3fv(obj, id, glm::value_ptr(v));
}
void set(OSPObject obj, const char* id, const Vector3d& v)
{
    Vector3f tmp = v;
    ospSet3fv(obj, id, glm::value_ptr(tmp));
}
void set(OSPObject obj, const char* id, const Vector3i& v)
{
    ospSet3iv(obj, id, glm::value_ptr(v));
}
void set(OSPObject obj, const char* id, const Vector4f& v)
{
    ospSet4fv(obj, id, glm::value_ptr(v));
}
void set(OSPObject obj, const char* id, const Vector4d& v)
{
    Vector4f tmp = v;
    ospSet4fv(obj, id, glm::value_ptr(tmp));
}
} // namespace osphelper
} // namespace brayns
