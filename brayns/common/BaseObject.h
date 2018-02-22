/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

namespace brayns
{
class BaseObject
{
public:
    BaseObject() = default;
    virtual ~BaseObject() = default;

    /**
     * @return true if any parameter has been modified since the last
     *         resetModified().
     */
    bool isModified() const { return _modified; }
    /**
     * Reset the modified state, typically done after changes have been applied.
     */
    void resetModified() { _modified = false; }
    void markModified() { _modified = true; }
protected:
    /**
     * Helper function for derived classes to update a parameter and mark it
     * modified if it has changed.
     */
    template <typename T>
    void _updateValue(T& member, const T& newValue)
    {
        if (member != newValue)
        {
            member = newValue;
            _modified = true;
        }
    }

private:
    std::atomic_bool _modified{true};
};
}
