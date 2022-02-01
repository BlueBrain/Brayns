/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#pragma once

#include <brayns/common/BaseObject.h>
#include <brayns/common/MathTypes.h>

#include <memory>

namespace brayns
{
class ClipPlane : public BaseObject
{
public:
    ClipPlane(const Plane &plane);

    size_t getID() const;
    void setID(size_t id);
    const Plane &getPlane() const;
    void setPlane(const Plane &plane);

    /** @internal */
    ClipPlane() = default;

    /** @internal */
    ClipPlane(const size_t id, const Plane &plane);

private:
    static size_t _nextID;
    size_t _id = 0;
    Plane _plane{0.0};
};

using ClipPlanePtr = std::shared_ptr<ClipPlane>;
} // namespace brayns
