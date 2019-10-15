/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Juan Hernando <juan.hernando@epfl.ch>
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

#ifndef ClipPlane_H
#define ClipPlane_H

#include <brayns/api.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(ClipPlane)

namespace brayns
{
class ClipPlane : public BaseObject
{
public:
    ClipPlane(const Plane& plane)
        : _id(_nextID++)
        , _plane(plane)
    {
    }

    size_t getID() const { return _id; }
    const Plane& getPlane() const { return _plane; };
    void setPlane(const Plane& plane) { _updateValue(_plane, plane); }
    /** @internal */
    ClipPlane() = default;
    /** @internal */
    ClipPlane(const size_t id, const Plane& plane)
        : _id(id)
        , _plane(plane)
    {
    }

private:
    static size_t _nextID;
    size_t _id = 0;
    Plane _plane = {{0}};
    SERIALIZATION_FRIEND(ClipPlane);
};
} // namespace brayns
#endif // Model_H
