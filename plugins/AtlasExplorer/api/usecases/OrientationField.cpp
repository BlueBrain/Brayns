/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "OrientationField.h"

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/geometry/types/Capsule.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>
#include <ospray/ospray_cpp/ext/rkcommon.h>

#include <api/ModelType.h>
#include <api/atlases/OrientationAtlas.h>

#include <cassert>

namespace
{
struct GizmoAxis
{
    brayns::Vector3f axis = brayns::Vector3f(0.f);
    std::vector<brayns::Capsule> geometry;
};

class GizmoBuilder
{
public:
    static std::array<GizmoAxis, 3> build(const OrientationAtlas &atlas)
    {
        auto validQuaternionIndices = _getValidQuaternionIndices(atlas);
        auto result = _allocateResult(validQuaternionIndices.size());

        auto [length, radius] = _getGeometrySizes(atlas.getSpacing());

#pragma omp parallel for
        for (size_t i = 0; i < validQuaternionIndices.size(); ++i)
        {
            auto index = validQuaternionIndices[i];
            auto voxelBounds = atlas.getVoxelBounds(index);
            auto voxelCenter = voxelBounds.center();
            auto &quaternion = atlas[index];
            for (auto &axis : result)
            {
                // * 0.5f so that the axis length does not invade surronding voxels
                auto vector = brayns::math::xfmNormal(quaternion, axis.axis) * length * 0.5f;
                auto &buffer = axis.geometry;
                buffer[i] = brayns::CapsuleFactory::cylinder(voxelCenter, voxelCenter + vector, radius);
            }
        }

        return result;
    }

private:
    static std::vector<size_t> _getValidQuaternionIndices(const OrientationAtlas &atlas)
    {
        auto quaternionCount = atlas.getVoxelCount();
        auto result = std::vector<size_t>();
        result.reserve(quaternionCount);
        for (size_t i = 0; i < quaternionCount; ++i)
        {
            if (brayns::math::dot(atlas[i], atlas[i]) == 0.f)
            {
                continue;
            }
            result.push_back(i);
        }
        return result;
    }

    static std::tuple<float, float> _getGeometrySizes(const brayns::Vector3f &spacing)
    {
        auto length = brayns::math::reduce_min(spacing);
        auto radius = length * 0.05f;
        return std::make_tuple(length, radius);
    }

    static std::array<GizmoAxis, 3> _allocateResult(size_t validQuaternionCount)
    {
        auto result = std::array<GizmoAxis, 3>();
        for (size_t i = 0; i < 3; ++i)
        {
            result[i].axis[i] = 1.f;
            result[i].geometry.resize(validQuaternionCount);
        }
        return result;
    }
};

class ModelBuilder
{
public:
    explicit ModelBuilder(brayns::Model &model):
        _model(model)
    {
    }

    void addComponents(std::array<GizmoAxis, 3> &gizmo)
    {
        auto &components = _model.getComponents();

        auto &geometries = components.add<brayns::Geometries>();
        geometries.elements.reserve(3);
        auto &views = components.add<brayns::GeometryViews>();
        views.elements.reserve(3);
        for (size_t i = 0; i < 3; ++i)
        {
            auto &axis = gizmo[i];
            auto &geometry = geometries.elements.emplace_back(std::move(axis.geometry));
            auto &view = views.elements.emplace_back(geometry);
            view.setColor(axis.axis);
        }
    }

    void addSystems()
    {
        auto &systems = _model.getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        systems.setDataSystem<brayns::GeometryDataSystem>();
    }

private:
    brayns::Model &_model;
};
}

std::string OrientationField::getName() const
{
    return "Orientation field";
}

bool OrientationField::isValidAtlas(const Atlas &atlas) const
{
    return atlas.getVoxelType() == VoxelType::Orientation;
}

std::shared_ptr<brayns::Model> OrientationField::run(const Atlas &atlas, const brayns::JsonValue &payload) const
{
    (void)payload;

    assert(dynamic_cast<const OrientationAtlas *>(&atlas));
    auto orientationAtlas = static_cast<const OrientationAtlas &>(atlas);
    auto gizmo = GizmoBuilder::build(orientationAtlas);

    auto model = std::make_shared<brayns::Model>(ModelType::atlas);
    auto builder = ModelBuilder(*model);
    builder.addComponents(gizmo);
    builder.addSystems();

    return model;
}
