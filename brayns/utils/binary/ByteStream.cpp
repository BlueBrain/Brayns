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

#include "ByteStream.h"

#include <cassert>

namespace brayns
{
ByteStream::ByteStream(std::string_view data)
    : _data(data)
{
}

bool ByteStream::canExtract(size_t size) const
{
    return _data.size() >= size;
}

std::string_view ByteStream::extract(size_t size)
{
    assert(canExtract(size));
    auto extracted = _data.substr(0, size);
    _data = _data.substr(size);
    return extracted;
}
} // namespace brayns
