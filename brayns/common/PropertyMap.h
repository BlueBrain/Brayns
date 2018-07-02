/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 *
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include <boost/any.hpp>
#include <memory>
#include <string>
#include <vector>

namespace brayns
{
/**
 * Container class for holding properties that are mapped by name to a supported
 * C++ type and their respective value.
 */
class PropertyMap
{
public:
    PropertyMap() = default;

    struct Property
    {
        enum class Type
        {
            Int,
            Float,
            String,
            Vec2i,
            Vec2f,
            Vec3i,
            Vec3f,
            Vec4f
        };

        Property(const std::string &name_, Type type_)
            : name(name_)
            , type(type_)
        {
        }

        template <typename T>
        void set(const T &v)
        {
            _data = v;
        }

        template <typename T>
        T get() const
        {
            return boost::any_cast<T>(_data);
        }

        void setData(const boost::any &data) { _data = data; }
        const boost::any &getData() const { return _data; }
        const std::string name;
        const Type type;

    private:
        boost::any _data;
    };

    /**
     * Set the property of the given name; will create the property if it does
     * not exist.
     */
    template <typename T>
    inline void setProperty(const std::string &name, const T &t)
    {
        auto property = findProperty(name);
        if (!property)
        {
            _properties.push_back(
                std::make_shared<Property>(name, getType<T>()));
            property = _properties[_properties.size() - 1].get();
        }
        property->set(t);
    }

    /** Update or add the given property. */
    void setProperty(const Property &newProperty)
    {
        auto property = findProperty(newProperty.name);
        if (!property)
        {
            _properties.push_back(std::make_shared<Property>(newProperty));
        }
        else
            property->setData(newProperty.getData());
    }

    /**
     * @return the property value of the given name or valIfNotFound otherwise.
     */
    template <typename T>
    inline T getProperty(const std::string &name, T valIfNotFound)
    {
        auto property = findProperty(name);
        if (property)
            return property->get<T>();
        return valIfNotFound;
    }

    /**
     * @return the property value of the given name
     * @throw std::runtime_error if value property value was not found.
     */
    template <typename T>
    inline T getProperty(const std::string &name)
    {
        auto property = findProperty(name);
        if (property)
            return property->get<T>();
        throw std::runtime_error("No property found with name " + name);
    }

    /** @return all the registered properties. */
    const auto &getProperties() const { return _properties; }
private:
    template <typename T>
    PropertyMap::Property::Type getType();

    Property *findProperty(const std::string &name)
    {
        auto foundProperty =
            std::find_if(_properties.begin(), _properties.end(),
                         [&](const auto &p) { return p->name == name; });

        return foundProperty != _properties.end() ? foundProperty->get()
                                                  : nullptr;
    }

    std::vector<std::shared_ptr<Property>> _properties;
};

template <>
inline PropertyMap::Property::Type PropertyMap::getType<float>()
{
    return PropertyMap::Property::Type::Float;
}
template <>
inline PropertyMap::Property::Type PropertyMap::getType<int32_t>()
{
    return PropertyMap::Property::Type::Int;
}
template <>
inline PropertyMap::Property::Type PropertyMap::getType<std::string>()
{
    return PropertyMap::Property::Type::String;
}
template <>
inline PropertyMap::Property::Type PropertyMap::getType<std::array<float, 2>>()
{
    return PropertyMap::Property::Type::Vec2f;
}
template <>
inline PropertyMap::Property::Type
    PropertyMap::getType<std::array<int32_t, 2>>()
{
    return PropertyMap::Property::Type::Vec2i;
}
template <>
inline PropertyMap::Property::Type PropertyMap::getType<std::array<float, 3>>()
{
    return PropertyMap::Property::Type::Vec3f;
}
template <>
inline PropertyMap::Property::Type
    PropertyMap::getType<std::array<int32_t, 3>>()
{
    return PropertyMap::Property::Type::Vec3i;
}
template <>
inline PropertyMap::Property::Type PropertyMap::getType<std::array<float, 4>>()
{
    return PropertyMap::Property::Type::Vec4f;
}
}
