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

#pragma once

#include <atomic>
#include <cmath>
#include <functional>
#include <type_traits>

namespace brayns
{
class BaseObject
{
public:
    BaseObject() = default;

    virtual ~BaseObject() = default;

    /**
     * @return true if or markModified() was called or any parameter has been modified since the last resetModified().
     */
    bool isModified() const;

    /**
     * @brief Reset the modified state, typically done after changes have been applied.
     */
    void resetModified();

    /**
     * @brief Mark the object as modified.
     */
    void markModified();

protected:
    /**
     * @brief Helper function for derived classes to update a parameter and mark it modified if it has changed.
     */
    template<typename T>
    void _updateValue(T &member, const T &newValue)
    {
        if (!_isEqual(member, newValue))
        {
            member = newValue;
        }
    }

    template<class T>
    bool _isEqual(const T &a, const T &b, typename std::enable_if<std::is_floating_point<T>::value>::type * = 0)
    {
        return std::fabs(a - b) < 0.000001;
    }

    template<class T>
    bool _isEqual(const T &a, const T &b, typename std::enable_if<!std::is_floating_point<T>::value>::type * = 0)
    {
        return a == b;
    }

private:
    bool _modified{true};
};
} // namespace brayns
