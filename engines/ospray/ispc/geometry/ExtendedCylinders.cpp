/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

// Brayns
#include <brayns/common/geometry/Cylinder.h>

// ospray
#include "ExtendedCylinders.h"
#include "ospray/SDK/common/Data.h"
#include "ospray/SDK/common/Model.h"
// ispc-generated files
#include "ExtendedCylinders_ispc.h"

#include <climits>

namespace ospray
{
ExtendedCylinders::ExtendedCylinders()
{
    this->ispcEquivalent = ispc::ExtendedCylinders_create(this);
}

void ExtendedCylinders::finalize(ospray::Model* model)
{
    data = getParamData("extendedcylinders", nullptr);
    constexpr size_t bytesPerCylinder = sizeof(brayns::Cylinder);

    if (data.ptr == nullptr || bytesPerCylinder == 0)
        throw std::runtime_error(
            "#ospray:geometry/extendedcylinders: "
            "no 'extendedcylinders' data specified");

    const size_t numExtendedCylinders = data->numBytes / bytesPerCylinder;

    bounds = empty;
    const auto geoms = static_cast<brayns::Cylinder*>(data->data);
    for (size_t i = 0; i < numExtendedCylinders; i++)
    {
        const brayns::Cylinder& geom = geoms[i];
        const auto center =
            vec3f(geom.center[0], geom.center[1], geom.center[2]);
        const auto up = vec3f(geom.up[0], geom.up[1], geom.up[2]);

        bounds.extend(center - geom.radius);
        bounds.extend(center + geom.radius);
        bounds.extend(up - geom.radius);
        bounds.extend(up + geom.radius);
    }

    ispc::ExtendedCylindersGeometry_set(getIE(), model->getIE(), data->data,
                                        numExtendedCylinders);
}

OSP_REGISTER_GEOMETRY(ExtendedCylinders, extendedcylinders);

} // ::brayns
