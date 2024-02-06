/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "ArgvValue.h"

#include <cassert>

namespace brayns
{
ArgvValue::ArgvValue(bool boolean):
    _type(ArgvType::Boolean),
    _boolean(boolean)
{
}

ArgvValue::ArgvValue(int64_t integer):
    _type(ArgvType::Integer),
    _integer(integer)
{
}

ArgvValue::ArgvValue(double number):
    _type(ArgvType::Number),
    _number(number)
{
}

ArgvValue::ArgvValue(std::string string):
    _type(ArgvType::String),
    _string(std::move(string))
{
}

ArgvType ArgvValue::getType() const
{
    return _type;
}

bool ArgvValue::toBoolean() const
{
    assert(_type == ArgvType::Boolean);
    return _boolean;
}

int64_t ArgvValue::toInteger() const
{
    assert(_type == ArgvType::Integer);
    return _integer;
}

double ArgvValue::toNumber() const
{
    assert(_type == ArgvType::Number);
    return _number;
}

const std::string &ArgvValue::toString() const
{
    assert(_type == ArgvType::String);
    return _string;
}
} // namespace brayns
