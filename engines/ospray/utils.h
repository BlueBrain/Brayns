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

#pragma once

#include <brayns/common/PropertyObject.h>
#include <brayns/common/Transformation.h>

#include <ospray/SDK/common/Managed.h>
#include <ospray/SDK/common/OSPCommon.h>

namespace brayns
{
/**
 * Set all the properties from the current property map of the given object to
 * the given ospray object.
 */
void toOSPRayProperties(const PropertyObject& object, OSPObject ospObject);
void toOSPRayProperties(const PropertyMap& object, OSPObject ospObject);

/** Update all the properties in the property map from the given ospray object.
 */
void fromOSPRayProperties(PropertyMap& object,
                          ospray::ManagedObject& ospObject);

/** Convert a brayns::Transformation to an ospcommon::affine3f. */
ospcommon::affine3f transformationToAffine3f(
    const Transformation& transformation);

/** Helper to add the given model as an instance to the given root model. */
void addInstance(OSPModel rootModel, OSPModel modelToAdd,
                 const Transformation& transform);
void addInstance(OSPModel rootModel, OSPModel modelToAdd,
                 const ospcommon::affine3f& affine);

/** Helper to convert a vector of double tuples to a vector of float tuples. */
template <int S>
std::vector<glm::vec<S, float>> convertVectorToFloat(
    const std::vector<glm::vec<S, double>>& input)
{
    std::vector<glm::vec<S, float>> output;
    output.reserve(input.size());
    for (const auto& value : input)
    {
        glm::vec<S, float> converted;
        for (int i = 0; i < S; ++i)
        {
            converted[i] = float(value[i]);
        }
        output.push_back(converted);
    }
    return output;
}

namespace osphelper
{
/** Helper methods for setting properties on OSPRay object */
void set(OSPObject obj, const char* id, const char* s);
void set(OSPObject obj, const char* id, const std::string& s);
void set(OSPObject obj, const char* id, double v);
void set(OSPObject obj, const char* id, float v);
void set(OSPObject obj, const char* id, bool v);
void set(OSPObject obj, const char* id, int32_t v);
void set(OSPObject obj, const char* id, const Vector2f& v);
void set(OSPObject obj, const char* id, const Vector2d& v);
void set(OSPObject obj, const char* id, const Vector2i& v);
void set(OSPObject obj, const char* id, const Vector3f& v);
void set(OSPObject obj, const char* id, const Vector3d& v);
void set(OSPObject obj, const char* id, const Vector3i& v);
void set(OSPObject obj, const char* id, const Vector4f& v);
void set(OSPObject obj, const char* id, const Vector4d& v);
} // namespace osphelper
} // namespace brayns
