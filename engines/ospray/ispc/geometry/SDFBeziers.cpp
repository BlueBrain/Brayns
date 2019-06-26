/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Author: Sebastien Speierer <sebastien.speierer@epfl.ch>
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
#include <brayns/common/geometry/SDFBezier.h>

// ospray
#include "SDFBeziers.h"
#include "ospray/SDK/common/Data.h"
#include "ospray/SDK/common/Model.h"
// ispc-generated files
#include "SDFBeziers_ispc.h"

#include <climits>

namespace ospray
{
SDFBeziers::SDFBeziers()
{
    this->ispcEquivalent = ispc::SDFBeziers_create(this);
}

void SDFBeziers::finalize(ospray::Model* model)
{
    data = getParamData("sdfbeziers", nullptr);
    constexpr size_t bytesPerSDFBezier = sizeof(brayns::SDFBezier);

    if (data.ptr == nullptr || bytesPerSDFBezier == 0)
        throw std::runtime_error(
            "#ospray:geometry/sdfbeziers: no 'sdfbeziers' data specified");

    const size_t numSDFBeziers = data->numBytes / bytesPerSDFBezier;

    bounds = empty;
    const auto geoms = static_cast<brayns::SDFBezier*>(data->data);
    for (size_t i = 0; i < numSDFBeziers; i++)
    {
        const auto bd = bezierBounds(geoms[i]);
        const auto& bMind = bd.getMin();
        const auto& bMaxd = bd.getMax();
        const auto bMinf = vec3f(bMind[0], bMind[1], bMind[2]);
        const auto bMaxf = vec3f(bMaxd[0], bMaxd[1], bMaxd[2]);

        bounds.extend(bMinf);
        bounds.extend(bMaxf);
    }

    ispc::SDFBeziersGeometry_set(getIE(), model->getIE(), data->data,
                                 numSDFBeziers);
}

OSP_REGISTER_GEOMETRY(SDFBeziers, sdfbeziers);

} // namespace ospray
