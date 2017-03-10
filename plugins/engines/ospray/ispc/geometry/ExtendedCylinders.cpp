/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

// ospray
#include "ExtendedCylinders.h"
#include "ospray/SDK/common/Data.h"
#include "ospray/SDK/common/Model.h"
// ispc-generated files
#include "ExtendedCylinders_ispc.h"

namespace ospray
{
ExtendedCylinders::ExtendedCylinders()
{
    this->ispcEquivalent = ispc::ExtendedCylinders_create(this);
}

void ExtendedCylinders::finalize(ospray::Model *model)
{
    radius = getParam1f("radius", 0.01f);
    materialID = getParam1i("materialID", 0);
    bytesPerCylinder = getParam1i("bytes_per_cylinder", 9 * sizeof(float));
    offset_v0 = getParam1i("offset_v0", 0);
    offset_v1 = getParam1i("offset_v1", 3 * sizeof(float));
    offset_radius = getParam1i("offset_radius", 6 * sizeof(float));
    offset_timestamp = getParam1i("offset_timestamp", 7 * sizeof(float));
    offset_value = getParam1i("offset_value", 8 * sizeof(float));
    offset_materialID = getParam1i("offset_materialID", -1);
    data = getParamData("extendedcylinders", nullptr);

    if (data.ptr == nullptr || bytesPerCylinder == 0)
        throw std::runtime_error(
            "#ospray:geometry/extendedcylinders: "
            "no 'extendedcylinders' data specified");
    numExtendedCylinders = data->numBytes / bytesPerCylinder;
    ispc::ExtendedCylindersGeometry_set(getIE(), model->getIE(), data->data,
                                        numExtendedCylinders, bytesPerCylinder,
                                        radius, materialID, offset_v0,
                                        offset_v1, offset_radius,
                                        offset_timestamp, offset_value,
                                        offset_materialID);
}

OSP_REGISTER_GEOMETRY(ExtendedCylinders, extendedcylinders);

} // ::brayns
