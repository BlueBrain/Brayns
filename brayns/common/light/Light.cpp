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

#include "Light.h"

#include "brayns/common/utils/enumUtils.h"
#include "brayns/common/utils/utils.h"

namespace brayns
{
template <>
inline std::vector<std::pair<std::string, brayns::LightType>> enumMap()
{
    return {{"sphere", brayns::LightType::SPHERE},
            {"directional", brayns::LightType::DIRECTIONAL},
            {"quad", brayns::LightType::QUAD},
            {"spotlight", brayns::LightType::SPOTLIGHT}};
}
} // namespace brayns

namespace brayns
{
Light::Light(const LightType type, const Vector3f& color, const float intensity,
             const int id)
    : PropertyMap()
    , _type(enumToString(type))
    , _id(id)
{
    setProperty({"color", toArray<3, double>(color)});
    setProperty({"intensity", static_cast<double>(intensity)});
}

LightType Light::getType() const
{
    return stringToEnum<LightType>(_type);
}

} // namespace brayns
