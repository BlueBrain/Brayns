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

#pragma once

#include <string_view>

#include "ByteParser.h"
#include "Endian.h"

namespace brayns
{
class ByteStream
{
public:
    ByteStream(std::string_view data);

    bool canExtract(size_t size) const;
    std::string_view extract(size_t size);

    template<typename T>
    bool canExtract() const
    {
        return canExtract(sizeof(T));
    }

    template<typename T>
    T extractBigEndian()
    {
        auto value = extract<T>();
        return Endian::convertBigEndianToLocalEndian(value);
    }

    template<typename T>
    T extractLittleEndian()
    {
        auto value = extract<T>();
        return Endian::convertLittleEndianToLocalEndian(value);
    }

    template<typename T>
    T extract()
    {
        auto data = extract(sizeof(T));
        return ByteParser<T>::parse(data);
    }

private:
    std::string_view _data;
};
} // namespace brayns
