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

#include "types.h"
#include <boost/any.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace brayns
{
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
        Double,
        String,
        Bool,
        Vec2i,
        Vec2d,
        Vec3i,
        Vec3d,
        Vec4d
    };

    struct MetaData
    {
        MetaData(std::string label_ = {},
                 const std::string& description_ = "no-description")
            : label(label_)
            , description(description_)
        {
        }
        const std::string label;
        const std::string description;
    };

    template <typename T>
    void assertValidType() const
    {
        static_assert(std::is_same<T, double>::value ||
                          std::is_same<T, int32_t>::value ||
                          std::is_same<T, std::string>::value ||
                          std::is_same<T, bool>::value ||
                          std::is_same<T, std::array<double, 2>>::value ||
                          std::is_same<T, std::array<int32_t, 2>>::value ||
                          std::is_same<T, std::array<double, 3>>::value ||
                          std::is_same<T, std::array<int32_t, 3>>::value ||
                          std::is_same<T, std::array<double, 4>>::value,
                      "Invalid property type.");
    }

    template <typename T>
    void assertValidEnumType() const
    {
        static_assert(std::is_same<T, int32_t>::value ||
                          std::is_same<T, std::string>::value,
                      "Invalid enum type.");
    }

    template <typename T>
    Type getType() const
    {
        assertValidType<T>();

        if (std::is_same<T, double>::value)
            return Property::Type::Double;
        if (std::is_same<T, int32_t>::value)
            return Property::Type::Int;
        if (std::is_same<T, std::string>::value)
            return Property::Type::String;
        if (std::is_same<T, bool>::value)
            return Property::Type::Bool;
        if (std::is_same<T, std::array<double, 2>>::value)
            return Property::Type::Vec2d;
        if (std::is_same<T, std::array<int32_t, 2>>::value)
            return Property::Type::Vec2i;
        if (std::is_same<T, std::array<double, 3>>::value)
            return Property::Type::Vec3d;
        if (std::is_same<T, std::array<int32_t, 3>>::value)
            return Property::Type::Vec3i;
        if (std::is_same<T, std::array<double, 4>>::value)
            return Property::Type::Vec4d;
        throw std::runtime_error("Could not match type for '" + name + "'");
    }

    template <typename T>
    Property(const std::string& name_, const T value,
             const MetaData& metaData_ = {})
        : name(name_)
        , metaData(metaData_)
        , type(getType<T>())
        , _data(std::move(value))
        , _min(T())
        , _max(T())
    {
        assertValidType<T>();
    }

    template <typename T>
    Property(const std::string& name_, const T value,
             const std::pair<T, T>& limit, const MetaData& metaData_ = {})
        : name(name_)
        , metaData(metaData_)
        , type(getType<T>())
        , _data(std::move(value))
        , _min(limit.first)
        , _max(limit.second)
    {
        assertValidType<T>();
    }

    /**
     * Specialized for enum properties, the possible enum values are passed
     * in enums_.
     */
    template <typename T>
    Property(const std::string& name_, const T value,
             const std::vector<std::string>& enums_, const MetaData& metaData_)
        : name(name_)
        , metaData(metaData_)
        , type(getType<T>())
        , enums(enums_)
        , _data(std::move(value))
        , _min(0)
        , _max(enums_.size())
    {
        assertValidEnumType<T>();
    }

    using ModifiedCallback = std::function<void(const Property&)>;

    /**
     * Set a function that is called after this property has been changed.
     */
    void onModified(const ModifiedCallback& callback)
    {
        _modifiedCallback = callback;
    }

    template <typename T>
    void set(const T& v)
    {
        assertValidType<T>();
        _checkType<T>();
        _data = v;
        if (_modifiedCallback)
            _modifiedCallback(*this);
    }

    template <typename T>
    T get() const
    {
        assertValidType<T>();
        return _castValue<T>(_data);
    }

    template <typename T>
    T min() const
    {
        assertValidType<T>();
        return _castValue<T>(_min);
    }

    template <typename T>
    T max() const
    {
        assertValidType<T>();
        return _castValue<T>(_max);
    }

    /**
     * Read-only property shall not be modified from the outside aka web API
     * via JSON.
     */
    void markReadOnly() { _readOnly = true; }
    bool readOnly() const { return _readOnly; }
    const std::string name;
    const MetaData metaData; //!< user-friendly name and description
    const Type type;

    /**
     * Name of enum values that are mapped to the integer value
     */
    const std::vector<std::string> enums;

private:
    friend class PropertyMap;
    boost::any _data;
    const boost::any _min;
    const boost::any _max;
    bool _readOnly{false};
    ModifiedCallback _modifiedCallback;

    template <typename T>
    void _checkType() const
    {
        const auto requestedType = getType<T>();
        if (requestedType != type)
            throw std::runtime_error("Type mismatch for property '" + name +
                                     "'");
    }
    template <typename T>
    T _castValue(const boost::any& v) const
    {
        _checkType<T>();
        return boost::any_cast<T>(v);
    }
};

/**
 * Container class for holding properties that are mapped by name to a supported
 * C++ type and their respective value.
 */
class PropertyMap
{
public:
    PropertyMap() = default;
    PropertyMap(const std::string& name)
        : _name(name)
    {
    }

    /**
     * @return the name of this property map e.g. to name commandline option
     *         group
     */
    const auto& getName() const { return _name; }
    /** Update the property of the given name */
    template <typename T>
    inline void updateProperty(const std::string& name, const T& t)
    {
        if (auto property = find(name))
        {
            if (property->type != property->getType<T>())
                throw std::runtime_error(
                    "updateProperty does not allow for changing the type");
            property->set(t);
        }
    }

    /** Update or add the given property. */
    void setProperty(const Property& newProperty)
    {
        if (auto property = find(newProperty.name))
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
        if (auto property = find(name))
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
        if (auto property = find(name))
            return property->get<T>();
        throw std::runtime_error("No property found with name " + name);
    }

    /** @return true if the property with the given name exists. */
    bool hasProperty(const std::string& name) const
    {
        return find(name) != nullptr;
    }

    /**
     * @return the enum values for the given property, empty if no enum
     *         property.
     */
    const auto& getEnums(const std::string& name) const
    {
        if (auto property = find(name))
            return property->enums;
        throw std::runtime_error("No property found with name " + name);
    }

    /** @return the type of the given property name. */
    Property::Type getPropertyType(const std::string& name) const
    {
        if (auto property = find(name))
            return property->type;
        throw std::runtime_error("No property found with name " + name);
    }

    /** @return the metadata for the given property. */
    const auto& getMetaData(const std::string& name) const
    {
        if (auto property = find(name))
            return property->metaData;
        throw std::runtime_error("No property found with name " + name);
    }

    /** @return all the registered properties. */
    const auto& getProperties() const { return _properties; }
    /** Merge this property map with properties from another. */
    void merge(const PropertyMap& input);

    /**
     * Parse and fill values from the commandline.
     *
     * Options are added by converting the camelCase property name to
     * hyphenated-case.
     *
     * Integer enums are handled by matching the string to int value from the
     * index in the enums vector of the property.
     *
     * @param argc number of commandline arguments
     * @param argv list of strings of commandline arguments
     * @return true if parsing went without errors for no '--help', false
     *              otherwise and error(s) or 'help' will be printed.
     */
    bool parse(int argc, const char** argv);

private:
    Property* find(const std::string& name) const
    {
        auto foundProperty =
            std::find_if(_properties.begin(), _properties.end(),
                         [&](const auto& p) { return p->name == name; });

        return foundProperty != _properties.end() ? foundProperty->get()
                                                  : nullptr;
    }

    std::string _name;
    std::vector<std::shared_ptr<Property>> _properties;
};
}
