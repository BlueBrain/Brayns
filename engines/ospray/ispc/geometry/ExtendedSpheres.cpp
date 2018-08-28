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
#include <brayns/common/geometry/Sphere.h>

// ospray
#include "ExtendedSpheres.h"
#include "ospray/SDK/common/Data.h"
#include "ospray/SDK/common/Model.h"
// ispc-generated files
#include "ExtendedSpheres_ispc.h"

#include <climits>

namespace ospray
{
ExtendedSpheres::ExtendedSpheres()
{
    this->ispcEquivalent = ispc::ExtendedSpheres_create(this);
}

void ExtendedSpheres::finalize(ospray::Model* model)
{
    data = getParamData("extendedspheres", nullptr);
    constexpr size_t bytesPerExtendedSphere = sizeof(brayns::Sphere);

    if (data.ptr == nullptr)
        throw std::runtime_error(
            "#ospray:geometry/extendedspheres: "
            "no 'extendedspheres' data specified");

    const size_t numExtendedSpheres = data->numBytes / bytesPerExtendedSphere;
    ispc::ExtendedSpheresGeometry_set(getIE(), model->getIE(), data->data,
                                      numExtendedSpheres);
}

OSP_REGISTER_GEOMETRY(ExtendedSpheres, extendedspheres);

} // ::brayns
