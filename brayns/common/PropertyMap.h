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

    /**
     * Defines a single property which always has a name, a label for e.g. UIs
     * and a typed value. Additionally, a user-defined minimum and maximum value
     * range can be specified, otherwise the types' limits apply.
     */
    struct Property
    {
        enum class Type
        {
            Int,
            Float,
            String,
            Bool,
            Vec2i,
            Vec2f,
            Vec3i,
            Vec3f,
            Vec4f
        };

        template <typename T>
        Property(const std::string& name_, const std::string& label_,
                 const T& value)
            : name(name_)
            , label(label_)
            , type(_getType<T>())
            , _data(value)
            , _min(std::numeric_limits<T>::min())
            , _max(std::numeric_limits<T>::max())
        {
        }

        template <typename T>
        Property(const std::string& name_, const std::string& label_,
                 const T& value, const std::pair<T, T>& limit)
            : name(name_)
            , label(label_)
            , type(_getType<T>())
            , _data(value)
            , _min(limit.first)
            , _max(limit.second)
        {
        }

        /**
         * Specialized for enum properties: type is int32_t, and the possible
         * enum values are passed in enums_. The enum/int value and the
         * corresponding string maps to the index in the vector.
         */
        template <typename T>
        Property(
            const std::string& name_, const std::string& label_, const T& value,
            const std::vector<std::string>& enums_,
            typename std::enable_if<std::is_same<T, int32_t>::value>::type* = 0)
            : name(name_)
            , label(label_)
            , type(_getType<T>())
            , enums(enums_)
            , _data(value)
            , _min(0)
            , _max(enums_.size())
        {
        }

        template <typename T>
        void set(const T& v)
        {
            _data = v;
        }

        template <typename T>
        T get() const
        {
            return boost::any_cast<T>(_data);
        }

        template <typename T>
        T min() const
        {
            return boost::any_cast<T>(_min);
        }

        template <typename T>
        T max() const
        {
            return boost::any_cast<T>(_max);
        }

        /**
         * Read-only property shall not be modified from the outside aka web API
         * via JSON.
         */
        void markReadOnly() { _readOnly = true; }
        bool readOnly() const { return _readOnly; }
        const std::string name;
        const std::string label; //!< user-friendly name of the property
        const Type type;

        /**
         * Name of enum values that are mapped to the integer value based on
         * the index.
         */
        const std::vector<std::string> enums;

    private:
        friend class PropertyMap;
        boost::any _data;
        const boost::any _min;
        const boost::any _max;
        bool _readOnly{false};
        template <typename T>
        Type _getType();
    };

    /** Update the property of the given name */
    template <typename T>
    inline void updateProperty(const std::string& name, const T& t)
    {
        if (auto property = findProperty(name))
        {
            if (property->type != property->_getType<T>())
                throw std::runtime_error(
                    "updateProperty does not allow for changing the type");
            property->set(t);
        }
    }

    /** Update or add the given property. */
    void setProperty(const Property& newProperty)
    {
        if (auto property = findProperty(newProperty.name))
        {
            if (property->type != newProperty.type)
                throw std::runtime_error(
                    "setProperty does not allow for changing the type");
            property->_data = newProperty._data;
        }
        else
            _properties.push_back(std::make_shared<Property>(newProperty));
    }

    /**
     * @return the property value of the given name or valIfNotFound otherwise.
     */
    template <typename T>
    inline T getProperty(const std::string& name, T valIfNotFound) const
    {
        if (auto property = findProperty(name))
            return property->get<T>();
        return valIfNotFound;
    }

    /**
     * @return the property value of the given name
     * @throw std::runtime_error if value property value was not found.
     */
    template <typename T>
    inline T getProperty(const std::string& name) const
    {
        if (auto property = findProperty(name))
            return property->get<T>();
        throw std::runtime_error("No property found with name " + name);
    }

    /** @return true if the property with the given name exists. */
    bool hasProperty(const std::string& name) const
    {
        return findProperty(name) != nullptr;
    }

    /**
     * @return the enum values for the given property, empty if no enum
     *         property.
     */
    const auto& getEnums(const std::string& name) const
    {
        if (auto property = findProperty(name))
            return property->enums;
        throw std::runtime_error("No property found with name " + name);
    }

    /** @return the type of the given property name. */
    Property::Type getPropertyType(const std::string& name) const
    {
        if (auto property = findProperty(name))
            return property->type;
        throw std::runtime_error("No property found with name " + name);
    }

    /** @return all the registered properties. */
    const auto& getProperties() const { return _properties; }
private:
    Property* findProperty(const std::string& name) const
    {
        auto foundProperty =
            std::find_if(_properties.begin(), _properties.end(),
                         [&](const auto& p) { return p->name == name; });

        return foundProperty != _properties.end() ? foundProperty->get()
                                                  : nullptr;
    }

    std::vector<std::shared_ptr<Property>> _properties;
};

template <>
inline PropertyMap::Property::Type PropertyMap::Property::_getType<float>()
{
    return PropertyMap::Property::Type::Float;
}
template <>
inline PropertyMap::Property::Type PropertyMap::Property::_getType<int32_t>()
{
    return PropertyMap::Property::Type::Int;
}
template <>
inline PropertyMap::Property::Type
    PropertyMap::Property::_getType<std::string>()
{
    return PropertyMap::Property::Type::String;
}
template <>
inline PropertyMap::Property::Type
    PropertyMap::Property::_getType<const char*>()
{
    return PropertyMap::Property::Type::String;
}
template <>
inline PropertyMap::Property::Type PropertyMap::Property::_getType<bool>()
{
    return PropertyMap::Property::Type::Bool;
}
template <>
inline PropertyMap::Property::Type
    PropertyMap::Property::_getType<std::array<float, 2>>()
{
    return PropertyMap::Property::Type::Vec2f;
}
template <>
inline PropertyMap::Property::Type
    PropertyMap::Property::_getType<std::array<int32_t, 2>>()
{
    return PropertyMap::Property::Type::Vec2i;
}
template <>
inline PropertyMap::Property::Type
    PropertyMap::Property::_getType<std::array<float, 3>>()
{
    return PropertyMap::Property::Type::Vec3f;
}
template <>
inline PropertyMap::Property::Type
    PropertyMap::Property::_getType<std::array<int32_t, 3>>()
{
    return PropertyMap::Property::Type::Vec3i;
}
template <>
inline PropertyMap::Property::Type
    PropertyMap::Property::_getType<std::array<float, 4>>()
{
    return PropertyMap::Property::Type::Vec4f;
}
}
