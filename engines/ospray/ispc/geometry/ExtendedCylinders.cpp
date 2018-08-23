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
    bytesPerCylinder =
        getParam1i("bytes_per_cylinder", sizeof(brayns::Cylinder));
    offset_center =
        getParam1i("offset_center", offsetof(struct brayns::Cylinder, center));
    offset_up = getParam1i("offset_up", offsetof(struct brayns::Cylinder, up));
    offset_radius =
        getParam1i("offset_radius", offsetof(struct brayns::Cylinder, radius));
    offset_timestamp = getParam1i("offset_timestamp",
                                  offsetof(struct brayns::Cylinder, timestamp));
    offset_texture_coords =
        getParam1i("offset_texture_coords",
                   offsetof(struct brayns::Cylinder, texture_coords));
    offset_materialID = getParam1i("offset_materialID", -1);
    data = getParamData("extendedcylinders", nullptr);

    if (data.ptr == nullptr || bytesPerCylinder == 0)
        throw std::runtime_error(
            "#ospray:geometry/extendedcylinders: "
            "no 'extendedcylinders' data specified");
    numExtendedCylinders = data->numBytes / bytesPerCylinder;
    ispc::ExtendedCylindersGeometry_set(getIE(), model->getIE(), data->data,
                                        numExtendedCylinders, bytesPerCylinder,
                                        radius, materialID, offset_center,
                                        offset_up, offset_radius,
                                        offset_timestamp, offset_texture_coords,
                                        offset_materialID);
}

OSP_REGISTER_GEOMETRY(ExtendedCylinders, extendedcylinders);

} // ::brayns
