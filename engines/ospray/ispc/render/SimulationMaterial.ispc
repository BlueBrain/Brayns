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

#include "SimulationMaterial.ih"

#include "ospray/SDK/geometry/Geometry.ih"

extern "C" unmasked uniform int SimulationMaterial_getBytesPerPrimitive(
    const void* uniform geometry);

static inline int getBytesPerPrimitive(const void* geometry)
{
    int bytesPerPrimitive;
    foreach_unique(g in geometry)
    {
        bytesPerPrimitive = SimulationMaterial_getBytesPerPrimitive(g);
    }
    return bytesPerPrimitive;
}


uint64 SimulationMaterial_getOffset(const uniform Geometry* geometry,
                                    const varying int primID)
{
    if (!geometry)
        return 0;

    // The data pointer in all "derived" geometries is just after data members
    // of the base Geometry struct.
    const uniform uint8* data =  *((const uniform uint8**)&geometry[1]);

    const int bytesPerPrimitive = getBytesPerPrimitive(geometry->cppEquivalent);
    const uint64 bytesPerPrimitive64 = (uint64)bytesPerPrimitive;
    if (primID * bytesPerPrimitive64 > 0x7FFFFFFF)
    {
        data = (const uniform uint8*)(
            (uint64)data + bytesPerPrimitive64 * primID);
    }
    else
    {
        data += bytesPerPrimitive * primID;
    }

    return *((const uniform uint64*)data);
}


export void* uniform SimulationMaterial_create(void* uniform cppE)
{
    uniform SimulationMaterial* uniform self =
        uniform new uniform SimulationMaterial;
    self->super.super.cppEquivalent = cppE;
    return self;
}

export void SimulationMaterial_set(void* uniform self_,
                                   const uniform bool withSimulationOffsets)
{
    uniform SimulationMaterial* uniform self =
        (uniform SimulationMaterial * uniform)self_;
    if (withSimulationOffsets)
        self->getSimulationOffset = SimulationMaterial_getOffset;
}
