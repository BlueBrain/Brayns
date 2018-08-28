/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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
#include "ExtendedCones.h"
#include "ospray/SDK/common/Data.h"
#include "ospray/SDK/common/Model.h"
// ispc-generated files
#include "ExtendedCones_ispc.h"

#include <climits>

namespace ospray
{
ExtendedCones::ExtendedCones()
{
    this->ispcEquivalent = ispc::ExtendedCones_create(this);
}

void ExtendedCones::finalize(ospray::Model* model)
{
    data = getParamData("extendedcones", nullptr);
    constexpr size_t bytesPerCone = sizeof(brayns::Cone);

    if (data.ptr == nullptr || bytesPerCone == 0)
        throw std::runtime_error(
            "#ospray:geometry/extendedcones: "
            "no 'extendedcones' data specified");
    const size_t numExtendedCones = data->numBytes / bytesPerCone;
    ispc::ExtendedConesGeometry_set(getIE(), model->getIE(), data->data,
                                    numExtendedCones);
}

OSP_REGISTER_GEOMETRY(ExtendedCones, extendedcones);

} // ::brayns
