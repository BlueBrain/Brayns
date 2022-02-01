/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <algorithm>

#include "ConversionRegistry.h"

namespace brayns
{
Property::Property(std::string name, EnumProperty value, MetaData metaData)
    : _name(std::move(name))
    , _label(std::move(metaData.label))
    , _description(std::move(metaData.description))
    , _value(std::move(value))
{
}

const std::string &Property::getName() const
{
    return _name;
}

const std::string &Property::getLabel() const
{
    return _label;
}

const std::string &Property::getDescription() const
{
    return _description;
}

std::type_index Property::getType() const
{
    return _value.getType();
}

const Any &Property::getValue() const
{
    return _value;
}

void Property::setValue(Any value)
{
    if (_value.getType() != value.getType())
    {
        throw std::runtime_error("Invalid type");
    }
    _value = std::move(value);
    notifyModification();
}

void Property::copyValue(const Any &value)
{
    if (!ConversionRegistry::convert(value, _value))
    {
        throw std::runtime_error("Invalid conversion");
    }
    notifyModification();
}

bool Property::isReadOnly() const
{
    return _readOnly;
}

void Property::setReadOnly(bool readOnly)
{
    _readOnly = readOnly;
}

void Property::onModified(Callback callback)
{
    _callback = std::move(callback);
}

void Property::notifyModification()
{
    if (_callback)
    {
        _callback(*this);
    }
}

bool Property::to(Any &value) const
{
    return ConversionRegistry::convert(_value, value);
}

PropertyMap::PropertyMap(std::string name)
    : _name(std::move(name))
{
}

const std::string &PropertyMap::getName() const
{
    return _name;
}

std::vector<Property>::const_iterator PropertyMap::begin() const
{
    return _properties.begin();
}

std::vector<Property>::iterator PropertyMap::begin()
{
    return _properties.begin();
}

std::vector<Property>::const_iterator PropertyMap::end() const
{
    return _properties.end();
}

std::vector<Property>::iterator PropertyMap::end()
{
    return _properties.end();
}

size_t PropertyMap::size() const
{
    return _properties.size();
}

bool PropertyMap::empty() const
{
    return _properties.empty();
}

bool PropertyMap::isValidIndex(int index) const
{
    return index >= 0 && size_t(index) < _properties.size();
}

int PropertyMap::getIndex(const std::string &name) const
{
    auto i = std::find_if(
        _properties.begin(),
        _properties.end(),
        [&](auto &property) { return property.getName() == name; });
    return i == _properties.end() ? -1 : int(i - _properties.begin());
}

const Property *PropertyMap::find(int index) const
{
    return isValidIndex(index) ? &_properties[index] : nullptr;
}

Property *PropertyMap::find(int index)
{
    return isValidIndex(index) ? &_properties[index] : nullptr;
}

const Property *PropertyMap::find(const std::string &name) const
{
    return find(getIndex(name));
}

Property *PropertyMap::find(const std::string &name)
{
    return find(getIndex(name));
}

Property &PropertyMap::add(const Property &property)
{
    auto index = getIndex(property.getName());
    if (index < 0)
    {
        _properties.push_back(property);
        return _properties.back();
    }
    _properties[index].setValue(property.getValue());
    return _properties[index];
}

void PropertyMap::update(const std::string &name, const Any &value)
{
    auto property = find(name);
    if (!property)
    {
        return;
    }
    property->copyValue(value);
}

void PropertyMap::merge(const PropertyMap &other)
{
    for (const auto &property : other._properties)
    {
        auto index = getIndex(property.getName());
        if (index < 0)
        {
            _properties.push_back(property);
            continue;
        }
        _properties[index].copyValue(property.getValue());
    }
}

void PropertyMap::update(const PropertyMap &other)
{
    for (const auto &property : other._properties)
    {
        auto index = getIndex(property.getName());
        if (index < 0)
        {
            continue;
        }
        _properties[index].copyValue(property.getValue());
    }
}

const Property &PropertyMap::operator[](size_t index) const
{
    if (!isValidIndex(index))
    {
        throw std::runtime_error("Missing property");
    }
    return _properties[index];
}

const Property &PropertyMap::operator[](const std::string &name) const
{
    return (*this)[getIndex(name)];
}
} // namespace brayns