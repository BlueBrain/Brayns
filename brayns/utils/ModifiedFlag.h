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

#include <utility>

namespace brayns
{
class ModifiedFlag
{
public:
    ModifiedFlag() = default;

    ModifiedFlag(ModifiedFlag &&other) noexcept = default;
    ModifiedFlag &operator=(ModifiedFlag &&other) noexcept = default;

    ModifiedFlag(const ModifiedFlag &other)
    {
        (void)other;
    }

    ModifiedFlag &operator=(const ModifiedFlag &other)
    {
        (void)other;
        _modified = true;
        return *this;
    }

    explicit operator bool() const noexcept
    {
        return _modified;
    }

    ModifiedFlag &operator=(bool modified) noexcept
    {
        _modified = modified;
        return *this;
    }

    void setModified(bool modified) noexcept
    {
        _modified = modified;
    }

    template<typename T, typename U>
    bool update(T &value, U &&newValue)
    {
        if (newValue != value)
        {
            value = std::forward<U>(newValue);
            _modified = true;
            return true;
        }

        return false;
    }

private:
    bool _modified = true;
};
}
