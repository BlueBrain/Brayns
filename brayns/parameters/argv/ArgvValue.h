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

#pragma once

#include <string>

#include "ArgvType.h"

namespace brayns
{
class ArgvValue
{
public:
    ArgvValue() = default;
    explicit ArgvValue(bool boolean);
    explicit ArgvValue(int64_t integer);
    explicit ArgvValue(double number);
    explicit ArgvValue(std::string string);

    ArgvType getType() const;
    bool toBoolean() const;
    int64_t toInteger() const;
    double toNumber() const;
    const std::string &toString() const;

private:
    ArgvType _type = ArgvType::String;
    bool _boolean = false;
    int64_t _integer = 0;
    double _number = 0.0;
    std::string _string;
};
} // namespace brayns
