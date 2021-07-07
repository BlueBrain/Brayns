/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

namespace brayns
{
class StreamController
{
public:
    bool isControlled() const
    {
        return _controlled;
    }

    bool setControlled(bool controlled)
    {
        _controlled = controlled;
        _triggered = false;
    }

    void trigger()
    {
        if (!_controlled)
        {
            return;
        }
        _triggered = true;
    }

    bool isTriggered()
    {
        if (_triggered)
        {
            _triggered = false;
            return true;
        }
        return false;
    }

private:
    bool _controlled = false;
    bool _triggered = false;
};
} // namespace brayns