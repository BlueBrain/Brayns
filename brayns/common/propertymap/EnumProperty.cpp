/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include "EnumProperty.h"

#include <algorithm>
#include <cassert>

namespace brayns
{
EnumProperty::EnumProperty(const std::string &value, std::vector<std::string> values)
    : _values(std::move(values))
{
    _assignOrThrow(value);
}

EnumProperty::EnumProperty(int index, std::vector<std::string> values)
    : _values(std::move(values))
{
    _assignOrThrow(index);
}

EnumProperty::EnumProperty(std::vector<std::string> values)
    : _values(std::move(values))
{
    _index = _values.empty() ? -1 : 0;
}

const std::string &EnumProperty::toString() const
{
    return isValidIndex(_index) ? _values[_index] : _getEmptyString();
}

int EnumProperty::toInt() const
{
    return _index;
}

const std::vector<std::string> &EnumProperty::getValues() const
{
    return _values;
}

bool EnumProperty::isValidIndex(int index) const
{
    return index >= 0 && size_t(index) < _values.size();
}

int EnumProperty::getIndex(const std::string &value) const
{
    auto i = std::find(_values.begin(), _values.end(), value);
    return i == _values.end() ? -1 : int(i - _values.begin());
}

const std::string *EnumProperty::find(const std::string &value) const
{
    auto i = std::find(_values.begin(), _values.end(), value);
    return i == _values.end() ? nullptr : &*i;
}

const std::string *EnumProperty::find(int index) const
{
    return isValidIndex(index) ? &_values[index] : nullptr;
}

EnumProperty &EnumProperty::operator=(const std::string &value)
{
    _assignOrThrow(value);
    return *this;
}

EnumProperty &EnumProperty::operator=(int index)
{
    _assignOrThrow(index);
    return *this;
}

const std::string &EnumProperty::_getEmptyString() const
{
    static const std::string emptyString;
    return emptyString;
}

void EnumProperty::_assignOrThrow(const std::string &value)
{
    auto index = getIndex(value);
    if (isValidIndex(index))
    {
        _index = index;
        return;
    }
    throw std::runtime_error("Could not match enum '" + value + "'");
}

void EnumProperty::_assignOrThrow(int index)
{
    if (isValidIndex(index))
    {
        _index = index;
        return;
    }
    throw std::runtime_error("Could not match enum index'" + std::to_string(index) + "'");
}

void Converter<EnumProperty, std::string>::convert(const EnumProperty &from, std::string &to)
{
    to = from.toString();
}

void Converter<std::string, EnumProperty>::convert(const std::string &from, EnumProperty &to)
{
    to = from;
}

void Converter<EnumProperty, int32_t>::convert(const EnumProperty &from, int32_t &to)
{
    to = from.toInt();
}

void Converter<int32_t, EnumProperty>::convert(int32_t from, EnumProperty &to)
{
    to = int(from);
}

void Converter<EnumProperty, int64_t>::convert(const EnumProperty &from, int64_t &to)
{
    to = from.toInt();
}

void Converter<int64_t, EnumProperty>::convert(int64_t from, EnumProperty &to)
{
    to = int(from);
}
} // namespace brayns