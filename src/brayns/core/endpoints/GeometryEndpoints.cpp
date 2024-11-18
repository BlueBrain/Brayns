/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "GeometryEndpoints.h"

#include <brayns/core/objects/GeometryObjects.h>

namespace brayns
{
void addGeometryEndpoints(ApiBuilder &builder, ObjectManager &objects, Device &device)
{
    builder.endpoint("createTriangleMesh", [&](CreateTriangleMeshParams params) { return createTriangleMesh(objects, device, std::move(params)); })
        .description("Create a mesh with triangular faces");
    builder.endpoint("getTriangleMesh", [&](GetObjectParams params) { return getTriangleMesh(objects, params); }).description("Get triangle mesh");

    builder.endpoint("createQuadMesh", [&](CreateQuadMeshParams params) { return createQuadMesh(objects, device, std::move(params)); })
        .description("Create a mesh with quad faces");
    builder.endpoint("getQuadMesh", [&](GetObjectParams params) { return getQuadMesh(objects, params); }).description("Get quad mesh");

    builder.endpoint("createSpheres", [&](CreateSpheresParams params) { return createSpheres(objects, device, std::move(params)); })
        .description("Create a geometry made of spheres");
    builder.endpoint("getSpheres", [&](GetObjectParams params) { return getSpheres(objects, params); }).description("Get spheres");

    builder.endpoint("createDiscs", [&](CreateDiscsParams params) { return createDiscs(objects, device, std::move(params)); })
        .description("Create a geometry made of discs");
    builder.endpoint("getDiscs", [&](GetObjectParams params) { return getDiscs(objects, params); }).description("Get discs");

    builder.endpoint("createCylinders", [&](CreateCylindersParams params) { return createCylinders(objects, device, std::move(params)); })
        .description("Create a geometry made of cylinders");
    builder.endpoint("getCylinders", [&](GetObjectParams params) { return getCylinders(objects, params); }).description("Get cylinders");

    builder.endpoint("createCurve", [&](CreateCurveParams params) { return createCurve(objects, device, std::move(params)); })
        .description("Create a curve");
    builder.endpoint("getCurve", [&](GetObjectParams params) { return getCurve(objects, params); }).description("Get curve");

    builder.endpoint("createRibbon", [&](CreateRibbonParams params) { return createRibbon(objects, device, std::move(params)); })
        .description("Create a ribbon");
    builder.endpoint("getRibbon", [&](GetObjectParams params) { return getRibbon(objects, params); }).description("Get ribbon");

    builder.endpoint("createBoxes", [&](CreateBoxesParams params) { return createBoxes(objects, device, std::move(params)); })
        .description("Create a geometry made of boxes");
    builder.endpoint("getBoxes", [&](GetObjectParams params) { return getBoxes(objects, params); }).description("Get boxes");

    builder.endpoint("createPlanes", [&](CreatePlanesParams params) { return createPlanes(objects, device, std::move(params)); })
        .description("Create a geometry made of planes");
    builder.endpoint("getPlanes", [&](GetObjectParams params) { return getPlanes(objects, params); }).description("Get planes");

    builder.endpoint("createIsosurfaces", [&](CreateIsosurfacesParams params) { return createIsosurfaces(objects, device, std::move(params)); })
        .description("Create a geometry made of isosurfaces from a volume");
    builder.endpoint("getIsosurfaces", [&](GetObjectParams params) { return getIsosurfaces(objects, params); }).description("Get isosurfaces");
}
}
