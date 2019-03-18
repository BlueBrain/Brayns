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

#pragma once

#include <brayns/common/types.h>
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
template <size_t S>
std::vector<std::array<float, S>> convertVectorToFloat(
    const std::vector<std::array<double, S>>& input)
{
    std::vector<std::array<float, S>> output;
    output.reserve(input.size());
    for (const auto& value : input)
    {
        std::array<float, S> converted;
        std::copy(value.data(), value.data() + S, converted.data());
        output.push_back(converted);
    }
    return output;
}

namespace osphelper
{
/** Helper methods for setting properties on OSPRay object */
void set(OSPObject obj, const char* id, const char* s);
void set(OSPObject obj, const char* id, const std::string& s);

void set(OSPObject obj, const char* id, float v);
void set(OSPObject obj, const char* id, bool v);
void set(OSPObject obj, const char* id, int32_t v);

void set(OSPObject obj, const char* id, const Vector2f& v);
void set(OSPObject obj, const char* id, const Vector2i& v);

void set(OSPObject obj, const char* id, const Vector3f& v);
void set(OSPObject obj, const char* id, const Vector3i& v);

void set(OSPObject obj, const char* id, const Vector4f& v);
} // namespace osphelper
} // namespace brayns
