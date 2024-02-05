/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/BaseObject.h>
#include <brayns/common/propertymap/PropertyMap.h>

#include <map>

namespace brayns
{
/**
 * Maps generic properties to user-defined types/keys/names and tracks the
 * current type/key/name for querying, setting and updating its properties.
 */
class PropertyObject : public BaseObject
{
public:
    /** Set the current type to use for 'type-less' queries and updates. */
    void setCurrentType(const std::string& type)
    {
        _updateValue(_currentType, type);

        // add default (empty) property map for new type
        if (_properties.count(type) == 0)
            _properties[type];
    }

    /** @return the current set type. */
    const std::string& getCurrentType() const { return _currentType; }
    /** Update the value of the given property for the current type. */
    template <typename T>
    inline void updateProperty(const std::string& name, const T& value,
                               const bool triggerCallback = true)
    {
        auto& propMap = _properties.at(_currentType);
        const auto oldValue = propMap.valueOr(name, value);
        if (!_isEqual(oldValue, value))
        {
            propMap.update(name, value);
            markModified(triggerCallback);
        }
    }

    /**
     * @return true if the property with the given name exists for the current
     *         type.
     */
    bool hasProperty(const std::string& name) const
    {
        return _properties.at(_currentType).find(name);
    }

    /**
     * @return the value of the property with the given name for the current
     *         type.
     */
    template <typename T>
    inline T getProperty(const std::string& name) const
    {
        return _properties.at(_currentType)[name].as<T>();
    }

    /**
     * @return the value of the property with the given name for the current
     *         type. If it does not exist return the given value.
     */
    template <typename T>
    inline T getPropertyOrValue(const std::string& name, T val) const
    {
        return hasProperty(name) ? getProperty<T>(name) : val;
    }

    /** Assign a new set of properties to the current type. */
    void setProperties(const PropertyMap& properties)
    {
        setProperties(_currentType, properties);
    }

    /** Assign a new set of properties to the given type. */
    void setProperties(const std::string& type, const PropertyMap& properties)
    {
        _properties[type] = properties;
        markModified();
    }

    /**
     * Update or add all the properties from the given map to the current type.
     */
    void updateProperties(const PropertyMap& properties)
    {
        _properties.at(_currentType).merge(properties);
        markModified();
    }

    /** @return the entire property map for the current type. */
    const auto& getPropertyMap() const { return _properties.at(_currentType); }
    /** @return the entire property map for the given type. */
    const auto& getPropertyMap(const std::string& type) const
    {
        return _properties.at(type);
    }

    /** @return the list of all registered types. */
    std::vector<std::string> getTypes() const
    {
        std::vector<std::string> types;
        for (const auto& i : _properties)
            types.push_back(i.first);
        return types;
    }

    /** Clear all current properties and clone new properties from object  */
    void clonePropertiesFrom(const PropertyObject& obj)
    {
        _currentType = obj._currentType;
        _properties.clear();
        for (const auto& kv : obj._properties)
        {
            const auto& key = kv.first;
            const auto& properties = kv.second;

            PropertyMap propertyMapClone;
            for (const auto& property : properties)
                propertyMapClone.add(property);

            _properties[key] = propertyMapClone;
        }
    }

    const auto& getProperties() const { return _properties; }

protected:
    std::string _currentType;
    std::map<std::string, PropertyMap> _properties;
};
} // namespace brayns
