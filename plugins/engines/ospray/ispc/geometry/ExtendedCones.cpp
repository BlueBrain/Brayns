/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

// ospray
#include "ExtendedCones.h"
#include "ospray/SDK/common/Data.h"
#include "ospray/SDK/common/Model.h"
// ispc-generated files
#include "ExtendedCones_ispc.h"

namespace ospray
{

ExtendedCones::ExtendedCones()
{
    this->ispcEquivalent = ispc::ExtendedCones_create(this);
}

void ExtendedCones::finalize( ospray::Model *model )
{
    radius              = getParam1f("radius",0.01f);
    length              = getParam1f("length",0.01f);
    materialID          = getParam1i("materialID",0);
    bytesPerCone        = getParam1i("bytes_per_extended_cone",10*sizeof(float));
    offset_center       = getParam1i("offset_center",0);
    offset_up           = getParam1i("offset_up",3*sizeof(float));
    offset_centerRadius = getParam1i("offset_centerRadius",6*sizeof(float));
    offset_upRadius     = getParam1i("offset_upRadius",7*sizeof(float));
    offset_timestamp    = getParam1i("offset_timestamp",8*sizeof(float));
    offset_value        = getParam1i("offset_value",9*sizeof(float));
    offset_materialID   = getParam1i("offset_materialID",-1);
    data                = getParamData("extendedcones",nullptr);

    if (data.ptr == nullptr || bytesPerCone == 0)
        throw std::runtime_error( "#ospray:geometry/extendedcones: " \
                                  "no 'extendedcones' data specified");
    numExtendedCones = data->numBytes / bytesPerCone;
    ispc::ExtendedConesGeometry_set(
                getIE(),
                model->getIE(),
                data->data,
                numExtendedCones,
                bytesPerCone,
                radius,
                length,
                materialID,
                offset_center,
                offset_up,
                offset_centerRadius,
                offset_upRadius,
                offset_timestamp,
                offset_value,
                offset_materialID);
}

OSP_REGISTER_GEOMETRY( ExtendedCones, extendedcones );

} // ::brayns
