/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Based on OSPRay implementation
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

#pragma once

#include "ospray/SDK/geometry/Geometry.h"
#include <brayns/common/types.h>

namespace ospray
{
struct ExtendedSDFGeometries : public ospray::Geometry
{
    std::string toString() const final { return "hbp::ExtendedSDFGeometries"; }
    void finalize(ospray::Model *model) final;

    int32 materialID;

    size_t numExtendedSDFGeometries;
    size_t bytesPerExtendedSDFGeometryIndex;
    size_t bytesPerSDFGeometry;
    size_t bytesPerNeighbour;

    ospray::Ref<ospray::Data> data;
    ospray::Ref<ospray::Data> materialList;

    ospray::Ref<ospray::Data> neighbours;
    ospray::Ref<ospray::Data> geometries;

    ExtendedSDFGeometries();

private:
    std::vector<void *> ispcMaterials_;
};

} // namespace ospray
