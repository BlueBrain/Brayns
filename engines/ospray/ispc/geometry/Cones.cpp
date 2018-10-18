/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Author: Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
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

// Brayns
#include <brayns/common/geometry/Cone.h>

// ospray
#include "Cones.h"
#include "ospray/SDK/common/Data.h"
#include "ospray/SDK/common/Model.h"
// ispc-generated files
#include "Cones_ispc.h"

#include <climits>

namespace ospray
{
Cones::Cones()
{
    this->ispcEquivalent = ispc::Cones_create(this);
}

void Cones::finalize(ospray::Model* model)
{
    data = getParamData("cones", nullptr);
    constexpr size_t bytesPerCone = sizeof(brayns::Cone);

    if (data.ptr == nullptr || bytesPerCone == 0)
        throw std::runtime_error(
            "#ospray:geometry/cones: no 'cones' data specified");

    const size_t numCones = data->numBytes / bytesPerCone;

    bounds = empty;
    const auto geoms = static_cast<brayns::Cone*>(data->data);
    for (size_t i = 0; i < numCones; i++)
    {
        const brayns::Cone& geom = geoms[i];
        const auto center =
            vec3f(geom.center[0], geom.center[1], geom.center[2]);
        const auto up = vec3f(geom.up[0], geom.up[1], geom.up[2]);

        bounds.extend(center - geom.centerRadius);
        bounds.extend(center + geom.centerRadius);
        bounds.extend(up - geom.upRadius);
        bounds.extend(up + geom.upRadius);
    }

    ispc::ConesGeometry_set(getIE(), model->getIE(), data->data, numCones);
}

OSP_REGISTER_GEOMETRY(Cones, cones);

} // ::brayns
