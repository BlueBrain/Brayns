/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
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

#include "VectorField.h"

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/geometry/types/Capsule.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>
#include <ospray/ospray_cpp/ext/rkcommon.h>

#include <api/ModelType.h>
#include <api/atlases/VectorAtlas.h>

#include <cassert>

namespace
{
float getAxisColor(float value)
{
    return (value + 1.0F) / 2.0F;
}

brayns::Vector4f getVectorColor(const brayns::Vector3f &vector)
{
    return {
        getAxisColor(vector.x),
        getAxisColor(vector.y),
        getAxisColor(vector.z),
        1.0F,
    };
}

std::vector<std::size_t> getNonEmptyVoxelIndices(const VectorAtlas &atlas)
{
    auto indices = std::vector<std::size_t>();

    auto itemCount = atlas.getVoxelCount();

    for (auto i = std::size_t(0); i < itemCount; ++i)
    {
        if (atlas[i] != brayns::Vector3f())
        {
            indices.push_back(i);
        }
    }

    return indices;
}

struct ColoredCapsules
{
    std::vector<brayns::Capsule> capsules;
    std::vector<brayns::Vector4f> colors;
};

ColoredCapsules createVectorCapsules(const VectorAtlas &atlas)
{
    auto indices = getNonEmptyVoxelIndices(atlas);
    auto indexCount = indices.size();

    auto result = ColoredCapsules();
    result.capsules.resize(indexCount);
    result.colors.resize(indexCount);

#pragma omp parallel for
    for (auto i = std::size_t(0); i < indexCount; ++i)
    {
        auto index = indices[i];
        const auto &direction = atlas[index];

        assert(direction != brayns::Vector3f());

        auto bounds = atlas.getVoxelBounds(index);
        auto origin = bounds.center();

        const auto &spacing = atlas.getSpacing();
        auto length = brayns::math::reduce_min(spacing) * 0.5F;

        auto radius = 0.05F * length;

        auto vector = direction * length;
        auto tip = origin + vector;

        result.capsules[i] = brayns::CapsuleFactory::cone(origin, radius, tip, 0.0F);
        result.colors[i] = getVectorColor(direction);
    }

    return result;
}

void buildVectorModel(brayns::Model &model, ColoredCapsules capsules)
{
    auto &components = model.getComponents();

    auto &geometries = components.add<brayns::Geometries>();
    auto &geometry = geometries.elements.emplace_back(std::move(capsules.capsules));

    auto &views = components.add<brayns::GeometryViews>();
    auto &view = views.elements.emplace_back(geometry);
    view.setColorPerPrimitive(ospray::cpp::CopiedData(capsules.colors));

    auto &systems = model.getSystems();
    systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
    systems.setDataSystem<brayns::GeometryDataSystem>();
}
}

std::string VectorField::getName() const
{
    return "Vector field";
}

bool VectorField::isValidAtlas(const Atlas &atlas) const
{
    return atlas.getVoxelType() == VoxelType::Vector;
}

std::shared_ptr<brayns::Model> VectorField::run(const Atlas &atlas, const brayns::JsonValue &payload) const
{
    (void)payload;

    assert(dynamic_cast<const VectorAtlas *>(&atlas));
    const auto &vectorAtlas = static_cast<const VectorAtlas &>(atlas);

    auto capsules = createVectorCapsules(vectorAtlas);

    auto model = std::make_shared<brayns::Model>(ModelType::atlas);
    buildVectorModel(*model, std::move(capsules));

    return model;
}
