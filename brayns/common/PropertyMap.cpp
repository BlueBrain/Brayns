/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 *
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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

#include "PropertyMap.h"

#include <array>

namespace
{
template <typename TDest, typename TSrc, size_t S>
std::array<TDest, S> convertArray(const std::array<TSrc, S> src)
{
    std::array<TDest, S> dest;
    for (size_t i = 0; i < S; i++)
        dest[i] = static_cast<TDest>(src[i]);
    return dest;
}
}

namespace brayns
{
void PropertyMap::merge(const PropertyMap& input)
{
    const auto setValues = [](Property& dest, const Property& src) {
        switch (dest.type)
        {
        case Property::Type::Int:
            dest.set<int32_t>(static_cast<int32_t>(src.get<double>()));
            break;
        case Property::Type::Double:
            dest.set<double>(static_cast<double>(src.get<int32_t>()));
            break;
        case Property::Type::Vec2i:
            dest.set<std::array<int32_t, 2>>(convertArray<int32_t, double, 2>(
                src.get<std::array<double, 2>>()));
            break;
        case Property::Type::Vec2d:
            dest.set<std::array<double, 2>>(convertArray<double, int32_t, 2>(
                src.get<std::array<int32_t, 2>>()));
            break;
        case Property::Type::Vec3i:
            dest.set<std::array<int32_t, 3>>(convertArray<int32_t, double, 3>(
                src.get<std::array<double, 3>>()));
            break;
        case Property::Type::Vec3d:
            dest.set<std::array<double, 3>>(convertArray<double, int32_t, 3>(
                src.get<std::array<int32_t, 3>>()));
            break;
        default:
            break;
        };
    };

    const auto isCompatbileTypes = [](Property::Type t0, Property::Type t1) {
        return (t0 == Property::Type::Int && t1 == Property::Type::Double) ||
               (t0 == Property::Type::Double && t1 == Property::Type::Int) ||
               (t0 == Property::Type::Vec2i && t1 == Property::Type::Vec2d) ||
               (t0 == Property::Type::Vec2d && t1 == Property::Type::Vec2i) ||
               (t0 == Property::Type::Vec3i && t1 == Property::Type::Vec3d) ||
               (t0 == Property::Type::Vec3d && t1 == Property::Type::Vec3i);
    };

    for (const std::shared_ptr<Property>& otherProperty : input.getProperties())
    {
        const auto& name = otherProperty->name;

        if (auto myProperty = findProperty(name))
        {
            const auto myType = myProperty->type;
            const auto otherType = otherProperty->type;

            if (myType == otherType)
            {
                myProperty->_data = otherProperty->_data;
            }
            else if (isCompatbileTypes(myType, otherType))
            {
                setValues(*myProperty, *otherProperty);
            }
            else
            {
                throw std::runtime_error("Incompatible types for property '" +
                                         name + "'");
            }
        }
        else
        {
            setProperty(*otherProperty.get());
        }
    }
}
}
