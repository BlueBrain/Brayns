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

#include <limits>
#include <stdexcept>
#include <vector>

namespace brayns
{
template<typename T>
class IdGenerator
{
public:
    T next()
    {
        if (!_recycled.empty())
        {
            auto id = _recycled.back();

            _recycled.pop_back();

            return id;
        }

        if (_counter == std::numeric_limits<T>::max())
        {
            throw std::out_of_range("No more available IDs");
        }

        return _counter++;
    }

    void recycle(T id)
    {
        _recycled.push_back(id);
    }

private:
    T _counter = 0;
    std::vector<T> _recycled;
};
}
