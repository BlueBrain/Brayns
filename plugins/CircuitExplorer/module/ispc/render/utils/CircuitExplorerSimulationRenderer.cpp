/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "CircuitExplorerSimulationRenderer.h"
#include "CircuitExplorerSimulationRenderer_ispc.h"

#include <brayns/common/Log.h>
#include <engines/ospray/ispc/geometry/Cones.h>
#include <engines/ospray/ispc/geometry/SDFGeometries.h>

#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/SDFGeometry.h>
#include <brayns/common/geometry/Sphere.h>

#include <ospray/SDK/geometry/Cylinders.h>
#include <ospray/SDK/geometry/Geometry.h>
#include <ospray/SDK/geometry/Spheres.h>

#include <ospray/SDK/transferFunction/TransferFunction.h>

extern "C"
{
    int CircuitExplorerSimulationRenderer_getBytesPerPrimitive(
        const void* geometry)
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

namespace circuitExplorer
{
void CircuitExplorerSimulationRenderer::commit()
{
    CircuitExplorerAbstractRenderer::commit();

    _secondaryModel = (ospray::Model*)getParamObject("secondaryModel", nullptr);
    _maxDistanceToSecondaryModel =
        getParam1f("maxDistanceToSecondaryModel", 30.f);

    _simulationData = getParamData("simulationData");
    _simulationDataSize = _simulationData ? _simulationData->size() : 0;

    _alphaCorrection = getParam1f("alphaCorrection", 0.5f);
    _fogThickness = getParam1f("fogThickness", 1e6f);
    _fogStart = getParam1f("fogStart", 0.f);

    // Transfer function
    ospray::TransferFunction* transferFunction =
        (ospray::TransferFunction*)getParamObject("transferFunction", nullptr);
    if (transferFunction)
        ispc::CircuitExplorerSimulationRenderer_setTransferFunction(
            getIE(), transferFunction->getIE());
}

} // namespace circuitExplorer
