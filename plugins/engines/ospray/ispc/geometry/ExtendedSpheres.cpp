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

#include <vector>

// ospray
#include "ExtendedSpheres.h"
#include "ospray/SDK/common/Data.h"
#include "ospray/SDK/common/Model.h"
// ispc-generated files
#include "ExtendedSpheres_ispc.h"

namespace ospray
{

ExtendedSpheres::ExtendedSpheres()
{
    this->ispcEquivalent = ispc::ExtendedSpheres_create(this);
}

void ExtendedSpheres::finalize(ospray::Model *model)
{
    radius            = getParam1f("radius",0.01f);
    materialID        = getParam1i("materialID",0);
    bytesPerExtendedSphere =
            getParam1i("bytes_per_extended_sphere",5*sizeof(float));
    offset_center     = getParam1i("offset_center",0);
    offset_radius     = getParam1i("offset_radius",-1);
    offset_timestamp  = getParam1i("offset_timestamp",-1);
    offset_value      = getParam1i("offset_value",-1);
    offset_materialID = getParam1i("offset_materialID",-1);
    data              = getParamData("extendedspheres",nullptr);
    materialList      = getParamData("materialList",nullptr);

    if (data.ptr == nullptr)
        throw std::runtime_error("#ospray:geometry/extendedspheres: " \
                                 "no 'extendedspheres' data specified");
    numExtendedSpheres = data->numBytes / bytesPerExtendedSphere;

    if (numExtendedSpheres >= (1ULL << 30))
    {
        throw std::runtime_error("#brayns::ExtendedSpheres: too many extended "\
                                 "spheres in this sphere geometry. Consider "\
                                 "splitting this geometry in multiple "\
                                 "geometries with fewer extended spheres (you "\
                                 "can still put all those geometries into a "\
                                 "single model, but you can't put that many "\
                                 "extended spheres into a single geometry "\
                                 "without causing address overflows)");
    }

    void *ispcMaterialList = nullptr;

    if (materialList)
    {
        ispcMaterials_.clear();
        ispcMaterials_.resize(materialList->numItems);
        for (size_t i=0; i<materialList->numItems; ++i)
        {
            ospray::Material *m = static_cast<ospray::Material**>(materialList->data)[i];
            ispcMaterials_[i] = m ? m->getIE() : nullptr;
        }
        ispcMaterialList = static_cast<void*>(ispcMaterials_.data());
    }
    ispc::ExtendedSpheresGeometry_set(getIE(),model->getIE(),
                                      data->data,ispcMaterialList,
                                      numExtendedSpheres, bytesPerExtendedSphere,
                                      radius, materialID,
                                      offset_center,offset_radius,
                                      offset_timestamp, offset_value,
                                      offset_materialID);
}

OSP_REGISTER_GEOMETRY( ExtendedSpheres, extendedspheres );

} // ::brayns
