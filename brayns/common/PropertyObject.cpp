/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "PropertyObject.h"

namespace brayns
{
void PropertyObject::setCurrentType(const std::string &type)
{
    _updateValue(_currentType, type);

    // add default (empty) property map for new type
    if (_properties.count(type) == 0)
        _properties[type];
}

const std::string &PropertyObject::getCurrentType() const
{
    return _currentType;
}

bool PropertyObject::hasProperty(const std::string &name) const
{
    return _properties.at(_currentType).find(name);
}

void PropertyObject::setProperties(const PropertyMap &properties)
{
    setProperties(_currentType, properties);
}

void PropertyObject::setProperties(const std::string &type, const PropertyMap &properties)
{
    _properties[type] = properties;
    markModified();
}

void PropertyObject::updateProperties(const PropertyMap &properties)
{
    _properties.at(_currentType).merge(properties);
    markModified();
}

const PropertyMap &PropertyObject::getPropertyMap() const
{
    return _properties.at(_currentType);
}

const PropertyMap &PropertyObject::getPropertyMap(const std::string &type) const
{
    return _properties.at(type);
}

std::vector<std::string> PropertyObject::getTypes() const
{
    std::vector<std::string> types;
    for (const auto &i : _properties)
        types.push_back(i.first);
    return types;
}

void PropertyObject::clonePropertiesFrom(const PropertyObject &obj)
{
    _currentType = obj._currentType;
    _properties.clear();
    for (const auto &kv : obj._properties)
    {
        const auto &key = kv.first;
        const auto &properties = kv.second;

        PropertyMap propertyMapClone;
        for (const auto &property : properties)
            propertyMapClone.add(property);

        _properties[key] = propertyMapClone;
    }
}

const std::map<std::string, PropertyMap> &PropertyObject::getProperties() const
{
    return _properties;
}
} // namespace brayns
