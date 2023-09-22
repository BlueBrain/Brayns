/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "NeuronGeometryInstantiator.h"

NeuronGeometryInstantiator::NeuronGeometryInstantiator(NeuronGeometry geometry):
    _geometry(std::move(geometry))
{
}

NeuronGeometry NeuronGeometryInstantiator::instantiate(
    const brayns::Vector3f &translation,
    const brayns::Quaternion &rotation) const
{
    auto copy = _geometry;
    for (auto &primitive : copy.primitives)
    {
        primitive.p0 = translation + brayns::math::xfmPoint(rotation, primitive.p0);
        primitive.p1 = translation + brayns::math::xfmPoint(rotation, primitive.p1);
    }
    return copy;
}
