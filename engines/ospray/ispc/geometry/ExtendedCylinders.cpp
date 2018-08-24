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
    radius = getParam1f("radius", 0.01f);
    materialID = getParam1i("materialID", 0);
    data = getParamData("extendedcylinders", nullptr);

    constexpr size_t bytesPerCylinder = sizeof(brayns::Cylinder);

    if (data.ptr == nullptr || bytesPerCylinder == 0)
        throw std::runtime_error(
            "#ospray:geometry/extendedcylinders: "
            "no 'extendedcylinders' data specified");

    const bool useSafeIndex = data->numBytes >= INT_MAX;
    const size_t numExtendedCylinders = data->numBytes / bytesPerCylinder;
    ispc::ExtendedCylindersGeometry_set(getIE(), model->getIE(), data->data,
                                        numExtendedCylinders, radius,
                                        materialID, useSafeIndex);
}

OSP_REGISTER_GEOMETRY(ExtendedCylinders, extendedcylinders);

} // ::brayns
