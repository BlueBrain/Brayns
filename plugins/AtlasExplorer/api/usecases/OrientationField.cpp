/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/geometry/types/Capsule.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>

namespace
{
struct QuaternionEntry
{
    size_t index;
    brayns::Quaternion quat;
};

class QuaternionExtractor
{
public:
    static std::vector<QuaternionEntry> extract(const AtlasVolume &volume)
    {
        auto &mangler = volume.getData();
        auto data = mangler.asFloats();

        auto numQuaternions = data.size() / 4u;
        auto result = std::vector<QuaternionEntry>();
        result.reserve(numQuaternions);

        for (size_t i = 0; i < numQuaternions; ++i)
        {
            auto index = i * 4;
            auto w = data[index];
            auto x = data[index + 1];
            auto y = data[index + 2];
            auto z = data[index + 3];

            if (!_isValidQuaternion({w, x, y, z}))
            {
                continue;
            }

            result.push_back({i, glm::normalize(brayns::Quaternion(w, x, y, z))});
        }

        return result;
    }

private:
    static bool _isValidQuaternion(const brayns::Vector4f &test)
    {
        if (glm::length(test) == 0.f)
        {
            return false;
        }
        if (!glm::compMin(glm::isfinite(test)))
        {
            return false;
        }
        return true;
    }
};

struct GizmoAxis
{
    brayns::Vector3f axis;
    std::vector<brayns::Capsule> geometry;
};

class GizmoBuilder
{
public:
    static std::array<GizmoAxis, 3> build(const AtlasVolume &volume)
    {
        auto quaternions = QuaternionExtractor::extract(volume);
        auto result = _allocateResult(quaternions.size());

        auto &size = volume.getSize();
        auto &spacing = volume.getSpacing();
        auto frameSize = size.x * size.y;
        auto minDimension = glm::compMin(spacing);
        auto radius = minDimension * 0.05f;

#pragma omp parallel for
        for (size_t i = 0; i < quaternions.size(); ++i)
        {
            auto &entry = quaternions[i];

            const auto z = entry.index / frameSize;
            const auto localFrame = entry.index % frameSize;
            const auto y = localFrame / size.x;
            const auto x = localFrame % size.x;

            const auto voxelCenter = _computeVoxelCenter(spacing, x, y, z);
            for (auto &axis : result)
            {
                // * 0.5f so that the axis length does not invade surronding voxels
                const auto vector = (entry.quat * axis.axis) * minDimension * 0.5f;
                auto &buffer = axis.geometry;
                buffer[i] = brayns::CapsuleFactory::cylinder(voxelCenter, voxelCenter + vector, radius);
            }
        }

        return result;
    }

private:
    static std::array<GizmoAxis, 3> _allocateResult(size_t validQuaternionCount)
    {
        auto result = std::array<GizmoAxis, 3>();
        for (size_t i = 0; i < 3; ++i)
        {
            auto axis = brayns::Vector3f(0.f);
            axis[i] = 1.f;
            result[i].axis = axis;
            result[i].geometry.resize(validQuaternionCount);
        }
        return result;
    }

    static brayns::Vector3f _computeVoxelCenter(const brayns::Vector3f &spacing, size_t x, size_t y, size_t z)
    {
        // Bottom front left corner
        const auto worldX = x * spacing.x;
        const auto worldY = y * spacing.y;
        const auto worldZ = z * spacing.z;
        const auto minCorner = brayns::Vector3f(worldX, worldY, worldZ - spacing.z);
        const auto maxCorner = brayns::Vector3f(worldX + spacing.x, worldY + spacing.y, worldZ);
        return (maxCorner + minCorner) * 0.5f;
    }
};

class ModelBuilder
{
public:
    ModelBuilder(brayns::Model &model)
        : _model(model)
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
        systems.setInitSystem<brayns::GeometryInitSystem>();
        systems.setCommitSystem<brayns::GeometryCommitSystem>();
    }

private:
    brayns::Model &_model;
};
}

std::string OrientationField::getName() const
{
    return "Orientation field";
}

bool OrientationField::isVolumeValid(const AtlasVolume &volume) const
{
    return volume.getVoxelSize() == 4;
}

std::unique_ptr<brayns::Model> OrientationField::execute(const AtlasVolume &volume, const brayns::JsonValue &payload)
    const
{
    (void)payload;

    auto gizmo = GizmoBuilder::build(volume);

    auto model = std::make_unique<brayns::Model>();
    auto builder = ModelBuilder(*model);
    builder.addComponents(gizmo);
    builder.addSystems();

    return model;
}
