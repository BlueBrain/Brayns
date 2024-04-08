/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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
class IDFactory
{
public:
    IDFactory()
    {
        // Narrows it down to unsigned integer types
        static_assert(std::is_unsigned_v<T>, "ID factory should be instantiated with unsigned integer types");
    }

    /**
     * @brief Returns an ID
     * @throws std::runtime_error if the IDs have been exhausted
     */
    T generateID()
    {
        if (!_releasedIDs.empty())
        {
            auto id = _releasedIDs.back();
            _releasedIDs.pop_back();
            return id;
        }

        if (_factory == std::numeric_limits<T>::max())
        {
            throw std::runtime_error("ID factory exhausted");
        }

        return _factory++;
    }

    /**
     * @brief Recycles an ID that is not used anymore
     */
    void releaseID(T id) noexcept
    {
        _releasedIDs.push_back(id);
    }

    /**
     * @brief Reset ID counter and erase ID buffer.
     *
     */
    void clear()
    {
        _factory = {};
        _releasedIDs.clear();
    }

private:
    T _factory{};
    std::vector<T> _releasedIDs;
};
}
