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

#pragma once

#include <brayns/api.h>
#include <brayns/common/PropertyObject.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(Light)

namespace brayns
{
/**
 * @brief The LightType enum defines the different types of light
 */
enum class LightType
{
    SPHERE,
    DIRECTIONAL,
    QUAD,
    SPOTLIGHT
};

/**
 * @brief The Light class defines a light source by type, id and properties
 */
class Light : public PropertyMap
{
public:
    Light() = default;
    Light(const LightType type, const Vector3f& color, const float intensity,
          const int id);

    LightType getType() const;
    int getId() const { return _id; }
private:
    std::string _type;
    int _id{-1};
    SERIALIZATION_FRIEND(Light);
};

} // namespace brayns
