/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "KindTable.h"

#include <io/nrrdloader/kinds/ColorKind.h>
#include <io/nrrdloader/kinds/RotationKind.h>
#include <io/nrrdloader/kinds/ScalarKind.h>
#include <io/nrrdloader/kinds/VectorKind.h>

std::unique_ptr<INRRDKind> KindTable::getKind(const NRRDHeader &header)
{
    const auto dimensions = header.dimensions;
    const auto &kinds = header.kinds;
    const auto &sizes = header.sizes;

    if (!kinds)
    {
        return std::make_unique<ScalarKind>();
    }

    auto rangeKind = (*kinds)[0];

    switch (rangeKind)
    {
    case NRRDKind::NONE:
    case NRRDKind::DOMAIN:
    case NRRDKind::SCALAR:
    case NRRDKind::SPACE:
        return std::make_unique<ScalarKind>();
    case NRRDKind::GRADIENT3:
    case NRRDKind::COLOR4:
    case NRRDKind::HSVCOLOR:
    case NRRDKind::RGBACOLOR:
    case NRRDKind::RGBCOLOR:
    case NRRDKind::XYZCOLOR:
        return std::make_unique<ColorKind>();
    case NRRDKind::QUATERNION:
        return std::make_unique<RotationKind>();
    case NRRDKind::VECTOR2D:
    case NRRDKind::VECTOR3D:
    case NRRDKind::VECTOR:
    case NRRDKind::NORMAL3D:
        return std::make_unique<VectorKind>();
    }
}
