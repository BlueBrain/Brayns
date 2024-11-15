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

#include "GeometryObjects.h"

#include <fmt/format.h>

#include <brayns/core/jsonrpc/Errors.h>

namespace
{
using namespace brayns;

bool sameSizeOrEmpty(const auto &left, const auto &right)
{
    if (left.empty() || right.empty())
    {
        return true;
    }

    return left.size() == right.size();
}

bool allSameSizeOrEmpty(const auto &first, const auto &second, const auto &...rest)
{
    if constexpr (sizeof...(rest) == 0)
    {
        return sameSizeOrEmpty(first, second);
    }
    else
    {
        return sameSizeOrEmpty(first, second) && allSameSizeOrEmpty(first, rest...);
    }
}

void checkAllSameSizeOrEmpty(const auto &...args)
{
    auto all = [](auto... values) { return (values && ...); };

    if (all(args.empty()...))
    {
        throw InvalidParams("Empty geometry is not supported");
    }

    if (!allSameSizeOrEmpty(args...))
    {
        throw InvalidParams("All geometry attributes must have the same item count if not empty");
    }
}

template<int N>
void checkIndices(const std::vector<Vector<std::uint32_t, N>> &indices, std::size_t positionCount)
{
    for (const auto &face : indices)
    {
        for (auto i = 0; i < N; ++i)
        {
            if (face[i] >= positionCount)
            {
                throw InvalidParams(fmt::format("Invalid index: {}", face[i]));
            }
        }
    }
}

void checkIndices(const std::vector<std::uint32_t> &indices, std::size_t positionCount)
{
    for (auto index : indices)
    {
        if (index >= positionCount)
        {
            throw InvalidParams(fmt::format("Invalid index: {}", index));
        }
    }
}

template<int N>
void checkMeshParams(const MeshParams<N> &settings)
{
    checkAllSameSizeOrEmpty(settings.value.positions, settings.value.normals, settings.value.colors, settings.value.uvs);
    checkIndices(settings.indices, settings.value.positions.size());
}

void checkSphereParams(const SphereSettings &settings)
{
    checkAllSameSizeOrEmpty(settings.positionsRadii, settings.uvs);
}

void checkDiscParams(const DiscParams &settings)
{
    checkSphereParams(settings.value);
    checkAllSameSizeOrEmpty(settings.value.positionsRadii, settings.normals);
}

void checkBasicCurveParams(const CurveSettings &settings, std::size_t segmentSize, const auto &...attributes)
{
    if (settings.positionsRadii.size() < segmentSize)
    {
        throw InvalidParams(fmt::format("This curve basis requires {} vertices per segment", segmentSize));
    }

    checkAllSameSizeOrEmpty(settings.positionsRadii, settings.colors, settings.uvs, attributes...);
    checkIndices(settings.indices, settings.positionsRadii.size() - segmentSize);
}

// std::size_t getSegmentSize(const auto &)
// {
//     return 4;
// }

// std::size_t getSegmentSize(const LinearBasis &)
// {
//     return 2;
// }

// std::size_t getSegmentSize(const HermiteBasis &)
// {
//     return 2;
// }

// void checkCurveParams(const CurveSettings &settings, const auto &basis)
// {
//     auto segmentSize = getSegmentSize(basis);
//     checkBasicCurveParams(settings, segmentSize);
// }

// void checkCurveParams(const CurveSettings &settings, const HermiteBasis &basis)
// {
//     auto segmentSize = getSegmentSize(basis);
//     checkBasicCurveParams(settings, segmentSize, basis.tangents);

//     if (basis.tangents.empty())
//     {
//         throw InvalidParams("Hermite curve requires tangents");
//     }
// }

void checkRibbonParams(const CurveSettings &settings, const std::vector<Vector3> &normals, const auto &basis)
{
    auto segmentSize = getSegmentSize(basis);
    checkBasicCurveParams(settings, segmentSize, normals);
}

CreateObjectResult createGeometry(ObjectManager &objects, Device &device, auto params, auto validate, auto create, const char *type)
{
    auto &[metadata, settings] = params;

    validate(settings);

    auto geometry = create(device, settings);

    auto ptr = toShared(UserGeometryOf<decltype(settings), decltype(geometry)>{std::move(settings), std::move(geometry)});

    auto object = UserGeometry{
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {type}, std::move(metadata));

    return getResult(stored);
}

template<typename T>
auto getGeometryAs(ObjectManager &objects, const GetObjectParams &params)
{
    auto stored = objects.getAsStored<UserGeometry>(params.id);
    auto &geometry = *castAsShared<T>(stored.get().value, stored);
    return getResult(geometry.settings);
}
}

namespace brayns
{
CreateObjectResult createTriangleMesh(ObjectManager &objects, Device &device, CreateTriangleMeshParams params)
{
    auto validate = [](const auto &settings) { checkMeshParams(settings); };
    auto create = [](auto &device, const auto &params) { return createTriangleMesh(device, params.value, params.indices); };
    return createGeometry(objects, device, std::move(params), validate, create, "TriangleMesh");
}

GetTriangleMeshResult getTriangleMesh(ObjectManager &objects, const GetObjectParams &params)
{
    return getGeometryAs<UserTriangleMesh>(objects, params);
}

CreateObjectResult createQuadMesh(ObjectManager &objects, Device &device, CreateQuadMeshParams params)
{
    auto validate = [](const auto &settings) { checkMeshParams(settings); };
    auto create = [](auto &device, const auto &params) { return createQuadMesh(device, params.value, params.indices); };
    return createGeometry(objects, device, std::move(params), validate, create, "QuadMesh");
}

GetQuadMeshResult getQuadMesh(ObjectManager &objects, const GetObjectParams &params)
{
    return getGeometryAs<UserQuadMesh>(objects, params);
}

CreateObjectResult createSpheres(ObjectManager &objects, Device &device, CreateSpheresParams params)
{
    auto validate = [](const auto &settings) { checkSphereParams(settings); };
    auto create = [](auto &device, const auto &settings) { return createSpheres(device, settings); };
    return createGeometry(objects, device, std::move(params), validate, create, "Spheres");
}

GetSpheresResult getSpheres(ObjectManager &objects, const GetObjectParams &params)
{
    return getGeometryAs<UserSpheres>(objects, params);
}

CreateObjectResult createDiscs(ObjectManager &objects, Device &device, CreateDiscsParams params)
{
    auto validate = [](const auto &settings) { checkDiscParams(settings); };
    auto create = [](auto &device, const auto &settings) { return createDiscs(device, settings.value, settings.normals); };
    return createGeometry(objects, device, std::move(params), validate, create, "Discs");
}

GetDiscsResult getDiscs(ObjectManager &objects, const GetObjectParams &params)
{
    return getGeometryAs<UserDiscs>(objects, params);
}

CreateObjectResult createCylinders(ObjectManager &objects, Device &device, CreateCylindersParams params)
{
    auto validate = [](const auto &settings) { checkBasicCurveParams(settings, 2); };
    auto create = [](auto &device, const auto &settings) { return createCylinders(device, settings); };
    return createGeometry(objects, device, std::move(params), validate, create, "Cylinders");
}

GetCylindersResult getCylinders(ObjectManager &objects, const GetObjectParams &params)
{
    return getGeometryAs<UserCylinders>(objects, params);
}
}
