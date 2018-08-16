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

#include <cstddef>

namespace ospray
{
ExtendedSDFGeometries::ExtendedSDFGeometries()
{
    this->ispcEquivalent = ispc::ExtendedSDFGeometries_create(this);
}

void ExtendedSDFGeometries::finalize(ospray::Model *model)
{
    materialID = getParam1i("materialID", 0);

    bytesPerExtendedSDFGeometryIndex = sizeof(uint32_t);
    bytesPerSDFGeometry = sizeof(brayns::SDFGeometry);
    bytesPerNeighbour = sizeof(size_t);

    data = getParamData("extendedsdfgeometries", nullptr);
    materialList = getParamData("materialList", nullptr);
    neighbours = getParamData("neighbours", nullptr);
    geometries = getParamData("geometries", nullptr);

    if (data.ptr == nullptr)
        throw std::runtime_error(
            "#ospray:geometry/ExtendedSDFGeometries: "
            "no 'ExtendedSDFGeometries' data specified");
    numExtendedSDFGeometries =
        data->numBytes / bytesPerExtendedSDFGeometryIndex;

    if (numExtendedSDFGeometries >= (1ULL << 30))
    {
        throw std::runtime_error(
            "#brayns::ExtendedSDFGeometries: too many extended "
            "SDF geometries in this sphere geometry. Consider "
            "splitting this geometry in multiple "
            "geometries with fewer SDF geometries (you "
            "can still put all those geometries into a "
            "single model, but you can't put that many "
            "SDF geometries into a single geometry "
            "without causing address overflows)");
    }

    void *ispcMaterialList = nullptr;

    if (materialList)
    {
        ispcMaterials_.clear();
        ispcMaterials_.resize(materialList->numItems);
        for (size_t i = 0; i < materialList->numItems; ++i)
        {
            ospray::Material *m =
                static_cast<ospray::Material **>(materialList->data)[i];
            ispcMaterials_[i] = m ? m->getIE() : nullptr;
        }
        ispcMaterialList = static_cast<void *>(ispcMaterials_.data());
    }
    ispc::ExtendedSDFGeometriesGeometry_set(
        getIE(), model->getIE(), data->data, ispcMaterialList,
        numExtendedSDFGeometries, bytesPerExtendedSDFGeometryIndex, materialID,
        neighbours->data, geometries->data, bytesPerSDFGeometry,
        bytesPerNeighbour);
}

OSP_REGISTER_GEOMETRY(ExtendedSDFGeometries, extendedsdfgeometries);

} // namespace ospray
