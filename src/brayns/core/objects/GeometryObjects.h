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

#pragma once

#include <any>
#include <concepts>
#include <functional>

#include <brayns/core/engine/Geometry.h>
#include <brayns/core/jsonrpc/PayloadReflector.h>
#include <brayns/core/manager/ObjectManager.h>

#include "VolumeObjects.h"

namespace brayns
{
struct UserGeometry
{
    std::any value;
    std::function<Geometry()> get;
};

template<typename Storage, std::derived_from<Geometry> T>
struct UserGeometryOf
{
    Storage storage;
    T value;
};

template<int N>
struct MeshParams
{
    MeshSettings value;
    std::vector<Vector<std::uint32_t, N>> indices;
};

template<int N>
struct JsonObjectReflector<MeshParams<N>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<MeshParams<N>>();
        builder.field("positions", [](auto &object) { return &object.value.positions; }).description("Vertex positions XYZ").minItems(std::size_t(N));
        builder.field("normals", [](auto &object) { return &object.value.normals; })
            .description("Vertex normals XYZ")
            .defaultValue(std::vector<Vector3>());
        builder.field("colors", [](auto &object) { return &object.value.colors; })
            .description("Vertex colors RGBA")
            .defaultValue(std::vector<Color4>());
        builder.field("uvs", [](auto &object) { return &object.value.uvs; })
            .description("Vertex texture coordinates XY")
            .defaultValue(std::vector<Vector2>());
        builder.field("indices", [](auto &object) { return &object.indices; })
            .description("Indices grouped per face")
            .defaultValue(std::vector<Vector<std::uint32_t, N>>());
        return builder.build();
    }
};

using TriangleMeshParams = MeshParams<3>;
using CreateTriangleMeshParams = CreateParamsOf<TriangleMeshParams>;
using GetTriangleMeshResult = GetResultOf<TriangleMeshParams>;
using UserTriangleMesh = UserGeometryOf<TriangleMeshParams, TriangleMesh>;

CreateObjectResult createTriangleMesh(ObjectManager &objects, Device &device, CreateTriangleMeshParams params);
GetTriangleMeshResult getTriangleMesh(ObjectManager &objects, const GetObjectParams &params);

using QuadMeshParams = MeshParams<4>;
using CreateQuadMeshParams = CreateParamsOf<QuadMeshParams>;
using GetQuadMeshResult = GetResultOf<QuadMeshParams>;
using UserQuadMesh = UserGeometryOf<QuadMeshParams, QuadMesh>;

CreateObjectResult createQuadMesh(ObjectManager &objects, Device &device, CreateQuadMeshParams params);
GetQuadMeshResult getQuadMesh(ObjectManager &objects, const GetObjectParams &params);

template<>
struct JsonObjectReflector<SphereSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<SphereSettings>();
        builder.field("positionsRadii", [](auto &object) { return &object.positionsRadii; }).description("Positions and radii XYZR").minItems(1);
        builder.field("uvs", [](auto &object) { return &object.uvs; })
            .description("Texture coordinates XY, constant per primitive")
            .defaultValue(std::vector<Vector2>());
        return builder.build();
    }
};

using CreateSpheresParams = CreateParamsOf<SphereSettings>;
using GetSpheresResult = GetResultOf<SphereSettings>;
using UserSpheres = UserGeometryOf<SphereSettings, Spheres>;

CreateObjectResult createSpheres(ObjectManager &objects, Device &device, CreateSpheresParams params);
GetSpheresResult getSpheres(ObjectManager &objects, const GetObjectParams &params);

struct DiscParams
{
    SphereSettings value;
    std::vector<Vector3> normals;
};

template<>
struct JsonObjectReflector<DiscParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<DiscParams>();
        builder.extend([](auto &object) { return &object.value; });
        builder.field("normals", [](auto &object) { return &object.normals; })
            .description("Normals XYZ, the disc will always face the camera if empty")
            .defaultValue(std::vector<Vector3>());
        return builder.build();
    }
};

using CreateDiscsParams = CreateParamsOf<DiscParams>;
using GetDiscsResult = GetResultOf<DiscParams>;
using UserDiscs = UserGeometryOf<DiscParams, Discs>;

CreateObjectResult createDiscs(ObjectManager &objects, Device &device, CreateDiscsParams params);
GetDiscsResult getDiscs(ObjectManager &objects, const GetObjectParams &params);

template<>
struct JsonObjectReflector<CurveSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<CurveSettings>();
        builder.field("positionsRadii", [](auto &object) { return &object.positionsRadii; })
            .description("Vertex positions and radii XYZR")
            .minItems(2);
        builder.field("indices", [](auto &object) { return &object.indices; })
            .description("Indices, each index points to the first vertex of a segment which size depends on the chosen basis")
            .minItems(1);
        builder.field("colors", [](auto &object) { return &object.colors; }).description("Vertex colors RGBA").defaultValue(std::vector<Color4>());
        builder.field("uvs", [](auto &object) { return &object.uvs; })
            .description("Vertex texture coordinates XY")
            .defaultValue(std::vector<Vector2>());
        return builder.build();
    }
};

using CreateCylindersParams = CreateParamsOf<CurveSettings>;
using GetCylindersResult = GetResultOf<CurveSettings>;
using UserCylinders = UserGeometryOf<CurveSettings, Cylinders>;

CreateObjectResult createCylinders(ObjectManager &objects, Device &device, CreateCylindersParams params);
GetCylindersResult getCylinders(ObjectManager &objects, const GetObjectParams &params);

template<>
struct EnumReflector<CurveType>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<CurveType>();
        builder.field("Flat", CurveType::Flat).description("Render the curve always facing camera (faster)");
        builder.field("Round", CurveType::Round).description("Renders a sweep surface by sweeping a varying radius circle tangential along the curve");
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<LinearBasis>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<LinearBasis>();
        builder.description("Each segment is a line composed of 2 consecutive vertices");
        builder.constant("type", "Linear");
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<BezierBasis>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<BezierBasis>();
        builder.description("Each segment is a Bezier interpolation of 4 consecutive vertices");
        builder.constant("type", "Bezier");
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<BsplineBasis>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<BsplineBasis>();
        builder.description("Each segment is a B-Spline of 4 consecutive vertices (no interpolation)");
        builder.constant("type", "Bspline");
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<HermiteBasis>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<HermiteBasis>();
        builder.description("Each segment is a Hermite interpolation of 2 consecutive vertices (requires tangents)");
        builder.constant("type", "Hermite");
        builder.field("tangents", [](auto &object) { return &object.tangents; }).description("Vertex trangents XYZW").minItems(2);
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<CatmullRomBasis>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<CatmullRomBasis>();
        builder.description("Each segment is a Catmull-Rom spline of 4 consecutive vertices (no interpolation)");
        builder.constant("type", "CatmullRom");
        return builder.build();
    }
};

struct CurveParams
{
    CurveSettings value;
    CurveType type;
    CurveBasis basis;
};

template<>
struct JsonObjectReflector<CurveParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<CurveParams>();
        builder.extend([](auto &object) { return &object.value; });
        builder.field("type", [](auto &object) { return &object.type; }).description("Curve type").defaultValue(CurveType::Round);
        builder.field("basis", [](auto &object) { return &object.basis; }).description("Curve basis").defaultValue(LinearBasis());
        return builder.build();
    }
};

using CreateCurveParams = CreateParamsOf<CurveParams>;
using GetCurveResult = GetResultOf<CurveParams>;
using UserCurve = UserGeometryOf<CurveParams, Curve>;

CreateObjectResult createCurve(ObjectManager &objects, Device &device, CreateCurveParams params);
GetCurveResult getCurve(ObjectManager &objects, const GetObjectParams &params);

struct RibbonParams
{
    CurveSettings value;
    std::vector<Vector3> normals;
    RibbonBasis basis;
};

template<>
struct JsonObjectReflector<RibbonParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<RibbonParams>();
        builder.extend([](auto &object) { return &object.value; });
        builder.field("normals", [](auto &object) { return &object.normals; }).description("Vertex normals XYZ").minItems(2);
        builder.field("basis", [](auto &object) { return &object.basis; }).description("Ribbon curve basis").defaultValue(BezierBasis());
        return builder.build();
    }
};

using CreateRibbonParams = CreateParamsOf<RibbonParams>;
using GetRibbonResult = GetResultOf<RibbonParams>;
using UserRibbon = UserGeometryOf<RibbonParams, Ribbon>;

CreateObjectResult createRibbon(ObjectManager &objects, Device &device, CreateRibbonParams params);
GetRibbonResult getRibbon(ObjectManager &objects, const GetObjectParams &params);

template<>
struct JsonObjectReflector<BoxSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<BoxSettings>();
        builder.field("boxes", [](auto &object) { return &object.boxes; }).description("Axis-aligned boxes min and max XYZ").minItems(1);
        return builder.build();
    }
};

using CreateBoxesParams = CreateParamsOf<BoxSettings>;
using GetBoxesResult = GetResultOf<BoxSettings>;
using UserBoxes = UserGeometryOf<BoxSettings, Boxes>;

CreateObjectResult createBoxes(ObjectManager &objects, Device &device, CreateBoxesParams params);
GetBoxesResult getBoxes(ObjectManager &objects, const GetObjectParams &params);

template<>
struct JsonObjectReflector<PlaneSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<PlaneSettings>();
        builder.field("coefficients", [](auto &object) { return &object.coefficients; })
            .description("Plane equations ABCD such as Ax + By + Cz + D = 0")
            .minItems(1);
        builder.field("bounds", [](auto &object) { return &object.bounds; })
            .description("Optional axis-aligned bounds per plane min and max XYZ")
            .defaultValue(std::vector<Box3>());
        return builder.build();
    }
};

using CreatePlanesParams = CreateParamsOf<PlaneSettings>;
using GetPlanesResult = GetResultOf<PlaneSettings>;
using UserPlanes = UserGeometryOf<PlaneSettings, Planes>;

CreateObjectResult createPlanes(ObjectManager &objects, Device &device, CreatePlanesParams params);
GetPlanesResult getPlanes(ObjectManager &objects, const GetObjectParams &params);

struct IsosurfaceParams
{
    IsosurfaceSettings value;
    ObjectId volume;
};

template<>
struct JsonObjectReflector<IsosurfaceParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<IsosurfaceParams>();
        builder.field("isovalues", [](auto &object) { return &object.value.isovalues; }).description("List of isovalue per isosurface").minItems(1);
        builder.field("volume", [](auto &object) { return &object.volume; }).description("ID of the volume to be isosurfaced");
        return builder.build();
    }
};

struct IsosurfaceStorage
{
    IsosurfaceSettings settings;
    Stored<UserVolume> volume;
};

using CreateIsosurfacesParams = CreateParamsOf<IsosurfaceParams>;
using GetIsosurfacesResult = GetResultOf<IsosurfaceParams>;
using UserIsosurfaces = UserGeometryOf<IsosurfaceStorage, Isosurfaces>;

CreateObjectResult createIsosurfaces(ObjectManager &objects, Device &device, CreateIsosurfacesParams params);
GetIsosurfacesResult getIsosurfaces(ObjectManager &objects, const GetObjectParams &params);
}
