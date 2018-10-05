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
#include <brayns/common/geometry/SDFGeometry.h>

// ospray
#include "ExtendedSDFGeometries.h"
#include "ospray/SDK/common/Data.h"
#include "ospray/SDK/common/Model.h"
// ispc-generated files
#include "ExtendedSDFGeometries_ispc.h"

#include <climits>
#include <cstddef>

namespace ospray
{
ExtendedSDFGeometries::ExtendedSDFGeometries()
{
    this->ispcEquivalent = ispc::ExtendedSDFGeometries_create(this);
}

void ExtendedSDFGeometries::finalize(ospray::Model* model)
{
    data = getParamData("extendedsdfgeometries", nullptr);
    neighbours = getParamData("neighbours", nullptr);
    geometries = getParamData("geometries", nullptr);

    if (data.ptr == nullptr)
        throw std::runtime_error(
            "#ospray:geometry/ExtendedSDFGeometries: "
            "no 'ExtendedSDFGeometries' data specified");

    const size_t numExtendedSDFGeometries = data->numItems;
    const size_t numNeighbours = neighbours->numItems;

    bounds = empty;
    const auto geoms = static_cast<brayns::SDFGeometry*>(geometries->data);
    for (size_t i = 0; i < numExtendedSDFGeometries; i++)
    {
        const auto bd = getSDFBoundingBox(geoms[i]);
        const auto& bMind = bd.getMin();
        const auto& bMaxd = bd.getMax();
        const auto bMinf = vec3f(bMind[0], bMind[1], bMind[2]);
        const auto bMaxf = vec3f(bMaxd[0], bMaxd[1], bMaxd[2]);

        bounds.extend(bMinf);
        bounds.extend(bMaxf);
    }

    ispc::ExtendedSDFGeometriesGeometry_set(getIE(), model->getIE(), data->data,
                                            numExtendedSDFGeometries,
                                            neighbours->data, numNeighbours,
                                            geometries->data);
}

OSP_REGISTER_GEOMETRY(ExtendedSDFGeometries, extendedsdfgeometries);

} // namespace ospray
