/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Juan Hernando <juan.hernando@epfl.ch>
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

#include "SimulationMaterial.h"
#include "SimulationMaterial_ispc.h"

#include <engines/ospray/ispc/geometry/Cones.h>
#include <engines/ospray/ispc/geometry/SDFGeometries.h>

#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/SDFGeometry.h>
#include <brayns/common/geometry/Sphere.h>

#include <ospray/SDK/geometry/Cylinders.h>
#include <ospray/SDK/geometry/Geometry.h>
#include <ospray/SDK/geometry/Spheres.h>

#include <map>
#include <typeindex>
#include <typeinfo>

extern "C"
{
    int SimulationMaterial_getBytesPerPrimitive(const void* geometry)
    {
        const ospray::Geometry* base =
            static_cast<const ospray::Geometry*>(geometry);
        if (dynamic_cast<const ospray::Spheres*>(base))
            return sizeof(brayns::Sphere);
        else if (dynamic_cast<const ospray::Cylinders*>(base))
            return sizeof(brayns::Cylinder);
        else if (dynamic_cast<const ospray::Cones*>(base))
            return sizeof(brayns::Cone);
        else if (dynamic_cast<const ospray::SDFGeometries*>(base))
            return sizeof(brayns::SDFGeometry);
        return 0;
    }
}

namespace brayns
{
void SimulationMaterial::commit()
{
    if (ispcEquivalent == nullptr)
        ispcEquivalent = ispc::SimulationMaterial_create(this);

    DefaultMaterial::commit();

    // FIXME(jonask): When supported by OSPRay use bool
    const bool withSimulationOffsets = getParam1i("apply_simulation", 0);
    ispc::SimulationMaterial_set(getIE(), withSimulationOffsets);
}

OSP_REGISTER_MATERIAL(advanced_simulation, SimulationMaterial, default);
OSP_REGISTER_MATERIAL(basic_simulation, SimulationMaterial, default);
} // namespace brayns
